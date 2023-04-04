# Bicycle Sensor Database

This is a collection of sensor data, that was gathered while riding a bicycle. This document serves as a description on how to use the data, what to expect from it and how it can be reproduced. The different sensors are:
- The IMU of an Arduino Nano BLE Sense
- GPS Coordinates of an external GPS Module (Neo 6M)
- GPS Coordinates of a mobile phone, recorded via PhyPhox
- A video, which was filmed using a second mobile phone



## Example Usage
The readings are saved in different .csv files. At the start of the recording, the rear of the bicycle was lifted up several times, in order create a synchronization reference. It is clearly visible in both the video, as well as the IMU data. 
### IMU and Neo-6M GPS data:
The structure of the IMU and Neo-6M is as follows:
| Timestamp (ms) | X-Accl. (g) | Y-Accl (g) | Z-Accl. (g) | X-Ang. Vel. (°/s) | Y-Ang. Vel. (°/s) | Z-Ang. Vel. (°/s) | Lat. (°)| Lon. (°) | Alt. (m) | 
|------------|-----------|-----------|----------------|----------------|----------------|------------|-----------|-----------|----------------|
| 1. Timestamp | 1. X-Accl. | 1. Y-Accl | 1. Z-Accl. | 1. X-Ang. Vel. | 1. Y-Ang. Vel. | 1. Z-Ang. Vel. | 1. Lat. | 1. Lon. | 1. Alt. | 
| 2. Timestamp | 2. X-Accl. | 2. Y-Accl | 2. Z-Accl. | 2. X-Ang. Vel. | 2. Y-Ang. Vel. | 2. Z-Ang. Vel. | 2. Lat. | 2. Lon. | 2. Alt. | 
etc...

In the `images` folder, the image `accl` and `gyro` describe the direction of the axes. An arrow indicates a positive value. You can also refer to the official [documentation](https://docs.arduino.cc/tutorials/nano-33-ble/imu-gyroscope).

**Note:** The file starts with a header, describing each column. The next rows represent the gathered samples. 

### Mobile Phone GPS data:
The GPS data from the mobile phone serves as a reference, in order to judge the accuracy of the external GPS Module. It is also saved as a .csv file, with the following structure:

| Timestamp (s) | Lat. (°) | Lon. (°) | Alt. (m) | Velocity (m/s) | Direction (°) | Horizontal Acc. (m) | Vertical Acc. (°) | 
|------------|-----------|-----------|----------------|----------------|----------------|------------|-----------|
| 1. Timestamp | 1. Lat. | 1. Lon.  | 1. Alt.  | 1. Velocity  | 1. Direction  | 1. Horizontal Acc.  | 1. Vertical Acc.  | 
| 2. Timestamp | 2. Lat. | 2. Lon.  | 2. Alt.  | 2. Velocity  | 2. Direction  | 2. Horizontal Acc.  | 2. Vertical Acc.  |
etc...

**Note:** The mobile phone GPS recording unfortunately stopped a little before the actual recording stopped. Since it is just a reference, we accepted this error, but it needs to be kept in mind.


### Usage in Python:
The files can either be loaded individually with something like `NumPy` or `Pandas`:

``` python
data = np.loadtxt("ArduinoLog.csv", delimiter=",")
```
Or they can be loaded into a Dictionary, labeled by their filename.
``` python
import os
import pandas as pd

dataset = dict()
dataset = pd.read_csv("ArduinoLog.csv", header=None).to_numpy()
```
Note that these are just two exemplary methods of loading the data. Depending on the intented usage, different methods may be used.

### Video recording:
The recorded video is saved as a .mp4 file, which was compressed to HD. It can be used to describe the current behavior accurately.




## Measurement Setup:
The four sensors are controlled by different devices. This section explains, how each sensor was controlled and how they are connected to each other. A few images are also uploaded to the `images` folder.

### IMU and Neo-6M GPS Module:
#### **Hardware:**
The [IMU](https://content.arduino.cc/assets/Nano_BLE_Sense_lsm9ds1.pdf) is integrated on the [Arduino Nano 33 BLE Sense](https://docs.arduino.cc/resources/datasheets/ABX00031-datasheet.pdf). The external [Neo 6M GPS Module](https://content.u-blox.com/sites/default/files/products/documents/NEO-6_ProductSummary_%28GPS.G6-HW-09003%29.pdf) is connected via SPI to the Arduino. Since the non-volatile memory of the Arduino is not very large, a [SD-Card Module](https://lastminuteengineers.com/arduino-micro-sd-card-module-tutorial/) was used, which enables to directly store the measured samples for extended periods of time onto a SD-Card. Everything got connected by using a breadboard. If frequent measurements are desired, we would personally recommend creating a dedicated board, but for now, this quick breadboard setup was more convenient.

The SD-Card Module should theoretically work with a supply voltage of 3.3 V, however while using 3.3 V SD-Cards always failed to be detected. Therefore, 5 V supply voltage was provided. We tried to use the 5 V output of the Arduino Nano. This also did not work, as we saw a significant voltage drop when powering on the SD-Card Module. 
The required 5V were provided in a different way. We used a powerbank, which contains two USB-A outputs. One of them was used to power the Arduino, which also powered the Neo 6M. The second one was exclusively used for the SD-Card Module.

The connections are as follows:

| Arduino Nano BLE Sense | Neo 6M GPS | SD-Card Module | External 5 V source |
|------------|-----------|-----------|-----------|
| 3.3 V | VCC | - | - |
| - | - | VCC | 5 V |
| GND | GND | GND | GND |
| D2 | TX | - | - |
| D3 | RX | - | - |
| D12 | - | MOSO | - |
| D11 | - | MOSI | - |
| D13 | - | SCK | - |
| D10 | - | CS | - |


**Note:** The pins do not need to be these exact ones, some can be changed in the code.  

This breadboard setup got tightly mounted on the bicycle rack with cable ties. 

#### **Software:**
The software used on the Arduino can be found in the `ArduinoCode` folder. At the top of the file the pins are defined, which may be changed if necessary. Also, depending on the used Arduino, you may need to change the internal IMU and include a different file. Otherwise, the internal RGB LED is used to communicate the current state:
- **Red LED:** Indicates an error related to the SD-Card. Check the wiring and if a Card is present or try to upload a standalone SD-Card test, such as the `CardInfo.ino`, included in the SD library.
- **Blue LED:** The SD-Card was initialized successfully, however, the recieved values from the GPS Module are not valid yet. The IMU data is still saved to the SD-Card.
- **Green LED:** Indicates valid GPS values, as well as a successful connection to the SD-Card. 

If connected to a computer, the code also prints the current status to the serial monitor.

A list of all required libraries is provided here:
- `Arduino Nano BLE Sense`
- `LSM9DS1` (IMU Sensor)
- `TinyGPS++` (Interpreting GPS Nema Code)
- `SD` (Arduino standard SD library)


### Mobile Phone GPS Setup:
For the GPS reference values, recorded with a mobile phone, the app [phyphox](https://phyphox.org/) was used. It is available for free for both iOS and Android devices. It is maintained by the [RWTH Aachen University](https://www.rwth-aachen.de/go/id/a/?lidx=1). 

### Mobile Phone video recording:
The mobile phone video recording was filmed by using a holder, which was attachted to the bicycle handle. The video was filmed at 30fps in FullHD, due to the file size it was compressed to HD. It is available in the Seafile Folder, which can be accessed by requesting permission. 
