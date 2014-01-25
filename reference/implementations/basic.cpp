#include <LPCUSB.h>

// example of a basic vendor-defined device with bulk input and output endpoints.

static USBDescriptorDevice const __attribute__((aligned(4))) kDeviceDescriptor =
{
	.header					= {.size = sizeof(USBDescriptorDevice), .type = DescriptorType_Device},

	.usbSpecification		= 0x0110,
	.deviceClass			= 0x02,
	.subClass				= 0x00,
	.protocol				= 0x00,

	.endpoint0Size			= 64,

	.vendorID				= 0x80b1,
	.productID				= 0x1234,
	.releaseNumber			= 0x0101,

	.manufacturerStrIndex	= 0x01,
	.productStrIndex		= 0x02,
	.serialNumStrIndex		= 0x03,

	.numberOfConfigurations	= 0x01
};

struct __attribute__((packed)) ApplicationConfigurationDescriptor
{
	USBDescriptorConfigurationHeader	header;
	USBDescriptorInterface				interface0;
	USBDescriptorEndpoint				endpoint0_0;
	USBDescriptorEndpoint				endpoint0_1;

	unsigned char						_sentinel;

} const kConfigurationDescriptor __attribute__((aligned(4))) =
{
	.header =
	{
		.header = {.size = sizeof(USBDescriptorConfigurationHeader), .type = DescriptorType_Configuration},

		.totalConfigurationSize = sizeof(ApplicationConfigurationDescriptor) - 1,
		.totalInterfaces		= 1,
		.configurationNumber	= 1,
		.configurationStrIndex	= 0,
		.configAttributes		= (0x80 | 0x40),
		.maxPowerConsumption	= (90 / 2),
	},
	.interface0 =
	{
		.header = {.size = sizeof(USBDescriptorInterface), .type = DescriptorType_Interface},
		.interfaceNumber	= 0,
		.alternateSetting	= 0,
		.totalEndpoints		= 2,
		.interfaceClass		= 0xFF,
		.subClass			= 0xFF,
		.protocol			= 0xFF,
		.interfaceStrIndex	= 0,
	},
	.endpoint0_0 =
	{
		.header = {.size = sizeof(USBDescriptorEndpoint), .type = DescriptorType_Endpoint},
		.endpointAddress	= 0x81,
		.attributes			= (USBEndpointType_Bulk | USBEndpointAttribute_NoSync | USBEndpointUsage_Data),
		.endpointSize		= 64,
		.pollingIntervalMS	= 1,
	},
	.endpoint0_1 =
	{
		.header = {.size = sizeof(USBDescriptorEndpoint), .type = DescriptorType_Endpoint},
		.endpointAddress	= 0x2,
		.attributes			= (USBEndpointType_Bulk | USBEndpointAttribute_NoSync | USBEndpointUsage_Data),
		.endpointSize		= 64,
		.pollingIntervalMS	= 1,
	},
	._sentinel = 0
};
