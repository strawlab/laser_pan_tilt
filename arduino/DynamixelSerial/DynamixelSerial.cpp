#include "DynamixelSerial.h"

#define TX_DELAY_TIME				600
#define Tx_MODE                     1
#define Rx_MODE                     0

#define MAX_RESPONSE                2

unsigned char DynamixelSerial::read_response(uint8_t *buf, unsigned char N)
{
    unsigned char tries,a,b,id,len,error;
    error = 0xFF;

	tries = 10;
	while((availableData() <= (6-N)) && --tries){  // Wait for data
		delayMicroseconds(500);
	}

	while (availableData() > 0){
		a = readData();
		if ( (a == 255) & (peekData() == 255) ){
            b = readData();
			id = readData();                               // Ax-12 ID
			len = readData() - 2;                          // Length
			error = readData();
            for (int i = 0; i < len; i++) {
                if ((buf != NULL) && (i < N)) {
                    buf[i] = (uint8_t)readData();
                }
            }
            readData();                                    // Checksum
		}
	}

    return error;
}

unsigned char DynamixelSerial::instruction_read_byte(uint8_t ID, uint8_t address, uint8_t &resp)
{
	unsigned char Checksum; 

    Checksum = (~(ID + AX_BYTE_READ_LENGTH + AX_READ_DATA + address + AX_BYTE_READ))&0xFF;

    digitalWrite(Direction_Pin,Tx_MODE);
    sendData(AX_START);
    sendData(AX_START);
    sendData(ID);
    sendData(AX_BYTE_READ_LENGTH);
    sendData(AX_READ_DATA);
    sendData(address);
    sendData(AX_BYTE_READ);
    sendData(Checksum);
    delayMicroseconds(TX_DELAY_TIME);
	digitalWrite(Direction_Pin,Rx_MODE);

    return read_response(&resp, 1);
}

unsigned char DynamixelSerial::instruction_no_params(uint8_t ID, uint8_t cmd)
{
	unsigned char Checksum;

	Checksum = (~(ID + AX_INSTRUCTION_0_PARAMS_LENGTH + cmd))&0xFF;
	
	digitalWrite(Direction_Pin,Tx_MODE);
	sendData(AX_START);                     
	sendData(AX_START);
	sendData(ID);
	sendData(AX_INSTRUCTION_0_PARAMS_LENGTH);
	sendData(cmd);    
	sendData(Checksum);
	delayMicroseconds(TX_DELAY_TIME);
	digitalWrite(Direction_Pin,Rx_MODE);

    return read_error();
}

void DynamixelSerial::begin(unsigned char directionPin, unsigned long baud)
{	
	Direction_Pin = directionPin;
    pinMode(Direction_Pin,OUTPUT);
    _ser.begin(baud);
}	

void DynamixelSerial::sendData(unsigned char d)
{
    _ser.write(d);
    _ser.flush();
    if (_debug)
        _debug->write(d);
}

unsigned char DynamixelSerial::move(unsigned char ID, int Position)
{
	unsigned char Checksum; 
    char Position_H,Position_L;

    Position_H = Position >> 8;           // 16 bits - 2 x 8 bits variables
    Position_L = Position;
	Checksum = (~(ID + AX_GOAL_LENGTH + AX_WRITE_DATA + AX_GOAL_POSITION_L + Position_L + Position_H))&0xFF;
    
	digitalWrite(Direction_Pin,Tx_MODE);
    sendData(AX_START);                 // Send Instructions over Serial
    sendData(AX_START);
    sendData(ID);
    sendData(AX_GOAL_LENGTH);
    sendData(AX_WRITE_DATA);
    sendData(AX_GOAL_POSITION_L);
    sendData(Position_L);
    sendData(Position_H);
    sendData(Checksum);
	delayMicroseconds(TX_DELAY_TIME);
	digitalWrite(Direction_Pin,Rx_MODE);

    return read_error();                 // Return the read error
}

unsigned char DynamixelSerial::move(unsigned char ID, int Position, int Speed)
{
	unsigned char Checksum; 
    char Position_H,Position_L,Speed_H,Speed_L;

    Position_H = Position >> 8;    
    Position_L = Position;                // 16 bits - 2 x 8 bits variables
    Speed_H = Speed >> 8;
    Speed_L = Speed;                      // 16 bits - 2 x 8 bits variables
	Checksum = (~(ID + AX_GOAL_SP_LENGTH + AX_WRITE_DATA + AX_GOAL_POSITION_L + Position_L + Position_H + Speed_L + Speed_H))&0xFF;
 
	digitalWrite(Direction_Pin,Tx_MODE);
    sendData(AX_START);                // Send Instructions over Serial
    sendData(AX_START);
    sendData(ID);
    sendData(AX_GOAL_SP_LENGTH);
    sendData(AX_WRITE_DATA);
    sendData(AX_GOAL_POSITION_L);
    sendData(Position_L);
    sendData(Position_H);
    sendData(Speed_L);
    sendData(Speed_H);
    sendData(Checksum);
    delayMicroseconds(TX_DELAY_TIME);
	digitalWrite(Direction_Pin,Rx_MODE);
    
    return read_error();               // Return the read error
}


