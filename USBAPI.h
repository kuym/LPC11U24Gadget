#ifndef __USBAPI_H__
#define __USBAPI_H__

#include <LPC11U00.h>

typedef LPCUSBErrorCode (*LPCUSBHandler)(	void* context,
											LPCUSBDescriptorHeader const* descriptor,
											LPCUSBSetup const* setupPacket
										);
	
LPCUSBErrorCode		LPCUSBInterrupt(void);
void				LPCUSBInit(void);
LPCUSBErrorCode		LPCUSBSetDescriptor(	void const* descriptor,
											unsigned int length
										);

// descriptor: pass a pointer to an endpoint descriptor to receive events for that endpoint,
//   or pass a pointer to the configuration descriptor to receive all other events
//   (including class and vendor requests for the device and interface)
// each handler can receive SETUP or OUT events.  You can distinguish these as follows:
//   if the setupPacket callback parameter is not 0, it is a SETUP packet and the packet
//     structure is available with that pointer.
//   otherwise, it is an OUT packet.
LPCUSBErrorCode		LPCUSBRegisterHandler(		void const* descriptor,
												LPCUSBHandler handler,
												void* context
											);
LPCUSBErrorCode		LPCUSBStart(void);
void				LPCUSBStop(void);

LPCUSBErrorCode		LPCUSBConnect(void);
void				LPCUSBDisconnect(void);
unsigned int		LPCUSBRead(unsigned int endpoint, unsigned char* dest);
unsigned int		LPCUSBWrite(unsigned int endpoint, unsigned char* source, unsigned int length);
void				LPCUSBSetStall(unsigned int endpoint, int stall);


#endif //!defined __USBAPI_H__
