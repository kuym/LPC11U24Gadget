#include <LPC11U00.h>
#include <LPC11U00API.h>
#include <LPCUSB.h>
#include <string.h>

using namespace LPC11U00;
using namespace LPCUSB;

USBHandle	gUSBAPIHandle, gUSBCDCHandle;

enum USBTask
{
	USBTask_Idle					= 0,
	USBTask_SendBuffer				= 1,
	USBTask_ChangeAddressPending	= 2,
	USBTask_ChangeAddress			= 3,
	USBTask_SetConfiguration		= 4
};

struct USBState
{
	unsigned int	task;
	unsigned int	taskParameter;

	unsigned char*	in;
	unsigned int	inDest;
	unsigned int	inLength;
};

static USBState gUSBState;

void	USB_IRQHandler(void)
{
	(*API)->usb->hardware->ISREntrypoint(gUSBAPIHandle);

	switch(gUSBState.task)
	{
	case USBTask_SendBuffer:
		{
			UART::writeSync("\ntx");
			unsigned int chunkSize = gUSBState.inLength;	// for now, limit in future

			(*API)->usb->hardware->EndpointWrite(	gUSBAPIHandle,
													gUSBState.inDest,
													gUSBState.in,
													gUSBState.inLength
												);

			gUSBState.inLength -= chunkSize;
			gUSBState.in += chunkSize;

			if(gUSBState.inLength == 0)
				gUSBState.task = USBTask_Idle;
		}
		break;

	case USBTask_SetConfiguration:
		(*API)->usb->hardware->SetConfiguration(	gUSBAPIHandle,
													gUSBState.taskParameter
												);
		//(*API)->usb->hardware->Reset(gUSBAPIHandle);
		gUSBState.task = USBTask_Idle;
		break;

	case USBTask_ChangeAddress:
		UART::writeSync("\nchg");
		//UART::writeSync(gChangeAddress, NumberFormatter::Hexadecimal);	//strict time limits on this one
		
		// @@infuriatingly, this API call does not appear to work - enumeration times out:
		//   need a Beagle to see what's up.
		//(*API)->usb->hardware->SetAddress(		gUSBAPIHandle,
		//										(unsigned char)gChangeAddress
		//									);

		// set the address on the USB controller manually
		*USBDeviceCommandStatus |= (gUSBState.taskParameter & 0x7F);
		
		gUSBState.task = USBTask_Idle;
		break;
	}
}

////////////////////////////////////////////////////////////////


static USBDescriptorDevice const __attribute__((aligned(4))) kDeviceDescriptor =
{
	.header					= {.size = sizeof(USBDescriptorDevice), .type = DescriptorType_Device},

	.usbSpecification		= 0x0110,
	.deviceClass			= 0xFF,
	.subClass				= 0xFF,
	.protocol				= 0xFF,

	.endpoint0Size			= 64,	// FIXED_CONTROL_ENDPOINT_SIZE,

	.vendorID				= 0x80b1,
	.productID				= 0x1234,
	.releaseNumber			= 0x0101,

	.manufacturerStrIndex	= 0x01,
	.productStrIndex		= 0x02,
	.serialNumStrIndex		= 0x03,

	.numberOfConfigurations	= 0x01
};

struct __attribute__((packed)) ApplicationConfigurationDescriptor
{
	USBDescriptorConfigurationHeader	header;
	USBDescriptorInterface				interface0;
	USBDescriptorEndpoint				endpoint0_0;
	USBDescriptorEndpoint				endpoint0_1;

	unsigned char						_sentinel;

} const kConfigurationDescriptor __attribute__((aligned(4))) =
{
	.header =
	{
		.header = {.size = sizeof(USBDescriptorConfigurationHeader), .type = DescriptorType_Configuration},

		.totalConfigurationSize = sizeof(ApplicationConfigurationDescriptor) - 1,
		.totalInterfaces		= 1,
		.configurationNumber	= 1,
		.configurationStrIndex	= 0,
		.configAttributes		= (0x80 | 0x40),
		.maxPowerConsumption	= (100 / 2),
	},
	.interface0 =
	{
		.header = {.size = sizeof(USBDescriptorInterface), .type = DescriptorType_Interface},
		.interfaceNumber	= 0,
		.alternateSetting	= 0,
		.totalEndpoints		= 2,
		.interfaceClass		= 0xFF,
		.subClass			= 0xFF,
		.protocol			= 0xFF,
		.interfaceStrIndex	= 0,
	},
	.endpoint0_0 =
	{
		.header = {.size = sizeof(USBDescriptorEndpoint), .type = DescriptorType_Endpoint},
		.endpointAddress	= 0x81,
		.attributes			= (USBEndpointType_Bulk | USBEndpointAttribute_NoSync | USBEndpointUsage_Data),
		.endpointSize		= 16,
		.pollingIntervalMS	= 1,
	},
	.endpoint0_1 =
	{
		.header = {.size = sizeof(USBDescriptorEndpoint), .type = DescriptorType_Endpoint},
		.endpointAddress	= 0x2,
		.attributes			= (USBEndpointType_Bulk | USBEndpointAttribute_NoSync | USBEndpointUsage_Data),
		.endpointSize		= 16,
		.pollingIntervalMS	= 1,
	},
	._sentinel = 0
};

static unsigned char const __attribute__((aligned(4))) kUSBStrings[] =
	"\x04\x03" "\x09\x04"
	"\x12\x03" "W\0" "h\0" "o\0" "m\0" "e\0" "v\0" "e\0" "r\0"
	"\x0E\x03" "G\0" "a\0" "d\0" "g\0" "e\0" "t\0"
	"\x0A\x03" "1\0" "2\0" "3\0" "4\0"
	;

unsigned char const*	findNthDescriptor(unsigned char const* base, size_t length, int ofType, int index)
{
	while(length > 0)
	{
		if((ofType == 0) || (base[1] == ofType))
		{
			if(index == 0)
				return(base);
			index--;
		}
		if(base[0] == 0)	// malformed
			break;
		
		length -= base[0];
		base += base[0];	// jump over this entry
	}
	return(0);	// not found
}


ErrorCode	classHandler(	USBHandle usb,
							void* closure,
							USBEvent endpointEvent
						)
{
	UART::writeSync("\nclass:");
	UART::writeSync((unsigned int)closure, NumberFormatter::Hexadecimal);
	UART::writeSync(",");
	UART::writeSync(endpointEvent, NumberFormatter::Hexadecimal);
	
	switch(endpointEvent)
	{
	case USBEvent_Setup:
		{
			USBSetup setupPacket;

			(*API)->usb->hardware->EndpointReadSetup(gUSBAPIHandle, 0, (unsigned int*)&setupPacket);
			
			UART::writeSync("\nbR:");
			UART::writeHexDumpSync((unsigned char const*)&setupPacket, sizeof(setupPacket));
			
			switch(setupPacket.bRequest)
			{
			case USBDeviceRequest_SetAddress:
				UART::writeSync(":sa");
				(*API)->usb->hardware->EndpointWrite(gUSBAPIHandle, 0x80, (unsigned char*)"", 0);
				
				gUSBState.taskParameter = setupPacket.wValue;
				gUSBState.task = USBTask_ChangeAddressPending;
				
				return(ErrorCode_OK);

			case USBDeviceRequest_GetDescriptor:
				{
					UART::writeSync(":dd");
					
					unsigned char const* descriptor = 0;	// determine the right one to send
					size_t descriptorLength = 0;

					switch(setupPacket.wValue >> 8)
					{
					case DescriptorType_Device:
						descriptor = (unsigned char*)&kDeviceDescriptor;
						descriptorLength = sizeof(kDeviceDescriptor);
						break;
					case DescriptorType_Configuration:
					case DescriptorType_Interface:
					case DescriptorType_Endpoint:
					case DescriptorType_CSInterface:
					case DescriptorType_CSEndpoint:
						descriptor = (unsigned char*)&kConfigurationDescriptor;
						descriptorLength = sizeof(kConfigurationDescriptor) - 1;
						break;
					case DescriptorType_String:
						descriptor = (unsigned char*)kUSBStrings;
						descriptorLength = sizeof(kUSBStrings);
						break;
					}

					UART::writeSync("\n<");
					UART::writeSync((unsigned int)descriptor, NumberFormatter::Hexadecimal);
					UART::writeSync("<");
					
					// get the Nth descriptor of the right type from the array
					descriptor = findNthDescriptor(		descriptor,
														descriptorLength,
														(setupPacket.wValue >> 8),
														(setupPacket.wValue & 0xFF)
													);
					
					// clip to the size of the descriptor element itself. Note that the root
					//   configuration descriptor's effective size includes child elements
					if(setupPacket.wValue == (DescriptorType_Configuration << 8))
						descriptorLength = descriptor[2];	// account for sentinel
					else
						descriptorLength = descriptor[0];
					
					if(setupPacket.wLength < descriptorLength)
						descriptorLength = setupPacket.wLength;
					
					UART::writeSync((unsigned int)descriptor, NumberFormatter::Hexadecimal);
					UART::writeSync("<");
					UART::writeSync(descriptorLength, NumberFormatter::DecimalUnsigned);
					UART::writeSync("<");
					UART::writeHexDumpSync(descriptor, descriptorLength);
					
					(*API)->usb->hardware->EndpointWrite(	gUSBAPIHandle,
															0x80,
															(unsigned char*)descriptor,
															descriptorLength
														);
				}
				return(ErrorCode_OK);

			case USBDeviceRequest_GetConfiguration:
				(*API)->usb->hardware->EndpointWrite(	gUSBAPIHandle,
														0x80,
														(unsigned char*)"\x01",
														1
													);
				return(ErrorCode_OK);

			case USBDeviceRequest_SetConfiguration:
				UART::writeSync(":sc");

				(*API)->usb->hardware->SetConfiguration(	gUSBAPIHandle,
															gUSBState.taskParameter
														);
				
				//(*API)->usb->hardware->Reset(gUSBAPIHandle);	//@@required by the spec but seems to not work
				
				// send a ZLP
				(*API)->usb->hardware->EndpointWrite(gUSBAPIHandle, 0x80, (unsigned char*)"", 0);

				gUSBState.taskParameter = setupPacket.wValue;
				gUSBState.task = USBTask_SetConfiguration;
				return(ErrorCode_OK);

			default:
				UART::writeSync("?");
				break;
			}
		}
		break;

	case USBEvent_In:
		if(gUSBState.task == USBTask_ChangeAddressPending)
			gUSBState.task = USBTask_ChangeAddress;
		
		return(0);
	}

	return(ErrorCode_Unhandled);
}

// @@currently unused
ErrorCode	endpointHandler(	USBHandle usb,
								void* closure,
								USBEvent endpointEvent
							)
{
	UART::writeSync("\ne");
	UART::writeSync((unsigned int)closure, NumberFormatter::DecimalUnsigned);
	UART::writeSync(": 0x");
	UART::writeSync(endpointEvent, NumberFormatter::Hexadecimal);
	
	return(ErrorCode_Unhandled);
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
	CoreDescriptors descriptors;
	descriptors.deviceDescriptor = (void*)&kDeviceDescriptor;
	descriptors.stringDescriptor = kUSBStrings;
	descriptors.fullSpeedDescriptor = (unsigned char*)&kConfigurationDescriptor;
	descriptors.highSpeedDescriptor = (unsigned char*)&kConfigurationDescriptor;
	descriptors.deviceQualifier = 0;

	
	HardwareInit hardwareInit = {0};
	hardwareInit.usbRegisterBase = 0x40080000;
	hardwareInit.memBase = 0x20004800;	//should be 0x20004000;
	hardwareInit.memSize = 0x2000;
	hardwareInit.maxNumEndpoints = 3;


	gUSBAPIHandle = 0;
	ErrorCode usbError = (*API)->usb->hardware->Init(&gUSBAPIHandle, &descriptors, &hardwareInit);
	UART::writeSync("\nhwInit: 0x");
	UART::writeSync(usbError, NumberFormatter::Hexadecimal);
	UART::writeSync("\nUSB: 0x");
	UART::writeSync((unsigned int)gUSBAPIHandle, NumberFormatter::Hexadecimal);
	
	ErrorCode registerError;
	registerError = (*API)->usb->core->RegisterClassHandler(gUSBAPIHandle, &classHandler, (void*)0);
	UART::writeSync("\nregisterClass: 0x");
	UART::writeSync(registerError, NumberFormatter::Hexadecimal);

	registerError = (*API)->usb->core->RegisterEndpointHandler(gUSBAPIHandle, 1, &classHandler, (void*)1);

	for(int i = 2; i < 6; i++)
	{
		registerError = (*API)->usb->core->RegisterEndpointHandler(gUSBAPIHandle, i, &endpointHandler, (void*)i);
		UART::writeSync("\nreg(");
		UART::writeSync(i, NumberFormatter::Hexadecimal);
		UART::writeSync("): 0x");
		UART::writeSync(registerError, NumberFormatter::Hexadecimal);
	}

	*InterruptEnableSet0 |= Interrupt0_USB;

	volatile unsigned int* p = (volatile unsigned int*)&gUSBState;
	for(int i = 0; i < ((sizeof(gUSBState) + 3) >> 2); i++)
		*p++ = 0;
	
	__asm__ volatile ("cpsie i"::);
	
	(*API)->usb->hardware->Connect(gUSBAPIHandle, 1);

	UART::writeSync("\nconnected");
	
	while(true)
	{
		//__asm__ volatile ("wfi"::);
		UART::writeSync("\nalive.");
		delay(1000);
	}
}
