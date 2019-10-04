#include <ArduinoJson.h>

typedef struct testStuct {
  float temp;
  float aq[6];
  String heading;
} testStruct;

testStruct test;

void setup() {
  Serial.begin(9600);
  while(!Serial) {
    delay(10);
  }
  
  test = {
            45.2,
            {1, 2, 3, 4, 5.6, 7},
            "NW"
         };

         DynamicJsonDocument testDoc(200);
         testDoc["temp"] = test.temp;
         testDoc["aq"] = test.aq;
         testDoc["head"] = test.heading;

         serializeJson(testDoc, Serial);
}

void loop() {
  delay(100);
}
