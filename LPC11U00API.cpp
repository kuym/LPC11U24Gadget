#include <LPC11U00API.h>
#include <LPC11U00.h>
#include <LPCUSB.h>

using namespace LPC11U00;
using namespace LPCUSB;

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
		*p++ = 0;
}

////////////////////////////////////////////////////////////////
// NumberFormatter formats numbers into text

int		NumberFormatter::format(char* output, unsigned int number, int fractionBits, Base base)
{
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
// System

#define HARDWARE_EXTERNAL_CRYSTAL_FREQUENCY	(12000000UL)	//a property of the board

void			System_strobeClockUpdateEnable(REGISTER updateEnable)
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
