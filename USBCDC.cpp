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

	readBuffer.alloc(readBufferSize);
	//writeBuffer.alloc(writeBufferSize);	//@@none for now
}

unsigned int		USBCDCDevice::Read(unsigned char* outData, unsigned int length)
{
	unsigned int bytesRead = readBuffer.read(outData, length);

	// if we could now accept another [endpoint size] bytes, unstall the pipe
	if(readBuffer.free() >= 64)	// @@[endpoint size]
		USB::SetStall(0x03, false);

	return(bytesRead);
}

unsigned int		USBCDCDevice::Write(unsigned char const* data, unsigned int length)
{
	//unsigned int bytesWritten = writeBuffer.write(data, length);

	// kick off USB write if we can ...?????

	//unsigned char buffer[64];
	//unsigned int chunkLen
	
	return(USB::Write(0x83, (unsigned char*)data, length));	// @@bad bad bad hack
}

ErrorCode			USBCDCDevice::USBCDCACMHandler(void* context, USBDescriptorHeader const* endpoint, USBSetup const* setupPacket)
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

	USBCDCDevice* state = (USBCDCDevice*)context;

	if(setupPacket != 0)
	{
		switch(setupPacket->bRequest)
		{
		case USBCDC_Request_SetLineCoding:
			state->expecting = USBCDC_Request_SetLineCoding;
			return(ErrorCode_SendZeroLengthPacket);

		case USBCDC_Request_GetLineCoding:
			USB::Write(		0x80,
							(unsigned char*)&state->lineCoding,
							sizeof(USBCDCLineCoding)
						);
			return(ErrorCode_OK);

		case USBCDC_Request_SetControlLineState:
			if(setupPacket->wValue & USBCDC_ControlLine_DTEPresent)
			{
			}
			if(setupPacket->wValue & USBCDC_ControlLine_ActivateCarrier)
			{
				if(!state->isConnected)
					UART::writeSync("\nonline");
				state->isConnected = 1;
			}
			else
			{
				if(state->isConnected)
					UART::writeSync("\noffline");
				state->isConnected = 0;
			}

			return(ErrorCode_SendZeroLengthPacket);
		}
	}
	else if(endpointAddress == 0x00)
	{
		unsigned char buffer[64];
		unsigned int bytesRead = USB::Read(endpointAddress, buffer);
		
		switch(state->expecting)
		{
		case USBCDC_Request_SetLineCoding:
			if(bytesRead != sizeof(USBCDCLineCoding))
			{
				UART::writeSync(" size mismatch!");
				break;
			}
			memcpy(&state->lineCoding, buffer, sizeof(USBCDCLineCoding));
			UART::writeSync("\nformat: ");
			UART::writeSync(state->lineCoding.dwDTERate, NumberFormatter::DecimalUnsigned);
			UART::writeSync(",");
			UART::writeSync(state->lineCoding.bDataBits, NumberFormatter::DecimalUnsigned);
			UART::writeSync(state->lineCoding.bParityType? "p" : "n");
			UART::writeSync(state->lineCoding.bCharFormat + 1, NumberFormatter::DecimalUnsigned);
			state->expecting = 0;
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
		
		UART::writeSync("\nep2 rd:");
		UART::writeSync(bytesRead, NumberFormatter::DecimalUnsigned);
		UART::writeSync(">");
		UART::writeHexDumpSync(buffer, bytesRead);

		state->readBuffer.write(buffer, bytesRead);

		// if we couldn't accept another [endpoint size] bytes, stall the pipe
		if(state->readBuffer.free() < 64)
			USB::SetStall(0x03, true);
	}
	else if(endpointAddress == 0x83)	// endpoint 0x82 IN
	{
		// anything additional to send in to the host?
	}

	return(ErrorCode_OK);
}
