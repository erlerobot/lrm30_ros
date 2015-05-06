#ifndef LRM30_SERIAL_H
#define LRM30_SERIAL_H

#include <string>
#include <iostream>
#include <bitset>
#include <unistd.h>

#include "serial/serial.h"

#define DEBUG 0


#define CRC8_INITIAL_VALUE  0xAA
#define CRC8_POLYNOMIAL     0xA6

class LRM30_serial
{
public:
    LRM30_serial(unsigned long baud, std::string port);
    LRM30_serial();

    void setBaud(unsigned long baud);
    unsigned long getBaud();

    void setPort(std::string port);
    std::string getPort();

    int connect();

    int laserON();
    int laserOFF();

    float singleshot();
    float continuousshot();
	float getMeasure();

private:
	unsigned long baud;
	std::string port;
	uint8_t CalcCrc8(uint8_t Data, unsigned char InitialValue);
	uint8_t CalcCrc8FromArray(std::vector<uint8_t> pData, uint8_t InitialValue);
	serial::Serial *my_serial;
};

#endif // LRM30_SERIAL_H
