/**
 * Test program for the ethernet functionality of the Weather Station unit.
 * 
 * Author: Josh Arabia
 * Created: 5 September 2019
 */


#include <Ethernet.h>
#include <SD.h>

byte mac[] = { 0x57, 0xd8, 0xc6, 0x84, 0x74, 0xbb };
byte ip[] = { 136, 142, 64, 190 };

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac ,ip);
  displayConnectionValues();
}

void loop() {
  switch(Ethernet.maintain()) {
    case 0:
      Serial.println("Nothing Happened");
      break;
    case 1:
      Serial.println("Renew Failed");
      break;
    case 2:
      Serial.println("Renew Success");
      break;
    case 3:
      Serial.println("Rebind Fail");
      break;
    case 4:
      Serial.println("Rebind Success");
      break;
  }
  
  displayConnectionValues();
  delay(600000);
}

void displayConnectionValues() {
  Ethernet.MACAddress(mac);
  Serial.print("mac_address: ");
  for(int i = 0; i < 6; i++) {
    Serial.print(mac[i], HEX);
    if(i == 5) {
      Serial.print("\n");
    } else {
      Serial.print("-");
    }
  }
  Serial.print("local_ip: ");
  Serial.println(Ethernet.localIP());
  Serial.print("gateway_ip: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("subnet_mask: ");
  Serial.println(Ethernet.subnetMask());
}
