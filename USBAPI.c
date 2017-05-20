#include <LPC11U00.h>
#include <LPC11U00API.h>
#include <USBAPI.h>
#include <USBCDC.h>
#include <string.h>
#include <stdlib.h>	// malloc/free

#define USB_DEBUG 1

enum USBTask
{
	USBTask_Idle					= 0,
	USBTask_ChangeAddressPending	= 1,
	USBTask_ChangeAddress			= 2,
};

typedef struct Handler
{
	LPCUSBDescriptorHeader const*	receiver;
	LPCUSBHandler					handler;
	void*							context;
	struct Handler*					next;

} Handler;

typedef struct USBState
{
	LPCUSBCoreDescriptors		descriptors;

	Handler*					handlers;

	unsigned int				deviceDescriptorLength;
	unsigned int				configurationDescriptorLength;
	unsigned int				stringDescriptorLength;

	unsigned int				task;
	unsigned int				taskParameter;
	
	unsigned int				lastSetupType;

} USBState;


static USBState gUSBState;
static LPCUSBHandle gUSBAPIHandle;


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

static LPCUSBErrorCode	packetHandler(LPCUSBHandle usb, void* closure, LPCUSBEvent endpointEvent);
static LPCUSBErrorCode	standardSetupHandler(void* context, LPCUSBDescriptorHeader const* endpoint, LPCUSBSetup const* setupPacket);

static LPCUSBErrorCode	registerHandlersFromDescriptor(unsigned char const* descriptor, unsigned int length)
{
	(*LPC11U00ROMAPI)->usb->core->RegisterEndpointHandler(		gUSBAPIHandle,
													0,
													&packetHandler,
													(void*)descriptor
												);
	(*LPC11U00ROMAPI)->usb->core->RegisterEndpointHandler(		gUSBAPIHandle,
													1,
													&packetHandler,
													(void*)descriptor
												);
	unsigned int index = 0;
	unsigned char const* ep;
	while((ep = findNthDescriptor(		descriptor,
										length,
										LPCUSBDescriptorType_Endpoint,
										index
									)) != 0)
	{
		unsigned int endpointHandle = ((LPCUSBDescriptorEndpoint*)ep)->endpointAddress;
		endpointHandle = ((endpointHandle << 1) | (endpointHandle >> 7)) & 0x1F;
		LPCUSBErrorCode registerError = (*LPC11U00ROMAPI)->usb->core->RegisterEndpointHandler(	gUSBAPIHandle,
																								endpointHandle,
																								&packetHandler,
																								(void*)ep
																							);
		if(registerError != 0)
		{
			UARTWriteStringSync("\nreg EP 0x");
			UARTWriteIntSync(((LPCUSBDescriptorEndpoint*)ep)->endpointAddress, NumberFormatter_Hexadecimal);
			UARTWriteStringSync(" err: ");
			UARTWriteIntSync(registerError, NumberFormatter_Hexadecimal);
			return(registerError);
		}
		index++;
	}
	return(LPCUSBErrorCode_OK);
}

static Handler*		findHandlerForDescriptor(LPCUSBDescriptorHeader const* header)
{
	Handler* h = gUSBState.handlers;
	
	while((h != 0) && (h->receiver != header))
		h = h->next;

	return(h);
}


static LPCUSBErrorCode	packetHandler(		LPCUSBHandle usb,
											void* closure,
											LPCUSBEvent endpointEvent
										)
{
	(void)usb;

#ifdef USB_DEBUG
	UARTWriteStringSync("\n$");
	UARTWriteHexIntSync((unsigned int)usb, 4);
	UARTWriteStringSync("/");
	UARTWriteHexIntSync((unsigned int)closure, 4);
	UARTWriteStringSync(":");
	UARTWriteIntSync((unsigned int)endpointEvent, NumberFormatter_DecimalUnsigned);
#endif //USB_DEBUG

	LPCUSBDescriptorConfigurationHeader* descriptor = 0;
	if(((LPCUSBDescriptorHeader const*)closure)->type == LPCUSBDescriptorType_Configuration)
		descriptor = (LPCUSBDescriptorConfigurationHeader*)closure;

#ifdef USB_DEBUG
	UARTWriteStringSync("\npkt:");
	UARTWriteIntSync(descriptor? 0 : ((LPCUSBDescriptorEndpoint*)closure)->endpointAddress, NumberFormatter_Hexadecimal);
	UARTWriteStringSync(",");
	UARTWriteIntSync(endpointEvent, NumberFormatter_Hexadecimal);
#endif //USB_DEBUG
	
	switch(endpointEvent)
	{
	case LPCUSBEvent_Setup:
		{
			LPCUSBSetup setupPacket;

			(*LPC11U00ROMAPI)->usb->hardware->EndpointReadSetup(gUSBAPIHandle, 0, (unsigned int*)&setupPacket);
			
#ifdef USB_DEBUG
			UARTWriteStringSync("\nbR:");
			UARTWriteHexDumpSync((unsigned char const*)&setupPacket, sizeof(setupPacket));
#endif //USB_DEBUG
			
			gUSBState.lastSetupType = setupPacket.bmRequestType;

			unsigned int result = LPCUSBErrorCode_Unhandled;

			// for standard setup requests, we always handle it
			if((setupPacket.bmRequestType & LPCUSBSetup_Type__Mask) == LPCUSBSetup_Type_Standard)
				result = standardSetupHandler(0, (LPCUSBDescriptorHeader const*)closure, &setupPacket);
			else
			{
				// for class and vendor requests, pass it on to the appropriate handler
				Handler* h = findHandlerForDescriptor((LPCUSBDescriptorHeader const*)closure);
				
				if(h != 0)
					result = h->handler(h->context, (LPCUSBDescriptorHeader const*)closure, &setupPacket);
				else
					UARTWriteStringSync(" unhandled!");
			}

			if(result == LPCUSBErrorCode_SendZeroLengthPacket)
			{
				unsigned int endpoint = descriptor? 0 : ((LPCUSBDescriptorEndpoint*)closure)->endpointAddress;
				(*LPC11U00ROMAPI)->usb->hardware->EndpointWrite(gUSBAPIHandle, 0x80 | endpoint, (unsigned char*)"", 0);
				result = LPCUSBErrorCode_OK;
			}

			return(result);
		}

	case LPCUSBEvent_In:
		if((gUSBState.lastSetupType & LPCUSBSetup_Type__Mask) == LPCUSBSetup_Type_Standard)
		{
			if(gUSBState.task == USBTask_ChangeAddressPending)
				gUSBState.task = USBTask_ChangeAddress;
			
		#ifdef USB_DEBUG
			UARTWriteStringSync("\nchaddr");
		#endif
			return(LPCUSBErrorCode_OK);
		}
		else /*if((gUSBState.lastSetupType & USBSetup_Recipient__Mask) == USBSetup_Recipient_Device)*/	// possibly prompt the next endpoint IN write to occur
		{
			Handler* h = findHandlerForDescriptor((LPCUSBDescriptorHeader const*)closure);
			if(h != 0)
			{
			#ifdef USB_DEBUG
				UARTWriteStringSync("\ncmpl");
			#endif
				return(h->handler(h->context, (LPCUSBDescriptorHeader const*)closure, 0));
			}
		#ifdef USB_DEBUG
			else
			{
				UARTWriteStringSync("\nNo H!");
			}
		#endif
		}
		break;

	case LPCUSBEvent_Out:
		{
			unsigned int result = LPCUSBErrorCode_Unhandled;
			if((gUSBState.lastSetupType & LPCUSBSetup_Type__Mask) == LPCUSBSetup_Type_Standard)
			{
				result = standardSetupHandler(&gUSBState, (LPCUSBDescriptorHeader const*)closure, 0);
			}
			else
			{
				Handler* h = findHandlerForDescriptor((LPCUSBDescriptorHeader const*)closure);

				if(h != 0)
					result = h->handler(h->context, (LPCUSBDescriptorHeader const*)closure, 0);
			#ifdef USB_DEBUG
				else
				{
					UARTWriteStringSync("\nNo H!");
				}
			#endif
			}
			return(result);
		}
	}

	return(LPCUSBErrorCode_Unhandled);
}

static LPCUSBErrorCode	standardSetupHandler(	void* context,
												LPCUSBDescriptorHeader const* endpoint,
												LPCUSBSetup const* setupPacket
											)
{
	(void)context;
	(void)endpoint;

	if(setupPacket == 0)	// is an OUT packet
	{
		// OUT packets to EP0 seem to be completely empty, perhaps a ROM API anomaly?
		return(LPCUSBErrorCode_OK);
	}

	// else is a SETUP
	switch(setupPacket->bRequest)
	{
	case LPCUSBDeviceRequest_ClearFeature:
		// feature in setupPacket->wValue
		return(LPCUSBErrorCode_SendZeroLengthPacket);

	case LPCUSBDeviceRequest_SetFeature:
		// feature in setupPacket->wValue
		return(LPCUSBErrorCode_SendZeroLengthPacket);

	case LPCUSBDeviceRequest_SetAddress:
		gUSBState.taskParameter = setupPacket->wValue;
		gUSBState.task = USBTask_ChangeAddressPending;
		
		return(LPCUSBErrorCode_SendZeroLengthPacket);

	case LPCUSBDeviceRequest_GetDescriptor:
		{
			unsigned char const* descriptor = 0;	// determine the right one to send
			size_t descriptorLength = 0;

			switch(setupPacket->wValue >> 8)
			{
			case LPCUSBDescriptorType_Device:
				descriptor = (unsigned char const*)gUSBState.descriptors.deviceDescriptor;
				descriptorLength = gUSBState.deviceDescriptorLength;
				break;
			case LPCUSBDescriptorType_Configuration:
			case LPCUSBDescriptorType_Interface:
			case LPCUSBDescriptorType_Endpoint:
			case LPCUSBDescriptorType_CSInterface:
			case LPCUSBDescriptorType_CSEndpoint:
				descriptor = (unsigned char const*)gUSBState.descriptors.fullSpeedDescriptor;
				descriptorLength = gUSBState.configurationDescriptorLength;
				break;
			case LPCUSBDescriptorType_String:
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
			if(setupPacket->wValue == (LPCUSBDescriptorType_Configuration << 8))
				descriptorLength = descriptor[2];	// account for sentinel
			else
				descriptorLength = descriptor[0];
			
			if(setupPacket->wLength < descriptorLength)
				descriptorLength = setupPacket->wLength;
			
			(*LPC11U00ROMAPI)->usb->hardware->EndpointWrite(	gUSBAPIHandle,
																0x80,
																(unsigned char*)descriptor,
																descriptorLength
															);
		}
		return(LPCUSBErrorCode_OK);

	case LPCUSBDeviceRequest_GetConfiguration:
		(*LPC11U00ROMAPI)->usb->hardware->EndpointWrite(	gUSBAPIHandle,
															0x80,
															(unsigned char*)"\x01",
															1
														);
		return(LPCUSBErrorCode_OK);

	case LPCUSBDeviceRequest_SetConfiguration:
		{
			(*LPC11U00ROMAPI)->usb->hardware->SetConfiguration(	gUSBAPIHandle,
																gUSBState.taskParameter
															);
			
			// configure all endpoints
			unsigned int index = 0;
			unsigned char const* ep;
			while((ep = findNthDescriptor(		(unsigned char const*)gUSBState.descriptors.fullSpeedDescriptor,
												gUSBState.configurationDescriptorLength,
												LPCUSBDescriptorType_Endpoint,
												index
											)) != 0)
			{
				unsigned int addr = ((LPCUSBDescriptorEndpoint*)ep)->endpointAddress;
				
#ifdef USB_DEBUG
				UARTWriteStringSync("\nenEP");
				UARTWriteIntSync(addr, NumberFormatter_Hexadecimal);
#endif //USB_DEBUG
				
				(*LPC11U00ROMAPI)->usb->hardware->ConfigureEndpoint(gUSBAPIHandle, (LPCUSBDescriptorEndpoint*)ep);

				(*LPC11U00ROMAPI)->usb->hardware->EnableEndpoint(gUSBAPIHandle, addr);

				index++;
			}
		}
		return(LPCUSBErrorCode_SendZeroLengthPacket);

	default:
		UARTWriteStringSync("?");
		break;
	}

	return(LPCUSBErrorCode_Unhandled);
}

////////////////////////////////////////////////////////////////


LPCUSBErrorCode		LPCUSBInterrupt(void)
{
	(*LPC11U00ROMAPI)->usb->hardware->ISREntrypoint(gUSBAPIHandle);

	switch(gUSBState.task)
	{
	case USBTask_ChangeAddress:
		// set the address on the USB controller manually
		*USBDeviceCommandStatus |= (gUSBState.taskParameter & 0x7F);
		
		gUSBState.task = USBTask_Idle;
		break;
	}
	return(LPCUSBErrorCode_OK);
}

void			LPCUSBInit(void)
{
	vmemset(&gUSBState, 0, sizeof(gUSBState));
	
	gUSBAPIHandle = 0;
}

LPCUSBErrorCode		LPCUSBSetDescriptor(void const* descriptor, unsigned int length)
{
	switch(((LPCUSBDescriptorHeader*)descriptor)->type)
	{
	case LPCUSBDescriptorType_Device:
		gUSBState.descriptors.deviceDescriptor = (LPCUSBDescriptorHeader*)descriptor;
		gUSBState.deviceDescriptorLength = length;
		break;
	case LPCUSBDescriptorType_Configuration:
		gUSBState.descriptors.fullSpeedDescriptor = (LPCUSBDescriptorHeader*)descriptor;
		gUSBState.descriptors.highSpeedDescriptor = (LPCUSBDescriptorHeader*)descriptor;
		gUSBState.configurationDescriptorLength = length - 1;
		break;
	case LPCUSBDescriptorType_String:
		gUSBState.descriptors.stringDescriptor = (LPCUSBDescriptorHeader*)descriptor;
		gUSBState.stringDescriptorLength = length;
		break;
	default:
		UARTWriteStringSync("\nset: bad descriptor");
		return(LPCUSBErrorCode_BadDescriptor);
	}
	return(LPCUSBErrorCode_OK);
}

LPCUSBErrorCode		LPCUSBStart(void)
{
	//USB setup
	LPCUSBHardwareInit hardwareInit =
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
	LPCUSBErrorCode usbError = (*LPC11U00ROMAPI)->usb->hardware->Init(&gUSBAPIHandle, &gUSBState.descriptors, &hardwareInit);
	if(usbError != 0)
	{
		UARTWriteStringSync("\ninit error: ");
		UARTWriteIntSync(usbError, NumberFormatter_Hexadecimal);
		return(usbError);
	}

	usbError = registerHandlersFromDescriptor(		(unsigned char const*)gUSBState.descriptors.fullSpeedDescriptor,
													gUSBState.configurationDescriptorLength
												);
	if(usbError != 0)
	{
		UARTWriteStringSync("\nreg handlers error: ");
		UARTWriteIntSync(usbError, NumberFormatter_Hexadecimal);
		return(usbError);
	}

	return(LPCUSBErrorCode_OK);
}

void			LPCUSBStop(void)
{
	// remove registered handlers
	for(Handler* h = gUSBState.handlers; h != 0;)
	{
		Handler* n = h;
		h = h->next;
		free(n);
	}
	gUSBState.handlers = 0;

	LPCUSBInit();
}

LPCUSBErrorCode		LPCUSBRegisterHandler(		void const* descriptor,
												LPCUSBHandler handler,
												void* context
											)
{
	switch(((LPCUSBDescriptorHeader const*)descriptor)->type)
	{
	case LPCUSBDescriptorType_Configuration:
	case LPCUSBDescriptorType_Endpoint:
		break;
	default:
		UARTWriteStringSync("\nreg: bad descriptor");
		return(LPCUSBErrorCode_BadDescriptor);
	}

	// ensure descriptor is unique in the set of handlers.
	Handler* h = gUSBState.handlers;
	while((h != 0) && (h->receiver != (LPCUSBDescriptorHeader const*)descriptor))
		h = h->next;
	if(h != 0)
	{
		UARTWriteStringSync("\nreg: duplicate handler");
		return(LPCUSBErrorCode_TooManyHandlers);
	}

	// allocate a new handler
	Handler* newHandler = (Handler*)malloc(sizeof(Handler));
	newHandler->receiver = (LPCUSBDescriptorHeader const*)descriptor;
	newHandler->handler = handler;
	newHandler->context = context;
	newHandler->next = 0;

	// add the handler to the list
	Handler** handlers = &gUSBState.handlers;
	while(*handlers != 0)
		handlers = &(*handlers)->next;
	*handlers = newHandler;

	return(LPCUSBErrorCode_OK);
}

LPCUSBErrorCode		LPCUSBConnect(void)
{
	*InterruptEnableSet0 |= Interrupt0_USB;
	(*LPC11U00ROMAPI)->usb->hardware->Connect(gUSBAPIHandle, 1);
	return(LPCUSBErrorCode_OK);
}

void			LPCUSBDisconnect(void)
{
	(*LPC11U00ROMAPI)->usb->hardware->Connect(gUSBAPIHandle, 0);
	*InterruptEnableSet0 &= ~Interrupt0_USB;
}


unsigned int	LPCUSBRead(unsigned int endpoint, unsigned char* dest)
{
	return((*LPC11U00ROMAPI)->usb->hardware->EndpointRead(gUSBAPIHandle, endpoint, dest));
}
unsigned int	LPCUSBWrite(unsigned int endpoint, unsigned char* source, unsigned int length)
{
	return((*LPC11U00ROMAPI)->usb->hardware->EndpointWrite(gUSBAPIHandle, endpoint, source, length));
}

void			LPCUSBSetStall(unsigned int endpoint, int stall)
{
	if(stall)
		(*LPC11U00ROMAPI)->usb->hardware->SetEndpointStall(gUSBAPIHandle, endpoint);
	else
		(*LPC11U00ROMAPI)->usb->hardware->ClearEndpointStall(gUSBAPIHandle, endpoint);
}
