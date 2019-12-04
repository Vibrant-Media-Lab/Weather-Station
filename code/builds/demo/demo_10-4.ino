/**
 * Arduino Weather Station
 * Demonstration Build: 4 October 2019
 * 
 * Funtionality -
 *  - Standard sensors
 *  - SD write
 *  - Serialized output
 */
// General Includes //
#include <math.h>
#include <SoftwareSerial.h>
#include <Ethernet.h>
#include <SD.h>
#include <ArduinoJson.h>

// Component Includes //
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Si7021.h>
#include <Adafruit_BMP280.h>

// Component Declarations //
SoftwareSerial airQualitySerial(14, 15);

Adafruit_Si7021 temperatureSensor = Adafruit_Si7021();

Adafruit_BMP280 pressureSensor;

#define rainRatePin (16)

#define windSpeedPin (18)
#define windHeadingPin (69)

#define SD_CARD_SS_PIN (4)

// Aliases and Definitions //
#define dataUpdateTime 10000
#define dataWriteTime 60000
#define dataUploadTime 360000

typedef struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
} airqualitydata_t;

SoftwareSerial aqSerial(9, 10);

typedef struct weatherdata_t {
  float temperature;
  float pressure;
  float humidity;
  uint16_t airQuality[6];
  float windSpeed;
  int windHeading;
  float rainRate;
  int rainRateBounce;
} weatherdata_t;

// Global Data Variables //
airqualitydata_t aq;
weatherdata_t data;

#define windHeadingOffset 0

int windInterruptCounter;
int windInterruptBounce;
int raininterruptCounter;

String serializedData, timeStamp;
File logFile;

// Timing //
unsigned long frameStart;
unsigned long frameLength;

long _dataUpdateTime;
long _dataWriteTime;
long _dataUploadTime;

// Flags //
bool hasSi;
bool hasBmp;
bool hasSD;
bool hasEnet;

// Network //
byte mac[] = {0x2c, 0x77, 0x68, 0xc4, 0x91, 0x85};
IPAddress ip(136, 142, 64, 212);
EthernetServer server(80);


void setup() {
  beginSerial();
  beginSD();
  //beginEnet();
  beginSensors();
  updateData();
}

void loop() {
  frameStart = millis();

  if(_dataUpdateTime <= 0) {
    updateData();
    serializeData(Serial);
    Serial.println("");
    _dataUpdateTime = dataUpdateTime;
  }

  if(_dataWriteTime <= 0) {
    if(hasSD) writeToSD();
    _dataWriteTime = dataWriteTime;
  }

  if(_dataUploadTime <= 0) {
    //if(hasEnet) uploadData();
    _dataUploadTime = dataUploadTime;
  }

  long delayTime = 1000 - (millis() - frameStart);
  if(delayTime > 0) delay(delayTime);
  //End of frame operations
  if(millis() >= frameStart) frameLength = millis() - frameStart;
  else frameLength = millis() + (unsigned long)(0x7FFFFFFF - frameStart);

  _dataUploadTime -= frameLength;
  _dataWriteTime -= frameLength;
  _dataUpdateTime -= frameLength;
}

// Initialize Components //

/**
 * Opens a serial output to the connected console, for debugging use. Busy waits until the
 * connection opens correctly.
 * Arguments: None
 * Return: None
 */
void beginSerial() {
  Serial.begin(9600);
  while(!Serial) {
    delay(10);
  }

  aqSerial.begin(9600);
  while(!aqSerial) {
  	delay(10);
  }
}

/**
 * Opens the onboard SD card storage and gets a reference to the datalog file. This reference
 * is stored in the global 'log' variable. If no SD is found or the file can't be opened, sets
 * the appropriate global flags to prevent SD access.
 * Arguments: None
 * Return: None
 */
void beginSD() {
  if(SD.begin(SD_CARD_SS_PIN)) {
    logFile = SD.open("log.txt", FILE_WRITE);
    hasSD = 1;
  } else {
    hasSD = 0;
  }
}

void beginEnet() {
  Ethernet.begin(mac, ip);
  server.begin();
}

/**
 * Finds the ethernet
 *
 *
 *
 */
void findEthernetClient() {
  EthernetClient client = server.available();
  if(client) {
    bool requestEnded = true;
    while(client.connected()) {
      if(client.available()) {
        char c = client.read();

        if(c == '\n' && requestEnded) {
          serializeData(client);
          break;
        }

        if(c == '\n') {
          requestEnded = true;
        } else if(c != '\r') {
          requestEnded = false;
        }
      }
    }

    delay(10);
    client.flush();
    client.stop();
  }
}

/**
 * Attemts to open attached sensors for future data measurement. Sets a series of global flags
 * based on which are present and which data metrics can be read.
 * Arguments: None
 * Return: None
 */
void beginSensors() {
	hasSi = temperatureSensor.begin();
	hasBmp = pressureSensor.begin();
}

// Updata Data fields //

/**
 * Updates the global data struct
 * Arguments: None
 * Return: None
 */
void updateData() {
  if(hasSi || hasBmp) updateTemperature();
  if(hasBmp) updatePressure();
  if(hasSi) updateHumidity();
  updateAirQuality();
  updateWindHeading();
  updateWindSpeed();
}

/**
 * Updates the global temperature data, using the Si7021 sensor if one exists and the BMP280
 * otherwise.
 * Arguments: None
 * Return: None
 */
void updateTemperature() {
  if(hasSi) data.temperature = temperatureSensor.readTemperature();
  else if(hasBmp) data.temperature = pressureSensor.readTemperature();
}

/**
 * Updates the global pressure data, using the BMP280 sensor
 * Arguments: None
 * Return: None
 */
void updatePressure() {
  data.pressure = pressureSensor.readPressure();
}

/**
 * Updates the global humidity data, using the Si7021 sensor
 * Arguments: None
 * Return: None
 */
void updateHumidity() {
  data.humidity = temperatureSensor.readHumidity();
}

/**
 * Updates the global air quality data, using the PM2.5 sensor
 * Arguments: None
 * Return: None
 */
void updateAirQuality() {
  if(readPMSdata(&aqSerial)) {
	   uint16_t tempData[6] = {aq.particles_03um, aq.particles_05um, aq.particles_10um, aq.particles_25um, 
                                aq.particles_50um, aq.particles_100um};
	 for(int i = 0; i < 6; i++) {
	      data.airQuality[i] = tempData[i];
	 }
	}
}

bool readPMSdata(Stream *s) {
  if (! s->available()) {
    return false;
  }
  
  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }
 
  // Now read all 32 bytes
  if (s->available() < 32) {
    return false;
  }
    
  uint8_t buffer[32];    
  uint16_t sum = 0;
  s->readBytes(buffer, 32);
 
  // get checksum ready
  for (uint8_t i=0; i<30; i++) {
    sum += buffer[i];
  }
 
  /* debugging
  for (uint8_t i=2; i<32; i++) {
    Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
  }
  Serial.println();
  */
  
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }
 
  // put it into a nice struct :)
  memcpy((void *)&aq, (void *)buffer_u16, 30);
 
  if (sum != aq.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
}

/**
 * Updates the global wind heading data, using the Davis anemometer
 * Arguments: None
 * Return: None
 */
void updateWindHeading() {
  int rawHeading = analogRead(windHeadingPin);
  int mappedHeading = map(rawHeading, 0, 1023, 0, 360);
  int actualHeading = (mappedHeading + windHeadingOffset) % 360;
  if(actualHeading < 0) actualHeading += 360;
  data.windHeading = actualHeading;
}

/**
 * Updates the global wind speed data, using the Davis wind speed gauge. Blocks for 1s.
 * Arguments: None
 * Return: None
 */
void updateWindSpeed() {
  windInterruptCounter = 0;
  
  pinMode(windSpeedPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(windSpeedPin), isr_rotation, FALLING);
  
  int i = 1000;
  while(i >= 0) {
    unsigned long start = millis();
    i -= (millis() - start);
  }
  
  detachInterrupt(digitalPinToInterrupt(windSpeedPin));
  data.windSpeed = windInterruptCounter * .75;
}

/**
* On interupt, increments the wind interrupt rotation count
* Arguments: None
* Return: None
*/
void isr_rotation() {
  if(millis() - windInterruptBounce > 15) {
    windInterruptCounter++;
    windInterruptBounce = millis();
  }
}

// Data Packaging and Output //

/**
 * Write json data out to the onboard storage (SD Card)
 * Arguments: None
 * Return: None
 */
void writeToSD() {
  serializeData(logFile);
  logFile.println("");
  logFile.flush();
}

/**
 * Write json data out through the ethernet connection
 * to an external webClient
 * Arguments: None
 * Return: None
 */
void uploadData() {

}

/**
 * Serialize the current weather data, overwriting or updating the currrent
 * global json object as necessary.
 * Arguments: Where - the stream to write to
 * Return: None
 */
void serializeData(Print& where) {
	int capacity = JSON_ARRAY_SIZE(6) + 7*JSON_OBJECT_SIZE(1);
	DynamicJsonDocument doc(capacity);

	doc["temperature"] = data.temperature;
	doc["pressure"] = data.pressure;
	doc["humidity"] = data.humidity;

	JsonArray airQuality = doc.createNestedArray("air quality");
	for(int i = 0; i < 6; i++) {
		airQuality.add(data.airQuality[i]);
	}

	doc["wind speed"] = data.windSpeed;
	doc["wind heading"] = data.windHeading;
	doc["rain rate"] = data.rainRate;
	doc["rain rate bounce"] = data.rainRateBounce;

	serializeJson(doc, where);
}