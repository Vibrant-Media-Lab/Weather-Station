#include <Wire.h> //For I2C Communications

#include <Adafruit_Si7021.h>  //Temp and RH
#include <Adafruit_BMP280.h>  //Temp and Pressure

#define DATA_UPDATE_TIME 10000
#define NaN 0xFFFF

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
long _dataUpdateTime = 0;

unsigned long cycleStart;
unsigned long cycleDuration;


//Data//

float temperature;
float humidity;
float pressure;

void setup() {
  Serial.begin(9600);
  while(!Serial) { 
    delay(10);
  }
  Serial.println("Serial open. Searching for sensor array...");
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
  
  
  endCycle();
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
}

void updateData() {
  if(hasSi && hasBmp) {
    temperature = si.readTemperature();
    humidity = si.readHumidity();
    pressure = bmp.readPressure();
  } else if(!hasSi && hasBmp) {
    temperature = bmp.readTemperature();
    humidity = NaN;
    pressure = bmp.readPressure();
  } else if (hasSi && !hasBmp) {
    temperature = si.readTemperature();
    humidity = si.readHumidity();
    pressure = NaN;
  }
}

void displayData() {
  if(temperature != NaN) {
    Serial.print(F("Temperature = "));
    Serial.print(temperature);
    Serial.println(" *C");
  }
  if(humidity != NaN) {
    Serial.print(F("Relative Humidity = "));
    Serial.print(humidity);
    Serial.println(" %");
  }
  if(pressure != NaN) {
    Serial.print(F("Pressure = "));
    Serial.print(pressure);
    Serial.println(" Pa");
  }
}
