/**
 * HTTP Request Parsing Test
 * Arduino Weather Station
 * University of Pittsburgh Vibrant Media Lab
 * Josh Arabia
 */
#include <Ethernet.h>
#include <SPI.h>


byte mac[] = {0xf2, 0x02, 0x56, 0x01, 0x78, 0xc8};
IPAddress ip(136,142,64,180);

IPAddress server(136,124,64,174);
//char server[] = "localhost:8000";

EthernetClient client;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial) {
    ;
  }

  Ethernet.begin(mac, ip);

  if(client.connect(server, 80)) {
    Serial.println("connected");
    //Serial.println("client sending: 'PUT /test_text'");
    client.println("http://localhost:8000/hello.php?var=test_text");
  } else {
    Serial.println("connection failed...exiting");
    return;
  }
}

void loop() {
  return;
}
