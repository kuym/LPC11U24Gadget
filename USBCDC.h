#ifndef __USBCDC_H__
#define __USBCDC_H__

#include <LPC11U00.h>
#include <LPC11U00API.h>

using namespace LPC11U00;
using namespace LPC11U00::ROMUSB;

struct __attribute__((packed)) USBCDCLineCoding
{
	unsigned int	dwDTERate;		//Data terminal rate in bits per second
	unsigned char	bCharFormat;	//Number of stop bits
	unsigned char	bParityType;	//Parity bit type
	unsigned char	bDataBits;		//Number of data bits
};

//Communication interface class code, section 4.2, Table 15)
enum USBCDC_Class
{
	USBCDC_Class_CommunicationInterfaceClass			= 0x02,
};

// Communication interface class subclass codes
// (usbcdc11.pdf, 4.3, Table 16)
enum USBCDC_Subclass
{
	USBCDC_Subclass_DirectLineControlModel				= 0x01,
	USBCDC_Subclass_AbstractControlModel				= 0x02,
	USBCDC_Subclass_TelephoneControlModel				= 0x03,
	USBCDC_Subclass_MultiChannelControlModel			= 0x04,
	USBCDC_Subclass_CapiControlModel					= 0x05,
	USBCDC_Subclass_EthernetNetworkingControlModel		= 0x06,
	USBCDC_Subclass_ATMNetworkingControlModel			= 0x07,
};

// Communication interface class control protocol codes
// (usbcdc11.pdf, 4.4, Table 17)
enum USBCDC_ControlProtocol
{
	USBCDC_ControlProtocol_ProtocolCommonAtCommands		= 0x01,
};

// Data interface class code
// (usbcdc11.pdf, 4.5, Table 18)
enum USBCDC_Interface
{
	USBCDC_Interface_DataInterfaceClass					= 0x0A,
};

// Data interface class protocol codes
// (usbcdc11.pdf, 4.7, Table 19)
enum USBCC_Protocol
{
	USBCC_Protocol_ISDNBri					= 0x30,
	USBCC_Protocol_HDLC						= 0x31,
	USBCC_Protocol_Transparent				= 0x32,
	USBCC_Protocol_Q921Management			= 0x50,
	USBCC_Protocol_Q921DataLink				= 0x51,
	USBCC_Protocol_Q921Multiplexor			= 0x52,
	USBCC_Protocol_V42						= 0x90,
	USBCC_Protocol_EuroISDN					= 0x91,
	USBCC_Protocol_V24RateAdaptation		= 0x92,
	USBCC_Protocol_Capi						= 0x93,
	USBCC_Protocol_HostBasedDriver			= 0xFD,
	USBCC_Protocol_DescribedInPUFD			= 0xFE,
};

// Type values for bDescriptorType field of functional descriptors
// (usbcdc11.pdf, 5.2.3, Table 24)
enum USBCDC_ClassSpecific
{
	USBCDC_ClassSpecific_Interface			= 0x24,
	USBCDC_ClassSpecific_Endpoint			= 0x25,
};

// Type values for bDescriptorSubtype field of functional descriptors
// (usbcdc11.pdf, 5.2.3, Table 25)
enum CDC_Subtype
{
	CDC_Subtype_Header						= 0x00,
	CDC_Subtype_CallManagement				= 0x01,
	CDC_Subtype_AbstractControlManagement	= 0x02,
	CDC_Subtype_DirectLineManagement		= 0x03,
	CDC_Subtype_TelephoneRinger				= 0x04,
	CDC_Subtype_ReportingCapabilities		= 0x05,
	CDC_Subtype_Union						= 0x06,
	CDC_Subtype_CountrySelection			= 0x07,
	CDC_Subtype_TelephoneOperationalModes	= 0x08,
	CDC_Subtype_USBTerminal					= 0x09,
	CDC_Subtype_NetworkChannel				= 0x0A,
	CDC_Subtype_ProtocolUnit				= 0x0B,
	CDC_Subtype_ExtensionUnit				= 0x0C,
	CDC_Subtype_MultiChannelManagement		= 0x0D,
	CDC_Subtype_CapiControlManagement		= 0x0E,
	CDC_Subtype_EthernetNetworking			= 0x0F,
	CDC_Subtype_ATMNetworking				= 0x10,
};

// CDC class-specific request codes
// (usbcdc11.pdf, 6.2, Table 46)
// see Table 45 for info about the specific requests.
enum USBCDC_Request
{
	USBCDC_Request_SendEncapsulatedCommand		= 0x00,
	USBCDC_Request_GetEncapsulatedResponse		= 0x01,
	USBCDC_Request_SetCommFeature				= 0x02,
	USBCDC_Request_GetCommFeature				= 0x03,
	USBCDC_Request_ClearCommFeature				= 0x04,
	USBCDC_Request_SetAuxLineState				= 0x10,
	USBCDC_Request_SetHookState					= 0x11,
	USBCDC_Request_PulseSetup					= 0x12,
	USBCDC_Request_SendPulse					= 0x13,
	USBCDC_Request_SetPulseTime					= 0x14,
	USBCDC_Request_RingAuxJack					= 0x15,
	USBCDC_Request_SetLineCoding				= 0x20,
	USBCDC_Request_GetLineCoding				= 0x21,
	USBCDC_Request_SetControlLineState			= 0x22,
	USBCDC_Request_SendBreak					= 0x23,
	USBCDC_Request_SetRingerParameters			= 0x30,
	USBCDC_Request_GetRingerParameters			= 0x31,
	USBCDC_Request_SetOperationParameters		= 0x32,
	USBCDC_Request_GetOperationParameters		= 0x33,
	USBCDC_Request_SetLineParameters			= 0x34,
	USBCDC_Request_GetLineParameters			= 0x35,
	USBCDC_Request_DialDigits					= 0x36,
	USBCDC_Request_SetUnitParameter				= 0x37,
	USBCDC_Request_GetUnitParameter				= 0x38,
	USBCDC_Request_ClearUnitParameter			= 0x39,
	USBCDC_Request_GetProfile					= 0x3A,
	USBCDC_Request_SetEthernetMulticastFilters	= 0x40,
	USBCDC_Request_SetEthernetPMPFilter			= 0x41,
	USBCDC_Request_GetEthernetPMPFilter			= 0x42,
	USBCDC_Request_SetEthernetPacketFilter		= 0x43,
	USBCDC_Request_GetEthernetStatistic			= 0x44,
	USBCDC_Request_SetATMDataFormat				= 0x50,
	USBCDC_Request_GetATMDeviceStatistics		= 0x51,
	USBCDC_Request_SetATMDefaultVc				= 0x52,
	USBCDC_Request_GetATMVcStatistics			= 0x53,
};

// Communication feature selector codes
// (usbcdc11.pdf, 6.2.2..6.2.4, Table 47)
enum USBCDC_Feature
{
	USBCDC_Feature_AbstractState					= 0x01,
	USBCDC_Feature_CountrySetting					= 0x02,
};

// Feature Status returned for ABSTRACTState Selector
// (usbcdc11.pdf, 6.2.3, Table 48)
enum USBCDC_AbstractState
{
	USBCDC_AbstractState_IdleSetting				= (1 << 0),
	USBCDC_AbstractState_DataMultplexedState		= (1 << 1),
};

// Control signal bitmap values for the SetControlLineState request
// (usbcdc11.pdf, 6.2.14, Table 51)
enum USBCDC_LineControl
{
	USBCDC_ControlLine_DTEPresent					= (1 << 0),
	USBCDC_ControlLine_ActivateCarrier				= (1 << 1),
};

// CDC class-specific notification codes
// (usbcdc11.pdf, 6.3, Table 68)
// see Table 67 for Info about class-specific notifications
enum USBCDC_ClassNotification
{
	USBCDC_ClassNotification_NotificationNetworkConnection	= 0x00,
	USBCDC_ClassNotification_ResponseAvailable				= 0x01,
	USBCDC_ClassNotification_AuxJackHookState				= 0x08,
	USBCDC_ClassNotification_RingDetect						= 0x09,
	USBCDC_ClassNotification_NotificationSerialState		= 0x20,
	USBCDC_ClassNotification_CallStateChange				= 0x28,
	USBCDC_ClassNotification_LineStateChange				= 0x29,
	USBCDC_ClassNotification_ConnectionSpeedChange			= 0x2A,
};

// UART state bitmap values (Serial state notification).
// (usbcdc11.pdf, 6.3.5, Table 69)
enum USBCDC_State
{
	USBCDC_State_SerialStateOverrun				= (1 << 6),		// receive data overrun error has occurred
	USBCDC_State_SerialStateParity				= (1 << 5),		// parity error has occurred
	USBCDC_State_SerialStateFraming				= (1 << 4),		// framing error has occurred
	USBCDC_State_SerialStateRing				= (1 << 3),		// state of ring signal detection
	USBCDC_State_SerialStateBreak				= (1 << 2),		// state of break detection
	USBCDC_State_SerialStateTxCarrier			= (1 << 1),		// state of transmission carrier
	USBCDC_State_SerialStateRxCarrier			= (1 << 0),		// state of receiver carrier
};

////////////////////////////////////////////////////////////////

struct __attribute__((aligned(4))) USBCDCDevice
{
public:
	inline bool			connected() volatile const		{return(((USBCDCDevice volatile*)this)->_connected);}
	inline unsigned int	bytesAvailable() volatile const	{return(_readBuffer.used());}	// ...to read
	inline unsigned int	bytesFree() volatile const		{return(_writeBuffer.free());}	// ...in which to write

						USBCDCDevice(unsigned int readBufferSize, unsigned int writeBufferSize);

	unsigned int		read(unsigned char* outData, unsigned int length);
	unsigned char		readByte(void);
	unsigned int		write(unsigned char const* data, unsigned int length);
	void				writeByte(unsigned char b);
	
	static ErrorCode	usbCDCACMHandler(void* context, USBDescriptorHeader const* endpoint, USBSetup const* setupPacket);

private:
	USBCDCLineCoding	_lineCoding;
	unsigned int		_expecting;
	unsigned int		_connected;
	CircularBuffer		_readBuffer;
	CircularBuffer		_writeBuffer;
};

#endif //!defined __USBCDC_H__
