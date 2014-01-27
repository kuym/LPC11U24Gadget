#ifndef __USBAPI_H__
#define __USBAPI_H__

#include <LPCUSB.h>
using namespace LPCUSB;

typedef ErrorCode (*USBHandler)(	void* context,
									USBDescriptorHeader const* descriptor,
									USBSetup const* setupPacket
								);
class USB
{
	static	ErrorCode		Interrupt(void);
	static	void			Init(void);
	static	ErrorCode		setDescriptor(		unsigned char const* descriptor,
												unsigned int length
											);
	static	ErrorCode		start(void);
	static	void			stop(void);

	// descriptor: pass a pointer to an endpoint descriptor to receive events for that endpoint,
	//   or pass a pointer to the configuration descriptor to receive all other events
	//   (including class and vendor requests for the device and interface)
	// each handler can receive SETUP or OUT events.  You can distinguish these as follows:
	//   if the setupPacket callback parameter is not 0, it is a SETUP packet and the packet
	//     structure is available with that pointer.
	//   otherwise, it is an OUT packet.
	static	ErrorCode		registerHandler(	USBDescriptorHeader* descriptor,
												USBHandler handler,
												void* context
											);
	static	ErrorCode		connect(void);
	static	void			disconnect(void);
	static	unsigned int	read(unsigned int endpoint, unsigned char* dest, unsigned int length);
	static	unsigned int	write(unsigned int endpoint, unsigned char* source, unsigned int length);
};

#endif //!defined __USBAPI_H__
