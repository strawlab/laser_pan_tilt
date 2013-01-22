// vi: set shiftwidth=2 tabstop=2 expandtab:

#include <DynamixelSerial.h>
#include <SPI.h>
#include <Ethernet.h>

#define   HW_VER 1
#include "PTCConfig.h"

#if PTC_DEBUG_DYNAMIXEL
  DynamixelSerial  pantilt(Serial1, Serial);
#else
  DynamixelSerial  pantilt(Serial1);
#endif

EthernetServer      server(PTC_TCP_PORT);
uint8_t             pantilt_wait;
uint8_t             init_ok;

void setup() {
  pinMode(PTC_PIN_LED_COMMAND, OUTPUT);
  pinMode(PTC_PIN_LASER, OUTPUT);
  pinMode(PTC_PIN_LED_LASER, OUTPUT);
  pinMode(PTC_PIN_LED_NET_OK, OUTPUT);
  pinMode(PTC_PIN_LED_SRV_OK, OUTPUT);

#if PTC_DEBUG_DYNAMIXEL || PTC_DEBUG
  Serial.begin(PTC_DEBUG_SERIAL_BAUD);
  Serial.println("POWER");
#endif

  Ethernet.begin(ptc_mac, ptc_ip);
  //the dhcp version of this function returns if a lease was acquired, the
  //static IP version returns nothing... yay
  digitalWrite(PTC_PIN_LED_NET_OK, HIGH);
  server.begin();

  pantilt.begin(PTC_PIN_COMM_DIRECTION, PTC_SERVO_BAUD);
  pantilt_wait = 0;

  //wait for both servos to be detected (?_ok = 0x00 when everythin is OK)
  uint8_t tries;
  uint8_t p_ok = 0xFF;
  uint8_t t_ok = 0xFF;
  digitalWrite(PTC_PIN_LED_SRV_OK, LOW);

  tries = 25;
  while ( (p_ok = pantilt.ping(PTC_SERVO_ID_PAN) != 0x00) && --tries )  {
    digitalWrite(PTC_PIN_LED_SRV_OK, digitalRead(PTC_PIN_LED_SRV_OK) ? LOW : HIGH);
    Serial.write('.');
    delay(100);
  }
  Serial.write('\n');
  tries = 25;
  while ( (t_ok = pantilt.ping(PTC_SERVO_ID_TILT) != 0x00) && --tries )  {
    digitalWrite(PTC_PIN_LED_SRV_OK, digitalRead(PTC_PIN_LED_SRV_OK) ? LOW : HIGH);
    Serial.write('.');
    delay(100);
  }
  Serial.write('\n');

  init_ok = (t_ok == 0x00) && (p_ok == 0x00);
  digitalWrite(PTC_PIN_LED_SRV_OK, init_ok);

#if PTC_DEBUG
  Serial.print("Connected Servos: ");
  Serial.print("P ");
  Serial.print(p_ok == 0x00, HEX);
  Serial.print(" T ");
  Serial.print(t_ok == 0x00, HEX);
  Serial.write('\n');
#endif
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

void wait_until_not_moving(uint8_t ID)
{
  uint8_t tries = 100;
  uint8_t moving = 1;

  while (moving && --tries) {
      pantilt.isMoving(ID, moving);
      delayMicroseconds(1000);
  }
}

void loop() {
  uint8_t rxok = 0;

  EthernetClient client = server.available();
  if (client) {
    rxok = ethernet_parse(client);
  }

  if (rxok) {
    uint8_t cmd, type, cmdok, parseok, cmdresp;
    uint16_t val;

    parseok = command_parse(cmd, type, val);
    if (parseok) {
      cmdok = 0x00;
      cmdresp = 0;
      //toggle activity LED
      digitalWrite(PTC_PIN_LED_COMMAND, digitalRead(PTC_PIN_LED_COMMAND) ? LOW : HIGH);
      switch (cmd) {
        case 'W':
          pantilt_wait = 1;
          break;
        case 'w':
          pantilt_wait = 0;
          break;
        case 'S':
          //FIXME set servo LED
          break;
        case 's':
          //FIXME set servo LED
          break;
        case 'L':
          digitalWrite(PTC_PIN_LASER, HIGH);
          digitalWrite(PTC_PIN_LED_LASER, HIGH);
          break;
        case 'l':
          digitalWrite(PTC_PIN_LASER, LOW);
          digitalWrite(PTC_PIN_LED_LASER, LOW);
          break;
        case 'P':
          cmdok = pantilt.move(PTC_SERVO_ID_PAN, val);
          if (pantilt_wait)
            wait_until_not_moving(PTC_SERVO_ID_PAN);
          break;
        case 'T':
          cmdok = pantilt.move(PTC_SERVO_ID_TILT, val);
          if (pantilt_wait)
            wait_until_not_moving(PTC_SERVO_ID_TILT);
          break;
        case 't':
          cmdok = pantilt.getTemperature(PTC_SERVO_ID_PAN, cmdresp);
          break;
        case 'v':
          cmdok = pantilt.getVoltage(PTC_SERVO_ID_PAN, cmdresp);
          break;
        case 'p':
          cmdok = pantilt.ping(val);
          break;
        default:
          cmdok = 0;
          break;
      }

#if 0
      client.write(cmd);
      client.write(',');
      for (int i=0; i < 20; i++) {
        uint8_t foo = Serial1.read();
        client.print(foo, HEX);
        client.write(',');
      }
      client.write('\n');
#else
      client.write(cmd);
      client.write(',');
      client.print(cmdok, HEX);
      client.write(',');
      client.print(cmdresp, HEX);
      client.write('\n');
#endif
       
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

