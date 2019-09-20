/**
 * Component Test: SD CARD
 * Arduino Weather Station
 * Pitt Vibrant Media Lab
 * Author: Josh Arabia
 */

#include <SD.h>
#include <SPI.h>

#define SDCARD_SS_PIN 4

File _config;
File _logger;
File _debug;


void setup() {
  Serial.begin(9600);
  while(!Serial) {
    delay(10);        // Since serial must be open to function, busy wait
  }

  Serial.println("Reinitializing SD Card...");
  openStandardFiles();
  Serial.println("Recording configuration...");
  //recordConfig();   // Optional writeout of sensor models and available storage space
}

void loop() {
  
}

void openStandardFiles() {
  if(SD.exists("_config.log")) SD.remove("_config.log");
    _config = SD.open("_config.log", FILE_WRITE);
  if(SD.exists("_logger.log")) SD.remove("_logger.log");
    _logger = SD.open("_logger.log", FILE_WRITE);
  if(SD.exists("_debug.log")) SD.remove("debug.log");
    _debug = SD.open("_debug.log", FILE_WRITE);
}

void recordConfig() {
  // Unimplemented
}

void writeDebug(char* buf, int len) {
  updateTimestamp();
  _debug.print(timestamp);
  _debug.write(buf, len);
}
