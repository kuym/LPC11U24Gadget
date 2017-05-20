#include <LPC11U00.h>
#include <LPC11U00API.h>
#include <USBAPI.h>
#include <USBCDC.h>
#include <string.h>
#include <stdlib.h>

#define MIN(a, b)	((a < b)? (a) : (b))

////////////////////////////////////////////////////////////////

static LPCUSBDescriptorDevice const __attribute__((aligned(4))) kDeviceDescriptor =
{
	.header					= {.size = sizeof(LPCUSBDescriptorDevice), .type = LPCUSBDescriptorType_Device},

	.usbSpecification		= 0x0200,
	.deviceClass			= 0x02,
	.subClass				= 0x00,
	.protocol				= 0x00,

	.endpoint0Size			= 64,

	.vendorID				= 0x80b1,
	.productID				= 0x1234,
	.releaseNumber			= 0x0101,

	.manufacturerStrIndex	= 0x01,
	.productStrIndex		= 0x02,
	.serialNumStrIndex		= 0x03,

	.numberOfConfigurations	= 0x01
};

struct __attribute__((packed)) CDCACMConfigurationDescriptor
{
	LPCUSBDescriptorConfigurationHeader		header;
	LPCUSBDescriptorInterface				interface0;
	LPCUSBCDCDescriptorFunctionalHeader		cdcFunctional0;
	LPCUSBCDCDescriptorFunctionalACM		cdcFunctionalACM;
	LPCUSBCDCDescriptorFunctionalHeader		cdcFunctional1;
	LPCUSBCDCDescriptorFunctionalUnion		cdcFunctionalUnion;
	LPCUSBDescriptorEndpoint				endpoint0_0;
	LPCUSBDescriptorInterface				interface1;
	LPCUSBDescriptorEndpoint				endpoint1_0;
	LPCUSBDescriptorEndpoint				endpoint1_1;

	unsigned char							_sentinel;

} const kCDCACMConfigurationDescriptor __attribute__((aligned(4))) =
{
	.header =
	{
		.header = {.size = sizeof(LPCUSBDescriptorConfigurationHeader), .type = LPCUSBDescriptorType_Configuration},

		.totalConfigurationSize = sizeof(struct CDCACMConfigurationDescriptor) - 1,
		.totalInterfaces		= 2,
		.configurationNumber	= 1,
		.configurationStrIndex	= 0,
		.configAttributes		= (0x80),
		.maxPowerConsumption	= (100 / 2),
	},
	.interface0 =
	{
		.header = {.size = sizeof(LPCUSBDescriptorInterface), .type = LPCUSBDescriptorType_Interface},
		.interfaceNumber	= 0,
		.alternateSetting	= 0,
		.totalEndpoints		= 1,
		.interfaceClass		= 2,
		.subClass			= 2,
		.protocol			= 1,
		.interfaceStrIndex	= 0,
	},
	.cdcFunctional0 =
	{
		.header = {.size = sizeof(LPCUSBCDCDescriptorFunctionalHeader), .type = LPCUSBDescriptorType_CSInterface},
		.subType = 0,
		.cdcSpecification = 0x0110
	},
	.cdcFunctionalACM =
	{
		.header = {.size = sizeof(LPCUSBCDCDescriptorFunctionalACM), .type = LPCUSBDescriptorType_CSInterface},
		.subType = 0x02,
		.capabilities = 0x02
	},
	.cdcFunctional1 =
	{
		.header = {.size = sizeof(LPCUSBCDCDescriptorFunctionalHeader), .type = LPCUSBDescriptorType_CSInterface},
		.subType = 0x06,
		.cdcSpecification = 0x0100
	},
	.cdcFunctionalUnion =
	{
		.header = {.size = sizeof(LPCUSBCDCDescriptorFunctionalUnion), .type = LPCUSBDescriptorType_CSInterface},
		.subType = 0x01,
		.masterInterfaceNumber = 0x00,
		.slaveInterfaceNumber = 0x01
	},
	.endpoint0_0 =
	{
		.header = {.size = sizeof(LPCUSBDescriptorEndpoint), .type = LPCUSBDescriptorType_Endpoint},
		.endpointAddress	= 0x82,
		.attributes			= (LPCUSBEndpointType_Interrupt | LPCUSBEndpointAttribute_NoSync | LPCUSBEndpointUsage_Data),
		.endpointSize		= 8,
		.pollingIntervalMS	= 2,
	},
	
	.interface1 =
	{
		.header = {.size = sizeof(LPCUSBDescriptorInterface), .type = LPCUSBDescriptorType_Interface},
		.interfaceNumber	= 1,
		.alternateSetting	= 0,
		.totalEndpoints		= 2,
		.interfaceClass		= 10,
		.subClass			= 0,
		.protocol			= 0,
		.interfaceStrIndex	= 0,
	},
	.endpoint1_0 =
	{
		.header = {.size = sizeof(LPCUSBDescriptorEndpoint), .type = LPCUSBDescriptorType_Endpoint},
		.endpointAddress	= 0x03,
		.attributes			= (LPCUSBEndpointType_Bulk | LPCUSBEndpointAttribute_NoSync | LPCUSBEndpointUsage_Data),
		.endpointSize		= 64,
		.pollingIntervalMS	= 0,
	},
	.endpoint1_1 =
	{
		.header = {.size = sizeof(LPCUSBDescriptorEndpoint), .type = LPCUSBDescriptorType_Endpoint},
		.endpointAddress	= 0x83,
		.attributes			= (LPCUSBEndpointType_Bulk | LPCUSBEndpointAttribute_NoSync | LPCUSBEndpointUsage_Data),
		.endpointSize		= 64,
		.pollingIntervalMS	= 0,
	},
	._sentinel = 0
};


static unsigned char const __attribute__((aligned(4))) kUSBStrings[] =
	"\x04\x03" "\x09\x04"
	"\x12\x03" "W\0" "h\0" "o\0" "m\0" "e\0" "v\0" "e\0" "r\0"
	"\x0E\x03" "G\0" "a\0" "d\0" "g\0" "e\0" "t\0"
	"\x0A\x03" "1\0" "2\0" "3\0" "4\0"
	;


void		USB_IRQHandler(void)
{
	LPCUSBInterrupt();
}


static volatile unsigned int gMS = 0;

void SysTick_Handler(void)
{
	gMS++;
}



typedef struct EncoderState
{
	unsigned short	ioState;
	unsigned int	count;
	unsigned int	lastCount;

	void			(*callback)(void* context, unsigned int newCount, int pushButtonState);
	void*			callbackContext;

} EncoderState;


void	EncoderInit(EncoderState* state)
{
	state->ioState = ((!!GPIO0[16]) << 1) | ((!!GPIO0[23]) << 0);
	state->count = 0;
	state->lastCount = 0;
}

void	onEncoderRotation(void* context)
{
	EncoderState* state = (EncoderState*)context;
	
	unsigned int currentIOState = ((!!GPIO0[16]) << 1) | ((!!GPIO0[23]) << 0);

	currentIOState ^= (currentIOState >> 1);

	signed int direction = (((signed int)currentIOState << 30) - (((signed int)state->ioState & 3) << 30)) >> 30;

	if((direction == 1) || (direction == -1))
	{
		state->ioState = (state->ioState & ~3) | currentIOState;
		state->count += direction;
	}

	if(state->callback != 0)
		state->callback(state->callbackContext, state->count, ((state->ioState >> 2) & 1));

	// B A
	// 0 0	->	0 0
	// 0 1	->	0 1
	// 1 1	->	1 0
	// 1 0	->	1 1
	// 0 0	->	0 0
}

void	onEncoderButton(void* context)
{
	EncoderState* state = (EncoderState*)context;
	
	if(state->callback != 0)
		state->callback(state->callbackContext, state->count, !((state->ioState >> 2) & 1));
	
	if(GPIO0[12])
		state->ioState &= ~4;
	else
		state->ioState |= 4;
}

static EncoderState gEncoderState = {0};


void	EncoderAddHandler(void (*handler)(void* context, unsigned int newCount, int pushButtonState), void* context)
{
	gEncoderState.callback = handler;
	gEncoderState.callbackContext = context;
}


void	FLEX_INT0_IRQHandler(void)
{
	*PinInterruptConfigIST = 7;
	*InterruptClearPending0 = Interrupt0_PinInt0;
	onEncoderButton(&gEncoderState);
}

void	FLEX_INT1_IRQHandler(void)
{
	*PinInterruptConfigIST = 7;
	*InterruptClearPending0 = Interrupt0_PinInt1;
	onEncoderRotation(&gEncoderState);
}

void	FLEX_INT2_IRQHandler(void)
{
	*PinInterruptConfigIST = 7;
	*InterruptClearPending0 = Interrupt0_PinInt2;
	onEncoderRotation(&gEncoderState);
}

static unsigned int lastCount = 0;
static int lastPushButtonState = 0;

void encoderHandler(void* context, unsigned int newCount, int pushButtonState)
{
	USBCDCDevice* cdc = (USBCDCDevice*)context;

	if(newCount < lastCount)
		USBCDCWrite(cdc, (unsigned char const*)"L", 1);
	if(newCount > lastCount)
		USBCDCWrite(cdc, (unsigned char const*)"R", 1);
	if(pushButtonState != lastPushButtonState)
		USBCDCWrite(cdc, (unsigned char const*)"B", 1);
}




int main(void)
{
	__asm__ volatile ("cpsid i"::);

	*ClockControl = (ClockControl_Core | ClockControl_ROM | ClockControl_RAM
		| ClockControl_FlashRegisters | ClockControl_FlashStorage | ClockControl_GPIO | (1 << 19)	// pin interrupt
		| ClockControl_IOConfig);

	*IOConfigPIO0_3 = (*IOConfigPIO0_3 & ~IOConfigPIO_FunctionMask) | IOConfigPIO0_3_Function_PIO;


	*GPIO0Dir |= (1 << 2);	// pin A0


	// Set up PLL to 48MHz, USB PLL to 48MHz also
	SystemSetupSystemPLL();
	SystemDelayMs(5);	// stabilize

	UARTStart(115200, UART_Default);


	*SysTickControl = SysTickControl_Enabled | SysTickControl_InterruptEnabled | SysTickControl_ClockSource;
	*SysTickReload = 48000000 / 1000;
	*SysTickValue = 0;
	

	*PinInterruptControl0 = 12;	// button
	*PinInterruptControl1 = 16;	// phase A
	*PinInterruptControl2 = 23;	// phase B

	//*PinInterruptConfigENRL = 7;
	//*PinInterruptConfigSIENR = 7;
	//*PinInterruptConfigSIENF = 7;

	*InterruptEnableSet0 |= (Interrupt0_PinInt0 | Interrupt0_PinInt1 | Interrupt0_PinInt2);


	EncoderInit(&gEncoderState);
	
	

	SystemSetupUSBPLL();

	//USB setup
	LPCUSBInit();

	LPCUSBSetDescriptor((unsigned char const*)&kDeviceDescriptor, sizeof(kDeviceDescriptor));
	LPCUSBSetDescriptor((unsigned char const*)&kCDCACMConfigurationDescriptor, sizeof(kCDCACMConfigurationDescriptor));
	LPCUSBSetDescriptor((unsigned char const*)&kUSBStrings, sizeof(kUSBStrings));

	USBCDCDevice cdcDevice;
	USBCDCInit(&cdcDevice, 256, 256);

	LPCUSBRegisterHandler((unsigned char const*)&kCDCACMConfigurationDescriptor, &USBCDCACMHandler, &cdcDevice);
	LPCUSBRegisterHandler((unsigned char const*)&kCDCACMConfigurationDescriptor.endpoint1_0, &USBCDCACMHandler, &cdcDevice);
	LPCUSBRegisterHandler((unsigned char const*)&kCDCACMConfigurationDescriptor.endpoint1_1, &USBCDCACMHandler, &cdcDevice);

	LPCUSBErrorCode error = LPCUSBStart();
	if(error != LPCUSBErrorCode_OK)
	{
		UARTWriteStringSync("\nStart() error: ");
		UARTWriteIntSync(error, NumberFormatter_Hexadecimal);
	}

	interruptsEnabled();
	
	LPCUSBConnect();
	UARTWriteStringSync("\nConnected");

	EncoderAddHandler(&encoderHandler, &cdcDevice);

	unsigned int deadline = gMS + 1000;

	unsigned char echoBuffer[256];
	
	while(1)
	{
		if(USBCDCConnected(&cdcDevice))
		{
			int length;
			while((length = USBCDCBytesAvailable(&cdcDevice)) > 0)
			{
				unsigned int bytesRead = USBCDCRead(&cdcDevice, echoBuffer, 256);

				if(bytesRead > 0)
				{
					UARTWriteStringSync("\nw(");
					UARTWriteIntSync((unsigned int)(2 * bytesRead + 5), NumberFormatter_DecimalUnsigned);
					UARTWriteStringSync(")");
					USBCDCWrite(&cdcDevice, (unsigned char const*)"\r\n< ", 4);
					USBCDCWrite(&cdcDevice, echoBuffer, bytesRead);
					USBCDCFlush(&cdcDevice);
				}
			}

			if(gMS > deadline)
			{
				UARTWriteStringSync("\nw(5)*");
				
				deadline = gMS + 1000;
				USBCDCWrite(&cdcDevice, (unsigned char const*)"\r\n> Hello", 9);
				USBCDCFlush(&cdcDevice);
			}
		}

		__asm__ volatile ("wfi"::);
	}
}
