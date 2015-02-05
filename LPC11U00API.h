#ifndef _LPC11U00API_H_
#define _LPC11U00API_H_

#include <LPC11U00.h>
using namespace LPC11U00;

void delay(unsigned int ms);
void delayMicro(unsigned int us);

extern "C" void vmemset(void volatile* dest, unsigned int value, unsigned int length);

enum Pin
{
	// Gnd								// Gadget pin 1
	// 3v3								// Gadget pin 2
	Pin_A1 =		((0 << 5) | 12),	// Gadget pin 3
	Pin_A2 =		((0 << 5) | 13),	// Gadget pin 4
	Pin_A3 =		((0 << 5) | 14),	// Gadget pin 5
	Pin_A5 =		((0 << 5) | 16),	// Gadget pin 6
	Pin_SCK1 =		((1 << 5) | 15),	// Gadget pin 7
	Pin_RXD =		((0 << 5) | 18),	// Gadget pin 8
	Pin_TXD =		((0 << 5) | 19),	// Gadget pin 9
	Pin_nCS1 =		((1 << 5) | 19),	// Gadget pin 10
	Pin_nRESET =	((0 << 5) | 0),		// Gadget pin 11
	Pin_P0 =		((0 << 5) | 1),		// Gadget pin 12
	Pin_nCS0 =		((0 << 5) | 2),		// Gadget pin 13
	Pin_SCL =		((0 << 5) | 4),		// Gadget pin 14
	Pin_SDA =		((0 << 5) | 5),		// Gadget pin 15
	Pin_MOSI1 =		((0 << 5) | 21),	// Gadget pin 16
	Pin_SCK0 =		((0 << 5) | 6),		// Gadget pin 17
	Pin_MISO0 =		((0 << 5) | 8),		// Gadget pin 18
	Pin_MOSI0 =		((0 << 5) | 9),		// Gadget pin 19
	Pin_MISO1 =		((0 << 5) | 22),	// Gadget pin 20
	Pin_A0 =		((0 << 5) | 11),	// Gadget pin 21
	// Vin								// Gadget pin 22
	Pin_SWCLK =		((0 << 5) | 10),	// Gadget pin 23
	Pin_SWDIO =		((0 << 5) | 15),	// Gadget pin 24
};

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
// CircularBuffer

class CircularBuffer
{
public:
	inline int		size() volatile const	{return(_p[0]);}
	inline int		used() volatile const	{return(_p[2]);}
	inline int		free() volatile const	{return(_p[0] - _p[2]);}
	
					CircularBuffer(void);
					~CircularBuffer(void);
	
	bool			alloc(int bufferSize);
	void			dealloc(void);
	void			reset(void);
	
	int				read(unsigned char* out, int length);
	unsigned char	readByte(void);

	int				write(unsigned char const* in, int length);
	void			writeByte(unsigned char b);

private:
	inline int				L(void) const
	{
		return(_p[0]);
	}
	inline unsigned char*	T(void) const
	{
		return(((unsigned char*)(_p + (3 * sizeof(unsigned short)))) + _p[1]);
	}
	inline int				U(void) const
	{
		return(_p[2]);
	}
	
	inline unsigned char*	h(void) const
	{
		return(T() + _p[2] - (((_p[1] + _p[2]) > _p[0])? _p[0] : 0));
	}
	inline int				f(void) const
	{
		return(_p[0] - _p[2]);
	}
	inline int				tb(void) const
	{
		return(((_p[1] + _p[2]) < _p[0])? _p[2] : (_p[0] - _p[1]));
	}
	inline int				hb(void) const
	{
		return(((_p[1] + _p[2]) < _p[0])? (_p[0] - _p[2] - _p[1]) : (_p[0] - _p[2]));
	}
	
	inline void			T(int increment)
	{
		int v = _p[1] + increment;
		if(v >= _p[0])	v -= _p[0];
		_p[1] = v;
		_p[2] -= increment;
	}
	inline void			h(int increment)
	{
		_p[2] += increment;
	}
	
	unsigned short volatile*	_p;
};

////////////////////////////////////////////////////////////////
// System

void	System_strobeClockUpdateEnable(REGISTER_REF updateEnable);

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
	static void				writeHexIntSync(unsigned int value, int minLength = 0);

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

////////////////////////////////////////////////////////////////
// I2C

typedef void (*TaskCallback)(int id, unsigned char* bytesRead, unsigned int length, void* context);

class I2C
{
public:
	static void			start(int bitRate);
	static inline void	stop(void)	{start(0);}
	
	static int			write(unsigned char address, void* data, unsigned int length, TaskCallback completion, void* context);
	static inline int	read(unsigned char address, unsigned int length, TaskCallback completion, void* context)
							{return(write(address, 0, length, completion, context));}
	
	static void			monitor(unsigned int maxLength, TaskCallback callback, void* context);
	static inline void	monitorOff(void)	{monitor(0, 0, 0);}
};


#endif //_LPC11U00API_H_
