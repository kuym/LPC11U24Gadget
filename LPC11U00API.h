#ifndef _LPC11U00API_H_
#define _LPC11U00API_H_

#include <LPC11U00.h>

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
// Low-level

void		vmemset(void volatile* dest, unsigned int value, unsigned int length);

////////////////////////////////////////////////////////////////
// System

void		SystemSetupSystemPLL(void);
void		SystemSetupUSBPLL(void);
void		SystemDelayMs(unsigned int ms);


////////////////////////////////////////////////////////////////
// NumberFormatter

typedef enum NumberFormatterBase
{
	NumberFormatter_Character = 255,
	NumberFormatter_Hexadecimal = 16,
	NumberFormatter_DecimalUnsigned = 10,
	NumberFormatter_DecimalSigned = -10,

} NumberFormatterBase;
	
int			NumberFormatterFormat(char* output, unsigned int number, int fractionBits, NumberFormatterBase base);


////////////////////////////////////////////////////////////////
// UART

enum UART
{
	UART_CharsAre5Bit			=	(0x00),
	UART_CharsAre6Bit			=	(0x01),
	UART_CharsAre7Bit			=	(0x02),
	UART_CharsAre8Bit			=	(0x03),
	
	UART_OneStopBit				=	(0x00),
	UART_TwoStopBits			=	(0x04),
	
	UART_NoParity				=	(0x00),
	UART_UseOddParity			=	(0x08 | (0x00 << 4)),
	UART_UseEvenParity			=	(0x08 | (0x01 << 4)),
	UART_UseConstant1Parity		=	(0x08 | (0x02 << 4)),
	UART_UseConstant0Parity		=	(0x08 | (0x03 << 4)),
	
	UART_Default				=	(UART_CharsAre8Bit | UART_NoParity | UART_OneStopBit)
};

void		UARTStart(unsigned int baudRate, unsigned int mode);
void		UARTWriteStringLengthSync(char const* msg, int length);
void		UARTWriteStringSync(char const* msg);
void		UARTWriteIntSync(unsigned int value, NumberFormatterBase base);
void		UARTWriteHexIntSync(unsigned int value, int minDigits);
void		UARTWriteHexDumpSync(unsigned char const* data, unsigned int length);


////////////////////////////////////////////////////////////////
// CircularBuffer

typedef struct CircularBuffer
{
	unsigned short* _p;

} CircularBuffer;

inline int					CircularBufferSize(CircularBuffer const* cb)		{return(cb->_p[0]);}
inline int					CircularBufferUsed(CircularBuffer const* cb)		{return(cb->_p[2]);}
inline int					CircularBufferFree(CircularBuffer const* cb)		{return(cb->_p[0] - cb->_p[2]);}
inline int 					CircularBufferSegmentSize(CircularBuffer const* cb)	{return(((cb->_p[1] + cb->_p[2]) <= cb->_p[0])? (cb->_p[2]) : (cb->_p[0] - cb->_p[1]));}
inline unsigned char const*	CircularBufferSegment(CircularBuffer const* cb)		{return((((unsigned char*)cb->_p) + (3 * sizeof(unsigned short))) + cb->_p[1]);}

int							CircularBufferInit(CircularBuffer* cb, int bufferSize);
void						CircularBufferDeinit(CircularBuffer* cb);

void						CircularBufferReset(CircularBuffer* cb);

int							CircularBufferRead(CircularBuffer* cb, unsigned char* out, int length);
unsigned char				CircularBufferReadByte(CircularBuffer* cb);

int							CircularBufferWrite(CircularBuffer* cb, unsigned char const* in, int length);
int							CircularBufferWriteByte(CircularBuffer* cb, unsigned char b);





#endif //_LPC11U00API_H_
