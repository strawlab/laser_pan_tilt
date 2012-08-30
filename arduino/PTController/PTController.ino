// vi: set shiftwidth=2 tabstop=2 expandtab:

#include <DynamixelSerial.h>
#include <Ethernet.h>

#include "PTCConfig.h"

class DynamixelSerialPTC : public DynamixelSerial {
  void sendData(unsigned char d) {
    Serial1.write(d);
#if PTC_DEBUG_DYNAMIXEL
    Serial.write(d);
#endif
    Serial1.flush();
  }
  int availableData(void) {return Serial1.available();}
  int readData(void)      {return Serial1.read();}
  int peekData(void)      {return Serial1.peek();}
  void beginCom(void)     {Serial1.begin(PTC_SERVO_BAUD);}
  void endCom(void)       {Serial1.end();}
};

DynamixelSerialPTC  pantilt;
EthernetServer      server(PTC_TCP_PORT);

void setup() {
  pinMode(PTC_LED_STATUS, OUTPUT);
  pinMode(PTC_LED_COMMAND, OUTPUT);
  pinMode(PTC_PIN_LASER, OUTPUT);

  Ethernet.begin(ptc_mac, ptc_ip);
  server.begin();

#if PTC_DEBUG_DYNAMIXEL || PTC_DEBUG
  Serial.begin(PTC_DEBUG_SERIAL_BAUD);
#endif
  
  pantilt.begin(PTC_COMM_DIR_PIN);
}

uint8_t ethernet_parse(EthernetClient &client)
{
    int i = 0;
    uint8_t ok = 0;

    //read all the data from the tcp packet
#if PTC_DEBUG
    Serial.print(" RX ");
#endif
    while (client.available() && client.connected()) {
      int c = client.read();
      if (c == -1)
        break;

      ptc_rxbuf[i++] = c;
#if PTC_DEBUG
      Serial.print(c);
      Serial.print(' ');
#endif
      if (i == PTC_RXBUF_LEN) {
        ok = 1;
        break;
      }
    }
#if PTC_DEBUG
    Serial.print("\r\n");
#endif

    return ok;
}

uint8_t command_parse(uint8_t &cmd, uint8_t &type, uint16_t &val)
{
  if ((ptc_rxbuf[PTC_RXBUF_START_IDX] == PTC_RXBUF_START) && \
      (ptc_rxbuf[PTC_RXBUF_END_IDX] == PTC_RXBUF_END)) {
    cmd = ptc_rxbuf[PTC_RXBUF_CMD_IDX];
    type = ptc_rxbuf[PTC_RXBUF_TYPE_IDX];
    val = *((uint16_t *)(&ptc_rxbuf[PTC_RXBUF_DATA_L_IDX]));
    return 1;
  }
  return 0;
}

void loop() {
  uint8_t rxok = 0;

  EthernetClient client = server.available();
  if (client) {
    rxok = ethernet_parse(client);
  }

  if (rxok) {
    uint8_t cmd, type;
    uint16_t val;
    int cmdok;

    cmdok = command_parse(cmd, type, val);
    if (cmdok) {
      //toggle activity LED
      digitalWrite(PTC_LED_COMMAND, digitalRead(PTC_LED_COMMAND) ? LOW : HIGH);
      switch (cmd) {
        case 'S':
          digitalWrite(PTC_LED_STATUS, HIGH);
          break;
        case 's':
          digitalWrite(PTC_LED_STATUS, LOW);
          break;
        case 'L':
          digitalWrite(PTC_PIN_LASER, HIGH);
          break;
        case 'l':
          digitalWrite(PTC_PIN_LASER, LOW);
          break;
        case 'P':
          cmdok = pantilt.move(PTC_SERVO_ID_PAN, val);
          break;
        case 'T':
          cmdok = pantilt.move(PTC_SERVO_ID_TILT, val);
          break;
        default:
          cmdok = 0;
          break;
      }
       
  #if PTC_DEBUG
      Serial.print("CMD: ");
      Serial.print(cmd);
      Serial.print(" TYPE: 0x");
      Serial.print(type, HEX);
      Serial.print(" VAL: 0x");
      Serial.print(val, HEX);
      Serial.print(" RES: ");
      Serial.print(cmdok);
      Serial.print("\r\n");
  #endif
    }
  }
}

