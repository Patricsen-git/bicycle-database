# bicycle-database
A collection of IMU and GPS sensor data, gathered while riding a bicycle.

This is a collection of sensor data, that was gathered while riding a bicycle. This document serves as a description on how to use the data, what to expect from it and how it can be reproduced. The different sensors are:
- The IMU of an Arduino Nano BLE Sense
- GPS Coordinates of an external GPS Module (Neo 6M)
- GPS Coordinates of a mobile phone, recorded via PhyPhox
- A video, which was filmed using a second mobile phone


The readings are saved as a .csv file. The structure is as follows:
| Timestamp (s) | X-Accl. | 1. Y-Accl | 1. Z-Accl. | 1. X-Ang. Vel. | 1. Y-Ang. Vel. | 1. Z-Ang. Vel. | 1. Lat. | 1. Lon. | 1. Alt. | 
|------------|-----------|-----------|----------------|----------------|----------------|------------|-----------|-----------|----------------|
| 2. X-Accl. | 2. Y-Accl | 2. Z-Accl | 2. X-Ang. Vel. | 2. Y-Ang. Vel. | 2. Z-Ang. Vel. | 2. X-Accl. | 2. Y-Accl | 2. Z-Accl | 2. X-Ang. Vel. | 
| 3. X-Accl. | 3. Y-Accl | 3. Z-Accl | 3. X-Ang. Vel. | 3. Y-Ang. Vel. | 3. Z-Ang. Vel. | 3. X-Accl. | 3. Y-Accl | 3. Z-Accl | 3. X-Ang. Vel. |
