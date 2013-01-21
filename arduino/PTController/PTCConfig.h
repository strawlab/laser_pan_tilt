#ifndef _PTC_CONFIG_H
#define _PTC_CONFIG_H

#include <Ethernet.h>

#define PTC_DEBUG               1
#define PTC_DEBUG_DYNAMIXEL     0

#define PTC_DEBUG_SERIAL_BAUD   57600

#define PTC_PIN_LED_STATUS      8   //A1 (not used in v1)
#define PTC_PIN_LED_COMMAND     7   //A5 (in v1)

#define PTC_PIN_LED_LASER       A4
#define PTC_PIN_LED_NET_OK      A2
#define PTC_PIN_LED_SRV_OK      A3

#define PTC_PIN_CAMERA_TRIG     3   //(not used in v0)
#define PTC_PIN_COMM_DIRECTION  2
#define PTC_PIN_LASER           9

#define PTC_RXBUF_START         '@'
#define PTC_RXBUF_START_IDX     0
#define PTC_RXBUF_CMD_IDX       1
#define PTC_RXBUF_TYPE_IDX      2
#define PTC_RXBUF_DATA_L_IDX    3
#define PTC_RXBUF_DATA_H_IDX    4
#define PTC_RXBUF_END_IDX       5
#define PTC_RXBUF_END           '='
#define PTC_RXBUF_LEN           6
uint8_t ptc_rxbuf[PTC_RXBUF_LEN];

byte ptc_mac[] = {0x90, 0xA2, 0xDA, 0x00, 0x79, 0xDF};
IPAddress ptc_ip(192,168,1,60);
#define     PTC_TCP_PORT        23

#define PTC_SERVO_ID_TILT       2
#define PTC_SERVO_ID_PAN        1
#define PTC_SERVO_BAUD          57600

#endif /* _PTC_CONFIG_H */
