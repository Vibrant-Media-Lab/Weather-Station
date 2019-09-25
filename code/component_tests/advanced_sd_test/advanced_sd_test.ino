/**
 * Component Test: SD CARD (Advanced Functionality)
 * Arduino Weather Station
 * Pitt Vibrant Media Lab
 * Author: Josh Arabia
 */

#include <SD.h>
#include <SPI.h>

#include <Wire.h> //For I2C Communications

#include <Adafruit_Si7021.h>  //Temp and RH
#include <Adafruit_BMP280.h>  //Temp and Pressure

#define SDCARD_SS_PIN 4
#define DATA_UPDATE_TIME 1000
#define DATA_LOG_TIME 10000


#define TEMP_ID 't'
#define HUM_ID 'h'
#define PRES_ID 'p'

File _config;
File _logger;
File _debug;

//Sensors//
Adafruit_Si7021 si = Adafruit_Si7021();
Adafruit_BMP280 bmp;

//Flags//
bool hasSi;
bool hasBmp;

bool recordingTmp;
bool recordingRH;
bool recordingBP;

//Timing//
long _dataUpdateTime = DATA_UPDATE_TIME;
long _dataLogTime = DATA_LOG_TIME;

unsigned long cycleStart;
unsigned long cycleDuration;


//Data//

float temperature;
float pressure;
float humidity;


void setup() {
  Serial.begin(9600);
  while(!Serial) {
    delay(10);        // Since serial must be open to function, busy wait
  }

  Serial.println("Reinitializing SD Card...");
  SD.begin(SDCARD_SS_PIN);
  openStandardFiles();
  Serial.println("Locating Sensors...");
  getSensors();
}

void loop() {
  startCycle();
  delay(10);
  if(_dataUpdateTime <= 0) {
    updateData();
    _dataUpdateTime = DATA_UPDATE_TIME;
    displayData();
  }
  if(_dataLogTime <= 0) {
    logData();
    _dataLogTime = DATA_LOG_TIME;
  }
  
  
  endCycle();
}

void startCycle() {
  cycleStart = millis();
}

void endCycle() {
  if(millis() > cycleStart) {
    cycleDuration = millis() - cycleStart;
  } else {  //Account for possible overflow of millis
    cycleDuration = millis() + (0xFFFFFFFF - cycleStart);
  }

  _dataUpdateTime -= cycleDuration;
  _dataLogTime -= cycleDuration;
}

void openStandardFiles() {
    //_config = SD.open("_config.log", FILE_WRITE);
    //if(_config) Serial.println("Opened config");

    if(SD.exists("_logger.log")) SD.remove("_logger.log");
    _logger = SD.open("_logger.log", FILE_WRITE);
    if(_logger) Serial.println("Opened logger");
    
    //_debug = SD.open("_debug.log", FILE_WRITE);
    //if(_debug) Serial.println("Opened debug");
}

void getSensors() {
  if(si.begin()) {
    Serial.print("Found Si7021: ");
    Serial.println(si.getModel());
    recordingTmp = true; recordingRH = true;
    hasSi = true;
  } else {
    Serial.println("No Si7021 sensor detected");
    hasSi = false;
  }

  if(bmp.begin()) {
    Serial.println("Found BMP280");
    hasBmp = true;
  } else {
    Serial.println("No BMP280 sensor detected");
    hasBmp = false;
  }
  
}

void setFlags() {
  recordingTmp = hasSi || hasBmp;
  recordingRH = hasSi;
  recordingBP = hasBmp;
}

void updateData() {
  if(hasSi && hasBmp) {
    temperature = si.readTemperature();
    humidity = si.readHumidity();
    pressure = bmp.readPressure(); 
  } else if(!hasSi && hasBmp) {
    temperature = bmp.readTemperature();
    pressure = bmp.readPressure();
  } else if (hasSi && !hasBmp) {
    temperature = si.readTemperature();
    humidity = si.readHumidity();
  }
}

void displayData() {
  if(recordingTmp) {
    Serial.print(F("Temperature = "));
    Serial.print(temperature);
    Serial.println(" *C");
  }
  if(recordingRH) {
    Serial.print(F("Relative Humidity = "));
    Serial.print(humidity);
    Serial.println(" %");
  }
  if(recordingBP) {
    Serial.print(F("Air Pressure = "));
    Serial.print(pressure);
    Serial.println(" Pa");
  }
}

void logData() {
  Serial.println("Logging data to SD Card");
  
  _logger.print(millis()); _logger.print(":");
  delay(100);
  
  if(recordingTmp) {
    _logger.print(TEMP_ID); _logger.print(temperature); _logger.print(":");
    delay(100);
  }
  if(recordingRH) {
    _logger.print(HUM_ID); _logger.print(humidity); _logger.print(":");
    delay(100);
  }
  if(recordingBP) {
    _logger.print(PRES_ID); _logger.print(pressure); _logger.print(":");
    delay(100);
  }

  _logger.println("");
  _logger.flush();
}
