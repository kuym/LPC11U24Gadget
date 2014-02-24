#include <LPC11U00.h>
#include <LPC11U00API.h>
#include <USBAPI.h>
#include <USBCDC.h>
#include <string.h>

using namespace LPC11U00;
using namespace LPC11U00::ROMUSB;
using namespace LPC11U00::ROMAPI;


enum USBTask
{
	USBTask_Idle					= 0,
	USBTask_ChangeAddressPending	= 1,
	USBTask_ChangeAddress			= 2,
};

struct Handler
{
	USBDescriptorHeader const*	receiver;
	USBHandler					handler;
	void*						context;
	Handler*					next;
};

struct USBState
{
	CoreDescriptors		descriptors;

	Handler*			handlers;

	unsigned int		deviceDescriptorLength;
	unsigned int		configurationDescriptorLength;
	unsigned int		stringDescriptorLength;

	unsigned int		task;
	unsigned int		taskParameter;
	
	unsigned int		lastSetupType;
};

static USBState gUSBState;
static USBHandle gUSBAPIHandle;


static unsigned char const*	findNthDescriptor(unsigned char const* base, size_t length, int ofType, int index)
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

static ErrorCode	packetHandler(USBHandle usb, void* closure, USBEvent endpointEvent);
static ErrorCode	standardSetupHandler(void* context, USBDescriptorHeader const* endpoint, USBSetup const* setupPacket);

static ErrorCode	registerHandlersFromDescriptor(unsigned char const* descriptor, unsigned int length)
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
			/*
			UART::writeSync("\nreg EP 0x");
			UART::writeSync(((USBDescriptorEndpoint*)ep)->endpointAddress, NumberFormatter::Hexadecimal);
			UART::writeSync(" err: ");
			UART::writeSync(registerError, NumberFormatter::Hexadecimal);
			*/
			return(registerError);
		}
		index++;
	}
	return(ErrorCode_OK);
}

static Handler*		findHandlerForDescriptor(USBDescriptorHeader const* header)
{
	Handler* h = gUSBState.handlers;
	
	while((h != 0) && (h->receiver != header))
		h = h->next;

	return(h);
}


static ErrorCode	packetHandler(		USBHandle usb,
										void* closure,
										USBEvent endpointEvent
									)
{
	(void)usb;

	USBDescriptorConfigurationHeader* descriptor = 0;
	if(((USBDescriptorHeader const*)closure)->type == DescriptorType_Configuration)
		descriptor = (USBDescriptorConfigurationHeader*)closure;

	/*
	UART::writeSync("\npkt:");
	UART::writeSync(descriptor? 0 : ((USBDescriptorEndpoint*)closure)->endpointAddress, NumberFormatter::Hexadecimal);
	UART::writeSync(",");
	UART::writeSync(endpointEvent, NumberFormatter::Hexadecimal);
	*/
	
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

			// for standard setup requests, we always handle it
			if((setupPacket.bmRequestType & USBSetup_Type__Mask) == USBSetup_Type_Standard)
				result = standardSetupHandler(0, (USBDescriptorHeader const*)closure, &setupPacket);
			else
			{
				// for class and vendor requests, pass it on to the appropriate handler
				Handler* h = findHandlerForDescriptor((USBDescriptorHeader const*)closure);
				
				if(h != 0)
					result = h->handler(h->context, (USBDescriptorHeader const*)closure, &setupPacket);
				else
					UART::writeSync(" unhandled!");
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
		else if((gUSBState.lastSetupType & USBSetup_Recipient__Mask) == USBSetup_Recipient_Device)	// possibly prompt the next endpoint IN write to occur
		{
			Handler* h = findHandlerForDescriptor((USBDescriptorHeader const*)closure);
			if(h != 0)
				return(h->handler(h->context, (USBDescriptorHeader const*)closure, 0));
		}
		break;

	case USBEvent_Out:
		{
			unsigned int result = ErrorCode_Unhandled;
			if((gUSBState.lastSetupType & USBSetup_Type__Mask) == USBSetup_Type_Standard)
			{
				result = standardSetupHandler(&gUSBState, (USBDescriptorHeader const*)closure, 0);
			}
			else
			{
				Handler* h = findHandlerForDescriptor((USBDescriptorHeader const*)closure);

				if(h != 0)
					result = h->handler(h->context, (USBDescriptorHeader const*)closure, 0);
			}
			return(result);
		}
	}

	return(ErrorCode_Unhandled);
}

static ErrorCode	standardSetupHandler(	void* context,
											USBDescriptorHeader const* endpoint,
											USBSetup const* setupPacket
										)
{
	(void)context;
	(void)endpoint;

	if(setupPacket == 0)	// is an OUT packet
	{
		// OUT packets to EP0 seem to be completely empty, perhaps a ROM API anomaly?
		return(ErrorCode_OK);
	}

	// else is a SETUP
	switch(setupPacket->bRequest)
	{
	case USBDeviceRequest_ClearFeature:
		// feature in setupPacket->wValue
		return(ErrorCode_SendZeroLengthPacket);

	case USBDeviceRequest_SetFeature:
		// feature in setupPacket->wValue
		return(ErrorCode_SendZeroLengthPacket);

	case USBDeviceRequest_SetAddress:
		gUSBState.taskParameter = setupPacket->wValue;
		gUSBState.task = USBTask_ChangeAddressPending;
		
		return(ErrorCode_SendZeroLengthPacket);

	case USBDeviceRequest_GetDescriptor:
		{
			unsigned char const* descriptor = 0;	// determine the right one to send
			size_t descriptorLength = 0;

			switch(setupPacket->wValue >> 8)
			{
			case DescriptorType_Device:
				descriptor = (unsigned char const*)gUSBState.descriptors.deviceDescriptor;
				descriptorLength = gUSBState.deviceDescriptorLength;
				break;
			case DescriptorType_Configuration:
			case DescriptorType_Interface:
			case DescriptorType_Endpoint:
			case DescriptorType_CSInterface:
			case DescriptorType_CSEndpoint:
				descriptor = (unsigned char const*)gUSBState.descriptors.fullSpeedDescriptor;
				descriptorLength = gUSBState.configurationDescriptorLength;
				break;
			case DescriptorType_String:
				descriptor = (unsigned char const*)gUSBState.descriptors.stringDescriptor;
				descriptorLength = gUSBState.stringDescriptorLength;
				break;
			}

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
		{
			(*API)->usb->hardware->SetConfiguration(	gUSBAPIHandle,
														gUSBState.taskParameter
													);
			
			// configure all endpoints
			unsigned int index = 0;
			unsigned char const* ep;
			while((ep = findNthDescriptor(		(unsigned char const*)gUSBState.descriptors.fullSpeedDescriptor,
												gUSBState.configurationDescriptorLength,
												DescriptorType_Endpoint,
												index
											)) != 0)
			{
				unsigned int addr = ((USBDescriptorEndpoint*)ep)->endpointAddress;
				UART::writeSync("\nenEP");
				UART::writeSync(addr, NumberFormatter::Hexadecimal);
				(*API)->usb->hardware->ConfigureEndpoint(gUSBAPIHandle, (USBDescriptorEndpoint*)ep);

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

////////////////////////////////////////////////////////////////


ErrorCode		USB::Interrupt(void)
{
	(*API)->usb->hardware->ISREntrypoint(gUSBAPIHandle);

	switch(gUSBState.task)
	{
	case USBTask_ChangeAddress:
		// set the address on the USB controller manually
		*USBDeviceCommandStatus |= (gUSBState.taskParameter & 0x7F);
		
		gUSBState.task = USBTask_Idle;
		break;
	}
	return(ErrorCode_OK);
}

void			USB::Init(void)
{
	memset_volatile(&gUSBState, 0, sizeof(gUSBState));
	
	gUSBAPIHandle = 0;
}

ErrorCode		USB::SetDescriptor(		void const* descriptor,
										unsigned int length
									)
{
	switch(((USBDescriptorHeader*)descriptor)->type)
	{
	case DescriptorType_Device:
		gUSBState.descriptors.deviceDescriptor = (USBDescriptorHeader*)descriptor;
		gUSBState.deviceDescriptorLength = length;
		break;
	case DescriptorType_Configuration:
		gUSBState.descriptors.fullSpeedDescriptor = (USBDescriptorHeader*)descriptor;
		gUSBState.descriptors.highSpeedDescriptor = (USBDescriptorHeader*)descriptor;
		gUSBState.configurationDescriptorLength = length - 1;
		break;
	case DescriptorType_String:
		gUSBState.descriptors.stringDescriptor = (USBDescriptorHeader*)descriptor;
		gUSBState.stringDescriptorLength = length;
		break;
	default:
		UART::writeSync("\nset: bad descriptor");
		return(ErrorCode_BadDescriptor);
	}
	return(ErrorCode_OK);
}

ErrorCode		USB::Start(void)
{
	//USB setup
	HardwareInit hardwareInit =
	{
		.usbRegisterBase =		0x40080000,
		.memBase =				0x20004800,	// should be 0x20004000 according to the datasheet
		.memSize =				0x2000,
		.maxNumEndpoints =		7,			// @@make parametric
		.__reserved0 =			{0},
		.ResetHandler =			0,
		.SuspendHandler =		0,
		.ResumeHandler =		0,
		.__reserved1 =			0,
		.SOFEventHandler =		0,
		.WakeUpEventHandler =	0,
		.__reserved2 =			0,
		.ErrorHandler =			0,
		.ConfigureHandler =		0,
		.InterfaceHandler =		0,
		.FeatureHandler =		0,
		.__reserved3 =			0,
		.__reserved4 =			0,
	};

	gUSBAPIHandle = 0;
	ErrorCode usbError = (*API)->usb->hardware->Init(&gUSBAPIHandle, &gUSBState.descriptors, &hardwareInit);
	if(usbError != 0)
	{
		UART::writeSync("\ninit error: ");
		UART::writeSync(usbError, NumberFormatter::Hexadecimal);
		return(usbError);
	}

	usbError = registerHandlersFromDescriptor(		(unsigned char const*)gUSBState.descriptors.fullSpeedDescriptor,
													gUSBState.configurationDescriptorLength
												);
	if(usbError != 0)
	{
		UART::writeSync("\nreg handlers error: ");
		UART::writeSync(usbError, NumberFormatter::Hexadecimal);
		return(usbError);
	}

	return(ErrorCode_OK);
}

void			USB::Stop(void)
{
	// remove registered handlers
	for(Handler* h = gUSBState.handlers; h != 0;)
	{
		Handler* n = h->next;
		delete n;
		n = h;
	}
	gUSBState.handlers = 0;

	Init();
}

ErrorCode		USB::RegisterHandler(	void const* descriptor,
										USBHandler handler,
										void* context
									)
{
	switch(((USBDescriptorHeader const*)descriptor)->type)
	{
	case DescriptorType_Configuration:
	case DescriptorType_Endpoint:
		break;
	default:
		UART::writeSync("\nreg: bad descriptor");
		return(ErrorCode_BadDescriptor);
	}

	// ensure descriptor is unique in the set of handlers.
	Handler* h = gUSBState.handlers;
	while((h != 0) && (h->receiver != (USBDescriptorHeader const*)descriptor))
		h = h->next;
	if(h != 0)
	{
		UART::writeSync("\nreg: duplicate handler");
		return(ErrorCode_TooManyHandlers);
	}

	// allocate a new handler
	Handler* newHandler = new Handler();
	newHandler->receiver = (USBDescriptorHeader const*)descriptor;
	newHandler->handler = handler;
	newHandler->context = context;
	newHandler->next = 0;

	// add the handler to the list
	Handler** handlers = &gUSBState.handlers;
	while(*handlers != 0)
		handlers = &(*handlers)->next;
	*handlers = newHandler;

	return(ErrorCode_OK);
}

ErrorCode		USB::Connect(void)
{
	*InterruptEnableSet0 |= Interrupt0_USB;
	(*API)->usb->hardware->Connect(gUSBAPIHandle, 1);
	return(ErrorCode_OK);
}

void			USB::Disconnect(void)
{
	(*API)->usb->hardware->Connect(gUSBAPIHandle, 0);
	*InterruptEnableSet0 &= ~Interrupt0_USB;
}


unsigned int	USB::Read(unsigned int endpoint, unsigned char* dest)
{
	return((*API)->usb->hardware->EndpointRead(gUSBAPIHandle, endpoint, dest));
}
unsigned int	USB::Write(unsigned int endpoint, unsigned char* source, unsigned int length)
{
	return((*API)->usb->hardware->EndpointWrite(gUSBAPIHandle, endpoint, source, length));
}
