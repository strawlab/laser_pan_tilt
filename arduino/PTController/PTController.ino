

#include <DynamixelSerial.h>
#include <SPI.h>
#include <Ethernet.h>

class DynamixelSerialPTC : public DynamixelSerial {
    void sendData(unsigned char d) {
      Serial1.write(d);
//      Serial.write(d);  // write to other serial port for debugging
      Serial1.flush();
    }
    int availableData(void) {return Serial1.available();}
    int readData(void) {return Serial1.read();}
    int peekData(void) {return Serial1.peek();}
    void beginCom(void) {Serial1.begin(57600);}
    void endCom(void) {Serial1.end();}
};


byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x79, 0xDF };
IPAddress ip(192,168,1,60);

const int LED_SERVO   = 8;
const int LED_COMMAND = 7;
const int PTC_COMM_DIR_PIN = 2;

DynamixelSerialPTC pantilt;
EthernetServer server(23);

void setup() {
  pinMode(LED_SERVO, OUTPUT);
  pinMode(LED_COMMAND, OUTPUT);

  Ethernet.begin(mac, ip);
  server.begin();

  Serial.begin(57600);
  
  pantilt.begin(PTC_COMM_DIR_PIN);
}

void loop() {
  static boolean clientConnected = false;
  static int pos = 0;
  int ok;

  EthernetClient client = server.available();
  if (client) {
    if (!clientConnected) {
      Serial.println("Client Connected");
      clientConnected = true;
    }
    
    if (client.available()) {
      char c = client.read();

      //toggle activity LED
      digitalWrite(LED_COMMAND, digitalRead(LED_COMMAND) ? LOW : HIGH);
    
      if (c == 'H') {
        digitalWrite(LED_SERVO, HIGH);
      } else if (c == 'L') {
        digitalWrite(LED_SERVO, LOW);
      } else if (c == 'M') {
        ok = pantilt.move(1,pos);
        pos += 10;
      } else if (c == 'Z') {
        unsigned char buf[] = {0xFF,0xFF,0x01,0x05,0x03,0x1E,0xFF,0x0F,0xCA};
        digitalWrite(PTC_COMM_DIR_PIN,1);
        Serial1.write(buf, sizeof(buf));
        Serial1.flush();
        delayMicroseconds(400);
        digitalWrite(PTC_COMM_DIR_PIN,0);
      }
      Serial.print("CMD: ");
      Serial.print(c);
      Serial.print(" RES: ");
      Serial.print(ok);
      Serial.print("\r\n");
    }
  }
}

