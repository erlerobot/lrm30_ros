#include "LRM30_serial.h"

LRM30_serial::LRM30_serial()
{
}

LRM30_serial::LRM30_serial(unsigned long baud, std::string port)
{
	this->baud = baud;
	this->port = port;
}

int LRM30_serial::connect()
{
	my_serial = new serial::Serial(this->port, 
									this->baud,
									serial::Timeout::simpleTimeout(200),
									serial::eightbits,
									serial::parity_none,
									serial::stopbits_one, 
									serial::flowcontrol_none);

	std::cout << "Is the serial port open?";
	if(my_serial->isOpen())
		std::cout << " Yes." << std::endl;
	else
		std::cout << " No." << std::endl;
}

void LRM30_serial::setBaud(unsigned long baud)
{
	this->baud = baud;
}

unsigned long LRM30_serial::getBaud()
{
	this-baud;
}

void LRM30_serial::setPort(std::string port)
{
	this->port = port;
}

std::string LRM30_serial::getPort()
{
	return this->port;
}

int LRM30_serial::laserON()
{
	std::vector<uint8_t> v;
	v.push_back(0xC0);
	v.push_back(0x41);
	v.push_back(0x0);
	v.push_back( CalcCrc8FromArray ( v, CRC8_INITIAL_VALUE ));

	size_t bytes_wrote = my_serial->write(v);
	std::vector<uint8_t> v_recv;

	int n  = my_serial->read(v_recv, 5);

#if DEBUG
		std::cout << "laserON Bytes written: " << bytes_wrote << " bytes recieved: " << n << std::endl;
#endif
}

int LRM30_serial::laserOFF()
{
	std::vector<uint8_t> v;
	v.push_back(0xC0);
	v.push_back(0x42);
	v.push_back(0x0);
	v.push_back( CalcCrc8FromArray ( v, CRC8_INITIAL_VALUE ));

	size_t bytes_wrote = my_serial->write(v);

	std::vector<uint8_t> v_recv;
	int n  = my_serial->read(v_recv, 5);
#if DEBUG
		std::cout << "laserON Bytes written: " << bytes_wrote << " bytes recieved: " << n << std::endl;
#endif
}

float LRM30_serial::singleshot()
{
	std::vector<uint8_t> v;
	v.push_back(0xC0);
	v.push_back(0x40);
	v.push_back(0x01);
	v.push_back(0x00);
	v.push_back(CalcCrc8FromArray ( v, CRC8_INITIAL_VALUE ));

	size_t bytes_wrote = my_serial->write(v);

	std::vector<uint8_t> v_rev;
	int n  = my_serial->read(v_rev, 10);
	if(n==0) return 0;

	uint32_t _Recv2 = v_rev[5]<<24 | v_rev[4]<<16 | v_rev[3]<<8 | v_rev[2];
	
#if DEBUG
	for(int i = 0; i < n;i ++){
		printf(" - %.2X - ", v_rev[i]);
		std::bitset<8> x(v_rev[i]);
		std::cout << x ;
	}
	std::cout << std::endl;
	printf("value: %.4f \n", _Recv2*50e-6);
	printf("value: %hhX \n", _Recv2);
	std::cout << "Bytes written: " << bytes_wrote << ", Bytes read: " << n << std::endl;
#endif
	return _Recv2*50e-6;
}
 
float LRM30_serial::continuousshot()
{

	std::vector<uint8_t> v;
	v.push_back(0xC0);
	v.push_back(0x40);
	v.push_back(0x01);
	v.push_back(0x01);
    v.push_back(CalcCrc8FromArray ( v, CRC8_INITIAL_VALUE ));

	size_t bytes_wrote = my_serial->write(v);

	std::vector<uint8_t> v_recv;
	int n  = my_serial->read(v_recv, 10);
	if(n==0) return -1;
	uint32_t _Recv2 = v_recv[5]<<24 | v_recv[4]<<16 | v_recv[3]<<8 | v_recv[2];

	return  _Recv2*50e-6;
}

float LRM30_serial::getMeasure()
{
	std::vector<uint8_t> v;
	v.push_back(0xC0);
	v.push_back(0x40);
	v.push_back(0x01);
	v.push_back(0x0D);
	v.push_back(CalcCrc8FromArray ( v, CRC8_INITIAL_VALUE ));

	std::vector<uint8_t> v_recv;
	size_t bytes_wrote = my_serial->write(v);
	int n  = my_serial->read(v_recv, 10);

	if(n==0) return  -1;
	if(v_recv[0]!=0) return  -1;
	uint32_t _Recv2 = v_recv[5]<<24 | v_recv[4]<<16 | v_recv[3]<<8 | v_recv[2];

#if DEBUG
	std::cout << "n: " << n << std::endl;
	for(int i = 0; i < n;i ++){
		printf(" - %.2X - ", v_recv[i]);
		std::bitset<8> x(v_recv[i]);
		std::cout << x ;
	}
	std::cout << std::endl;
#endif


	return  _Recv2*50e-6;
}

//***************************************************************************************************************************
// Calculate the CRC8 checksum for the given value. No reflection, inversion, reversion or final XOR used. Just plain CRC8.
//   input:
//     -- Data:         Data to calculate the CRC for.
//     -- InitialValue: The initial value for the CRC (e.g. previous calculated value).
//   output:
//      -- Calculated CRC8 checksum
//
//***************************************************************************************************************************
uint8_t LRM30_serial::CalcCrc8(uint8_t Data, unsigned char InitialValue)
{
  uint8_t i;
  
  for (i=0; i<8; i++){
    if (((InitialValue & 0x80) != 0) != ((Data >> (7-i)) & 1))
      InitialValue = (InitialValue << 1) ^ CRC8_POLYNOMIAL;
    else
      InitialValue <<= 1;
  }
  return InitialValue;
}

//***************************************************************************************************************************
// Calculate the CRC8 checksum for the given array. No reflection, inversion, reversion or final XOR used. Just plain CRC8.
//   input:
//     -- pData:        Pointer to the array to calculate the CRC for.
//     -- NumBytes:     Size of array in bytes.
//     -- InitialValue: The initial value for the CRC (e.g. previous calculated value).
//   output:
//      -- Calculated CRC8 checksum
//
//***************************************************************************************************************************
uint8_t LRM30_serial::CalcCrc8FromArray(std::vector<uint8_t> pData, uint8_t InitialValue)
{
  for(int i = 0; i < pData.size(); i++){
    InitialValue = CalcCrc8(pData[i], InitialValue);
  }
  return InitialValue;
}