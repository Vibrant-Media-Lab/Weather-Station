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

#include <Adafruit_Si7021.h>
#include <Adafruit_BMP280.h>

// Component Declarations //
SoftwareSerial airQualitySerial(10, 11);
Adafruit_Si7021 temperatureSensor = Adafruit_Si7021();
Adafruit_BMP280 pressureSensor;
#define rainRatePin (16)
#define windSpeedPin (18)
#define windHeadingPin (A15)

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


void setup() {
  
}

void loop() {

}

// Updata Data fields //

/**
 * Updates the global data struct
 * Arguments: None
 * Return: None
 */
void updateData() {
  updateTemperature();
  updatePressure();
  updateHumidity();
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
   uint16_t tempData[6] = {aq.particles_03um, aq.particles_05um, aq.particles_10um, aq.particles_25um, 
                        aq.particles_50um, aq.particles_100um};
   for(int i = 0; i < 6; i++) {
      data.airQuality[i] = tempData[i];
   }
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
  
  sei();      // Begin critical section
    delay(1000);
  cli();      // End critical section
  
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
