# lowcostthermalcmofortdevice
Arduino IDE code for a low-cost and user-friendly meteorological device for mobile thermal comfort mapping.

The device is based on an Adafruit Feather M0 Adalogger and sensors for: air temperature and humidity (SHT 85 Sensirion), air pressure (BMP 390 Bosch), surface temperature (MLX90614 Melexis), solar irradiance (SR2AD PM Ecology), and globe temperature (Max 31865 Pt100 and black aluminium globe). These variables are important inputs for calculating thermal comfort indices, which offers a comprehensive understanding of pedestrian thermal comfort conditions. The inclusion of a NEO-M9N Sparkfun Global Navigation Satellite System (GNSS) module enables precise spatial referencing of the meteorological data, with an accuracy of approximately 2.0 m Circular Error Probable (CEP), according to its data sheet. Comparison data to established high-cost systems which demonstrates the suitability for purpose of this device is provided in related works.

A how to document for building the device is available at: 
Gallacher, C. (2024). How to build a low-cost and user-friendly meteorological device for mobile thermal comfort mapping. Zenodo. https://doi.org/10.5281/zenodo.11217591 
