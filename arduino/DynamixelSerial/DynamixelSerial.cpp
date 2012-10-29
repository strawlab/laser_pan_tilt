#if defined(ARDUINO) && ARDUINO >= 100  // Arduino IDE Version
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "DynamixelSerial.h"

#define TIME_OUT                    10
#define TX_DELAY_TIME				1000
#define Tx_MODE                     1
#define Rx_MODE                     0

#define MAX_RESPONSE                2

unsigned char DynamixelSerial::read_response(uint8_t *buf, unsigned char N)
{
    unsigned char tries,a,b,len,error;

//    return 0;

	tries = 0;
	while(!availableData() && (tries < TIME_OUT)){  // Wait for any data
		tries++;
		delayMicroseconds(100);
	}

    error = 0xFF;
	while (availableData() > 0){
		a = readData();
        b = readData();
		if ( (a == 255) & (b == 255) ){
			readData();                                    // Ax-12 ID
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

unsigned char DynamixelSerial::read_register_byte(uint8_t ID, uint8_t address, uint8_t &resp)
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

unsigned char DynamixelSerial::read_response(uint8_t &resp)
{
    return read_response(&resp, 1);
}

void DynamixelSerial::begin(unsigned char directionPin)
{	
	Direction_Pin = directionPin;
    pinMode(Direction_Pin,OUTPUT);
	beginCom();
}	

void DynamixelSerial::end()
{
	endCom();
}

unsigned char DynamixelSerial::reset(unsigned char ID)
{
	unsigned char Checksum;

	Checksum = (~(ID + AX_RESET_LENGTH + AX_RESET))&0xFF;
	
	digitalWrite(Direction_Pin,Tx_MODE);
	sendData(AX_START);                     
	sendData(AX_START);
	sendData(ID);
	sendData(AX_RESET_LENGTH);
	sendData(AX_RESET);    
	sendData(Checksum);
	delayMicroseconds(TX_DELAY_TIME);
	digitalWrite(Direction_Pin,Rx_MODE);

    return read_error();  
}

unsigned char DynamixelSerial::ping(unsigned char ID)
{
	unsigned char Checksum; 

	Checksum = (~(ID + AX_PING_LENGTH + AX_PING))&0xFF;
	
	digitalWrite(Direction_Pin,Tx_MODE);
	sendData(AX_START);                     
	sendData(AX_START);
	sendData(ID);
	sendData(AX_PING_LENGTH);
	sendData(AX_PING);    
	sendData(Checksum);
	delayMicroseconds(TX_DELAY_TIME);
	digitalWrite(Direction_Pin,Rx_MODE);
    
    return read_error();              
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


