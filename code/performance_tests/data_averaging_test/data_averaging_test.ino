#include <Wire.h> //For I2C Communications

#include <Adafruit_Si7021.h>  //Temp and RH
#include <Adafruit_BMP280.h>  //Temp and Pressure

#define DATA_UPDATE_TIME 1000
#define NaN 0xFFFF
#define ROLLING_AVERAGE_UPDATE_CYCLES 10

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
float avgTemperature;
float humidity;
float avgHumidity;
float pressure;
float avgPressure;

float cyclesAveraged = 1;

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
    if(++cyclesAveraged > ROLLING_AVERAGE_UPDATE_CYCLES) {
      displayAveragedData();
      cyclesAveraged = 1;
    }
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
    avgTemperature = expMovAvg(avgTemperature, temperature, cyclesAveraged);
    
    humidity = si.readHumidity();
    avgHumidity = expMovAvg(avgHumidity, humidity, cyclesAveraged);
    
    pressure = bmp.readPressure();
    avgPressure = expMovAvg(avgTemperature, temperature, cyclesAveraged);  
  } else if(!hasSi && hasBmp) {
    temperature = bmp.readTemperature();
    avgTemperature = expMovAvg(avgTemperature, temperature, cyclesAveraged);
    
    humidity = NaN;
    
    pressure = bmp.readPressure();
    avgPressure = expMovAvg(avgPressure, pressure, cyclesAveraged);
  } else if (hasSi && !hasBmp) {
    temperature = si.readTemperature();
    avgTemperature = expMovAvg(avgTemperature, temperature, cyclesAveraged);
    
    humidity = si.readHumidity();
    avgHumidity = expMovAvg(avgHumidity, humidity, cyclesAveraged);
    
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
    Serial.print(F("Air Pressure = "));
    Serial.print(pressure);
    Serial.println(" Pa");
  }
}

void displayAveragedData() {
  if(temperature != NaN) {
    Serial.print(F("Temperature Rolling Average - Last 10 Cycles: "));
    Serial.print(avgTemperature);
    Serial.println(" *C");
    avgTemperature = temperature;
  }
  if(humidity != NaN) {
    Serial.print(F("Humidity Rolling Average - Last 10 Cycles: "));
    Serial.print(avgHumidity);
    Serial.println(" %");
    avgHumidity = humidity;
  }
  if(pressure != NaN) {
    Serial.print(F("Air Pressure Rolling Average - Last 10 Cycles: "));
    Serial.print(avgPressure);
    Serial.println(" Pa");
    avgPressure = pressure;
  }
}

/*
 * Calculates the exponential moving average of the metric, which weights the newest elements
 * of the set more significantly and reduces the effect of large data sets on precision.
 */
float expMovAvg(float avg, float entry, byte cycles) {
  avg -= avg/cycles;
  avg += entry/cycles;
  return avg;
}
