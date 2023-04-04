/* This code saves the internal IMU and external GPS values to a SD-Card.

The internal IMU and external GPS, connected via GPS_RX_PIN and GPS_TX_PIN are saved to an SD-Card. 
The standard SD Library by Arduino is used, which may be improved in the future by using the faster SD-Fat library.
The loop contiously reads the GPS SPI connection and once the timer has increased by UpdateTime milliseconds, both gps and IMU are written to the SD-Card.
Note that, at the current moment, writing to the SD-Card and reading the Serial.connection takes roughly 500ms, that means, that a UpdateTime below that is not possible.
This might be solved by using the SD-Fat Library, or by first writing to a buffer, which is then written to the SD-Card in defined periods, reducing the amount of accesses on the SD-Card.
*/


/*
* Arduino Nano 33 BLE Sense saves IMU and external GPS to SD-Card*
* Author: Patric Mueller */
#include <Arduino.h>
#include "wiring_private.h"
#include <Arduino_LSM9DS1.h>
#include "TinyGPS++.h"
#include <SPI.h>
#include <SD.h>

//-----------------------------------------------------------------------------
// Adjustable Settings by user:

// SD Card Chip Select Pin
const int chipSelect = 10;

// GPS Pins
#define GPS_RX_PIN 3 // which pin is connected to the GPS RX PIN?
#define GPS_TX_PIN 2 // which pin is connected to the GPS TX PIN?

// LED Pins:
// LED
#define RED 22     
#define BLUE 24     
#define GREEN 23


// Update Period Time in ms
int UpdateTime = 200;
int subroutineTime = 6000;
//------------------------------------------------------------------------------



// create the tinygps object globally
TinyGPSPlus gps;

// create a software Global UART connection on Pin GPS_TX_PIN and GPS_RX_PIN
UART mySerial1(digitalPinToPinName(GPS_RX_PIN), digitalPinToPinName(GPS_TX_PIN), NC, NC);

void GPSInit(){
  mySerial1.begin(9600);
}


// Accelerometer
float Ax, Ay, Az;
float Gx, Gy, Gz;

// GPS
float LAT, LNG, ALT;
bool Valid;

// Timestamp
float Timestamp;

// Write Array:
float Arr[11];






void ValidGPS(bool Valid){
  // check if GPS is valid, toggle LED if it is or is not  
  if (Valid == true){
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, HIGH);
  }
  else{
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, LOW);
  }
}

void WriteCSV(String filename, String dataString){
  
  File dataFile = SD.open(filename, FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    //Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening file");
    digitalWrite(BLUE, HIGH);
    digitalWrite(GREEN, HIGH);
    digitalWrite(RED, LOW);
  }
}


String AssembleString(float* Arr){
  // create String, which represents one row
  String dataString = "";
  // hardcoded for now
  for (int i=0; i<10; i++){
    dataString += String(Arr[i], 10);
    if (i<9){
      dataString += ",";  
    }
  }
  return dataString;
}



void setup() {
  // Start serial, for debugging
  Serial.begin(9600);
  //Started, wait for Serial to begin
  //while(!Serial);
  Serial.println("\n---Start ---");
  
  // IMU Init
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  // GPS init
  GPSInit();

  // LED Init
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  // set all LEDs to HIGH, which actually turns them off
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(RED, HIGH);
  digitalWrite(BLUE, HIGH);
  digitalWrite(GREEN, HIGH);
  delay(1500);
  // Init SD Card:
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    digitalWrite(RED, LOW);
    while (1);
  }
  Serial.println("card initialized.");
  digitalWrite(LED_BUILTIN, LOW);
  


  // Create a logging file, and write the first (header) row
  // first open it:
  File dataFile = SD.open("log.csv", FILE_WRITE);


  // if the file is available, write to it:
  if (dataFile) {
    String headerString =  "Time, Ax, Ay, Az, Gx, Gy, Gz, LAT, LNG, ALT";
    dataFile.println(headerString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(headerString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening file.");
  }


  

}


void loop() {
  digitalWrite(BLUE, LOW);

  while(1){
  // constantly feed the gps object
  while (mySerial1.available() > 0){
    int currentTime = millis();
    
    gps.encode(mySerial1.read());
    currentTime = millis();


    // perform other actions only in defined periods
    if(currentTime > subroutineTime){
      // IMU data
      if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(Arr[1], Arr[2], Arr[3]);
      }
        
      if (IMU.gyroscopeAvailable()) {
        IMU.readGyroscope(Arr[4], Arr[5], Arr[6]);
      }
      
      // GPS Data
      LAT = gps.location.lat();
      LNG = gps.location.lng();
      ALT = gps.altitude.meters();
      // save to big string
      Arr[7] = LAT;
      Arr[8] = LNG;
      Arr[9] = ALT;
      
      Valid = gps.location.isValid(); 

      // Call function, to interpret Valid boolean
      // While gps values are not VALID, the blue LED will be on
      // Once they are VALID, the Green LED will be on
      ValidGPS(Valid);
      
      // Get current Timestamp:
      Arr[0] = millis();

      // Finally, assemble string and save to SD
      String dataString = AssembleString(Arr);
      Serial.println(dataString);


      // Write to file:
      // If at any time an error occurs related to the SD Module, the red LED will be on
      WriteCSV("log.csv", dataString);

      
      
      // increase subroutineTime by the defined period time
      subroutineTime = millis() + UpdateTime;
    }
  }
  }
}
