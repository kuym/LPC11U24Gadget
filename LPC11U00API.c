#include <LPC11U00API.h>
#include <LPC11U00.h>
#include <string.h>
#include <stdlib.h>


#define HARDWARE_EXTERNAL_CRYSTAL_FREQUENCY (12000000UL)


static struct LPC11U00APIState
{
	unsigned int t1Millisecond;
	unsigned int t8Microseconds;

} gLPC11U00APIState;




static void			System_strobeClockUpdateEnable(u32 volatile* const updateEnable)
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

unsigned int	SystemGetMainClockFrequency(void)
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
unsigned int	SystemGetCoreFrequency(void)
{
	//note no /0 check, but I would be very alarmed if this code could excecute with a stopped core :-/
	return(SystemGetMainClockFrequency() / *MainBusDivider);
}


void			SystemSetupSystemPLL(void)
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

	gLPC11U00APIState.t1Millisecond = SystemGetCoreFrequency() / 18000UL;
	gLPC11U00APIState.t8Microseconds = SystemGetCoreFrequency() / 6000000UL;	// @@needs adjustment
}


void			SystemSetupUSBPLL(void)
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




int		NumberFormatterFormat(char* output, unsigned int number, int fractionBits, NumberFormatterBase base)
{
	(void)fractionBits;	// not currently supported

	static char const charTable[] = "0123456789ABCDEF";
	char buf[11];
	int len = 0;
	int neg = 0;
	
	if(base == NumberFormatter_DecimalSigned)
	{
		base = NumberFormatter_DecimalUnsigned;
		if(((int)number) < 0)
		{
			neg = 1;
			number = -number;
		}
	}
	else if(base == NumberFormatter_Character)
	{
		*output++ = (unsigned char)number;
		return(1);
	}
	
	do
	{
		LPCuidivResult r = (*LPC11U00ROMAPI)->divider->uidivmod(number, (unsigned int)base);
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




int				CircularBuffer_L(CircularBuffer* cb)
{
	return(cb->_p[0]);
}
unsigned char*	CircularBuffer_T(CircularBuffer* cb)
{
	return((((unsigned char*)cb->_p) + (3 * sizeof(unsigned short))) + cb->_p[1]);
}
int				CircularBuffer_U(CircularBuffer* cb)
{
	return(cb->_p[2]);
}

unsigned char*	CircularBuffer_h(CircularBuffer* cb)
{
	return(CircularBuffer_T(cb) + cb->_p[2] - (((cb->_p[1] + cb->_p[2]) >= cb->_p[0])? cb->_p[0] : 0));
}
int				CircularBuffer_f(CircularBuffer* cb)
{
	return(cb->_p[0] - cb->_p[2]);
}
int				CircularBuffer_tb(CircularBuffer* cb)
{
	return(((cb->_p[1] + cb->_p[2]) < cb->_p[0])? cb->_p[2] : (cb->_p[0] - cb->_p[1]));
}
int				CircularBuffer_hb(CircularBuffer* cb)
{
	return(((cb->_p[1] + cb->_p[2]) < cb->_p[0])? (cb->_p[0] - cb->_p[2] - cb->_p[1]) : (cb->_p[0] - cb->_p[2]));
}

void			CircularBuffer_Tincrement(CircularBuffer* cb, int increment)
{
	int v = cb->_p[1] + increment;
	if(v >= cb->_p[0])	v -= cb->_p[0];
	cb->_p[1] = v;
	cb->_p[2] -= increment;
}
void			CircularBuffer_hincrement(CircularBuffer* cb, int increment)
{
	cb->_p[2] += increment;
}



int						CircularBufferInit(CircularBuffer* cb, int bufferSize)
{
	CircularBufferDeinit(cb);

	// crop to size for this implementation. This is adjustable
	bufferSize = (unsigned short)bufferSize;
	
	// operator new may return 0 in this ABI
	interruptsDisabled();
	cb->_p = (unsigned short*)malloc((3 * sizeof(unsigned short)) + bufferSize);
	
	if(cb->_p == 0)
		return(0);
	
	((unsigned short*)cb->_p)[0] = bufferSize;
	((unsigned short*)cb->_p)[1] = 0;
	((unsigned short*)cb->_p)[2] = 0;
	
	interruptsEnabled();
	return(1);
}

void						CircularBufferDeinit(CircularBuffer* cb)
{
	interruptsDisabled();
	if(cb->_p != 0)
	{
		free(cb->_p);
		cb->_p = 0;
	}
	interruptsEnabled();
}

void						CircularBufferReset(CircularBuffer* cb)
{
	interruptsDisabled();
	cb->_p[2] = 0;
	interruptsEnabled();
}

int							CircularBufferRead(CircularBuffer* cb, unsigned char* out, int length)
{
	interruptsDisabled();
	
	if(length < 0)	length = 0;
	int count = 0, b2, b1 = CircularBuffer_tb(cb);
	if(length < b1)	b1 = length;
	if(out != 0)
		memcpy(out, CircularBuffer_T(cb), b1);
	CircularBuffer_Tincrement(cb, b1);
	out += b1;
	length -= b1;
	count += b1;
	
	if((length > 0) && ((b2 = CircularBuffer_tb(cb)) > 0))
	{
		if(length < b2)	b2 = length;
		if(out != 0)
			memcpy(out, CircularBuffer_T(cb), b2);
		count += b2;
		CircularBuffer_Tincrement(cb, b2);
	}
	
	interruptsEnabled();
	return(count);
}

unsigned char				CircularBufferReadByte(CircularBuffer* cb)
{
	interruptsDisabled();
	unsigned char b = 0;
	if(CircularBuffer_tb(cb) > 0)
	{
		b = *CircularBuffer_T(cb);
		CircularBuffer_Tincrement(cb, 1);
	}
	interruptsEnabled();
	return(b);
}

int							CircularBufferWrite(CircularBuffer* cb, unsigned char const* in, int length)
{
	interruptsDisabled();
	
	if(length < 0)	length = 0;
	int count = 0, b2, b1 = CircularBuffer_hb(cb);
	if(length < b1)	b1 = length;
	memcpy(CircularBuffer_h(cb), in, b1);
	CircularBuffer_hincrement(cb, b1);
	in += b1;
	length -= b1;
	count += b1;
	
	if((length > 0) && ((b2 = CircularBuffer_hb(cb)) > 0))
	{
		if(length < b2)	b2 = length;
		memcpy(CircularBuffer_h(cb), in, b2);
		count += b2;
		CircularBuffer_hincrement(cb, b2);
	}
	
	interruptsEnabled();
	return(count);
}

int							CircularBufferWriteByte(CircularBuffer* cb, unsigned char b)
{
	int v = 0;
	interruptsDisabled();
	if(CircularBuffer_hb(cb) > 0)
	{
		*CircularBuffer_h(cb) = b;
		CircularBuffer_hincrement(cb, 1);
		v = 1;
	}
	interruptsEnabled();
	return(v);
}






void		SystemDelayMs(unsigned int ms)
{
	while(ms--)
		for(unsigned int i = gLPC11U00APIState.t1Millisecond; i; i--)
			__asm__ volatile ("nop \n nop \n nop \n nop \n" ::);
}


void		UARTStart(unsigned int baudRate, unsigned int mode)
{
	if(baudRate > 0)
	{
		*ClockControl |= ClockControl_UART;
		*UARTClockDivider = 1;
		
		*UARTLineControl = (mode & 0xFF) | UARTLineControl_DivisorLatch;	//apply mode and enter DLAB state
		
		unsigned int divider = SystemGetCoreFrequency() / (16 * baudRate);

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

void		UARTWriteStringLengthSync(char const* msg, int length)
{
	while(((length == -1) && *msg) || (length-- > 0))
	{
		while(!(*UARTLineStatus & UARTLineStatus_TxHoldingRegisterEmpty));
		*UARTData = *msg++;
	}
}

void		UARTWriteStringSync(char const* msg)
{
	UARTWriteStringLengthSync(msg, -1);
}

void		UARTWriteIntSync(unsigned int value, NumberFormatterBase base)
{
	char buffer[34];
	int len = NumberFormatterFormat(buffer, value, 0, base);
	char* b = buffer;
	while(len--)
	{
		while(!(*UARTLineStatus & UARTLineStatus_TxHoldingRegisterEmpty));
		*UARTData = *b++;
	}
}

void		UARTWriteHexIntSync(unsigned int value, int minDigits)
{
	int length = (value >= 0x10000)? (value >= 0x1000000)? 4 : 3 : (value >= 0x100)? 2 : 1;
	value = __builtin_bswap32(value);
	UARTWriteHexDumpSync(((unsigned char*)&value) + (4 - length), (length > minDigits)? length : minDigits);
}

void		UARTWriteHexDumpSync(unsigned char const* data, unsigned int length)
{
	static char const* hex = "0123456789ABCDEF";
	while(length--)
	{
		unsigned char b = *data++;
		while(!(*UARTLineStatus & UARTLineStatus_TxHoldingRegisterEmpty));
		*UARTData = hex[b >> 4];
		while(!(*UARTLineStatus & UARTLineStatus_TxHoldingRegisterEmpty));
		*UARTData = hex[b & 0xF];
	}
}

