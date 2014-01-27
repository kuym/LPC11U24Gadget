#ifndef _LPCUSB_H_
#define _LPCUSB_H_

namespace LPCUSB
{
	typedef unsigned int ErrorCode;
	typedef void* USBHandle;

	enum USBEndpointDirection
	{
		USBEndpointDirection_Out	= 0,
		USBEndpointDirection_In		= 1
	};

	enum USBEndpointType
	{
		USBEndpointType_Control		= 0x00,
		USBEndpointType_Isochronous	= 0x01,
		USBEndpointType_Bulk		= 0x02,
		USBEndpointType_Interrupt   = 0x03
	};

	enum USBEndpointAttribute
	{
		USBEndpointAttribute_NoSync		= (0 << 2),
	};
	enum USBEndpointUsage
	{
		USBEndpointUsage_Data			= (0 << 4),
	};

	enum USBDescriptorType
	{
		DescriptorType_Device				= 0x01, // Indicates that the descriptor is a device descriptor.
		DescriptorType_Configuration		= 0x02, // Indicates that the descriptor is a configuration descriptor.
		DescriptorType_String				= 0x03, // Indicates that the descriptor is a string descriptor.
		DescriptorType_Interface			= 0x04, // Indicates that the descriptor is an interface descriptor.
		DescriptorType_Endpoint				= 0x05, // Indicates that the descriptor is an endpoint descriptor.
		DescriptorType_DeviceQualifier		= 0x06, // Indicates that the descriptor is a device qualifier descriptor.
		DescriptorType_Other				= 0x07, // Indicates that the descriptor is of other type.
		DescriptorType_InterfacePower		= 0x08, // Indicates that the descriptor is an interface power descriptor.
		DescriptorType_InterfaceAssociation	= 0x0B, // Indicates that the descriptor is an interface association descriptor.
		DescriptorType_CSInterface			= 0x24, // Indicates that the descriptor is a class specific interface descriptor.
		DescriptorType_CSEndpoint			= 0x25 // Indicates that the descriptor is a class specific endpoint descriptor.
	};

	enum USBDeviceRequest
	{
		USBDeviceRequest_GetStatus			= 0,	// GET_STATUS
		USBDeviceRequest_ClearFeature		= 1,	// CLEAR_FEATURE
		USBDeviceRequest_SetFeature			= 3,	// SET_FEATURE
		USBDeviceRequest_SetAddress			= 5,	// SET_ADDRESS
		USBDeviceRequest_GetDescriptor		= 6,	// GET_DESCRIPTOR
		USBDeviceRequest_SetDescriptor		= 7,	// SET_DESCRIPTOR
		USBDeviceRequest_GetConfiguration	= 8,	// GET_CONFIGURATION
		USBDeviceRequest_SetConfiguration	= 9,	// SET_CONFIGURATION
		USBDeviceRequest_GetInterface		= 10,	// GET_INTERFACE
		USBDeviceRequest_SetInterface		= 11,	// SET_INTERFACE
		USBDeviceRequest_SynchFrame			= 12	// SYNCH_FRAME
	};

	enum USBEvent
	{
		USBEvent_Setup = 1,		// 1	Setup Packet received
		USBEvent_Out,			// 2	OUT Packet received
		USBEvent_In,			// 3	IN Packet sent
		USBEvent_OutNAK,		// 4	OUT Packet - Not Acknowledged
		USBEvent_InNAK,			// 5	IN Packet - Not Acknowledged
		USBEvent_OutStall,		// 6	OUT Packet - Stalled
		USBEvent_InStall,		// 7	IN Packet - Stalled
		USBEvent_OutDMAEnd,		// 8	DMA OUT EP - End of Transfer
		USBEvent_InDMAEnd,		// 9	DMA  IN EP - End of Transfer
		USBEvent_OutDMANew,		// 10	DMA OUT EP - New Descriptor Request
		USBEvent_InDMANew,		// 11	DMA  IN EP - New Descriptor Request
		USBEvent_OutDMAError,	// 12	DMA OUT EP - Error
		USBEvent_InDMAError,	// 13	DMA  IN EP - Error
		USBEvent_Reset,			// 14	Reset event recieved
		USBEvent_SOF,			// 15	Start of Frame event
		USBEvent_DeviceState,	// 16	Device status events
		USBEvent_DeviceError,	// 17	Device error events
	};
	
	enum ErrorCodes
	{
		ErrorCode_OK					= 0x0,
		ErrorCode_Unhandled 			= 0x00040002,	// Callback did not process the event
		ErrorCode_Stall     			= 0x00040003,	// Stall the endpoint on which the call back is called
		ErrorCode_SendZeroLengthPacket	= 0x00040004,	// Send ZLP packet on the endpoint on which the call back is called
		ErrorCode_SendData	 			= 0x00040005,	// Send data packet on the endpoint on which the call back is called
		ErrorCode_BadDescriptor			= 0x00040006,	// Bad descriptor
		
		ErrorCode_TooManyHandlers		= 0x0004000c,	// Too many [class] handlers
	};

	typedef ErrorCode (*EndpointHandler)(	USBHandle usb,
											void* closure,
											USBEvent endpointEvent
										);
	
	enum USBSetupRequestType
	{
		USBSetup_Direction_ToDevice		= (0 << 7),
		USBSetup_Direction_ToHost		= (1 << 7),
		USBSetup_Direction__Mask		= (1 << 7),
		
		USBSetup_Type_Standard			= (0 << 5),
		USBSetup_Type_Class				= (1 << 5),
		USBSetup_Type_Vendor			= (2 << 5),
		USBSetup_Type_Reserved			= (3 << 5),
		USBSetup_Type__Mask				= (3 << 5),
		
		USBSetup_Recipient_Device		= (0 << 0),
		USBSetup_Recipient_Interface	= (1 << 0),
		USBSetup_Recipient_Endpoint		= (2 << 0),
		USBSetup_Recipient_Other		= (3 << 0),
		USBSetup_Recipient__Mask		= (0x1F),
	};

	struct __attribute__((packed)) USBSetup
	{
		unsigned char	bmRequestType;
		unsigned char	bRequest;
		unsigned short	wValue;
		unsigned short	wIndex;
		unsigned short	wLength;
	};

	struct __attribute__((packed)) ACMLineCoding
	{
		unsigned int	dwDTERate;
		unsigned char	bCharFormat;
		unsigned char	bParityType;
		unsigned char	bDataBits;
	};

	struct __attribute__((packed)) USBDescriptorHeader
	{
		unsigned char	size;	// Size of the descriptor, in bytes.
		unsigned char	type;	// Type of the descriptor, either one of USBDescriptorType_* or a value
	};

	struct __attribute__((packed)) USBDescriptorDevice
	{
		USBDescriptorHeader header;				// Descriptor header, including type and size.
		
		unsigned short	usbSpecification;		// BCD of the supported USB specification.
		unsigned char	deviceClass;			// USB device class.
		unsigned char	subClass;				// USB device subclass.
		unsigned char	protocol;				// USB device protocol.
		
		unsigned char	endpoint0Size;			// Size of the control (address 0) endpoint's bank in bytes.
		
		unsigned short	vendorID;				// Vendor ID for the USB product.
		unsigned short	productID;				// Unique product ID for the USB product.
		unsigned short	releaseNumber;			// Product release (version) number.
		
		unsigned char	manufacturerStrIndex;	// String index for the manufacturer's name.
		unsigned char	productStrIndex; 		// String index for the product name/details.
		unsigned char	serialNumStrIndex;		// String index for the product's globally unique hexadecimal
		unsigned char	numberOfConfigurations;	// Total number of configurations supported by
	};

	struct __attribute__((packed)) USBDescriptorConfigurationHeader
	{
		USBDescriptorHeader	header;					// Descriptor header, including type and size.

		unsigned short		totalConfigurationSize;	// Size of the configuration descriptor header
		unsigned char		totalInterfaces;		// Total number of interfaces in the configuration.

		unsigned char		configurationNumber;	// Configuration index of the current configuration.
		unsigned char		configurationStrIndex;	// Index of a string descriptor describing the configuration.

		unsigned char		configAttributes;		// Configuration attributes, comprised of a mask of zero or
		
		unsigned char		maxPowerConsumption;	// Maximum power consumption of the device while in the
	};
	
	struct __attribute__((packed)) USBDescriptorInterface
	{
		USBDescriptorHeader	header;				// Descriptor header, including type and size.

		unsigned char		interfaceNumber;	// Index of the interface in the current configuration.
		unsigned char		alternateSetting;	// Alternate setting for the interface number. The same
		unsigned char		totalEndpoints;		// Total number of endpoints in the interface.

		unsigned char		interfaceClass;		// Interface class ID.
		unsigned char		subClass;			// Interface subclass ID.
		unsigned char		protocol;			// Interface protocol ID.

		unsigned char		interfaceStrIndex;	// Index of the string descriptor describing the interface.
	};

	struct __attribute__((packed)) USBDescriptorEndpoint
	{
		USBDescriptorHeader	header;			// Descriptor header, including type and size.

		unsigned char	endpointAddress;	// Logical address of the endpoint within the device for the current
		unsigned char	attributes;			// Endpoint attributes, comprised of a mask of the endpoint type (EP_TYPE_*)
		unsigned short	endpointSize;		// Size of the endpoint bank, in bytes. This indicates the maximum packet
		unsigned char	pollingIntervalMS;	// Polling interval in milliseconds for the endpoint if it is an INTERRUPT
	};

	////////////////////////////////////////////////////////////////
	
	struct __attribute__((packed)) CDCDescriptorFunctionalHeader
	{
		USBDescriptorHeader	header;				// Descriptor header, including type and size.
	
		unsigned char		subType;			// Sub type value used to distinguish between CDC class-specific descriptors
		unsigned short		cdcSpecification;	// Version number of the CDC specification implemented by the device
	};

	struct __attribute__((packed)) CDCDescriptorFunctionalACM
	{
		USBDescriptorHeader	header;				// Descriptor header, including type and size.
	
		unsigned char		subType;			// Sub type value used to distinguish between CDC class-specific descriptors
		unsigned char		capabilities;		// Capabilities of the ACM interface, given as a bit mask. For most devices, 0x06
	};
	
	struct __attribute__((packed)) CDCDescriptorFunctionalUnion
	{
		USBDescriptorHeader	header;					// Descriptor header, including type and size.
	
		unsigned char		subType;				// Sub type value used to distinguish between CDC class-specific descriptors
		unsigned char		masterInterfaceNumber;	// Interface number of the CDC Control interface
		unsigned char		slaveInterfaceNumber;	// Interface number of the CDC Data interface
	};
	
	////////////////////////////////////////////////////////////////
	
	struct CDCInit
	{
		unsigned int	memBase;
		unsigned int	size;
		void const*		controlInterfaceDescriptor;
		void const*		dataInterfaceDescriptor;

		ErrorCode	(*CICGetRequest)(	USBHandle cdc,
										USBSetup* setup,
										unsigned char** buffer,
										unsigned short* length
									);

		ErrorCode	(*CICSetRequest)(	USBHandle cdc,
										USBSetup* setup,
										unsigned char** buffer,
										unsigned short length
									);

		ErrorCode	(*CDCBulkIn)(		USBHandle usb,
										void* closure,
										USBEvent endpointEvent
									);

		ErrorCode	(*CDCBulkOut)(		USBHandle usb,
										void* closure,
										USBEvent endpointEvent
									);

		ErrorCode	(*ACMSetEncapsulatedCommand)(	USBHandle cdc,
													unsigned char* buffer,
													unsigned short length
												);

		ErrorCode	(*ACMGetEncapsulatedCommand)(	USBHandle cdc,
													unsigned char** buffer,
													unsigned short* length
												);

		ErrorCode	(*ACMSetCommFeature)(	USBHandle cdc,
											unsigned short feature,
											unsigned char* buffer,
											unsigned short length
										);

		ErrorCode	(*ACMGetCommFeature)(	USBHandle cdc,
											unsigned short feature,
											unsigned char** buffer,
											unsigned short* length
										);

		ErrorCode	(*ACMClearCommFeature)(		USBHandle cdc,
												unsigned short feature
											);

		ErrorCode	(*ACMSetControlLineState)(		USBHandle cdc,
													unsigned short stateFlags
												);

		ErrorCode	(*ACMSendBreak)(	USBHandle cdc,
										unsigned short breakDurationMs
									);

		ErrorCode	(*ACMSetLineCoding)(	USBHandle cdc,
											ACMLineCoding* options
										);

		ErrorCode	(*CDCInterruptHandler)(		USBHandle usb,
												void* closure,
												unsigned int endpointEvent
											);

		ErrorCode	(*CDCEndpoint0Handler)(		USBHandle usb,
												void* closure,
												unsigned int endpointEvent
											);
	};

	struct CDCAPI
	{
		unsigned int	(*GetMemSize)(CDCInit* init);
		
		ErrorCode		(*Init)(	USBHandle usb,
									CDCInit* init,
									USBHandle* outCDC
								);

		ErrorCode		(*SendNotification)(	USBHandle usb,
												unsigned char notification,
												unsigned short data
											);
	};

	struct CoreAPI
	{
		ErrorCode	(*RegisterClassHandler)(	USBHandle usb,
												EndpointHandler handler,
												void* closure
											);

		ErrorCode	(*RegisterEndpointHandler)(		USBHandle usb,
													unsigned int endpointIndex,
													EndpointHandler handler,
													void* closure
												);

		// specialized hooks
		void		(*SetupStage)(USBHandle usb);
		void		(*DataInStage)(USBHandle usb);
		void		(*DataOutStage)(USBHandle usb);
		void		(*StatusInStage)(USBHandle usb);
		void		(*StatusOutStage)(USBHandle usb);
		void		(*StallEndpoint0)(USBHandle usb);
	};
	
	struct CoreDescriptors
	{
		void const*		deviceDescriptor;
		void const*		stringDescriptor;
		void const*		fullSpeedDescriptor;
		void const*		highSpeedDescriptor;
		void const*		deviceQualifier;
	};
	
	struct HardwareInit
	{
		unsigned int	usbRegisterBase;
		unsigned int	memBase;
		unsigned int	memSize;
		unsigned char	maxNumEndpoints;
		unsigned char	__reserved0[3];

		ErrorCode		(*ResetHandler)();
		ErrorCode		(*SuspendHandler)();
		ErrorCode		(*ResumeHandler)();
		void			(*__reserved1)();
		ErrorCode		(*SOFEventHandler)();
		ErrorCode		(*WakeUpEventHandler)();
		void			(*__reserved2)();
		ErrorCode		(*ErrorHandler)();
		ErrorCode		(*ConfigureHandler)();
		ErrorCode		(*InterfaceHandler)();
		ErrorCode		(*FeatureHandler)();
		void			(*__reserved3)();
		void			(*__reserved4)();
	};

	struct HardwareAPI
	{
		unsigned int	(*GetMemSize)(HardwareInit* init);

		ErrorCode		(*Init)(	USBHandle* outUSB,
									CoreDescriptors* descriptors,
									HardwareInit* init
								);

		void			(*Connect)(		USBHandle usb,
										bool connect
									);

		void			(*ISREntrypoint)(USBHandle usb);	//call directly from USB IRQ handler
		
		void			(*Reset)(USBHandle usb);

		void			(*ForceFullSpeed)(		USBHandle usb,
												bool fullSpeed
											);

		void			(*SetWakeOnEvent)(		USBHandle usb,
												bool wakeOnEvent
											);

		//special override function, not normally necessary to call
		void			(*SetAddress)(		USBHandle usb,
											unsigned int usbAddress
										);
		//special override function, not normally necessary to call
		void			(*SetConfiguration)(	USBHandle usb,
												unsigned int configurationIndex
											);
		//special override function, not normally necessary to call
		void			(*ConfigureEndpoint)(	USBHandle usb,
												USBDescriptorEndpoint* descriptor
											);
		//special override function, not normally necessary to call
		void			(*SetEndpointDirection)(	USBHandle usb,
													unsigned int direction	// 0 is OUT, 1 is IN
												);
		
		void			(*EnableEndpoint)(		USBHandle usb,
												unsigned int endpointNum
											);
		
		void			(*DisableEndpoint)(		USBHandle usb,
												unsigned int endpointNum
											);
		
		void			(*ResetEndpoint)(		USBHandle usb,
												unsigned int endpointNum
											);
		
		void			(*SetEndpointStall)(	USBHandle usb,
												unsigned int endpointNum
											);
		
		void			(*ClearEndpointStall)(		USBHandle usb,
													unsigned int endpointNum
												);

		void			(*SetTestMode)(		USBHandle usb,
											unsigned char testMode
										);

		unsigned int	(*EndpointRead)(	USBHandle usb,
											unsigned int endpointNum,
											unsigned char* data
										);

		unsigned int	(*EndpointReadRequest)(		USBHandle usb,
													unsigned int endpointNum,
													unsigned char* data,
													unsigned int length
												);

		unsigned int	(*EndpointReadSetup)(	USBHandle usb,
												unsigned int endpointNum,
												unsigned int* data
											);


		unsigned int	(*EndpointWrite)(	USBHandle usb,
											unsigned int endpointNum,
											unsigned char* data,
											unsigned int length
										);

		void			(*WakeUp)(USBHandle usb);

		ErrorCode		(*EnableEvent)(		USBHandle usb,
											unsigned int endpointNum,
											unsigned int eventType,
											unsigned int enable
										);
	};

	struct USBAPI
	{
		HardwareAPI*		hardware;
		CoreAPI*			core;
		void*				unimplementedMSC;
		void*				unimplementedDFU;
		void*				unimplementedHID;
		CDCAPI*				cdc;
		void*				__reserved0;
		unsigned int		version;
	};

	struct idivResult
	{
		int				quotient;
		int				remainder;
	};
	struct uidivResult
	{
		unsigned int	quotient;
		unsigned int	remainder;
	};

	struct DividerAPI
	{
		int				(*sidiv)(int numerator, int denominator);
		unsigned int	(*uidiv)(unsigned int numerator, unsigned int denominator);
		idivResult		(*sidivmod)(int numerator, int denominator);
		uidivResult 	(*uidivmod)(unsigned int numerator, unsigned int denominator);
	};

	struct LPCAPI
	{
		USBAPI*				usb;
		void*				_reserved[3];
		DividerAPI*			divider;
	};

	LPCAPI** const API = (LPCAPI**)0x1FFF1FF8;

}	//ns

#endif //!defined _LPCUSB_H_
