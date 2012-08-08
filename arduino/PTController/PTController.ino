

#include <DynamixelSerial.h>
#include <SPI.h>
#include <Ethernet.h>

class DynamixelSerial1 : public DynamixelSerial {
    void sendData(unsigned char d) {;}
    int availableData(void) {return 0;}
    int readData(void) {return 0;}
    int peekData(void) {return 0;}
    void beginCom(long d) {;}
    void endCom(void) {;}
};


byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x79, 0xDF };
IPAddress ip(192,168,1,60);
const int ledPin = 8;

DynamixelSerial1 pantilt;
EthernetServer server(23);
boolean gotAMessage = false; // whether or not you got a message from the client yet

void setup() {
  pinMode(ledPin, OUTPUT);
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.begin(9600);
}

void loop() {
  // wait for a new client:
  EthernetClient client = server.available();
  
  // when the client sends the first byte, say hello:
  if (client) {
    if (!gotAMessage) {
      Serial.println("We have a new client");
      client.println("Hello, client!"); 
      gotAMessage = true;
    }
    
    // read the bytes incoming from the client:
    char thisChar = client.read();
    // echo the bytes back to the client:
    server.write(thisChar);
    // echo the bytes to the server as well:
    Serial.print(thisChar);
    
    if (thisChar == 'H') {
      digitalWrite(ledPin, HIGH);
    }
    if (thisChar == 'L') {
      digitalWrite(ledPin, LOW);
    }
    
  }
}
