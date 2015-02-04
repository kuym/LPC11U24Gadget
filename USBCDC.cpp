#include "USBCDC.h"

#include <LPC11U00.h>
#include <LPC11U00API.h>
#include <USBAPI.h>
#include <string.h>		// for memcpy

using namespace LPC11U00;
using namespace LPC11U00::ROMAPI;

					USBCDCDevice::USBCDCDevice(unsigned int readBufferSize, unsigned int writeBufferSize)
{
	(void)writeBufferSize;
	vmemset(this, 0, sizeof(USBCDCDevice));

	_readBuffer.alloc(readBufferSize);
	//writeBuffer.alloc(writeBufferSize);	//@@none for now
}

unsigned int		USBCDCDevice::read(unsigned char* outData, unsigned int length)
{
	unsigned int bytesRead = _readBuffer.read(outData, length);

	// if we could now accept another [endpoint size] bytes, unstall the pipe
	if(_readBuffer.free() >= 64)	// @@[endpoint size]
		USB::SetStall(0x03, false);

	return(bytesRead);
}

unsigned char		USBCDCDevice::readByte(void)
{
	unsigned char b = 0;
	
	if(_readBuffer.free() > 0)
	{
		b = _readBuffer.readByte();
		
		// if we could now accept another [endpoint size] bytes, unstall the pipe
		if(_readBuffer.free() >= 64)	// @@[endpoint size]
			USB::SetStall(0x03, false);
	}

	return(b);
}

unsigned int		USBCDCDevice::write(unsigned char const* data, unsigned int length)
{
	//unsigned int bytesWritten = writeBuffer.write(data, length);

	// kick off USB write if we can ...?????

	//unsigned char buffer[64];
	//unsigned int chunkLen
	
	unsigned char notification[8] = {0xA1, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	USB::Write(0x81, notification, 8);	// @@hack
	return(USB::Write(0x83, (unsigned char*)data, length));	// @@bad bad bad hack
}

void				USBCDCDevice::writeByte(unsigned char b)
{
	USB::Write(0x83, &b, 1);	// @@bad bad bad hack
}

ErrorCode			USBCDCDevice::usbCDCACMHandler(void* context, USBDescriptorHeader const* endpoint, USBSetup const* setupPacket)
{
	unsigned int endpointAddress = (endpoint->type == DescriptorType_Endpoint)?
										((USBDescriptorEndpoint*)endpoint)->endpointAddress
										: 0;
	
	// debug
	/*
	if(endpointAddress == 0)
	{
		UART::writeSync("\ncls:");
		UART::writeHexDumpSync((unsigned char*)setupPacket, sizeof(USBSetup));
	}
	else
	{
		UART::writeSync("\nep");
		UART::writeSync(endpointAddress, NumberFormatter::Hexadecimal);
		UART::writeSync(":");
	}
	*/
	// end debug

	//UART::writeSync("#");
	USBCDCDevice* state = (USBCDCDevice*)context;

	if(setupPacket != 0)
	{
		switch(setupPacket->bRequest)
		{
		case USBCDC_Request_SetLineCoding:
			state->_expecting = USBCDC_Request_SetLineCoding;
			return(ErrorCode_SendZeroLengthPacket);

		case USBCDC_Request_GetLineCoding:
			USB::Write(		0x80,
							(unsigned char*)&state->_lineCoding,
							sizeof(USBCDCLineCoding)
						);
			return(ErrorCode_OK);

		case USBCDC_Request_SetControlLineState:
			if(setupPacket->wValue & USBCDC_ControlLine_DTEPresent)
			{
			}
			if(setupPacket->wValue & USBCDC_ControlLine_ActivateCarrier)
			{
				if(!state->_connected)
					UART::writeSync("\nonline");
				state->_connected = 1;
			}
			else
			{
				if(state->_connected)
					UART::writeSync("\noffline");
				state->_connected = 0;
			}

			return(ErrorCode_SendZeroLengthPacket);
		}
	}
	else if(endpointAddress == 0x00)
	{
		unsigned char buffer[64];
		unsigned int bytesRead = USB::Read(endpointAddress, buffer);
		
		switch(state->_expecting)
		{
		case USBCDC_Request_SetLineCoding:
			if(bytesRead != sizeof(USBCDCLineCoding))
			{
				UART::writeSync(" size mismatch!");
				break;
			}
			memcpy(&state->_lineCoding, buffer, sizeof(USBCDCLineCoding));
			UART::writeSync("\nformat: ");
			UART::writeSync(state->_lineCoding.dwDTERate, NumberFormatter::DecimalUnsigned);
			UART::writeSync(",");
			UART::writeSync(state->_lineCoding.bDataBits, NumberFormatter::DecimalUnsigned);
			UART::writeSync(state->_lineCoding.bParityType? "p" : "n");
			UART::writeSync(state->_lineCoding.bCharFormat + 1, NumberFormatter::DecimalUnsigned);
			state->_expecting = 0;
			break;

		default:
			UART::writeSync("\nunhandled class ep0 rd:");
			UART::writeSync(bytesRead, NumberFormatter::DecimalUnsigned);
			UART::writeSync(">");
			UART::writeHexDumpSync(buffer, bytesRead);

			break;
		}
	}
	else if(endpointAddress == 0x03)	// endpoint 0x02 OUT
	{
		unsigned char buffer[64];
		unsigned int bytesRead = USB::Read(endpointAddress, buffer);
		
		//UART::writeSync("\nep2 rd:");
		//UART::writeSync(bytesRead, NumberFormatter::DecimalUnsigned);
		//UART::writeSync(">");
		//UART::writeHexDumpSync(buffer, bytesRead);

		state->_readBuffer.write(buffer, bytesRead);

		// if we couldn't accept another [endpoint size] bytes, stall the pipe
		if(state->_readBuffer.free() < 64)
			USB::SetStall(0x03, true);
	}
	else if(endpointAddress == 0x83)	// endpoint 0x82 IN
	{
		// anything additional to send in to the host?
	}

	return(ErrorCode_OK);
}
