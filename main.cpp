#include <LPC11U00.h>
#include <LPC11U00API.h>
#include <LPCUSB.h>
#include <USBCDC.h>
#include <USBAPI.h>
#include <string.h>

using namespace LPC11U00;
using namespace LPCUSB;

/*
USBHandle	gUSBAPIHandle, gUSBCDCHandle;

enum USBTask
{
	USBTask_Idle					= 0,
	USBTask_ChangeAddressPending	= 1,
	USBTask_ChangeAddress			= 2,
};

struct USBState
{
	unsigned char const*	configurationDescriptor;
	unsigned int			configurationDescriptorLength;

	unsigned int			task;
	unsigned int			taskParameter;
	
	unsigned int			lastSetupType;
	
	unsigned char*			out;
	unsigned int			outLength;
};

static USBState gUSBState;


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

void	USB_IRQHandler(void)
{
	(*API)->usb->hardware->ISREntrypoint(gUSBAPIHandle);

	switch(gUSBState.task)
	{
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
*/

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


/*
ErrorCode	USBStandardHandler(USBDescriptorHeader const* endpoint, USBSetup const* setupPacket);
ErrorCode	USBClassHandler(USBDescriptorHeader const* endpoint, USBSetup const* setupPacket);
ErrorCode	USBVendorHandler(USBDescriptorHeader const* endpoint, USBSetup const* setupPacket);

ErrorCode	packetHandler(		USBHandle usb,
								void* closure,
								USBEvent endpointEvent
							)
{
	USBDescriptorConfigurationHeader* descriptor = 0;
	if(((USBDescriptorHeader*)closure)->type == DescriptorType_Configuration)
		descriptor = (USBDescriptorConfigurationHeader*)closure;

	UART::writeSync("\npkt:");
	UART::writeSync(descriptor? 0 : ((USBDescriptorEndpoint*)closure)->endpointAddress, NumberFormatter::Hexadecimal);
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
			
			gUSBState.lastSetupType = setupPacket.bmRequestType;

			unsigned int result = ErrorCode_Unhandled;
			switch(setupPacket.bmRequestType & USBSetup_Type__Mask)
			{
			case USBSetup_Type_Standard:
				result = USBStandardHandler((USBDescriptorHeader*)closure, &setupPacket);
				break;
			case USBSetup_Type_Class:
				result = USBClassHandler((USBDescriptorHeader*)closure, &setupPacket);
				break;
			case USBSetup_Type_Vendor:
				result = USBVendorHandler((USBDescriptorHeader*)closure, &setupPacket);
				break;
			default:
				UART::writeSync(" unhandled!");
				break;
			}

			if(result == ErrorCode_SendZeroLengthPacket)
			{
				unsigned int endpoint = descriptor? 0 : ((USBDescriptorEndpoint*)closure)->endpointAddress;
				(*API)->usb->hardware->EndpointWrite(gUSBAPIHandle, 0x80 | endpoint, (unsigned char*)"", 0);
				result = ErrorCode_OK;
			}

			return(result);
		}

	case USBEvent_In:
		if((gUSBState.lastSetupType & USBSetup_Type__Mask) == USBSetup_Type_Standard)
		{
			if(gUSBState.task == USBTask_ChangeAddressPending)
				gUSBState.task = USBTask_ChangeAddress;
			
			return(ErrorCode_OK);
		}
		else
			; // prompt the next write to occur
		break;

	case USBEvent_Out:
		{
			unsigned int result = ErrorCode_Unhandled;
			switch(gUSBState.lastSetupType & USBSetup_Type__Mask)
			{
			case USBSetup_Type_Standard:
				result = USBStandardHandler((USBDescriptorHeader*)closure, 0);
				break;
			case USBSetup_Type_Class:
				result = USBClassHandler((USBDescriptorHeader*)closure, 0);
				break;
			case USBSetup_Type_Vendor:
				result = USBVendorHandler((USBDescriptorHeader*)closure, 0);
				break;
			default:
				UART::writeSync(" OUT unhandled!");
				break;
			}
			return(result);
		}
	}

	return(ErrorCode_Unhandled);
}

ErrorCode	USBStandardHandler(USBDescriptorHeader const* endpoint, USBSetup const* setupPacket)
{
	if(setupPacket == 0)	// is an OUT packet
	{
		unsigned char buffer[64];
		unsigned int bytesRead = (*API)->usb->hardware->EndpointRead(gUSBAPIHandle, 0, buffer);
		UART::writeSync(" srd:");
		UART::writeSync(bytesRead, NumberFormatter::DecimalUnsigned);
		UART::writeSync(">");
		UART::writeHexDumpSync(buffer, bytesRead);

		return(ErrorCode_OK);
	}

	// else is a SETUP
	switch(setupPacket->bRequest)
	{
	case USBDeviceRequest_ClearFeature:
		UART::writeSync(":cf:");
		UART::writeSync((unsigned int)setupPacket->wValue, NumberFormatter::Hexadecimal);
		return(ErrorCode_SendZeroLengthPacket);

	case USBDeviceRequest_SetFeature:
		UART::writeSync(":sf:");
		UART::writeSync((unsigned int)setupPacket->wValue, NumberFormatter::Hexadecimal);

	case USBDeviceRequest_SetAddress:
		UART::writeSync(":sa");
		
		gUSBState.taskParameter = setupPacket->wValue;
		gUSBState.task = USBTask_ChangeAddressPending;
		
		return(ErrorCode_SendZeroLengthPacket);

	case USBDeviceRequest_GetDescriptor:
		{
			UART::writeSync(":dd");
			
			unsigned char const* descriptor = 0;	// determine the right one to send
			size_t descriptorLength = 0;

			switch(setupPacket->wValue >> 8)
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
				descriptor = (unsigned char*)&kCDCACMConfigurationDescriptor;
				descriptorLength = sizeof(kCDCACMConfigurationDescriptor) - 1;
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
												(setupPacket->wValue >> 8),
												(setupPacket->wValue & 0xFF)
											);
			
			// clip to the size of the descriptor element itself. Note that the root
			//   configuration descriptor's effective size includes child elements
			if(setupPacket->wValue == (DescriptorType_Configuration << 8))
				descriptorLength = descriptor[2];	// account for sentinel
			else
				descriptorLength = descriptor[0];
			
			if(setupPacket->wLength < descriptorLength)
				descriptorLength = setupPacket->wLength;
			
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
		UART::writeSync(":gc");
		(*API)->usb->hardware->EndpointWrite(	gUSBAPIHandle,
												0x80,
												(unsigned char*)"\x01",
												1
											);
		return(ErrorCode_OK);

	case USBDeviceRequest_SetConfiguration:
		{
			UART::writeSync(":sc");

			(*API)->usb->hardware->SetConfiguration(	gUSBAPIHandle,
														gUSBState.taskParameter
													);
			
			// configure all endpoints
			unsigned int index = 0;
			unsigned char const* ep;
			while((ep = findNthDescriptor(		(unsigned char const*)&kCDCACMConfigurationDescriptor, //gUSBState.configurationDescriptor,
												sizeof(kCDCACMConfigurationDescriptor) - 1, //gUSBState.configurationDescriptorLength,
												DescriptorType_Endpoint,
												index
											)) != 0)
			{
				UART::writeSync("\ncfg#");
				UART::writeSync(index, NumberFormatter::DecimalUnsigned);
				UART::writeSync("@0x");
				UART::writeSync((unsigned int)ep, NumberFormatter::Hexadecimal);
				(*API)->usb->hardware->ConfigureEndpoint(gUSBAPIHandle, (USBDescriptorEndpoint*)ep);

				unsigned int addr = ((USBDescriptorEndpoint*)ep)->endpointAddress;
				UART::writeSync(", en:");
				UART::writeSync(addr, NumberFormatter::Hexadecimal);
				(*API)->usb->hardware->EnableEndpoint(gUSBAPIHandle, addr);

				index++;
			}
		}
		return(ErrorCode_SendZeroLengthPacket);

	default:
		UART::writeSync("?");
		break;
	}

	return(ErrorCode_Unhandled);
}
*/

struct __attribute__((align(4))) USBCDCDevice	//@@needed?
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
	UART::writeSync("\nstate=0x");
	UART::writeSync((unsigned int)state, NumberFormatter::Hexadecimal);

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
			/*(*API)->usb->hardware->EndpointWrite(	gUSBAPIHandle,
													0x80,	//always EP0
													(unsigned char*)&state->lineCoding,
													sizeof(USBCDCLineCoding)
												);
			*/
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
	else
	{
		unsigned char buffer[64];
		unsigned int bytesRead = USB::Read(endpointAddress, buffer);		//(*API)->usb->hardware->EndpointRead(gUSBAPIHandle, endpointAddress, buffer);
		
		switch(state->expecting)
		{
		case USBCDC_Request_SetLineCoding:
			memcpy(&state->lineCoding, buffer, sizeof(USBCDCLineCoding));
			UART::writeSync(" sl: ");
			UART::writeSync(state->lineCoding.dwDTERate, NumberFormatter::DecimalUnsigned);
			UART::writeSync(",");
			UART::writeSync(state->lineCoding.bDataBits, NumberFormatter::DecimalUnsigned);
			UART::writeSync(state->lineCoding.bParityType? "p" : "n");
			UART::writeSync(state->lineCoding.bCharFormat + 1, NumberFormatter::DecimalUnsigned);
			state->expecting = 0;
			//break;	//@@debug temp

		default:
			UART::writeSync(" crd:");
			UART::writeSync(bytesRead, NumberFormatter::DecimalUnsigned);
			UART::writeSync(">");
			UART::writeHexDumpSync(buffer, bytesRead);

			buffer[bytesRead++] = '|';

			//(*API)->usb->hardware->EndpointWrite(	gUSBAPIHandle,
			USB::Write(		0x80 | endpointAddress,
							buffer,
							bytesRead
						);
			break;
		}
	}
	return(ErrorCode_OK);
}

/*
ErrorCode	registerHandlersFromDescriptor(unsigned char const* descriptor, unsigned int length)
{
	(*API)->usb->core->RegisterEndpointHandler(		gUSBAPIHandle,
													0,
													&packetHandler,
													(void*)descriptor
												);
	(*API)->usb->core->RegisterEndpointHandler(		gUSBAPIHandle,
													1,
													&packetHandler,
													(void*)descriptor
												);
	unsigned int index = 0;
	unsigned char const* ep;
	while((ep = findNthDescriptor(		descriptor,
										length,
										DescriptorType_Endpoint,
										index
									)) != 0)
	{
		unsigned int endpointHandle = ((USBDescriptorEndpoint*)ep)->endpointAddress;
		endpointHandle = ((endpointHandle << 1) | (endpointHandle >> 7)) & 0x1F;
		ErrorCode registerError = (*API)->usb->core->RegisterEndpointHandler(	gUSBAPIHandle,
																				endpointHandle,
																				&packetHandler,
																				(void*)ep
																			);
		if(registerError != 0)
		{
			UART::writeSync("\nreg EP 0x");
			UART::writeSync(((USBDescriptorEndpoint*)ep)->endpointAddress, NumberFormatter::Hexadecimal);
			UART::writeSync(" err: ");
			UART::writeSync(registerError, NumberFormatter::Hexadecimal);
			return(registerError);
		}
		index++;
	}
}
*/

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
	
	/*
	//USB setup
	CoreDescriptors descriptors;
	descriptors.deviceDescriptor = (void*)&kDeviceDescriptor;
	descriptors.stringDescriptor = kUSBStrings;
	descriptors.fullSpeedDescriptor = (unsigned char*)&kCDCACMConfigurationDescriptor;
	descriptors.highSpeedDescriptor = (unsigned char*)&kCDCACMConfigurationDescriptor;
	descriptors.deviceQualifier = 0;

	
	HardwareInit hardwareInit = {0};
	hardwareInit.usbRegisterBase = 0x40080000;
	hardwareInit.memBase = 0x20004800;	//should be 0x20004000;
	hardwareInit.memSize = 0x2000;
	hardwareInit.maxNumEndpoints = 5;


	gUSBAPIHandle = 0;
	ErrorCode usbError = (*API)->usb->hardware->Init(&gUSBAPIHandle, &descriptors, &hardwareInit);
	UART::writeSync("\nhwInit: 0x");
	UART::writeSync(usbError, NumberFormatter::Hexadecimal);
	UART::writeSync("\nUSB: 0x");
	UART::writeSync((unsigned int)gUSBAPIHandle, NumberFormatter::Hexadecimal);
	
	registerHandlersFromDescriptor((unsigned char const*)&kCDCACMConfigurationDescriptor, sizeof(kCDCACMConfigurationDescriptor));

	*InterruptEnableSet0 |= Interrupt0_USB;

	volatile unsigned int* p = (volatile unsigned int*)&gUSBState;
	for(int i = 0; i < ((sizeof(gUSBState) + 3) >> 2); i++)
		*p++ = 0;

	p = (volatile unsigned int*)&cdcDeviceState;
	for(int i = 0; i < ((sizeof(cdcDeviceState) + 3) >> 2); i++)
		*p++ = 0;
	*/

	USB::Init();

	USB::SetDescriptor((unsigned char const*)&kDeviceDescriptor, sizeof(kDeviceDescriptor));
	USB::SetDescriptor((unsigned char const*)&kCDCACMConfigurationDescriptor, sizeof(kCDCACMConfigurationDescriptor));
	USB::SetDescriptor((unsigned char const*)&kUSBStrings, sizeof(kUSBStrings));

	USBCDCDevice cdcDeviceState;
	memset_volatile(&cdcDeviceState, 0, sizeof(cdcDeviceState));
	UART::writeSync("\nstate=0x");
	UART::writeSync((unsigned int)&cdcDeviceState, NumberFormatter::Hexadecimal);
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
	
	//(*API)->usb->hardware->Connect(gUSBAPIHandle, 1);
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
