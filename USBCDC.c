#include "USBCDC.h"

#include <LPC11U00.h>
#include <LPC11U00API.h>
#include <USBAPI.h>
#include <string.h>		// for memcpy


#define USB_DEBUG 1


int					USBCDCInit(USBCDCDevice* cdc, unsigned int readBufferSize, unsigned int writeBufferSize)
{
	vmemset(cdc, 0, sizeof(USBCDCDevice));

	CircularBufferInit(&cdc->_readBuffer, readBufferSize);
	CircularBufferInit(&cdc->_writeBuffer, writeBufferSize);
	cdc->_writing = 0;
	cdc->_writeLength = 0;

	return(1);
}

unsigned int		USBCDCRead(USBCDCDevice* cdc, unsigned char* outData, unsigned int length)
{
	unsigned int bytesRead = CircularBufferRead(&cdc->_readBuffer, outData, length);

	// if we could now accept another [endpoint size] bytes, unstall the pipe
	if(CircularBufferFree(&cdc->_readBuffer) >= 64)	// @@[endpoint size]
	{
		#ifdef USB_DEBUG
			UARTWriteStringSync("\nep2 !stl");
		#endif //USB_DEBUG
		//USBSetStall(0x03, false);
	}

	return(bytesRead);
}

unsigned char		USBCDCReadByte(USBCDCDevice* cdc)
{
	unsigned char b = 0;
	
	if(CircularBufferFree(&cdc->_readBuffer) > 0)
	{
		b = CircularBufferReadByte(&cdc->_readBuffer);
		
		// if we could now accept another [endpoint size] bytes, unstall the pipe
		if(CircularBufferFree(&cdc->_readBuffer) >= 64)	// @@[endpoint size]
		{
		#ifdef USB_DEBUG
			UARTWriteStringSync("\nep2 !stl");
		#endif //USB_DEBUG
			//USBSetStall(0x03, false);
		}
	}

	return(b);
}

unsigned int		USBCDCWrite(USBCDCDevice* cdc, unsigned char const* data, unsigned int length)
{
	//unsigned int bytesWritten = writeBuffer.write(data, length);

	// kick off USB write if we can ...?????

	//unsigned char buffer[64];
	//unsigned int chunkLen
	
	//unsigned char notification[8] = {0xA1, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	//LPCUSBWrite(0x82, notification, 8);	// @@hack

	return(CircularBufferWrite(&cdc->_writeBuffer, data, length));
}

unsigned int		USBCDCWriteByte(USBCDCDevice* cdc, unsigned char b)
{
	//LPCUSBWrite(0x83, &b, 1);	// @@bad bad bad hack
	return(CircularBufferWriteByte(&cdc->_writeBuffer, b));
}

int					USBCDCFlush(USBCDCDevice* cdc)
{
	// dump the buffer into the USB subsystem and notify the host it's ready for pick-up
	if(!cdc->_writing)
	{
		unsigned int length = CircularBufferSegmentSize(&cdc->_writeBuffer);

	#ifdef USB_DEBUG
		UARTWriteStringSync("\nep83 w(");
		UARTWriteHexIntSync((unsigned int)CircularBufferSegment(&cdc->_writeBuffer), 4);
		UARTWriteStringSync(",");
		UARTWriteIntSync((unsigned int)length, NumberFormatter_DecimalUnsigned);
		UARTWriteStringSync(":");

		for(unsigned int i = 0; i < length; i++)
			UARTWriteHexIntSync(CircularBufferSegment(&cdc->_writeBuffer)[i], 1);
		
		UARTWriteStringSync(")");
	#endif //USB_DEBUG
		
		cdc->_writeLength = length;
		LPCUSBWrite(0x83, (unsigned char*)CircularBufferSegment(&cdc->_writeBuffer), length);
		cdc->_writing = 1;	// gets reset in handler when "pkt:83,3" occurs
		return(1);
	}
	return(0);
}

LPCUSBErrorCode			USBCDCACMHandler(void* context, LPCUSBDescriptorHeader const* endpoint, LPCUSBSetup const* setupPacket)
{
	unsigned int endpointAddress = (endpoint->type == LPCUSBDescriptorType_Endpoint)?
										((LPCUSBDescriptorEndpoint*)endpoint)->endpointAddress
										: 0;
	
#ifdef USB_DEBUG
	if(endpointAddress == 0)
	{
		UARTWriteStringSync("\ncls:");
		UARTWriteHexDumpSync((unsigned char*)setupPacket, sizeof(LPCUSBSetup));
	}
	else
	{
		UARTWriteStringSync("\nep");
		UARTWriteIntSync(endpointAddress, NumberFormatter_Hexadecimal);
		UARTWriteStringSync(":");
	}

	UARTWriteStringSync("#");
#endif //USB_DEBUG

	USBCDCDevice* state = (USBCDCDevice*)context;

	if(setupPacket != 0)
	{
		switch(setupPacket->bRequest)
		{
		case USBCDC_Request_SetLineCoding:
			state->_expecting = USBCDC_Request_SetLineCoding;
			return(LPCUSBErrorCode_SendZeroLengthPacket);

		case USBCDC_Request_GetLineCoding:
			LPCUSBWrite(	0x80,
							(unsigned char*)&state->_lineCoding,
							sizeof(USBCDCLineCoding)
						);
			return(LPCUSBErrorCode_OK);

		case USBCDC_Request_SetControlLineState:
			if(setupPacket->wValue & USBCDC_ControlLine_DTEPresent)
			{
			}
			if(setupPacket->wValue & USBCDC_ControlLine_ActivateCarrier)
			{
				if(!state->_connected)
					UARTWriteStringSync("\nonline");
				state->_connected = 1;
			}
			else
			{
				if(state->_connected)
					UARTWriteStringSync("\noffline");
				state->_connected = 0;
			}

			return(LPCUSBErrorCode_SendZeroLengthPacket);
		}
	}
	else if(endpointAddress == 0x00)
	{
		unsigned char buffer[64];
		unsigned int bytesRead = LPCUSBRead(endpointAddress, buffer);
		
		switch(state->_expecting)
		{
		case USBCDC_Request_SetLineCoding:
			if(bytesRead != sizeof(USBCDCLineCoding))
			{
				UARTWriteStringSync(" size mismatch!");
				break;
			}
			memcpy(&state->_lineCoding, buffer, sizeof(USBCDCLineCoding));
			UARTWriteStringSync("\nformat: ");
			UARTWriteIntSync(state->_lineCoding.dwDTERate, NumberFormatter_DecimalUnsigned);
			UARTWriteStringSync(",");
			UARTWriteIntSync(state->_lineCoding.bDataBits, NumberFormatter_DecimalUnsigned);
			UARTWriteStringSync(state->_lineCoding.bParityType? "p" : "n");
			UARTWriteIntSync(state->_lineCoding.bCharFormat + 1, NumberFormatter_DecimalUnsigned);
			state->_expecting = 0;
			break;

		default:
			UARTWriteStringSync("\nunhandled class ep0 rd:");
			UARTWriteIntSync(bytesRead, NumberFormatter_DecimalUnsigned);
			UARTWriteStringSync(">");
			UARTWriteHexDumpSync(buffer, bytesRead);

			break;
		}
	}
	else if(endpointAddress == 0x03)	// endpoint 0x02 OUT
	{
		unsigned char buffer[64];
		unsigned int bytesRead = LPCUSBRead(endpointAddress, buffer);
		
#ifdef USB_DEBUG
		UARTWriteStringSync("\nep2 rd:");
		UARTWriteIntSync(bytesRead, NumberFormatter_DecimalUnsigned);
		UARTWriteStringSync(">");
		UARTWriteHexDumpSync(buffer, bytesRead);
#endif //USB_DEBUG

		CircularBufferWrite(&state->_readBuffer, buffer, bytesRead);

		// if we couldn't accept another [endpoint size] bytes, stall the pipe
		if(CircularBufferFree(&state->_readBuffer) < 64)
		{
		#ifdef USB_DEBUG
			UARTWriteStringSync("\nep2 stl");
		#endif //USB_DEBUG
			LPCUSBSetStall(0x03, 1);
		}
	}
	else if(endpointAddress == 0x83)	// endpoint 0x82 IN
	{
		// anything additional to send in to the host?
		CircularBufferRead(&state->_writeBuffer, 0, state->_writeLength);	// dequeue
		state->_writeLength = 0;
		state->_writing = 0;
		if(CircularBufferUsed(&state->_writeBuffer) > 0)
			USBCDCFlush(state);
	}

	return(LPCUSBErrorCode_OK);
}
