/*

 Copyright 2022 Claire Gallacher and Paul Gallacher. All Rights Reserved.
 
 This work is licensed under the MIT license, see the file LICENSE for details or
 (https://en.wikipedia.org/wiki/MIT_License)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

************************************************************************************************ 


The device uses An Adafruit MO Adalogger to save meterological data from sensors to an SD card, 
which can then be exported and processed through further software to calculate thermal comfort
metrics and indices. The senors connected measure Air Temperature, Ground Temperature, Globe 
Temperature,Pressure, Humidity and Global Radiation linked with GNNS co-ordinates and UTC time.

 
                        Feather M0 Adalogger


 SSR2AD-----|-----|       _____________             
            |12 V |      |             |
            |-----|      | Reset       |                      ______
   3.3 V devices -|------| 3.3 v  NEG  |---------------------| LiPo |
                         | Aref   POS  |--Push Button--/-----| Bat  |
   All devices {- |------| Gnd         |                     |______|
                         | A0     VBAT |
     SSR2AD--R1K---------| A1     EN   |
              |          | A2     VBUS |
             R2K         | A3     D13  |
         |----|          | A4     D12  |
         |               | A5     D11  |
         |  Max31865-----| SCK    D10  |--Max31865--PT100---0
         |  Max31865-----| MOSI   D9   |
         |  Max31865-----| MISO   D6   |--R200-----------------Led---------|
         |               | D0     D5   |                                   |
         |               | D1     SCL  |--NEO9--SHT85--BMP390--MLX90614    |
         |---------------| Gnd    SDA  |--NEO9--SHT85--BMP390--MLX90614    |
         |               |             |                                   |
         |               |___8__7__4___|                                   |
         |                   |  |  |                                       |
         |                   G  SD SD                                      |
         |                  LED CD CS                                      |                                 
         |_________________________________________________________________|         


Components:
Feather M0 Adalogger
MicroSD Card (32 GB Max)
NeoM9 GPS
Max31865 amplifier and PT100
SHT 85 temperature & humidity sensor
BMP390 Pressure sensor
MLX90614 IR sensor
SSR2AD Pyranometer
Power on switch/ led/ LiPo battery
   
 The circuit:
 * SD card uses chip select pin 4
 * NeoM9 GPS, SHT 85, BMP390 and MLX IR sensor all use 12c bus
 * MAX31865 uses hardware SPI MISO/MOSI/SCK and CS PIN 10
 * SSR2AD Uses Analog input pin 1 via voltage divider to prevent overvoltage of 3.3 v device
 * Led out uses pin 6
 
 */

// Initial libraries and parameters for the device

#include <SPI.h>
#include <SD.h>
#include <Wire.h> // for I2C to GNSS
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"   // for pressure sensor
#include <Adafruit_MAX31865.h> // for PT100
#include <Adafruit_MLX90614.h> // for IR sensor
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Adafruit_BMP3XX bmp;
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> // for GNSS module
SFE_UBLOX_GNSS myGNSS;
#include "SHT85.h"  // for air temp sensor
#define SHT85_ADDRESS         0x44 // address of Air temperature sensor 



Adafruit_MAX31865 thermo = Adafruit_MAX31865(10); // for hardware SPI for PT100 amp, pass the CS pin used

// The resistance value of the Rref. Use 430.0 for PT100 and 4300.0 for PT1000 (PT1000 can give less self heating by current flow I2R losses)
#define RREF      430.0
// The 'nominal' Zero-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100


long lastTime = 0; //local timer to limit amount of I2C traffic to u-blox module.
long LedTime = 0; //local timer for led flashing.


uint32_t start;
uint32_t stop;


int sensorPin = A1;    // The input pin for the pyranometer
int sensorValue = 0;  // variable to store the value coming from the sensor

int sensorPinP = A7;    // The input pin for the Battery power gpio no 9
int sensorValueP = 0;  // variable to store the value coming from the Battery
  
  

SHT85 sht;

// user defined filename and header
char filename[] = "MData_00.csv";  // filename for each data logging session

unsigned long lastupdate = 0;  // timer value when last  update was done
uint32_t timer = 0;    
unsigned long delayTime;
File logile;
const int chipSelect = 4; // pin for CS of SD card on MO Adalogger

// Variables will change:
int ledState = LOW;             // ledState used to set the LED

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 1000;           //

const int ledPin =  6;      // the number of the Red LED pin built into the on/off switch

// variables will change:
int count_value =0;
int prestate =0;

// initalisation of devices and checks

void setup()
{
  Serial.println("Starting Device");
  Serial.begin(9600);
  Wire.begin();
  thermo.begin(MAX31865_3WIRE);  // or set to 2WIRE or 4WIRE as necessary
  pinMode(6, OUTPUT); // set pin 6 as an output to led
  Serial.print("\nInitializing SD card..."); // just testing if the card is installed and working!

 if (!mlx.begin()) {
    Serial.println("Problem connecting to MLX sensor. Check wiring."); // just checking IR sensor
    while (1);
  };

  Serial.print("Emissivity = "); Serial.println(mlx.readEmissivity());
  Serial.println("================================================");
  

  delay(3000); // wait 3 s
  
   if (!SD.begin(chipSelect)){
    Serial.println(" SD card initialization failed. Check:");
    Serial.println("* if a card is inserted?");
    Serial.println("* if the wiring is still correct?");
    digitalWrite(6, HIGH); // turn the LED on (HIGH is the voltage level)
    while (1);
  } else {
    Serial.println("Wiring is all correct and a card is present.");
 
    // create a new file name for each reset/start
  
    for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
       break;  // leave the loop!
       }
  
    }


  if (myGNSS.begin() == false) //Connect to the u-blox module using the Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. check wiring."));
    while (1);
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to turn off NMEA noise
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings 

  sht.begin(SHT85_ADDRESS);
  Wire.setClock(100000);

  uint16_t stat = sht.readStatus();

  if (!bmp.begin_I2C()) {   // hardware I2C mode, pass address & alt Wire
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
     digitalWrite(6, HIGH); // turn the LED on (HIGH is the voltage level)
    while (1);
  }

  // Set up the oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);


  // open the file, close this one before opening another.
 // File logfile = SD.open("MData_00.csv", FILE_WRITE);
File logfile = SD.open(filename, FILE_WRITE);

  //print header in CSV file top row
  logfile.println(F("Date,Time,Air Temperature,Globe Temperature,Ground Temperature,Pressure,Humidity,Global Irradiation,Lattitude,Longditude"));
  // logfile.println(F(",,Deg C,Deg C,Deg C,hPa,% RH,W/m2,"));

#if ECHO_TO_SERIAL 
 Serial.print(F("setup done"));
#endif

logfile.close();


// Flash led if all ok, else led on for fault

   digitalWrite(6, HIGH); // turn the LED on (HIGH is the voltage level)
   delay(1000); // wait for a second
   digitalWrite(6, LOW); // turn the LED off by making the voltage LOW
   delay(1000); // wait for a second
   digitalWrite(6, HIGH); // turn the LED on (HIGH is the voltage level)
   delay(1000); // wait for a second
   digitalWrite(6, LOW); // turn the LED off by making the voltage LOW
   delay(1000); // wait for a second
   digitalWrite(6, HIGH); // turn the LED on (HIGH is the voltage level)
   delay(1000); // wait for a second
   digitalWrite(6, LOW); // turn the LED off by making the voltage LOW
   delay(1000); // wait for a second

}

}
// Start of main program to run constantly

void loop() {
  
 
 
  if (millis() - lastTime > 1000) //Query module every second more often will cause too much I2C traffic.
  {
  //start = micros();
  sht.read();         // read temp sensor
  
  uint16_t rtd = thermo.readRTD();
  
  sensorValue = analogRead(sensorPin);
  sensorValueP = analogRead(sensorPinP);

  Serial.print("Air Temperature= "); Serial.print(sht.getTemperature(),1); Serial.print(" *C");Serial.println();
  Serial.print("Globe Temp= "); Serial.print(thermo.temperature(RNOMINAL, RREF)); Serial.print(" *C");Serial.println();
  Serial.print("Ground Temperature= "); Serial.print(mlx.readObjectTempC()); Serial.println(" *C"); Serial.println(); 
  Serial.print("Pressure= "); Serial.print(bmp.pressure / 100.0); Serial.print(" hPa");Serial.println();
  Serial.print("Humidity= "); Serial.print(sht.getHumidity(), 1); Serial.print(" %RH");Serial.println();
  Serial.print("Global Irradiation= "); Serial.print(sensorValue*1.55914); Serial.print(" W/m2");Serial.println();
// scaling factor for adc = 1.55914 (1595/1023), possibly 1024 is more correct
    lastTime = millis(); //Update the timer
    Serial.println(); 
    long latitude = myGNSS.getLatitude();
    Serial.print(F("Lat: "));
    Serial.print(latitude);
    Serial.print("\t");
    long longitude = myGNSS.getLongitude();
    Serial.print(F(" Long: "));
    Serial.print(longitude);Serial.println();
    long altitude = myGNSS.getAltitude();
    Serial.print(F(" Alt: "));
    Serial.print(altitude);
    long speed = myGNSS.getGroundSpeed();
    Serial.print(F(" Speed: "));
    Serial.print(speed);
    Serial.print(F(" (mm/s)"));Serial.println();
    long heading = myGNSS.getHeading();
    Serial.print(F(" Heading: "));
    Serial.print(heading);
    Serial.print(F(" (degrees * 10^-5)"));
    Serial.println();
    int pDOP = myGNSS.getPDOP();
    Serial.print(F(" pDOP: "));
    Serial.print(pDOP / 100.0, 2); // Convert pDOP scaling from 0.01 to 1
    Serial.println();

    Serial.print("Battery power= "); Serial.print((sensorValueP*1.29)-640); Serial.print(" %");
    Serial.println();
   // scaling factor for adc = 2*3.3/1024 x 200 - 640
   
  if (! bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    digitalWrite(6, HIGH); // turn the LED on
    return;
  }

  Serial.println();

File logfile = SD.open(filename, FILE_WRITE);

Serial.println();
  logfile.print(myGNSS.getMonth());
  logfile.print("/");
  logfile.print(myGNSS.getDay());
  logfile.print("/");
  logfile.print(myGNSS.getYear());
  logfile.print(F(",")); 
  logfile.print(myGNSS.getHour());
  logfile.print(F(":")); 
  logfile.print(myGNSS.getMinute());
  logfile.print(F(":"));
  logfile.print(myGNSS.getSecond());
  logfile.print(F(",")); 
  logfile.print(sht.getTemperature(), 1);
  logfile.print(F(",")); 
  logfile.print(thermo.temperature(RNOMINAL, RREF));
  logfile.print(F(",")); 
  logfile.print(mlx.readObjectTempC());
  logfile.print(F(","));
  logfile.print(bmp.pressure / 100.0);
  logfile.print(F(",")); 
  logfile.print(sht.getHumidity(), 1); 
  logfile.print(F(","));
  logfile.print(sensorValue*1.55914); 
  logfile.print(F(","));
  logfile.print(myGNSS.getLatitude());
  logfile.print(F(", ")); 
  logfile.print(myGNSS.getLongitude());
  logfile.print(F(","));

  logfile.println(F(""));

 digitalWrite(6, LOW);
 if (millis() - LedTime > 10000)
  {
    LedTime = millis(); //Update the timer
     if (digitalRead(6) == LOW)
     {
    digitalWrite(6, HIGH);
 }
  else
    digitalWrite(6, LOW);
  }
    
  delay(100);

  logfile.close();
 //blink LED every other pass through main loop
  
}
}

// -- END OF FILE --
