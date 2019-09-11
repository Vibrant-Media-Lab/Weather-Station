/**
 * Custom test protocol for the Arduino Weather Station
 * 
 * Pitt Vibrant Media Lab
 * Josh Arabia
 */

#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {0xe2, 0x6d, 0x7f, 0xd9, 0xd5, 0x4c};
IPAddress ip(136, 142, 64, 175);

IPAddress server(74, 125, 232, 128); //Google.com

EthernetClient client;

long readTime = 10000;
long startIterTime;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial){
    ;
  }

  Ethernet.begin(mac, ip);
  delay(1000);
  if(client.connect(server, 80)) {
    Serial.print("Connection established: ");
    Serial.println(client.remoteIP());
  } else {
    Serial.println("Connection failed...exiting");
    return;
  }

  client.println("GET /search?q=clouds HTTP/1.0");
  client.println();
}

void loop() {
  if(readTime <= 0) return;

  startIterTime = millis();
  
  int len = client.available();
  if (len > 0) {
    byte buf[100];
    len = (len > 100) ? 100 : len;
    client.read(buf, len);
    Serial.write(buf, len);
    Serial.println();
  }

  readTime -= millis() - startIterTime;
}
