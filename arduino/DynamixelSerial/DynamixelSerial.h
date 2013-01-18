#ifndef DynamixelSerial1_h
#define DynamixelSerial1_h

#if defined(ARDUINO) && ARDUINO >= 100  // Arduino IDE Version
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

	// EEPROM AREA  ///////////////////////////////////////////////////////////
#define AX_MODEL_NUMBER_L           0
#define AX_MODEL_NUMBER_H           1
#define AX_VERSION                  2
#define AX_ID                       3
#define AX_BAUD_RATE                4
#define AX_RETURN_DELAY_TIME        5
#define AX_CW_ANGLE_LIMIT_L         6
#define AX_CW_ANGLE_LIMIT_H         7
#define AX_CCW_ANGLE_LIMIT_L        8
#define AX_CCW_ANGLE_LIMIT_H        9
#define AX_SYSTEM_DATA2             10
#define AX_LIMIT_TEMPERATURE        11
#define AX_DOWN_LIMIT_VOLTAGE       12
#define AX_UP_LIMIT_VOLTAGE         13
#define AX_MAX_TORQUE_L             14
#define AX_MAX_TORQUE_H             15
#define AX_RETURN_LEVEL             16
#define AX_ALARM_LED                17
#define AX_ALARM_SHUTDOWN           18
#define AX_OPERATING_MODE           19
#define AX_DOWN_CALIBRATION_L       20
#define AX_DOWN_CALIBRATION_H       21
#define AX_UP_CALIBRATION_L         22
#define AX_UP_CALIBRATION_H         23

	// RAM AREA  //////////////////////////////////////////////////////////////
#define AX_TORQUE_ENABLE            24
#define AX_LED                      25
#define AX_CW_COMPLIANCE_MARGIN     26
#define AX_CCW_COMPLIANCE_MARGIN    27
#define AX_CW_COMPLIANCE_SLOPE      28
#define AX_CCW_COMPLIANCE_SLOPE     29
#define AX_GOAL_POSITION_L          30
#define AX_GOAL_POSITION_H          31
#define AX_GOAL_SPEED_L             32
#define AX_GOAL_SPEED_H             33
#define AX_TORQUE_LIMIT_L           34
#define AX_TORQUE_LIMIT_H           35
#define AX_PRESENT_POSITION_L       36
#define AX_PRESENT_POSITION_H       37
#define AX_PRESENT_SPEED_L          38
#define AX_PRESENT_SPEED_H          39
#define AX_PRESENT_LOAD_L           40
#define AX_PRESENT_LOAD_H           41
#define AX_PRESENT_VOLTAGE          42
#define AX_PRESENT_TEMPERATURE      43
#define AX_REGISTERED_INSTRUCTION   44
#define AX_PAUSE_TIME               45
#define AX_MOVING                   46
#define AX_LOCK                     47
#define AX_PUNCH_L                  48
#define AX_PUNCH_H                  49

    // Status Return Levels ///////////////////////////////////////////////////////////////
#define AX_RETURN_NONE              0
#define AX_RETURN_READ              1
#define AX_RETURN_ALL               2

    // Instruction Set ///////////////////////////////////////////////////////////////
#define AX_PING                     1
#define AX_READ_DATA                2
#define AX_WRITE_DATA               3
#define AX_REG_WRITE                4
#define AX_ACTION                   5
#define AX_RESET                    6
#define AX_SYNC_WRITE               131

	// Specials ///////////////////////////////////////////////////////////////
#define AX_INSTRUCTION_0_PARAMS_LENGTH  2


#define OFF                         0
#define ON                          1
#define LEFT						0
#define RIGTH                       1
#define AX_PING_LENGTH              0

#define AX_BYTE_READ                1
#define AX_BYTE_READ_LENGTH         4


#define AX_BYTE_READ_POS            2
#define AX_RESET_LENGTH				2
#define AX_ACTION_LENGTH			2
#define AX_ID_LENGTH                4
#define AX_LR_LENGTH                4
#define AX_SRL_LENGTH               4
#define AX_RDT_LENGTH               4
#define AX_LEDALARM_LENGTH          4
#define AX_SALARM_LENGTH            4
#define AX_TL_LENGTH                4
#define AX_VL_LENGTH                6
#define AX_CM_LENGTH                6
#define AX_CS_LENGTH                6
#define AX_CCW_CW_LENGTH            8
#define AX_BD_LENGTH                4
#define AX_TEM_LENGTH               4
#define AX_MOVING_LENGTH            4
#define AX_RWS_LENGTH               4
#define AX_VOLT_LENGTH              4
#define AX_LED_LENGTH               4
#define AX_TORQUE_LENGTH            4
#define AX_POS_LENGTH               4
#define AX_GOAL_LENGTH              5
#define AX_MT_LENGTH                5
#define AX_PUNCH_LENGTH             5
#define AX_SPEED_LENGTH             5
#define AX_GOAL_SP_LENGTH           7
#define AX_ACTION_CHECKSUM			250
#define BROADCAST_ID                254
#define AX_START                    255
#define AX_CCW_AL_L                 255 
#define AX_CCW_AL_H                 3
#define LOCK                        1

#include <inttypes.h>

class DynamixelSerial {
private:
    HardwareSerial &_ser;
    HardwareSerial *_debug;
	unsigned char Direction_Pin;

	unsigned char read_response(uint8_t *buf, unsigned char N);
	unsigned char read_response(uint8_t &resp) { read_response(&resp, 1); }
	unsigned char read_error(void) { read_response(NULL, 0); }
	unsigned char instruction_read_byte(uint8_t ID, uint8_t address, uint8_t &resp);
    unsigned char instruction_no_params(uint8_t ID, uint8_t cmd);

    void sendData(unsigned char);
    int availableData(void) { return _ser.available(); }
    int readData(void)      { return _ser.read(); }
    int peekData(void)      { return _ser.peek(); }

public:
    DynamixelSerial(HardwareSerial &ser) : _ser(ser), _debug(NULL) {}
    DynamixelSerial(HardwareSerial &ser, HardwareSerial &debug) : _ser(ser), _debug(&debug) {}
	
	void          begin(unsigned char directionPin, unsigned long baud);
	void          end(void) { _ser.end(); }

	unsigned char move(unsigned char ID, int Position);
	unsigned char move(unsigned char ID, int Position, int Speed);
	unsigned char reset(unsigned char ID) {
                    return instruction_no_params(ID, AX_RESET); }
	unsigned char ping(unsigned char ID) {
                    return instruction_no_params(ID, AX_PING); }
    unsigned char getTemperature(unsigned char ID, uint8_t &val) { 
                    return instruction_read_byte(ID, AX_PRESENT_TEMPERATURE, val); }
    unsigned char getVoltage(unsigned char ID, uint8_t &val) { 
                    return instruction_read_byte(ID, AX_PRESENT_VOLTAGE, val); }
    unsigned char isMoving(unsigned char ID, uint8_t &val) {
                    return instruction_read_byte(ID, AX_MOVING, val); }

};

#endif
