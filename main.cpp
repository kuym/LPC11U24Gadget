#include <LPC11U00.h>
#include <LPC11U00API.h>
#include <LPCUSB.h>
#include <USBAPI.h>
#include <USBCDC.h>
#include <string.h>

using namespace LPC11U00;
using namespace LPCUSB;

////////////////////////////////////////////////////////////////

static USBDescriptorDevice const __attribute__((aligned(4))) kDeviceDescriptor =
{
	.header					= {.size = sizeof(USBDescriptorDevice), .type = DescriptorType_Device},

	.usbSpecification		= 0x0110,
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
	USBDescriptorConfigurationHeader	header;
	USBDescriptorInterface				interface0;
	CDCDescriptorFunctionalHeader		cdcFunctional0;
	CDCDescriptorFunctionalACM			cdcFunctionalACM;
	CDCDescriptorFunctionalHeader		cdcFunctional1;
	CDCDescriptorFunctionalUnion		cdcFunctionalUnion;
	USBDescriptorEndpoint				endpoint0_0;
	USBDescriptorInterface				interface1;
	USBDescriptorEndpoint				endpoint1_0;
	USBDescriptorEndpoint				endpoint1_1;

	unsigned char						_sentinel;

} const kCDCACMConfigurationDescriptor __attribute__((aligned(4))) =
{
	.header =
	{
		.header = {.size = sizeof(USBDescriptorConfigurationHeader), .type = DescriptorType_Configuration},

		.totalConfigurationSize = sizeof(CDCACMConfigurationDescriptor) - 1,
		.totalInterfaces		= 2,
		.configurationNumber	= 1,
		.configurationStrIndex	= 0,
		.configAttributes		= (0x80),
		.maxPowerConsumption	= (100 / 2),
	},
	.interface0 =
	{
		.header = {.size = sizeof(USBDescriptorInterface), .type = DescriptorType_Interface},
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
		.header = {.size = sizeof(CDCDescriptorFunctionalHeader), .type = DescriptorType_CSInterface},
		.subType = 0,
		.cdcSpecification = 0x0110
	},
	.cdcFunctionalACM =
	{
		.header = {.size = sizeof(CDCDescriptorFunctionalACM), .type = DescriptorType_CSInterface},
		.subType = 0x02,
		.capabilities = 0x02
	},
	.cdcFunctional1 =
	{
		.header = {.size = sizeof(CDCDescriptorFunctionalHeader), .type = DescriptorType_CSInterface},
		.subType = 0x06,
		.cdcSpecification = 0x0100
	},
	.cdcFunctionalUnion =
	{
		.header = {.size = sizeof(CDCDescriptorFunctionalUnion), .type = DescriptorType_CSInterface},
		.subType = 0x01,
		.masterInterfaceNumber = 0x00,
		.slaveInterfaceNumber = 0x01
	},
	.endpoint0_0 =
	{
		.header = {.size = sizeof(USBDescriptorEndpoint), .type = DescriptorType_Endpoint},
		.endpointAddress	= 0x82,
		.attributes			= (USBEndpointType_Interrupt | USBEndpointAttribute_NoSync | USBEndpointUsage_Data),
		.endpointSize		= 8,
		.pollingIntervalMS	= 2,
	},
	
	.interface1 =
	{
		.header = {.size = sizeof(USBDescriptorInterface), .type = DescriptorType_Interface},
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
		.header = {.size = sizeof(USBDescriptorEndpoint), .type = DescriptorType_Endpoint},
		.endpointAddress	= 0x03,
		.attributes			= (USBEndpointType_Bulk | USBEndpointAttribute_NoSync | USBEndpointUsage_Data),
		.endpointSize		= 64,
		.pollingIntervalMS	= 0,
	},
	.endpoint1_1 =
	{
		.header = {.size = sizeof(USBDescriptorEndpoint), .type = DescriptorType_Endpoint},
		.endpointAddress	= 0x83,
		.attributes			= (USBEndpointType_Bulk | USBEndpointAttribute_NoSync | USBEndpointUsage_Data),
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


struct __attribute__((aligned(4))) USBCDCDevice
{
	USBCDCLineCoding	lineCoding;
	unsigned int		expecting;
	unsigned int		connected;
};


ErrorCode	USBCDCACMHandler(void* context, USBDescriptorHeader const* endpoint, USBSetup const* setupPacket)
{
	unsigned int endpointAddress = (endpoint->type == DescriptorType_Endpoint)?
										((USBDescriptorEndpoint*)endpoint)->endpointAddress
										: 0;
	
	// debug
	if(endpointAddress == 0)
	{
		UART::writeSync("\ncls:");
	}
	else
	{
		UART::writeSync("\nep");
		UART::writeSync(endpointAddress, NumberFormatter::Hexadecimal);
		UART::writeSync(":");
	}
	// end debug

	USBCDCDevice* state = (USBCDCDevice*)context;

	if(setupPacket != 0)
	{
		switch(setupPacket->bRequest)
		{
		case USBCDC_Request_SetLineCoding:
			state->expecting = USBCDC_Request_SetLineCoding;
			UART::writeSync(" e:sl");
			return(ErrorCode_SendZeroLengthPacket);

		case USBCDC_Request_GetLineCoding:
			UART::writeSync(" gl");
			USB::Write(		0x80,
							(unsigned char*)&state->lineCoding,
							sizeof(USBCDCLineCoding)
						);
			return(ErrorCode_OK);

		case USBCDC_Request_SetControlLineState:
			if(setupPacket->wValue & USBCDC_ControlLine_DTEPresent)
				;
			if(setupPacket->wValue & USBCDC_ControlLine_ActivateCarrier)
				state->connected = 1;
			else
				state->connected = 0;

			return(ErrorCode_SendZeroLengthPacket);
		}
	}
	else if(endpointAddress == 0x00)
	{
		unsigned char buffer[64];
		unsigned int bytesRead = USB::Read(endpointAddress, buffer);
		
		switch(state->expecting)
		{
		case USBCDC_Request_SetLineCoding:
			if(bytesRead != sizeof(USBCDCLineCoding))
			{
				UART::writeSync(" size mismatch!");
				break;
			}
			memcpy(&state->lineCoding, buffer, sizeof(USBCDCLineCoding));
			UART::writeSync(" sl: ");
			UART::writeSync(state->lineCoding.dwDTERate, NumberFormatter::DecimalUnsigned);
			UART::writeSync(",");
			UART::writeSync(state->lineCoding.bDataBits, NumberFormatter::DecimalUnsigned);
			UART::writeSync(state->lineCoding.bParityType? "p" : "n");
			UART::writeSync(state->lineCoding.bCharFormat + 1, NumberFormatter::DecimalUnsigned);
			state->expecting = 0;
			break;

		default:
			UART::writeSync(" crd:");
			UART::writeSync(bytesRead, NumberFormatter::DecimalUnsigned);
			UART::writeSync(">");
			UART::writeHexDumpSync(buffer, bytesRead);

			UART::writeSync(" unhandled class EP0 OUT!");
			break;
		}
	}
	else if(endpointAddress == 0x03)	// endpoint 0x03 OUT
	{
		unsigned char buffer[64];
		unsigned int bytesRead = USB::Read(endpointAddress, buffer);
		
		UART::writeSync(" crd:");
		UART::writeSync(bytesRead, NumberFormatter::DecimalUnsigned);
		UART::writeSync(">");
		UART::writeHexDumpSync(buffer, bytesRead);

		// echo with '|'
		buffer[bytesRead++] = '|';

		USB::Write(		0x80 | endpointAddress,
						buffer,
						bytesRead
					);
	}
	else if(endpointAddress == 0x83)	// endpoint 0x83 IN
	{
		// anything additional to send in to the host?
	}

	return(ErrorCode_OK);
}

void	USB_IRQHandler(void)
{
	USB::Interrupt();
}

int main(void)
{
	__asm__ volatile ("cpsid i"::);

	*ClockControl = (ClockControl_Core | ClockControl_ROM | ClockControl_RAM
		| ClockControl_FlashRegisters | ClockControl_FlashStorage | ClockControl_GPIO
		| ClockControl_IOConfig);

	*IOConfigPIO0_3 = (*IOConfigPIO0_3 & ~IOConfigPIO_FunctionMask) | IOConfigPIO0_3_Function_PIO;

	UART::start(57600, UART::Default);
	UART::writeSync("\nboot");
	UART::flush();
	UART::stop();

	System::setClockOutput();	//output the core clock

	//Set up PLL to 48MHz, USB PLL to 48MHz also
	System::setupSystemPLL();
	delay(5);	//stabilize

	UART::start(57600, UART::Default);
	UART::writeSync("\nclocks ok");

	System::setupUSBPLL();

	UART::writeSync("\nusb clock setup");
	
	//USB setup
	USB::Init();

	USB::SetDescriptor((unsigned char const*)&kDeviceDescriptor, sizeof(kDeviceDescriptor));
	USB::SetDescriptor((unsigned char const*)&kCDCACMConfigurationDescriptor, sizeof(kCDCACMConfigurationDescriptor));
	USB::SetDescriptor((unsigned char const*)&kUSBStrings, sizeof(kUSBStrings));

	USBCDCDevice cdcDeviceState;
	memset_volatile(&cdcDeviceState, 0, sizeof(cdcDeviceState));
	USB::RegisterHandler((unsigned char const*)&kCDCACMConfigurationDescriptor, &USBCDCACMHandler, &cdcDeviceState);
	USB::RegisterHandler((unsigned char const*)&kCDCACMConfigurationDescriptor.endpoint1_0, &USBCDCACMHandler, &cdcDeviceState);
	USB::RegisterHandler((unsigned char const*)&kCDCACMConfigurationDescriptor.endpoint1_1, &USBCDCACMHandler, &cdcDeviceState);

	ErrorCode error = USB::Start();
	if(error != ErrorCode_OK)
	{
		UART::writeSync("\nStart() error: ");
		UART::writeSync(error, NumberFormatter::Hexadecimal);
	}

	__asm__ volatile ("cpsie i"::);
	
	USB::Connect();
	UART::writeSync("\nconnected");
	
	while(true)
	{
		//__asm__ volatile ("wfi"::);
		UART::writeSync("\nalive.");
		delay(1000);
		
		/*if(cdcDeviceState.connected)	//@@HACK!
			(*API)->usb->hardware->EndpointWrite(	gUSBAPIHandle,
													0x83,
													(unsigned char*)"\nhello :-)",
													10
												);
		*/
	}
}
