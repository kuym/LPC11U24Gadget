#include <LPC11U00API.h>
#include <LPC11U00.h>
#include <string.h>

using namespace LPC11U00;
using namespace LPC11U00::ROMDivider;
using namespace LPC11U00::ROMAPI;

static unsigned int g1Millisecond = (12e6 / 1000 / 18);		// number of spinloop cycles in 1ms
static unsigned int g8Microseconds = (12e6 / 1000000 / 6);	// number of spinloop cycles in 8us

void delay(unsigned int ms)
{
	while(ms--)
		for(unsigned int i = g1Millisecond; i; i--)
			__asm__ volatile ("nop \n nop \n nop \n nop \n" ::);
}

void delayMicro(unsigned int us)
{
	us = (us + 7) >> 3;
	while(us--)
		for(unsigned int i = g8Microseconds; i; i--)
			__asm__ volatile ("nop \n nop \n nop \n nop \n" ::);
}

void		vmemset(void volatile* dest, unsigned int value, unsigned int length)
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

// Inner workings: seven quantities are defined, three primary and
//   four derived.  They are:
//   L:  Length of circular buffer; stays constant throughout use
//   T:  tail position, range [0, L)
//   U:  used space, range [0, L)
//   h:  head position, range [0, L); derived property
//   f:  free space, range [0, L); derived
//   tb: tail blocksize, range [0, L); how big a span could be read contiguously, derived
//   hb: head blocksize, range [0, L); how big a span could be written contiguously, derived
//
// examples:
//
// [ ] [ ] [T] [X] [X] [X] [h] [ ] [ ] [ ]	// L = 10, T = 2, U = 4		"forwards"
//   h = 6; f = 6; tb = 4; hb = 4
// [X] [h] [ ] [ ] [ ] [ ] [ ] [T] [X] [X]	// L = 10, T = 7, U = 4		"backwards"
//   h = 1; f = 6; tb = 3; hb = 6
// [ ] [ ] [ ] [ ] [b] [ ] [ ] [ ] [ ] [ ]	// L = 10, T = 4, U = 0		"empty"
//   h = 4; f = 10; tb = 0; hb = 6
// [X] [X] [X] [B] [X] [X] [X] [X] [X] [X]	// L = 10, T = 3, U = 10	"full"
//   h = 3, f = 0; tb = 7; hb = 0
//
// the derived relationships of each of f, h, tb, hb:
//
// .:   h = ((T + U) > L)? (T + U - L) : (T + U)
//      f = L - U
//      tb = ((T + U) < L)? U : (L - T)
//      hb = ((T + U) < L)? (L - U - T) : (L - U);

		CircularBuffer::CircularBuffer(void):
			_p(0)
{}
		CircularBuffer::~CircularBuffer(void)
{
	dealloc();
}

bool	CircularBuffer::alloc(int bufferSize)
{
	dealloc();

	// crop to size for this implementation. This is adjustable
	bufferSize = (unsigned short)bufferSize;
	
	// operator new may return 0 in this ABI
	interruptsDisabled();
	_p = (unsigned short*)new unsigned char[(3 * sizeof(unsigned short)) + bufferSize];
	if(_p == 0)
		return(false);
	
	((unsigned short*)_p)[0] = bufferSize;
	((unsigned short*)_p)[1] = 0;
	((unsigned short*)_p)[2] = 0;
	
	interruptsEnabled();
	return(true);
}

void	CircularBuffer::dealloc(void)
{
	interruptsDisabled();
	if(_p != 0)
	{
		delete[] _p;
		_p = 0;
	}
	interruptsEnabled();
}

void	CircularBuffer::reset(void)
{
	interruptsDisabled();
	_p[2] = 0;
	interruptsEnabled();
}

int		CircularBuffer::read(unsigned char* out, int length)
{
	interruptsDisabled();
	
	if(length < 0)	length = 0;
	int count = 0, b2, b1 = tb();
	if(length < b1)	b1 = length;
	memcpy(out, T(), b1);
	T(b1);
	out += b1;
	length -= b1;
	count += b1;
	
	if((length > 0) && ((b2 = tb()) > 0))
	{
		if(length < b2)	b2 = length;
		memcpy(out, T(), b2);
		count += b2;
		T(b2);
	}
	
	interruptsEnabled();
	return(count);
}

unsigned char	CircularBuffer::readByte(void)
{
	interruptsDisabled();
	unsigned char b = 0;
	if(tb() > 0)
	{
		b = *T();
		T(1);
	}
	interruptsEnabled();
	return(b);
}


int		CircularBuffer::write(unsigned char const* in, int length)
{
	interruptsDisabled();
	
	if(length < 0)	length = 0;
	int count = 0, b2, b1 = hb();
	if(length < b1)	b1 = length;
	memcpy(h(), in, b1);
	h(b1);
	in += b1;
	length -= b1;
	count += b1;
	
	if((length > 0) && ((b2 = hb()) > 0))
	{
		if(length < b2)	b2 = length;
		memcpy(h(), in, b2);
		count += b2;
		h(b2);
	}
	
	interruptsEnabled();
	return(count);
}

void	CircularBuffer::writeByte(unsigned char b)
{
	interruptsDisabled();
	if(hb() > 0)
	{
		*h() = b;
		h(1);
	}
	interruptsEnabled();
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
	
	// update the time constants for the new clock frequency
	g1Millisecond = getCoreFrequency() / 18000UL;
	g8Microseconds = getCoreFrequency() / 6000000UL;	// @@needs adjustment
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
void				UART::writeHexIntSync(unsigned int value, int minLength)
{
	int length = (value >= 0x10000)? (value >= 0x1000000)? 4 : 3 : (value >= 0x100)? 2 : 1;
	value = __builtin_bswap32(value);
	writeHexDumpSync(((unsigned char*)&value) + (4 - length), (length > minLength)? length : minLength);
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
	}
}




////////////////////////////////////////////////////////////////
// I2C

static unsigned int gTaskID = 0;

struct TaskQueueItem
{
	TaskQueueItem*	next;
	int				id;
	TaskCallback	completion;
	void*			context;

	TaskQueueItem(TaskCallback callback, void* userContext):
		next(0),
		completion(callback),
		context(userContext)
	{
		id = ++gTaskID;
	}
};

void	queueItem(TaskQueueItem** list, TaskQueueItem* task)
{
	while(*list)	list = &((*list)->next);
	*list = task;
}
void	flushQueue(TaskQueueItem** list)
{
	while(*list)
	{
		TaskQueueItem** t = list;
		list = &((*list)->next);
		delete[] (unsigned char*)*t;
		*t = 0;
	}
}

struct I2CTask: TaskQueueItem
{
	unsigned short	len;
	unsigned short	idx;
	unsigned char	data[1];

	void*			operator new(size_t s, size_t length)
	{
		return(new unsigned char[s + 1 + length]);
	}
	void			operator delete(void* p)
	{
		delete[] (unsigned char*)p;
	}

					I2CTask(unsigned int length, TaskCallback callback, void* userContext):
						TaskQueueItem(callback, userContext),
						len(length),
						idx(0)
	{
	}
};


static I2CTask* gI2cCurrentTask = 0;

bool				i2cRepeatedStart(I2CTask* currentTask)
{
	return(		(currentTask->next != 0)
				&& ((((I2CTask*)(currentTask->next))->data[0] | 1) == (currentTask->data[0] | 1))	//same slave?
			);
}

void				i2cCompletePacket(I2CTask* currentTask, bool success)
{
	gI2cCurrentTask = (I2CTask*)currentTask->next;
	
	if(currentTask->completion != 0)
		currentTask->completion(	currentTask->id,
									(success && (currentTask->data[0] & 1))? (currentTask->data + 1) : 0,
									(currentTask->data[0] & 1)? currentTask->len : currentTask->idx,
									currentTask->context
								);

	delete currentTask;

	if(gI2cCurrentTask != 0)
		*I2CControlSet = I2CControlSet_StartCondition;
}

extern "C"
void	I2C_IRQHandler(void)
{
	int status = *I2CStatus;
	I2CTask* currentTask = gI2cCurrentTask;

	switch(status)
	{
	case 0x08:	//start bit sent
	case 0x10:	//repeated start
		*I2CData = currentTask->data[0];
		*I2CControlClear = I2CControlSet_StartCondition;
		break;
		
	case 0x20:	//write address NACKed, stop
	case 0x48:	//read address NACKed, stop
		*I2CControlSet = I2CControlSet_StopCondition;
		i2cCompletePacket(currentTask, false);
		break;
		
	case 0x40:	//read address ACKed, ready to read
		if(currentTask->len > 1)	*I2CControlSet = I2CControlSet_Ack;
		break;
	
	case 0x18:	//write address ACKed, ready to write
	case 0x28:	//byte sent, ACK received
		if(currentTask->idx < currentTask->len)
		{
			*I2CData = currentTask->data[++currentTask->idx];
			break;
		}
		goto repeatedStart;
		
	case 0x58:	//byte received, NACK sent
		currentTask->data[++currentTask->idx] = *I2CData;

	repeatedStart:
		if(i2cRepeatedStart(currentTask))	*I2CControlSet = I2CControlSet_StartCondition;
		else								*I2CControlSet = I2CControlSet_StopCondition;
		i2cCompletePacket(currentTask, true);
		break;
		
	case 0x30:	//byte sent, NACK received
		*I2CControlSet = I2CControlSet_StopCondition;
		i2cCompletePacket(currentTask, false);
		break;
		
	case 0x50:	//byte received, ACK sent
		currentTask->data[++currentTask->idx] = *I2CData;
		
		// +2 accounts for the address byte and the fact that we have to decide the response one byte ahead
		if((currentTask->idx + 2) < currentTask->len)	*I2CControlSet = I2CControlSet_Ack;
		else											*I2CControlClear = I2CControlSet_Ack;
		
		break;
	
	case 0x00:
		*I2CControlSet = I2CControlSet_StopCondition;
		// (intentional)

	case 0xF8:
		//protocol errors!
		// (intentional)

	default:
		// (intentional)

	case 0x38:	//arbitration loss, abort
		i2cCompletePacket(currentTask, false);
		break;
	}
	
	*I2CControlClear = I2CControlSet_Interrupt;
}

void		I2C::start(int bitRate)
{
	*PeripheralnReset &= ~PeripheralnReset_I2C;	//assert reset
	
	interruptsDisabled();

	//empty the queue in any case
	flushQueue((TaskQueueItem**)&gI2cCurrentTask);
	
	if(bitRate > 0)
	{
		*ClockControl |= ClockControl_I2C;
		*PeripheralnReset |= PeripheralnReset_I2C;	//deassert reset
		
		*IOConfigPIO0_4 = (*IOConfigPIO0_4 & ~IOConfigPIO0_4_Supported) | IOConfigPIO0_4_Function_SCL | IOConfigPIO0_4_I2CMode_StandardFast;
		*IOConfigPIO0_5 = (*IOConfigPIO0_5 & ~IOConfigPIO0_5_Supported) | IOConfigPIO0_5_Function_SDA | IOConfigPIO0_5_I2CMode_StandardFast;
		
		*I2CControlClear = I2CControlSet_Ack | I2CControlSet_Interrupt
							| I2CControlSet_StartCondition | I2CControlSet_EnableI2C;
		
		unsigned int bitHalfPeriod = (12000000UL / bitRate) >> 1;	// system.getCoreFrequency()
		//@@depending on the time-constant of the bus (1 / (pull-up resistance * capacitance)),
		//  the low-time should be smaller and the high-time should be higher
		*I2CClockHighTime = bitHalfPeriod;
		*I2CClockLowTime = bitHalfPeriod;
		
		//IOCore.irqI2C = IO_onI2CInterrupt;
		//@@enable i2c interrupt
		
		//enable interrupt before enabling I2C state machine:
		*InterruptEnableSet0 = Interrupt0_I2C;
		*I2CControlSet = I2CControlSet_EnableI2C;
	}
	else
	{
		*IOConfigPIO0_4 = (*IOConfigPIO0_4 & ~IOConfigPIO0_4_Supported);
		*IOConfigPIO0_5 = (*IOConfigPIO0_5 & ~IOConfigPIO0_5_Supported);
		
		*I2CControlClear = I2CControlSet_Ack | I2CControlSet_Interrupt
							| I2CControlSet_StartCondition | I2CControlSet_EnableI2C;
		
		//shut down I2C clock
		*ClockControl &= ~ClockControl_I2C;
		*InterruptEnableClear0 = Interrupt0_I2C;
	}
	interruptsEnabled();
}

int		I2C::write(unsigned char address, void* data, unsigned int length, TaskCallback completion, void* context)
{
	if(!(*PeripheralnReset | PeripheralnReset_I2C))	//if in reset mode, I2C system is inactive and this job should fail.
		return(0);
	
	I2CTask* i2cTask = new(length) I2CTask(length, completion, context);
	i2cTask->data[0] = address;
	if(!(address & 1))
		memcpy(i2cTask->data + 1, data, length);
	
	interruptsDisabled();
		bool mustStart = (gI2cCurrentTask == 0);
		queueItem((TaskQueueItem**)&gI2cCurrentTask, i2cTask);
		
		if(mustStart)
			*I2CControlSet = I2CControlSet_StartCondition;
		
	interruptsEnabled();
	
	return(i2cTask->id);
}

