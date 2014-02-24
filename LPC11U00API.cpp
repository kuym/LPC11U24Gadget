#include <LPC11U00API.h>
#include <LPC11U00.h>

using namespace LPC11U00;
using namespace LPC11U00::ROMDivider;
using namespace LPC11U00::ROMAPI;

static unsigned int g1Millisecond = (12e6 / 1000 / 18);

void delay(unsigned int ms)
{
	while(ms--)
		for(unsigned int i = g1Millisecond; i; i--)
			__asm__ volatile ("nop \n nop \n nop \n nop \n" ::);
}

void		memset_volatile(void volatile* dest, unsigned int value, unsigned int length)
{
	volatile unsigned char* p = (volatile unsigned char*)dest;
	for(unsigned int i = 0; i < length; i++)
		*p++ = value;
}

////////////////////////////////////////////////////////////////
// NumberFormatter formats numbers into text

int		NumberFormatter::format(char* output, unsigned int number, int fractionBits, Base base)
{
	(void)fractionBits;	// not currently supported

	static char const charTable[] = "0123456789ABCDEF";
	char buf[11];
	int len = 0;
	bool neg = false;
	
	if(base == DecimalSigned)
	{
		base = DecimalUnsigned;
		if(((int)number) < 0)
		{
			neg = true;
			number = -number;
		}
	}
	else if(base == Character)
	{
		*output++ = (unsigned char)number;
		return(1);
	}
	
	do
	{
		uidivResult r = (*API)->divider->uidivmod(number, (unsigned int)base);
		buf[len++] = charTable[r.remainder];
		number = r.quotient;
	}
	while((len < 10) && (number != 0));
	
	if(neg)
		buf[len++] = '-';
	
	for(int i = len; i > 0;)
		*output++ = buf[--i];
	
	return(len);
}

////////////////////////////////////////////////////////////////
// CircularBuffer


//  returns the number of bytes occupied (available for reading) in this buffer
int			CircularBuffer::used() const
{
	unsigned short	bufferSize = bsize(),
					h = head(),
					t = tail();

	return((h >= t)? (h - t) : (bufferSize - (t - h)));
}

//  returns the number of bytes available for writing in this buffer
int			CircularBuffer::free() const
{
	unsigned short	bufferSize = bsize(),
					h = head(),
					t = tail();

	return((h >= t)? (bufferSize - (h - t) - 1) : (t - h - 1));
}

unsigned char	CircularBuffer::operator [](int offset) const
{
	unsigned short	bufferSize = bsize();

	offset += tail();
	if(offset > bufferSize)
		offset -= bufferSize;

	return(bp()[offset]);
}

			CircularBuffer::CircularBuffer(void):
				buffer(0)
{
}

			CircularBuffer::~CircularBuffer(void)
{
	dealloc();
}

bool		CircularBuffer::alloc(int bufferSize)
{
	dealloc();

	// crop to 16-bit for this implementation
	bufferSize = (unsigned short)bufferSize;
	
	// operator new may return 0 in this ABI
	buffer = new unsigned char[(3 * sizeof(unsigned short)) + bufferSize];
	if(buffer == 0)
		return(false);
	
	((unsigned short*)buffer)[0] = bufferSize;
	head() = 0;
	tail() = 0;

	return(true);
}

void		CircularBuffer::dealloc(void)
{
	if(buffer != 0)
	{
		delete[] buffer;
		buffer = 0;
	}
}

void		CircularBuffer::reset(void)
{
	head() = tail();
}

//  writes 'length' bytes from an input array ('in') into a circular buffer.  Returns
//    the number of bytes written.
//  'buffer' points at a circular buffer (see above)
//  'bufferSize' is the size of this buffer (see above)
int			CircularBuffer::write(unsigned char const* in, int length)
{
	unsigned short	bufferSize = bsize(),
					h = head(),
					t = tail(),
					count = 0;

	unsigned char volatile* b = bp() + h;

	if(h >= t)
	{
		while(length && (h < bufferSize))
		{
			*b++ = *in++;
			h++;
			count++;
			length--;
		}
		if(h == bufferSize)
			h = 0;
	}
	if(length > 0)
	{
		b = bp() + h;
		
		while(length-- && (h < (t - 1)))
		{
			*b++ = *in++;
			h++;
			count++;
		}
	}
	
	head() = h;
	return(count);
}

int			CircularBuffer::writeByte(unsigned char b)
{
	unsigned short	bufferSize = bsize(),
					h = head();
	
	if((h + 1) != tail())	// if we can fit it, write a byte
	{
		bp()[h] = b;
		if(++h == bufferSize)	// roll the circular buffer around
			h = 0;
		head() = h;
		return(1);
	}
	else
		return(0);
}

//  reads 'length' bytes from a circular buffer into an output array ('out').  Returns
//    the number of bytes read.
//  'buffer' points at a circular buffer (see above)
//  'bufferSize' is the size of this buffer (see above)
int			CircularBuffer::read(unsigned char* out, int length)
{
	unsigned short	bufferSize = bsize(),
					h = head(),
					t = tail(),
					count = 0;

	unsigned char volatile* b;
	
	if(t > h)
	{
		b = bp() + t;
		while(length && (t < bufferSize))
		{
			*out++ = *b++;
			t++;
			count++;
			length--;
		}
		if(t == bufferSize)
			t = 0;
	}
	if(length > 0)
	{
		b = bp() + t;
		
		while(length-- && (t < h))
		{
			*out++ = *b++;
			t++;
			count++;
		}
	}
	
	tail() = t;
	return(count);
}

//  reads one byte from a circular buffer into an output byte ('b').  Returns 1 on success,
//    0 on failure
int			CircularBuffer::readByte(unsigned char* b)
{
	unsigned short	bufferSize = bsize(),
					t = tail();

	if(t != head())	// if we can spare a byte, read it
	{
		*b = bp()[t];
		if(++t == bufferSize)	// roll the circular buffer around
			t = 0;
		tail() = t;
		return(1);
	}
	else
		return(0);
}


////////////////////////////////////////////////////////////////
// System

#define HARDWARE_EXTERNAL_CRYSTAL_FREQUENCY	(12000000UL)	//a property of the board

void			System_strobeClockUpdateEnable(REGISTER_REF updateEnable)
{
	*updateEnable = 1;
	*updateEnable = 0;
	*updateEnable = 1;
	
	while(!(*updateEnable & 1));	//spinwait for the change to take effect before returning
}

static unsigned int	System_getPLLInputFrequency(void)
{
	switch(*PLLSource)
	{
	default:
	case PLLSource_InternalCrystal:
		return(12000000UL);
	case PLLSource_ExternalClock:
		return(HARDWARE_EXTERNAL_CRYSTAL_FREQUENCY);
	}
}

static unsigned int	System_getPLLOutputFrequency(void)
{
	//the PLL on the LPC1xxx parts isn't very well designed, as the fixed relationship between
	//  FCLKOUT, the M-divider and FCLKIN means that the output is always an integer multiple
	//  of the input.  A better design would use relationship: Fcco / M = Fin; Fout = Fcco / D.
	//  (The LPC1xxx uses relationship: Fcco / D / M = Fin; Fout = Fcco / D.)
	
	return(System_getPLLInputFrequency() * ((*PLLControl & PLLControl_MultiplierBitsMask) + 1));
}


void			System::setupSystemPLL(void)
{
	//Note LPC1*** PLLs have transfer function H(f) = f * ((M + 1) / 2^^(P + 1))

	//ensure the internal crystal is on
	*PowerDownControl &= ~(PowerDownControl_InternalCrystalOutput | PowerDownControl_InternalCrystal);
	
	*MainClockSource = MainClockSource_InternalCrystal;
	System_strobeClockUpdateEnable(MainClockSourceUpdate);
	
	//ensure the external crystal is on
	*PowerDownControl &= ~PowerDownControl_SystemOscillator;

	//wait 200 clocks for it to stabilize
	for(int i = 0; i < 200; i++)
		__asm__ volatile ("nop"::);
	
	//turn on the PLL
	*PowerDownControl &= ~PowerDownControl_SystemPLL;
	
	*PLLSource = PLLSource_ExternalClock;
	System_strobeClockUpdateEnable(PLLSourceUpdate);

	//no: M = 4, P = 2 (m = 3, p = 1)
	//48MHz:
	*PLLControl = ((4) - 1) | (((1) - 1) << PLLControl_DividerBitsShift);

	//wait for lock
	while(!(*PLLStatus & PLLStatus_Locked));
	
	//run the core from the PLL with this divider:
	*MainBusDivider = 1;
	
	*MainClockSource = MainClockSource_PLLOutput;
	System_strobeClockUpdateEnable(MainClockSourceUpdate);
	
	g1Millisecond = getCoreFrequency() / 18000UL;	//the number of spinloop iterations per millisecond
}


void			System::setupUSBPLL(void)
{
	//ensure the necessary analog components are on
	*PowerDownControl &= ~(PowerDownControl_SystemOscillator | PowerDownControl_USBPLL | PowerDownControl_USBPins);
	
	//disconnect the clocks while we update
	*ClockControl &= ~(ClockControl_USB | ClockControl_USBRAM);
	*USBClockDivider = 0;

	*USBPLLSource = USBPLLSource_ExternalClock;
	System_strobeClockUpdateEnable(USBPLLSourceUpdate);

	//generate the required 48MHz
	*USBPLLControl = ((4) - 1) | (((1) - 1) << USBPLLControl_DividerBitsShift);

	//wait for PLL lock
	while(!(*USBPLLStatus & USBPLLStatus_Locked));

	*USBClockSource = USBClockSource_USBPLL_Output;
	System_strobeClockUpdateEnable(USBClockSourceUpdate);

	//clock and power on the parts
	*USBClockDivider = 1;
	*ClockControl |= (ClockControl_USB | ClockControl_USBRAM);
}

void			System::setClockOutput(void)
{
	*ClockOutputSource = ClockOutputSource_MainClock;
	*IOConfigPIO0_1 = (*IOConfigPIO0_1 & ~IOConfigPIO_FunctionMask) | IOConfigPIO0_1_Function_ClockOutput;
	System_strobeClockUpdateEnable(ClockOutputSourceUpdate);
	*ClockOutputDivider = 1;
}

unsigned int	System::getMainClockFrequency(void)
{
	switch(*MainClockSource)
	{
	default:
	case MainClockSource_InternalCrystal:
		return(12000000UL);
	case MainClockSource_PLLInput:
		return(System_getPLLInputFrequency());
	case MainClockSource_WDTOscillator:
		return(3400000UL);		//@@todo: confirm this is a reliable figure
	case MainClockSource_PLLOutput:
		return(System_getPLLOutputFrequency());
	}
}

//this is also the AHB (main high-speed bus) frequency
unsigned int	System::getCoreFrequency(void)
{
	//note no /0 check, but I would be very alarmed if this code could excecute with a stopped core :-/
	return(getMainClockFrequency() / *MainBusDivider);
}

////////////////////////////////////////////////////////////////
// UART

void				UART::start(unsigned int baudRate, unsigned int mode)
{
	if(baudRate > 0)
	{
		*ClockControl |= ClockControl_UART;
		*UARTClockDivider = 1;
		
		*UARTLineControl = (mode & 0xFF) | UARTLineControl_DivisorLatch;	//apply mode and enter DLAB state
		
		unsigned int divider = System::getCoreFrequency() / (16 * baudRate);

		*UARTDivisorLow = divider & 0xFF;		//write divisor bytes
		*UARTDivisorHigh = (divider >> 8) & 0xFF;
		
		*UARTLineControl = (mode & 0xFF);	//exit DLAB state
		*UARTModemControl = (mode >> 8) & 0xC7;
		
		*UARTFIFOControl |= (UARTFIFOControl_Enable | UARTFIFOControl_RxReset | UARTFIFOControl_TxReset); //| UARTFIFOControl_RxInterrupt1Char);
		
		(void)*UARTLineStatus;	//clear status

		//TXD in UART mode, no pullup/down
		*IOConfigPIO0_18 = (*IOConfigPIO0_18 & ~IOConfigPIO_FunctionMask) | IOConfigPIO0_18_Function_RXD;
		*IOConfigPIO0_19 = (*IOConfigPIO0_19 & ~IOConfigPIO_FunctionMask) | IOConfigPIO0_19_Function_TXD;
	}
	else
	{
		*IOConfigPIO0_18 = (*IOConfigPIO0_18 & ~IOConfigPIO_FunctionMask);
		*IOConfigPIO0_19 = (*IOConfigPIO0_19 & ~IOConfigPIO_FunctionMask);
		
		*UARTFIFOControl = (UARTFIFOControl_RxReset | UARTFIFOControl_TxReset);	//disable and reset
		
		*ClockControl &= ~ClockControl_UART;
	}
}
void				UART::writeSync(char const* bytesOut, int length)
{
	while(((length == -1) && *bytesOut) || (length-- > 0))
	{
		while(!(*UARTLineStatus & UARTLineStatus_TxHoldingRegisterEmpty));
		*UARTData = *bytesOut++;
	}
}
void				UART::writeSync(unsigned int value, NumberFormatter::Base format)
{
	char buffer[34];
	int len = NumberFormatter::format(buffer, value, 0, format);
	//writeSync(buffer, len);
	char* b = buffer;
	while(len--)
	{
		while(!(*UARTLineStatus & UARTLineStatus_TxHoldingRegisterEmpty));
		*UARTData = *b++;
	}
}
void				UART::writeHexDumpSync(unsigned char const* bytesOut, int length)
{
	static char const* hex = "0123456789ABCDEF";
	while(length--)
	{
		unsigned char b = *bytesOut++;
		while(!(*UARTLineStatus & UARTLineStatus_TxHoldingRegisterEmpty));
		*LPC11U00::UARTData = hex[b >> 4];
		while(!(*UARTLineStatus & UARTLineStatus_TxHoldingRegisterEmpty));
		*LPC11U00::UARTData = hex[b & 0xF];
	}
}
void				UART::flush(void)
{
	while(!(*UARTLineStatus & UARTLineStatus_TransmitterEmpty));
}


////////////////////////////////////////////////////////////////


void			SPI::start(int bitRate, Mode mode, Role role)
{
	(void)role;	// not currently supported
	
	//assert reset
	if(deviceNum == 0)
		*PeripheralnReset &= ~PeripheralnReset_SPI0;
	else
		*PeripheralnReset &= ~PeripheralnReset_SPI1;

	if(bitRate > 0)
	{
		if(deviceNum == 0)
		{
			*ClockControl |= ClockControl_SPI0;	//enable SPI0 clock
			*SPI0ClockDivider = 1;
			*PeripheralnReset |= PeripheralnReset_SPI0;	//deassert reset
		}
		else
		{
			*ClockControl |= ClockControl_SPI1;	//enable SPI1 clock
			*SPI1ClockDivider = 1;
			*PeripheralnReset |= PeripheralnReset_SPI1;	//deassert reset
		}
		
		//This finds three prescalers, A, B and C such that (Fcpu / (A + 1) / B / C) = bitRate, where C is an even number 2 to 254.
		// so as to avoid factoring, we cheat here by extracting an 8-bit mantissa (A) and computing 2^exponent, expressed in B and C
		
		unsigned int divisor = (System::getCoreFrequency() / bitRate) >> 1;
		unsigned int magnitude = 32 - __builtin_clz(divisor);
		unsigned int scale = 0, prescale = 2;
		
		if(magnitude > 8)
		{
			if(magnitude > 16)
			{
				prescale = 1 << (magnitude - 15);
				magnitude = 16;
			}
			divisor >>= (magnitude - 8); //set mantissa
			
			scale = ((1 << (magnitude - 8)) - 1);
		}
		else
			scale = 0;
		
		if(deviceNum == 0)
		{
			*SPI0Control0 = (scale << 8) | mode;
			*SPI0Control1 = SPI0Control1_Enable;
			*SPI0ClockPrescaler = prescale;
			*SPI0ClockDivider = divisor - 1;

			*IOConfigPIO0_6 = (*IOConfigPIO0_6 & ~IOConfigPIO_FunctionMask) | IOConfigPIO0_6_Function_SCK0;
			*IOConfigPIO0_8 = (*IOConfigPIO0_8 & ~IOConfigPIO_FunctionMask) | IOConfigPIO0_8_Function_MISO0;
			*IOConfigPIO0_9 = (*IOConfigPIO0_9 & ~IOConfigPIO_FunctionMask) | IOConfigPIO0_9_Function_MOSI0;
			*IOConfigPIO0_2 = (*IOConfigPIO0_2 & ~IOConfigPIO_FunctionMask) | IOConfigPIO0_2_Function_SSEL0;
			
			*InterruptEnableSet0 = Interrupt0_SPI0;
		}
		else
		{
			*SPI1Control0 = (scale << 8) | mode;
			*SPI1Control1 = SPI1Control1_Enable;
			*SPI1ClockPrescaler = prescale;
			*SPI1ClockDivider = divisor - 1;

			*IOConfigPIO1_15 = (*IOConfigPIO1_15 & ~IOConfigPIO_FunctionMask) | IOConfigPIO1_15_Function_SCK1;
			*IOConfigPIO0_22 = (*IOConfigPIO0_22 & ~IOConfigPIO_FunctionMask) | IOConfigPIO0_22_Function_MISO1;
			*IOConfigPIO0_21 = (*IOConfigPIO0_9 & ~IOConfigPIO_FunctionMask) | IOConfigPIO0_21_Function_MOSI1;
			*IOConfigPIO1_19 = (*IOConfigPIO1_19 & ~IOConfigPIO_FunctionMask) | IOConfigPIO1_19_Function_SSEL1;
			
			*InterruptEnableSet0 = Interrupt0_SPI1;
		}
	}
	else
	{
		if(deviceNum == 0)
		{
			*ClockControl &= ~ClockControl_SPI0;	//disable SPI0 clock
			*InterruptEnableClear0 = Interrupt0_SPI0;
			*SPI0InterruptClear = 0x0F;	//disable interrupts

			*IOConfigPIO0_6 = (*IOConfigPIO0_6 & ~IOConfigPIO_FunctionMask);
			*IOConfigPIO0_8 = (*IOConfigPIO0_8 & ~IOConfigPIO_FunctionMask);
			*IOConfigPIO0_9 = (*IOConfigPIO0_9 & ~IOConfigPIO_FunctionMask);
			*IOConfigPIO0_2 = (*IOConfigPIO0_2 & ~IOConfigPIO_FunctionMask);
		}
		else
		{
			*ClockControl &= ~ClockControl_SPI1;	//disable SPI0 clock
			*InterruptEnableClear0 = Interrupt0_SPI1;
			*SPI1InterruptClear = 0x0F;	//disable interrupts
			
			*IOConfigPIO1_15 = (*IOConfigPIO1_15 & ~IOConfigPIO_FunctionMask);
			*IOConfigPIO0_22 = (*IOConfigPIO0_22 & ~IOConfigPIO_FunctionMask);
			*IOConfigPIO0_21 = (*IOConfigPIO0_9 & ~IOConfigPIO_FunctionMask);
			*IOConfigPIO1_19 = (*IOConfigPIO1_19 & ~IOConfigPIO_FunctionMask);
		}
		
		/*
			InterruptFreeEnter();
			
				IOCore.flushQueue((IOCore::TaskQueueItem**)&IOCore.spiCurrentTask);
			
			InterruptFreeLeave();
		*/
	}
}
