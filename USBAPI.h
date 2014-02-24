#ifndef __USBAPI_H__
#define __USBAPI_H__

#include <LPC11U00.h>

using namespace LPC11U00;
using namespace LPC11U00::ROMUSB;

typedef ErrorCode (*USBHandler)(	void* context,
									USBDescriptorHeader const* descriptor,
									USBSetup const* setupPacket
								);
class USB
{
public:
	static	ErrorCode		Interrupt(void);
	static	void			Init(void);
	static	ErrorCode		SetDescriptor(		void const* descriptor,
												unsigned int length
											);
	// descriptor: pass a pointer to an endpoint descriptor to receive events for that endpoint,
	//   or pass a pointer to the configuration descriptor to receive all other events
	//   (including class and vendor requests for the device and interface)
	// each handler can receive SETUP or OUT events.  You can distinguish these as follows:
	//   if the setupPacket callback parameter is not 0, it is a SETUP packet and the packet
	//     structure is available with that pointer.
	//   otherwise, it is an OUT packet.
	static	ErrorCode		RegisterHandler(	void const* descriptor,
												USBHandler handler,
												void* context
											);
	static	ErrorCode		Start(void);
	static	void			Stop(void);
	
	static	ErrorCode		Connect(void);
	static	void			Disconnect(void);
	static	unsigned int	Read(unsigned int endpoint, unsigned char* dest);
	static	unsigned int	Write(unsigned int endpoint, unsigned char* source, unsigned int length);
	static	void			SetStall(unsigned int endpoint, bool stall);
};

#endif //!defined __USBAPI_H__
