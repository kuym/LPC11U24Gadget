#ifndef _LPC11U00API_H_
#define _LPC11U00API_H_

#include <LPC11U00.h>
using namespace LPC11U00;

void delay(unsigned int ms);

extern "C" void memset_volatile(void volatile* dest, unsigned int value, unsigned int length);


////////////////////////////////////////////////////////////////
// NumberFormatter

class NumberFormatter
{
public:
	typedef enum
	{
		Character = 255,
		Hexadecimal = 16,
		DecimalUnsigned = 10,
		DecimalSigned = -10,
	} Base;
	
	static int		format(char* output, unsigned int number, int fractionBits, Base base);
};

////////////////////////////////////////////////////////////////
// System

void	System_strobeClockUpdateEnable(REGISTER updateEnable);

class System
{
public:
	static void			setupSystemPLL(void);
	
	static void			setupUSBPLL(void);

	static void			setClockOutput(void);

	static unsigned int	getMainClockFrequency(void);

	//this is also the AHB (main high-speed bus) frequency
	static unsigned int	getCoreFrequency(void);
};

////////////////////////////////////////////////////////////////
// UART

class UART
{
public:
	enum
	{
		CharsAre5Bit			=	(0x00),
		CharsAre6Bit			=	(0x01),
		CharsAre7Bit			=	(0x02),
		CharsAre8Bit			=	(0x03),
		
		OneStopBit				=	(0x00),
		TwoStopBits				=	(0x04),
		
		NoParity				=	(0x00),
		UseOddParity			=	(0x08 | (0x00 << 4)),
		UseEvenParity			=	(0x08 | (0x01 << 4)),
		UseConstant1Parity		=	(0x08 | (0x02 << 4)),
		UseConstant0Parity		=	(0x08 | (0x03 << 4)),
		
		Default					=	(CharsAre8Bit | NoParity | OneStopBit)
	};
	static void				start(unsigned int baudRate, unsigned int mode = Default);
	static inline void		stop(void)	{start(0);}
	static void				writeSync(char const* bytesOut, int length = -1);
	static inline void		writeSync(int value, NumberFormatter::Base format = NumberFormatter::DecimalSigned)
		{writeSync((unsigned int)value, format);}
	static inline void		writeSync(char value, NumberFormatter::Base format = NumberFormatter::DecimalSigned)
		{writeSync((unsigned int)value, format);}
	static inline void		writeSync(unsigned char value, NumberFormatter::Base format = NumberFormatter::DecimalSigned)
		{writeSync((unsigned int)value, format);}
	static void				writeSync(unsigned int value, NumberFormatter::Base format = NumberFormatter::DecimalSigned);
	
	static void				writeHexDumpSync(unsigned char const* bytesOut, int length);

	static void				flush(void);
};

////////////////////////////////////////////////////////////////
// SPI

class SPI
{
public:
	typedef enum
	{
		Master,
		Slave,
	} Role;
	
	enum
	{
		CharsAre4Bit	=	(0x03),
		CharsAre5Bit	=	(0x04),
		CharsAre6Bit	=	(0x05),
		CharsAre7Bit	=	(0x06),
		CharsAre8Bit	=	(0x07),
		CharsAre9Bit	=	(0x08),
		CharsAre10Bit	=	(0x09),
		CharsAre11Bit	=	(0x0A),
		CharsAre12Bit	=	(0x0B),
		CharsAre13Bit	=	(0x0C),
		CharsAre14Bit	=	(0x0D),
		CharsAre15Bit	=	(0x0E),
		CharsAre16Bit	=	(0x0F),
		
		Mode0			= (0x00 << 6),	//SCK idles low, data changed on SCK's falling edge, read on rising edge.
		Mode1			= (0x02 << 6),	//SCK idles low, data changed on SCK's rising edge, read on falling edge.
		Mode2			= (0x01 << 6),	//SCK idles high, data changed on SCK's falling edge, read on rising edge.
		Mode3			= (0x03 << 6),	//SCK idles high, data changed on SCK's rising edge, read on falling edge.
		
		Default			= (CharsAre8Bit | Mode0),
	};
	typedef int Mode;
	
	void			start(int bitRate, Mode mode, Role role);

private:
	unsigned int	deviceNum;
};

#endif //_LPC11U00API_H_
