[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.11242382.svg)](https://doi.org/10.5281/zenodo.11242382)

# Low-cost thermal comfort device
Arduino IDE code for a low-cost and user-friendly meteorological device for mobile thermal comfort mapping.

The device is based on an Adafruit Feather M0 Adalogger and sensors for: 
- air temperature and humidity [SHT 85 Sensirion](https://sensirion.com/products/catalog/SHT85)
- air pressure [BMP 390 Bosch](https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/pressure-sensors-bmp390.html)
- surface temperature [MLX90614 Melexis](https://www.melexis.com/en/product/mlx90614/digital-plug-play-infrared-thermometer-to-can)
- solar irradiance [SSR2AD PM Ecology](https://www.measurementsystems.co.uk/docs/SSR2AD_Specification%20EN.pdf)
- global temperature [Max 31865 Pt100 and black aluminium globe]([)](https://www.antratek.de/pt100-rtd-temperature-sensor-amplifier-max31865)

These variables are important inputs for calculating thermal comfort indices, which offer a comprehensive understanding of pedestrian thermal comfort conditions. 
According to its data sheet, the inclusion of a [NEO-M9N Sparkfun Global Navigation Satellite System (GNSS)]([)](https://exp-tech.de/en/products/sparkfun-gps-breakout-neo-m9n-sma-qwiic) module enables precise spatial referencing of the meteorological data, with an accuracy of approximately 2.0 m Circular Error Probable (CEP). Related works provide comparison data to established high-cost systems, which demonstrate the suitability of this device for its purpose.

# Other sources

A how-to document for building the device is available at: 

- Gallacher, C. (2024). How to build a low-cost and user-friendly meteorological device for mobile thermal comfort mapping https://doi.org/10.5281/zenodo.11217591


All data collected during the comparisons of the new low-cost device with the established high-end alternatives are available as open data on Zenodo.
- Gallacher, C. (2024a). Stationary comparison data between a new low-cost meteorological device from the Technical University of Dresden Chair of Meteorology’s backpack meteorological device. Zenodo.   https://doi.org/10.5281/zenodo.10340571
- Gallacher, C. (2024b). Mobile comparison data and analysis between a new low-cost meteorological device and the Technical University of Dresden Chair of Meteorology’s backpack meteorological device. Zenodo. https://doi.org/10.5281/zenodo.11217910
- Gallacher, C. (2024c). Stationary comparison data and analysis between a new low-cost meteorological device and the MaRTy device. Zenodo. https://doi.org/10.5281/zenodo.11217736 
