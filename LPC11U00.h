#ifndef __LPC11U00_SERIES_INCLUDED__
#define __LPC11U00_SERIES_INCLUDED__

#ifndef __ASSEMBLER__
	
#ifdef __cplusplus
namespace LPC11U00
{
	#endif //c++
		
	typedef unsigned char		u8;
	typedef signed char			s8;
	typedef unsigned short		u16;
	typedef signed short		s16;
	typedef unsigned int		u32;
	typedef signed int			s32;
	typedef unsigned long long	u64;
	typedef signed long long	s64;
	
	#endif //!__ASSEMBLER__
	
	////////////////////////////////////////////////////////////////

	#define							_BIT(n)		(1 << (n))
	
	#define REGISTER				static u32 volatile* const
	#define REGISTER_REF			u32 volatile* const
	#define REGISTER_ADDRESS(x)		((u32 volatile* const)(x))

	#define INTERRUPT				__attribute__ ((interrupt ("IRQ")))

	#ifndef __ASSEMBLER__
	
	// generic features
	enum IOConfig
	{
		IOConfigPIO_FunctionMask			=	(0x03),

		IOConfigPIO_PullDown				=	(0x01 << 3),
		IOConfigPIO_PullUp					=	(0x02 << 3),
		IOConfigPIO_Repeat					=	(0x03 << 3),
		
		IOConfigPIO_Hysteresis				=	(0x01 << 5),
		
		IOConfigPIO_InvertInput				=	(0x01 << 6),
		
		IOConfigPIO_AnalogMask				=	(0x03 << 7),

		IOConfigPIO_DigitalMode				=	(0x01 << 7),
		
		IOConfigPIO_GlitchFilter			=	(0x01 << 8),

		IOConfigPIO_OpenDrain				=	(0x01 << 10),
	};

	REGISTER IOConfigPIO0_0		= REGISTER_ADDRESS(0x40044000);
		enum IOConfigPIO0_0
		{
			IOConfigPIO0_0_Function_nReset		=	(0x00 << 0),
			IOConfigPIO0_0_Function_PIO			=	(0x01 << 0),

			IOConfigPIO0_0_Supported			=	(0x7F | IOConfigPIO_OpenDrain)
		};

	REGISTER IOConfigPIO0_1		= REGISTER_ADDRESS(0x40044004);
		enum IOConfigPIO0_1
		{
			IOConfigPIO0_1_Function_PIO			=	(0x00 << 0),
			IOConfigPIO0_1_Function_ClockOutput	=	(0x01 << 0),
			IOConfigPIO0_1_Function_T2_M2		=	(0x02 << 0),
			IOConfigPIO0_1_Function_USBToggle	=	(0x03 << 0),

			IOConfigPIO0_1_Supported			=	(0x7F | IOConfigPIO_OpenDrain)
		};
		
	REGISTER IOConfigPIO0_2		= REGISTER_ADDRESS(0x40044008);
		enum IOConfigPIO0_2
		{
			IOConfigPIO0_2_Function_PIO			=	(0x00 << 0),
			IOConfigPIO0_2_Function_SSEL0		=	(0x01 << 0),
			IOConfigPIO0_2_Function_T0_C0		=	(0x02 << 0),
			IOConfigPIO0_2_Function_IOH_2		=	(0x03 << 0),

			IOConfigPIO0_2_Supported			=	(0x7F | IOConfigPIO_OpenDrain)
		};
		
	REGISTER IOConfigPIO0_3		= REGISTER_ADDRESS(0x4004400C);
		enum IOConfigPIO0_3
		{
			IOConfigPIO0_3_Function_PIO			=	(0x00 << 0),
			IOConfigPIO0_3_Function_VBUS		=	(0x01 << 0),
			IOConfigPIO0_3_Function_IOH_1		=	(0x02 << 0),

			IOConfigPIO0_3_Supported			=	(0x7F | IOConfigPIO_OpenDrain)
		};

	REGISTER IOConfigPIO0_4		= REGISTER_ADDRESS(0x40044010);
		enum IOConfigPIO0_4
		{
			IOConfigPIO0_4_Function_PIO			=	(0x00 << 0),
			IOConfigPIO0_4_Function_SCL			=	(0x01 << 0),
			IOConfigPIO0_4_Function_IOH_2		=	(0x02 << 0),

			IOConfigPIO0_4_I2CMode_StandardFast	=	(0x00 << 8),
			IOConfigPIO0_4_I2CMode_GPIO			=	(0x01 << 8),
			IOConfigPIO0_4_I2CMode_FastModePlus	=	(0x02 << 8),

			IOConfigPIO0_4_Supported			=	(0x03 | (0x03 << 8))
		};
		
	REGISTER IOConfigPIO0_5		= REGISTER_ADDRESS(0x40044014);
		enum IOConfigPIO0_5
		{
			IOConfigPIO0_5_Function_PIO			=	(0x00 << 0),
			IOConfigPIO0_5_Function_SDA			=	(0x01 << 0),
			IOConfigPIO0_5_Function_IOH_3		=	(0x02 << 0),

			IOConfigPIO0_5_I2CMode_StandardFast	=	(0x00 << 8),
			IOConfigPIO0_5_I2CMode_GPIO			=	(0x01 << 8),
			IOConfigPIO0_5_I2CMode_FastModePlus	=	(0x02 << 8),

			IOConfigPIO0_5_Supported			=	(0x03 | (0x03 << 8))
		};
		
	REGISTER IOConfigPIO0_6		= REGISTER_ADDRESS(0x40044018);
		enum IOConfigPIO0_6
		{
			IOConfigPIO0_6_Function_PIO			=	(0x00 << 0),
			IOConfigPIO0_6_Function_nUSBConnect	=	(0x01 << 0),
			IOConfigPIO0_6_Function_SCK0		=	(0x02 << 0),
			IOConfigPIO0_6_Function_IOH_4		=	(0x03 << 0),

			IOConfigPIO0_6_Supported			=	(0x7F | IOConfigPIO_OpenDrain)
		};

	REGISTER IOConfigPIO0_8		= REGISTER_ADDRESS(0x40044020);
		enum IOConfigPIO0_8
		{
			IOConfigPIO0_8_Function_PIO			=	(0x00 << 0),
			IOConfigPIO0_8_Function_MISO0		=	(0x01 << 0),
			IOConfigPIO0_8_Function_T0_M0		=	(0x02 << 0),
			IOConfigPIO0_8_Function_IOH_6		=	(0x03 << 0),

			IOConfigPIO0_8_Supported			=	(0x7F | IOConfigPIO_OpenDrain)
		};

	REGISTER IOConfigPIO0_9		= REGISTER_ADDRESS(0x40044024);
		enum IOConfigPIO0_9
		{
			IOConfigPIO0_9_Function_PIO			=	(0x00 << 0),
			IOConfigPIO0_9_Function_MOSI0		=	(0x01 << 0),
			IOConfigPIO0_9_Function_T0_M1		=	(0x02 << 0),
			IOConfigPIO0_9_Function_IOH_7		=	(0x03 << 0),

			IOConfigPIO0_9_Supported			=	(0x7F | IOConfigPIO_OpenDrain)
		};

	REGISTER IOConfigPIO0_11	= REGISTER_ADDRESS(0x4004402C);
		enum IOConfigPIO0_11
		{
			IOConfigPIO0_11_Function_JTAGTDI	=	(0x00 << 0),
			IOConfigPIO0_11_Function_PIO		=	(0x01 << 0),
			IOConfigPIO0_11_Function_AD0		=	(0x02 << 0),
			IOConfigPIO0_11_Function_T2_M3		=	(0x03 << 0),

			IOConfigPIO0_11_Supported			=	(0x7F | IOConfigPIO_AnalogMask | IOConfigPIO_GlitchFilter | IOConfigPIO_OpenDrain)
		};

	REGISTER IOConfigPIO0_12	= REGISTER_ADDRESS(0x40044030);
		enum IOConfigPIO0_12
		{
			IOConfigPIO0_12_Function_JTAGTMS	=	(0x00 << 0),
			IOConfigPIO0_12_Function_PIO		=	(0x01 << 0),
			IOConfigPIO0_12_Function_AD1		=	(0x02 << 0),
			IOConfigPIO0_12_Function_T3_C0		=	(0x03 << 0),

			IOConfigPIO0_12_Supported			=	(0x7F | IOConfigPIO_AnalogMask | IOConfigPIO_GlitchFilter | IOConfigPIO_OpenDrain)
		};

	REGISTER IOConfigPIO0_13	= REGISTER_ADDRESS(0x40044034);
		enum IOConfigPIO0_13
		{
			IOConfigPIO0_13_Function_JTAGTDO	=	(0x00 << 0),
			IOConfigPIO0_13_Function_PIO		=	(0x01 << 0),
			IOConfigPIO0_13_Function_AD2		=	(0x02 << 0),
			IOConfigPIO0_13_Function_T3_M0		=	(0x03 << 0),

			IOConfigPIO0_13_Supported			=	(0x7F | IOConfigPIO_AnalogMask | IOConfigPIO_GlitchFilter | IOConfigPIO_OpenDrain)
		};

	REGISTER IOConfigPIO0_14	= REGISTER_ADDRESS(0x40044038);
		enum IOConfigPIO0_14
		{
			IOConfigPIO0_14_Function_JTAGTRST	=	(0x00 << 0),
			IOConfigPIO0_14_Function_PIO		=	(0x01 << 0),
			IOConfigPIO0_14_Function_AD3		=	(0x02 << 0),
			IOConfigPIO0_14_Function_T3_M1		=	(0x03 << 0),

			IOConfigPIO0_14_Supported			=	(0x7F | IOConfigPIO_AnalogMask | IOConfigPIO_GlitchFilter | IOConfigPIO_OpenDrain)
		};

	REGISTER IOConfigPIO0_18	= REGISTER_ADDRESS(0x40044048);
		enum IOConfigPIO0_18
		{
			IOConfigPIO0_18_Function_PIO		=	(0x00 << 0),
			IOConfigPIO0_18_Function_RXD		=	(0x01 << 0),
			IOConfigPIO0_18_Function_T2_M0		=	(0x02 << 0),
			
			IOConfigPIO0_18_Supported			=	(0x7F | IOConfigPIO_OpenDrain)
		};

	REGISTER IOConfigPIO0_19	= REGISTER_ADDRESS(0x4004404C);
		enum IOConfigPIO0_19
		{
			IOConfigPIO0_19_Function_PIO		=	(0x00 << 0),
			IOConfigPIO0_19_Function_TXD		=	(0x01 << 0),
			IOConfigPIO0_19_Function_T2_M1		=	(0x02 << 0),
			
			IOConfigPIO0_19_Supported			=	(0x7F | IOConfigPIO_OpenDrain)
		};
	
	REGISTER IOConfigPIO0_21	= REGISTER_ADDRESS(0x40044054);
		enum IOConfigPIO0_21
		{
			IOConfigPIO0_21_Function_PIO		=	(0x00 << 0),
			IOConfigPIO0_21_Function_T1_M0		=	(0x01 << 0),
			IOConfigPIO0_21_Function_MOSI1		=	(0x02 << 0),

			IOConfigPIO0_21_Supported			=	(0x7F | IOConfigPIO_OpenDrain)
		};

	REGISTER IOConfigPIO0_22	= REGISTER_ADDRESS(0x40044058);
		enum IOConfigPIO0_22
		{
			IOConfigPIO0_22_Function_PIO		=	(0x00 << 0),
			IOConfigPIO0_22_Function_AD6		=	(0x01 << 0),
			IOConfigPIO0_22_Function_T1_M1		=	(0x02 << 0),
			IOConfigPIO0_22_Function_MISO1		=	(0x03 << 0),

			IOConfigPIO0_22_Supported			=	(0x7F | IOConfigPIO_AnalogMask | IOConfigPIO_OpenDrain)
		};


	REGISTER IOConfigPIO1_15	= REGISTER_ADDRESS(0x4004409C);
		enum IOConfigPIO1_15
		{
			IOConfigPIO1_15_Function_PIO		=	(0x00 << 0),
			IOConfigPIO1_15_Function_nDCD		=	(0x01 << 0),
			IOConfigPIO1_15_Function_T0_M2		=	(0x02 << 0),
			IOConfigPIO1_15_Function_SCK1		=	(0x03 << 0),

			IOConfigPIO0_15_Supported			=	(0x7F | IOConfigPIO_OpenDrain)
		};

	REGISTER IOConfigPIO1_19	= REGISTER_ADDRESS(0x400440AC);
		enum IOConfigPIO1_19
		{
			IOConfigPIO1_19_Function_PIO		=	(0x00 << 0),
			IOConfigPIO1_19_Function_nDTR		=	(0x01 << 0),
			IOConfigPIO1_19_Function_SSEL1		=	(0x02 << 0),

			IOConfigPIO1_19_Supported			=	(0x7F | IOConfigPIO_OpenDrain)
		};

	////////////////////////////////////////////////////////////////


	REGISTER	MemoryRemap =			REGISTER_ADDRESS(0x40048000);
		enum MemoryRemap
		{
			MemoryRemap_BootROM 	=	0x00,
			MemoryRemap_SRAM		= 	0x01,
			MemoryRemap_Flash 		=	0x02,
		};
	REGISTER	PeripheralnReset =		REGISTER_ADDRESS(0x40048004);
		enum PeripheralnReset
		{
			PeripheralnReset_SPI0 	=	0x01,
			PeripheralnReset_I2C	= 	0x02,
			PeripheralnReset_SPI1 	=	0x04,
		};
	//the system PLL takes a selectable input frequency and has transform function H(f) = f * ((M + 1) / 2^^(P + 1))
	REGISTER	PLLControl =			REGISTER_ADDRESS(0x40048008);
		enum PLLControl
		{
			PLLControl_MultiplierBitsMask	= 0x1F,	//value M, multiplication factor is (M + 1)
			PLLControl_DividerBitsMask		= 0x60,	//value P, division factor is 2^^(P + 1)
			PLLControl_DividerBitsShift		= 6,
		};
	REGISTER	PLLStatus =				REGISTER_ADDRESS(0x4004800C);
		enum PLLStatus
		{
			PLLStatus_Locked	= 0x01,
		};
	//the USB PLL takes a selectable input frequency and has transform function H(f) = f * ((M + 1) / 2^^(P + 1))
	REGISTER	USBPLLControl =			REGISTER_ADDRESS(0x40048010);
		enum USBPLLControl
		{
			USBPLLControl_MultiplierBitsMask	= 0x1F,	//value M, multiplication factor is (M + 1)
			USBPLLControl_DividerBitsMask		= 0x60,	//value P, division factor is 2^^(P + 1)
			USBPLLControl_DividerBitsShift		= 6,
		};
	REGISTER	USBPLLStatus =			REGISTER_ADDRESS(0x40048014);
		enum USBPLLStatus
		{
			USBPLLStatus_Locked	= 0x01,
		};
	REGISTER	OscillatorControl =		REGISTER_ADDRESS(0x40048020);
		enum PLLSourceSelect
		{
			PLLSourceSelect_Normal		= 0x00,
			PLLSourceSelect_Bypassed	= 0x01,
			PLLSourceSelect_1_to_20MHz	= 0x00,
			PLLSourceSelect_15_to_25MHz	= 0x02,
		};
	REGISTER	WatchdogControl =			REGISTER_ADDRESS(0x40048024);
		enum WatchdogControl
		{
			WatchdogControl_Frequency_0_6MHz	= (0x01 << 5),
			WatchdogControl_Frequency_1_05MHz	= (0x02 << 5),
			WatchdogControl_Frequency_1_4MHz	= (0x03 << 5),
			WatchdogControl_Frequency_1_75MHz	= (0x04 << 5),
			WatchdogControl_Frequency_2_1MHz	= (0x05 << 5),
			WatchdogControl_Frequency_2_4MHz	= (0x06 << 5),
			WatchdogControl_Frequency_2_7MHz	= (0x07 << 5),
			WatchdogControl_Frequency_3_0MHz	= (0x08 << 5),
			WatchdogControl_Frequency_3_25MHz	= (0x09 << 5),
			WatchdogControl_Frequency_3_5MHz	= (0x0A << 5),
			WatchdogControl_Frequency_3_75MHz	= (0x0B << 5),
			WatchdogControl_Frequency_4_0MHz	= (0x0C << 5),
			WatchdogControl_Frequency_4_2MHz	= (0x0D << 5),
			WatchdogControl_Frequency_4_4MHz	= (0x0E << 5),
			WatchdogControl_Frequency_4_6MHz	= (0x0F << 5),
		};
	REGISTER	InternalCrystalTrim =		REGISTER_ADDRESS(0x40048028);
	REGISTER	ResetStatus =				REGISTER_ADDRESS(0x40048030);
		enum ResetStatus
		{
			ResetStatus_PowerOnReset			= 0x01,
			ResetStatus_ExternalReset			= 0x02,
			ResetStatus_WatchdogReset			= 0x04,
			ResetStatus_BrownoutReset			= 0x08,
			ResetStatus_SoftwareReset			= 0x10,
		};
	REGISTER	PLLSource =				REGISTER_ADDRESS(0x40048040);		//aka SYSPLLCLKSEL
		enum PLLSource
		{
			PLLSource_InternalCrystal	= 0x00,
			PLLSource_ExternalClock		= 0x01,
		};
	REGISTER	PLLSourceUpdate =			REGISTER_ADDRESS(0x40048044);	//aka SYSPLLUEN
		enum PLLSourceUpdate
		{
			PLLSourceUpdate_Enable		= 0x01,
		};
	REGISTER	USBPLLSource =				REGISTER_ADDRESS(0x40048048);		//aka USBPLLCLKSEL
		enum USBPLLSource
		{
			USBPLLSource_InternalCrystal	= 0x00,
			USBPLLSource_ExternalClock		= 0x01,
		};
	REGISTER	USBPLLSourceUpdate =			REGISTER_ADDRESS(0x4004804C);	//aka USBPLLCLKUEN
		enum USBPLLSourceUpdate
		{
			USBPLLSourceUpdate_Enable		= 0x01,
		};
	REGISTER	MainClockSource =			REGISTER_ADDRESS(0x40048070);	//aka MAINCLKSEL
		enum MainClockSource
		{
			MainClockSource_InternalCrystal		= 0x00,
			MainClockSource_PLLInput			= 0x01,
			MainClockSource_WDTOscillator		= 0x02,
			MainClockSource_PLLOutput			= 0x03,
		};
	REGISTER	MainClockSourceUpdate =		REGISTER_ADDRESS(0x40048074);	//aka MAINCLKUEN
		enum MainClockSourceUpdate
		{
			MainClockSourceUpdate_Enable	= 0x01,
		};
	REGISTER	MainBusDivider =			REGISTER_ADDRESS(0x40048078);	//aka SYSAHBCLKDIV
	REGISTER	ClockControl =				REGISTER_ADDRESS(0x40048080);	//aka SYSAHBCLKCTRL
		enum ClockControl
		{
			ClockControl_Core				= (1 << 0),
			ClockControl_ROM				= (1 << 1),
			ClockControl_RAM				= (1 << 2),
			ClockControl_FlashRegisters		= (1 << 3),
			ClockControl_FlashStorage		= (1 << 4),
			ClockControl_I2C				= (1 << 5),
			ClockControl_GPIO				= (1 << 6),
			ClockControl_Timer0				= (1 << 7),		//Timer16B0 or Timer16_0 in the docs
			ClockControl_Timer1				= (1 << 8),		//Timer16B1 or Timer16_1
			ClockControl_Timer2				= (1 << 9),		//Timer32B0 or Timer32_0
			ClockControl_Timer3				= (1 << 10),	//Timer32B1 or Timer32_1
			ClockControl_SPI0				= (1 << 11),
			ClockControl_UART				= (1 << 12),
			ClockControl_ADC				= (1 << 13),
			ClockControl_USB				= (1 << 14),
			ClockControl_Watchdog			= (1 << 15),
			ClockControl_IOConfig			= (1 << 16),
			//reserved
			ClockControl_SPI1				= (1 << 18),
			ClockControl_PinInt				= (1 << 20),
			//reserved..
			ClockControl_Group0Int			= (1 << 23),
			ClockControl_Group1Int			= (1 << 24),
			//reserved
			ClockControl_RAM1				= (1 << 26),	// not all parts have this
			ClockControl_USBRAM				= (1 << 27),
		};
	REGISTER	SPI0ClockDivider =			REGISTER_ADDRESS(0x40048094);
	REGISTER	UARTClockDivider =			REGISTER_ADDRESS(0x40048098);
	REGISTER	SPI1ClockDivider =			REGISTER_ADDRESS(0x4004809C);

	/*REGISTER	SystickClockDivider =		REGISTER_ADDRESS(0x400480B0);*/
	
	REGISTER	USBClockSource =			REGISTER_ADDRESS(0x400480C0);	//aka USBCLKSEL
		enum USBClockSource
		{
			USBClockSource_USBPLL_Output	= 0x00,
			USBClockSource_MainClock		= 0x01,
		};
	REGISTER	USBClockSourceUpdate =		REGISTER_ADDRESS(0x400480C4);	//aka USBCLKUEN
		enum USBClockSourceUpdate
		{
			USBClockSourceUpdate_Enable		= 0x01,
		};
	REGISTER	USBClockDivider =			REGISTER_ADDRESS(0x400480C8);	//aka USBCLKDIV

	/*	//not present on LPC11U
	REGISTER	WatchdogSource =			REGISTER_ADDRESS(0x400480D0);
		enum WatchdogSource
		{
			WatchdogSource_InternalCrystal		= 0x00,
			WatchdogSource_MainClock			= 0x01,
			WatchdogSource_WDTOscillator		= 0x02,
		};
	REGISTER	WatchdogSourceUpdate =	REGISTER_ADDRESS(0x400480D4);
		enum WatchdogSourceUpdate
		{
			WatchdogSourceUpdate_Enable	=	0x01,
		};
	REGISTER	WatchdogClockDivider =	REGISTER_ADDRESS(0x400480D8);
	*/
	
	REGISTER	ClockOutputSource =		REGISTER_ADDRESS(0x400480E0);
		enum ClockOutputSource
		{
			ClockOutputSource_InternalCrystal		= 0x00,
			ClockOutputSource_ExternalOscillator	= 0x01,
			ClockOutputSource_WDTOscillator			= 0x02,
			ClockOutputSource_MainClock				= 0x03,
		};
	REGISTER	ClockOutputSourceUpdate =	REGISTER_ADDRESS(0x400480E4);
		enum ClockOutputSourceUpdate
		{
			ClockOutputSourceUpdate_Enable	=	0x01,
		};
	REGISTER	ClockOutputDivider =		REGISTER_ADDRESS(0x400480E8);
	//...
	REGISTER	BrownoutControl =			REGISTER_ADDRESS(0x40048150);
		enum BrownoutControl
		{
			BrownoutControl_Reset_1460mV_1630mV			= 0x00,
			BrownoutControl_Reset_2060mV_2150mV			= 0x01,
			BrownoutControl_Reset_2350mV_2430mV			= 0x02,
			BrownoutControl_Reset_2630mV_2710mV			= 0x03,
			
			BrownoutControl_Interrupt_1650mV_1800mV		= 0x00,
			BrownoutControl_Interrupt_2220mV_2350mV		= 0x04,
			BrownoutControl_Interrupt_2520mV_2660mV		= 0x08,
			BrownoutControl_Interrupt_2800mV_2900mV		= 0x0C,
			
			BrownoutControl_ResetEnabled				= 0x10,
		};
	
	//PowerDownControl and PowerDownWakeupControl set the power state of various features
	//  while running or upon wakeup (respectivley.)  Modify only the enumerated bits of these
	//  registers, as modifying undocumented bits will lead to undefined behaviour and may
	//  damage the chip.
	//  A 0 for a bit means running, while 1 means powered-down.
	REGISTER	PowerDownWakeupControl =	REGISTER_ADDRESS(0x40048234);	//aka PDAWAKECFG
	REGISTER	PowerDownControl =			REGISTER_ADDRESS(0x40048238);	//aka PDRUNCFG
		enum PowerDownControl
		{
			PowerDownControl_InternalCrystalOutput		= (1 << 0),
			PowerDownControl_InternalCrystal			= (1 << 1),
			PowerDownControl_Flash						= (1 << 2),
			PowerDownControl_BrownOutDetector			= (1 << 3),
			PowerDownControl_ADC						= (1 << 4),
			PowerDownControl_SystemOscillator			= (1 << 5),	//for an external crystal
			PowerDownControl_WatchdogOscillator			= (1 << 6),
			PowerDownControl_SystemPLL					= (1 << 7),
			PowerDownControl_USBPLL						= (1 << 8),
			PowerDownControl_USBPins					= (1 << 10),
		};

	//SPI 0
	REGISTER	SPI0Control0 =			REGISTER_ADDRESS(0x40040000);
		enum SPI0Control0
		{
			SPI0Control0_4BitTransfer 	=	0x03,
			SPI0Control0_5BitTransfer 	=	0x04,
			SPI0Control0_6BitTransfer 	=	0x05,
			SPI0Control0_7BitTransfer 	=	0x06,
			SPI0Control0_8BitTransfer 	=	0x07,
			SPI0Control0_9BitTransfer 	=	0x08,
			SPI0Control0_10BitTransfer 	=	0x09,
			SPI0Control0_11BitTransfer 	=	0x0A,
			SPI0Control0_12BitTransfer 	=	0x0B,
			SPI0Control0_13BitTransfer 	=	0x0C,
			SPI0Control0_14BitTransfer 	=	0x0D,
			SPI0Control0_15BitTransfer 	=	0x0E,
			SPI0Control0_16BitTransfer 	=	0x0F,

			SPI0Control0_FrameFormat_SPI		=	(0x00 << 4),
			SPI0Control0_FrameFormat_TI			=	(0x00 << 4),
			SPI0Control0_FrameFormat_Microwire	=	(0x00 << 4),

			SPI0Control0_ClockPolarity_IdleLow	=	(0x00 << 6),
			SPI0Control0_ClockPolarity_IdleHigh	=	(0x01 << 6),
			
			SPI0Control0_ClockPhase_IdleToActive =	(0x00 << 7),	//for ...ClockPolarity_IdleLow, this means reads are on the rising edge.
			SPI0Control0_ClockPhase_ActiveToIdle =	(0x01 << 7),	//for ...ClockPolarity_IdleLow, this means reads are on the falling edge.
			
			SPI0Control0_SPIMode0				=	(0x00 << 6),	//polarity 0, phase 0
			SPI0Control0_SPIMode1				=	(0x02 << 6),	//polarity 0, phase 1
			SPI0Control0_SPIMode2				=	(0x01 << 6),	//polarity 1, phase 0
			SPI0Control0_SPIMode3				=	(0x03 << 6),	//polarity 1, phase 1

			SPI0Control0_ClockRateMinus1 		=	(0 << 8),		//bits 8-15
		};
	REGISTER	SPI0Control1 =			REGISTER_ADDRESS(0x40040004);
		enum SPI0Control1
		{
			SPI0Control1_LoopbackMode 	=	0x01,
			SPI0Control1_Enable		 	=	0x02,
			SPI0Control1_SlaveMode		=	0x04,
			SPI0Control1_OutputDisable	=	0x08,
		};
	REGISTER	SPI0Data =				REGISTER_ADDRESS(0x40040008);
	REGISTER	SPI0Status =			REGISTER_ADDRESS(0x4004000C);
		enum SPI0Status
		{
			SPI0Status_TransmitFIFOEmpty 	=	0x01,
			SPI0Status_TransmitFIFONotFull 	=	0x02,
			SPI0Status_ReceiveFIFONotEmpty	=	0x04,
			SPI0Status_ReceiveFIFOFull 		=	0x08,
			
			SPI0Status_Busy 				=	0x10,
		};
	REGISTER	SPI0ClockPrescaler =	REGISTER_ADDRESS(0x40040010);
	REGISTER	SPI0InterruptEnable =	REGISTER_ADDRESS(0x40040014);
		enum SPI0Interrupt
		{
			SPI0Interrupt_ReceiveOverrun		=	0x01,
			SPI0Interrupt_ReceiveTimeout		=	0x02,
			SPI0Interrupt_ReceiveFIFOHalfFull	=	0x04,
			SPI0Interrupt_TransmitFIFOHalfEmpty =	0x08,
		};
	//(enum values are identical to SPI0Interrupt)
	REGISTER	SPI0RawInterrupt =		REGISTER_ADDRESS(0x40040018);
	REGISTER	SPI0MaskedInterrupt =	REGISTER_ADDRESS(0x4004001C);
	REGISTER	SPI0InterruptClear =	REGISTER_ADDRESS(0x40040020);
		enum SPI0InterruptClear
		{
			SPI0InterruptClear_ReceiveOverrun		=	0x01,
			SPI0InterruptClear_ReceiveTimeout		=	0x02,
		};

	//SPI 1
	REGISTER	SPI1Control0 =			REGISTER_ADDRESS(0x40058000);
		enum SPI1Control0
		{
			SPI1Control0_4BitTransfer 	=	0x03,
			SPI1Control0_5BitTransfer 	=	0x04,
			SPI1Control0_6BitTransfer 	=	0x05,
			SPI1Control0_7BitTransfer 	=	0x06,
			SPI1Control0_8BitTransfer 	=	0x07,
			SPI1Control0_9BitTransfer 	=	0x08,
			SPI1Control0_10BitTransfer 	=	0x09,
			SPI1Control0_11BitTransfer 	=	0x0A,
			SPI1Control0_12BitTransfer 	=	0x0B,
			SPI1Control0_13BitTransfer 	=	0x0C,
			SPI1Control0_14BitTransfer 	=	0x0D,
			SPI1Control0_15BitTransfer 	=	0x0E,
			SPI1Control0_16BitTransfer 	=	0x0F,

			SPI1Control0_FrameFormat_SPI		=	(0x00 << 4),
			SPI1Control0_FrameFormat_TI			=	(0x00 << 4),
			SPI1Control0_FrameFormat_Microwire	=	(0x00 << 4),

			SPI1Control0_ClockPolarity_IdleLow	=	(0x00 << 6),
			SPI1Control0_ClockPolarity_IdleHigh	=	(0x01 << 6),
			
			SPI1Control0_ClockPhase_IdleToActive =	(0x00 << 7),	//for ...ClockPolarity_IdleLow, this means reads are on the rising edge.
			SPI1Control0_ClockPhase_ActiveToIdle =	(0x01 << 7),	//for ...ClockPolarity_IdleLow, this means reads are on the falling edge.
			
			SPI1Control0_SPIMode0				=	(0x00 << 6),	//polarity 0, phase 0
			SPI1Control0_SPIMode1				=	(0x02 << 6),	//polarity 0, phase 1
			SPI1Control0_SPIMode2				=	(0x01 << 6),	//polarity 1, phase 0
			SPI1Control0_SPIMode3				=	(0x03 << 6),	//polarity 1, phase 1

			SPI1Control0_ClockRateMinus1 		=	(0 << 8),		//bits 8-15
		};
	REGISTER	SPI1Control1 =			REGISTER_ADDRESS(0x40058004);
		enum SPI1Control1
		{
			SPI1Control1_LoopbackMode 	=	0x01,
			SPI1Control1_Enable		 	=	0x02,
			SPI1Control1_SlaveMode		=	0x04,
			SPI1Control1_OutputDisable	=	0x08,
		};
	REGISTER	SPI1Data =				REGISTER_ADDRESS(0x40058008);
	REGISTER	SPI1Status =			REGISTER_ADDRESS(0x4005800C);
		enum SPI1Status
		{
			SPI1Status_TransmitFIFOEmpty 	=	0x01,
			SPI1Status_TransmitFIFONotFull 	=	0x02,
			SPI1Status_ReceiveFIFONotEmpty	=	0x04,
			SPI1Status_ReceiveFIFOFull 		=	0x08,
			
			SPI1Status_Busy 				=	0x10,
		};
	REGISTER	SPI1ClockPrescaler =	REGISTER_ADDRESS(0x40058010);
	REGISTER	SPI1InterruptEnable =	REGISTER_ADDRESS(0x40058014);
		enum SPI1Interrupt
		{
			SPI1Interrupt_ReceiveOverrun		=	0x01,
			SPI1Interrupt_ReceiveTimeout		=	0x02,
			SPI1Interrupt_ReceiveFIFOHalfFull	=	0x04,
			SPI1Interrupt_TransmitFIFOHalfEmpty =	0x08,
		};
	//(enum values are identical to SPI0Interrupt)
	REGISTER	SPI1RawInterrupt =		REGISTER_ADDRESS(0x40058018);
	REGISTER	SPI1MaskedInterrupt =	REGISTER_ADDRESS(0x4005801C);
	REGISTER	SPI1InterruptClear =	REGISTER_ADDRESS(0x40058020);
		enum SPI11InterruptClear
		{
			SPI1InterruptClear_ReceiveOverrun		=	0x01,
			SPI1InterruptClear_ReceiveTimeout		=	0x02,
		};
	

	//UART
	REGISTER	UARTData =					REGISTER_ADDRESS(0x40008000);	//accessible only when UARTLineControl_DivisorLatch = 0
	REGISTER	UARTInterrupts =			REGISTER_ADDRESS(0x40008004);	// "
		enum UARTInterrupts
		{
			UARTInterrupts_ReceivedData		=	(0x01),
			UARTInterrupts_TxBufferEmpty	=	(0x02),
			UARTInterrupts_RxLineStatus		=	(0x04),
			
			UARTInterrupts_AutoBaudComplete	=	(0x100),
			UARTInterrupts_AutoBaudTimeout	=	(0x200),
		};
	REGISTER	UARTDivisorLow =			REGISTER_ADDRESS(0x40008000);	//when UARTLineControl_DivisorLatch = 1
	REGISTER	UARTDivisorHigh =			REGISTER_ADDRESS(0x40008004);	// "
	REGISTER	UARTInterruptID =			REGISTER_ADDRESS(0x40008008);	//read-only
		enum UARTInterruptID
		{
			UARTInterruptID_InterruptPending	=	(0x01),
			
			UARTInterruptID_ReasonMask			=	(0x7 << 1),
			
			UARTInterruptID_ReceiveException	=	(3 << 1),	//signaled on receive overrun, parity, framing or break errors
			UARTInterruptID_DataAvailable		=	(2 << 1),
			UARTInterruptID_ReceiveTimeout		=	(6 << 1),	//not as bad as it sounds: not enough chars received to trip interrupt
			UARTInterruptID_TxBufferEmpty		=	(1 << 1),
			UARTInterruptID_Modem				=	(0 << 1),
			
			UARTInterruptID_FIFOEnabled			=	(1 << 6),
			
			UARTInterruptID_AutoBaudComplete	=	(0x100),
			UARTInterruptID_AutoBaudTimeout		=	(0x200),
		};
	REGISTER	UARTFIFOControl =			REGISTER_ADDRESS(0x40008008);	//write-only
		enum UARTFIFOControl
		{
			UARTFIFOControl_Enable					=	(0x01),		
			UARTFIFOControl_RxReset					=	(0x02),		//self-clearing bit
			UARTFIFOControl_TxReset					=	(0x04),		//self-clearing bit
			
			UARTFIFOControl_RxInterrupt1Char		=	(0x00 << 6),
			UARTFIFOControl_RxInterrupt4Chars		=	(0x01 << 6),
			UARTFIFOControl_RxInterrupt8Chars		=	(0x02 << 6),
			UARTFIFOControl_RxInterrupt14Chars		=	(0x03 << 6),
		};
	REGISTER	UARTLineControl =			REGISTER_ADDRESS(0x4000800C);
		enum UARTLineControl
		{
			UARTLineControl_5BitChars				=	(0x00),
			UARTLineControl_6BitChars				=	(0x01),
			UARTLineControl_7BitChars				=	(0x02),
			UARTLineControl_8BitChars				=	(0x03),
			
			UARTLineControl_UseStopBit				=	(0x04),
			UARTLineControl_UseParity				=	(0x08),
			
			UARTLineControl_UseOddParity			=	(0x00 << 4),
			UARTLineControl_UseEvenParity			=	(0x01 << 4),
			UARTLineControl_Constant1Parity			=	(0x02 << 4),
			UARTLineControl_Constant0Parity			=	(0x03 << 4),
			
			UARTLineControl_BreakControlEnable		=	(0x40),
			UARTLineControl_DivisorLatch			=	(0x80),
		};
	REGISTER	UARTModemControl =			REGISTER_ADDRESS(0x40008010);
		enum UARTModemControl
		{
			UARTModemControl_DTRPinState			=	(0x01),
			UARTModemControl_RTSPinState			=	(0x02),
			UARTModemControl_LoopbackMode			=	(0x04),
			UARTModemControl_RTSEnable				=	(0x40),
			UARTModemControl_CTSEnable				=	(0x80),
		};
	REGISTER	UARTLineStatus =			REGISTER_ADDRESS(0x40008014);
		enum UARTLineStatus
		{
			UARTLineStatus_ReceiverDataReady		=	(0x01),
			UARTLineStatus_OverrunError				=	(0x02),
			UARTLineStatus_ParityError				=	(0x04),
			UARTLineStatus_FramingError				=	(0x08),
			UARTLineStatus_BreakInterrupt			=	(0x10),
			UARTLineStatus_TxHoldingRegisterEmpty	=	(0x20),
			UARTLineStatus_TransmitterEmpty			=	(0x40),
			UARTLineStatus_RxFIFOEmpty				=	(0x80),
		};
	REGISTER	UARTModemStatus =			REGISTER_ADDRESS(0x40008018);	//read-only
	REGISTER	UARTScratch =				REGISTER_ADDRESS(0x4000801C);
	REGISTER	UARTAutoBaudControl =		REGISTER_ADDRESS(0x40008020);
	REGISTER	UARTFractionalDivider =		REGISTER_ADDRESS(0x40008028);
	REGISTER	UARTTransmitEnabled =		REGISTER_ADDRESS(0x40008030);

	////////////////////////////////////////////////////////////////
	
	REGISTER	I2CControlSet =				REGISTER_ADDRESS(0x40000000);
	REGISTER	I2CControlClear =			REGISTER_ADDRESS(0x40000018);
		enum I2CControlSet
		{
			I2CControlSet_Ack				=	(0x04),
			I2CControlSet_Interrupt			=	(0x08),
			I2CControlSet_StopCondition		=	(0x10),		// do not clear I2CControlSet_StopCondition
			I2CControlSet_StartCondition	=	(0x20),
			I2CControlSet_EnableI2C			=	(0x40),
		};
	REGISTER	I2CStatus =					REGISTER_ADDRESS(0x40000004);
	REGISTER	I2CData =					REGISTER_ADDRESS(0x40000008);
	REGISTER	I2CBuffer =					REGISTER_ADDRESS(0x4000002C);

	REGISTER	I2CMonitorMode =			REGISTER_ADDRESS(0x4000001C);
		enum I2CMonitorMode
		{
			I2CMonitorMode_Enabled			=	(0x01),
			I2CMonitorMode_SCLControl		=	(0x02),
			I2CMonitorMode_MatchAll			=	(0x04),
		};
	
	REGISTER	I2CClockHighTime =			REGISTER_ADDRESS(0x40000010);
	REGISTER	I2CClockLowTime =			REGISTER_ADDRESS(0x40000014);
	
	REGISTER	I2CAddress0 =				REGISTER_ADDRESS(0x4000000C);
	REGISTER	I2CAddress1 =				REGISTER_ADDRESS(0x40000020);
	REGISTER	I2CAddress2 =				REGISTER_ADDRESS(0x40000024);
	REGISTER	I2CAddress3 =				REGISTER_ADDRESS(0x40000028);

	REGISTER	I2CAddressMask0 =			REGISTER_ADDRESS(0x40000030);
	REGISTER	I2CAddressMask1 =			REGISTER_ADDRESS(0x40000034);
	REGISTER	I2CAddressMask2 =			REGISTER_ADDRESS(0x40000038);
	REGISTER	I2CAddressMask3 =			REGISTER_ADDRESS(0x4000003C);
	
	////////////////////////////////////////////////////////////////

	REGISTER USBDeviceCommandStatus = REGISTER_ADDRESS(0x40080000);
	
	////////////////////////////////////////////////////////////////
	
	REGISTER GPIO0 =	REGISTER_ADDRESS(0x50001000UL);	// access with GPIO0[pinNum]
	REGISTER GPIO0Dir =	REGISTER_ADDRESS(0x50002000UL);
	REGISTER GPIO0Raw =	REGISTER_ADDRESS(0x50002100UL);
	
	REGISTER GPIO1 =	REGISTER_ADDRESS(0x50001080UL);	// access with GPIO1[pinNum]
	REGISTER GPIO1Dir =	REGISTER_ADDRESS(0x50002004UL);
	REGISTER GPIO1Raw =	REGISTER_ADDRESS(0x50002104UL);

	////////////////////////////////////////////////////////////////

	//Write 1 to set interrupts on InterruptSet, write 1 to clear interrupts on InterruptClear
	REGISTER	InterruptEnableSet0 =		REGISTER_ADDRESS(0xE000E100);
		enum Interrupt0
		{
			Interrupt0_PinInt0 =		(1 << 0),
			Interrupt0_PinInt1 =		(1 << 1),
			Interrupt0_PinInt2 =		(1 << 2),
			Interrupt0_PinInt3 =		(1 << 3),
			Interrupt0_PinInt4 =		(1 << 4),
			Interrupt0_PinInt5 =		(1 << 5),
			Interrupt0_PinInt6 =		(1 << 6),
			Interrupt0_PinInt7 =		(1 << 7),
			Interrupt0_GPIOGroup0 =		(1 << 8),
			Interrupt0_GPIOGroup1 =		(1 << 9),
			// 10-13 reserved
			Interrupt0_SPI1 =			(1 << 14),
			Interrupt0_I2C =			(1 << 15),
			Interrupt0_Timer0 =			(1 << 16),
			Interrupt0_Timer1 =			(1 << 17),
			Interrupt0_Timer2 =			(1 << 18),
			Interrupt0_Timer3 =			(1 << 19),
			Interrupt0_SPI0 =			(1 << 20),
			Interrupt0_UART =			(1 << 21),
			Interrupt0_USB =			(1 << 22),
			Interrupt0_USBFast =		(1 << 23),
			Interrupt0_ADC =			(1 << 24),
			Interrupt0_Watchdog =		(1 << 25),
			Interrupt0_BrownOut =		(1 << 26),
			Interrupt0_Flash =			(1 << 27),
			// 28-29 reserved
			Interrupt0_SBWakeup =		(1 << 30),
		};
	REGISTER	InterruptEnableClear0 =		REGISTER_ADDRESS(0xE000E180);
	REGISTER	InterruptSetPending0 =		REGISTER_ADDRESS(0xE000E200);
	REGISTER	InterruptClearPending0 =	REGISTER_ADDRESS(0xE000E280);
	REGISTER	InterruptActive0 =			REGISTER_ADDRESS(0xE000E300);

	REGISTER	InterruptControl =			REGISTER_ADDRESS(0xE000ED04);
		enum InterruptControl
		{
			InterruptControl_NMIPendingSet =		(1 << 31),	// set NMI interrupt pending flag
			InterruptControl_PendSVSet =			(1 << 28),	// set PendSV interrupt pending flag
			InterruptControl_PendSVClear =			(1 << 27),	// clear PendSV interrupt pending flag
			InterruptControl_PendSystickSet =		(1 << 26),	// set SysTick interrupt pending flag
			InterruptControl_PendSystickClear =		(1 << 25),	// clear SysTick interrupt pending flag
			InterruptControl_ISRPending =			(1 << 22),	// read-only; is an ISR pending?

			//for these two, 0 means normal mode; subtract 16 to get the IRQ number (ala InterruptEnableSet0)
			InterruptControl_PendingVector__Mask =	(0x3F << 12),	//the vector number of the next pending interrupt
			InterruptControl_ActiveVector__Mask =	(0x3F),			//the vector number of the current state of execution
		};

	////////////////////////////////////////////////////////////////

	#ifdef __cplusplus
	
	extern "C"
	{

		#endif //__cplusplus
	
		void HardFault_Handler(void);
		void SVCall_Handler(void);
		void SysTick_Handler(void);

		void FLEX_INT0_IRQHandler(void);
		void FLEX_INT1_IRQHandler(void);
		void FLEX_INT2_IRQHandler(void);
		void FLEX_INT3_IRQHandler(void);
		void FLEX_INT4_IRQHandler(void);
		void FLEX_INT5_IRQHandler(void);
		void FLEX_INT6_IRQHandler(void);
		void FLEX_INT7_IRQHandler(void);
		void GINT0_IRQHandler(void);
		void GINT1_IRQHandler(void);
		void SSP1_IRQHandler(void);
		void I2C_IRQHandler(void);
		void TIMER16_0_IRQHandler(void);
		void TIMER16_1_IRQHandler(void);
		void TIMER32_0_IRQHandler(void);
		void TIMER32_1_IRQHandler(void);
		void SSP0_IRQHandler(void);
		void UART_IRQHandler(void);
		void USB_IRQHandler(void);
		void USB_FIQHandler(void);
		void ADC_IRQHandler(void);
		void WDT_IRQHandler(void);
		void BOD_IRQHandler(void);
		void FMC_IRQHandler(void);
		void USBWakeup_IRQHandler(void);

	
		typedef void (*IRQVector)(void);

		#ifdef __cplusplus

	}	//extern "C"

	namespace ROMUSB
	{
		#endif //__cplusplus

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

		typedef struct __attribute__((packed)) USBSetup
		{
			unsigned char	bmRequestType;
			unsigned char	bRequest;
			unsigned short	wValue;
			unsigned short	wIndex;
			unsigned short	wLength;
		
		} USBSetup;

		typedef struct __attribute__((packed)) ACMLineCoding
		{
			unsigned int	dwDTERate;
			unsigned char	bCharFormat;
			unsigned char	bParityType;
			unsigned char	bDataBits;
		
		} ACMLineCoding;

		typedef struct __attribute__((packed)) USBDescriptorHeader
		{
			unsigned char	size;	// Size of the descriptor, in bytes.
			unsigned char	type;	// Type of the descriptor, either one of USBDescriptorType_* or a value
		
		} USBDescriptorHeader;

		typedef struct __attribute__((packed)) USBDescriptorDevice
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
		
		} USBDescriptorDevice;

		typedef struct __attribute__((packed)) USBDescriptorConfigurationHeader
		{
			USBDescriptorHeader	header;					// Descriptor header, including type and size.

			unsigned short		totalConfigurationSize;	// Size of the configuration descriptor header
			unsigned char		totalInterfaces;		// Total number of interfaces in the configuration.

			unsigned char		configurationNumber;	// Configuration index of the current configuration.
			unsigned char		configurationStrIndex;	// Index of a string descriptor describing the configuration.

			unsigned char		configAttributes;		// Configuration attributes, comprised of a mask of zero or
			
			unsigned char		maxPowerConsumption;	// Maximum power consumption of the device while in the
		
		} USBDescriptorConfigurationHeader;
		
		typedef struct __attribute__((packed)) USBDescriptorInterface
		{
			USBDescriptorHeader	header;				// Descriptor header, including type and size.

			unsigned char		interfaceNumber;	// Index of the interface in the current configuration.
			unsigned char		alternateSetting;	// Alternate setting for the interface number. The same
			unsigned char		totalEndpoints;		// Total number of endpoints in the interface.

			unsigned char		interfaceClass;		// Interface class ID.
			unsigned char		subClass;			// Interface subclass ID.
			unsigned char		protocol;			// Interface protocol ID.

			unsigned char		interfaceStrIndex;	// Index of the string descriptor describing the interface.
		
		} USBDescriptorInterface;

		typedef struct __attribute__((packed)) USBDescriptorEndpoint
		{
			USBDescriptorHeader	header;			// Descriptor header, including type and size.

			unsigned char	endpointAddress;	// Logical address of the endpoint within the device for the current
			unsigned char	attributes;			// Endpoint attributes, comprised of a mask of the endpoint type (EP_TYPE_*)
			unsigned short	endpointSize;		// Size of the endpoint bank, in bytes. This indicates the maximum packet
			unsigned char	pollingIntervalMS;	// Polling interval in milliseconds for the endpoint if it is an INTERRUPT
		
		} USBDescriptorEndpoint;

		////////////////////////////////////////////////////////////////
		
		typedef struct __attribute__((packed)) CDCDescriptorFunctionalHeader
		{
			USBDescriptorHeader	header;				// Descriptor header, including type and size.
		
			unsigned char		subType;			// Sub type value used to distinguish between CDC class-specific descriptors
			unsigned short		cdcSpecification;	// Version number of the CDC specification implemented by the device
		
		} CDCDescriptorFunctionalHeader;

		typedef struct __attribute__((packed)) CDCDescriptorFunctionalACM
		{
			USBDescriptorHeader	header;				// Descriptor header, including type and size.
		
			unsigned char		subType;			// Sub type value used to distinguish between CDC class-specific descriptors
			unsigned char		capabilities;		// Capabilities of the ACM interface, given as a bit mask. For most devices, 0x06
		
		} CDCDescriptorFunctionalACM;
		
		typedef struct __attribute__((packed)) CDCDescriptorFunctionalUnion
		{
			USBDescriptorHeader	header;					// Descriptor header, including type and size.
		
			unsigned char		subType;				// Sub type value used to distinguish between CDC class-specific descriptors
			unsigned char		masterInterfaceNumber;	// Interface number of the CDC Control interface
			unsigned char		slaveInterfaceNumber;	// Interface number of the CDC Data interface
		
		} CDCDescriptorFunctionalUnion;
		
		////////////////////////////////////////////////////////////////
		
		typedef struct CDCInit
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

		} CDCInit;

		typedef struct CDCAPI
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
		
		} CDCAPI;

		typedef struct CoreAPI
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

		} CoreAPI;
		
		typedef struct CoreDescriptors
		{
			void const*		deviceDescriptor;
			void const*		stringDescriptor;
			void const*		fullSpeedDescriptor;
			void const*		highSpeedDescriptor;
			void const*		deviceQualifier;

		} CoreDescriptors;
		
		typedef struct HardwareInit
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

		} HardwareInit;

		typedef struct HardwareAPI
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

		} HardwareAPI;

		typedef struct USBAPI
		{
			HardwareAPI*		hardware;
			CoreAPI*			core;
			void*				unimplementedMSC;
			void*				unimplementedDFU;
			void*				unimplementedHID;
			CDCAPI*				cdc;
			void*				__reserved0;
			unsigned int		version;

		} USBAPI;

		#ifdef __cplusplus

	}	// c++ LPC11U00::USB namespace

	namespace ROMDivider
	{
		#endif //__cplusplus

		typedef struct idivResult
		{
			int				quotient;
			int				remainder;
		} idivResult;

		typedef struct uidivResult
		{
			unsigned int	quotient;
			unsigned int	remainder;
		} uidivResult;

		typedef struct DividerAPI
		{
			int				(*sidiv)(int numerator, int denominator);
			unsigned int	(*uidiv)(unsigned int numerator, unsigned int denominator);
			idivResult		(*sidivmod)(int numerator, int denominator);
			uidivResult 	(*uidivmod)(unsigned int numerator, unsigned int denominator);
		} DividerAPI;

		#ifdef __cplusplus

	}	// c++ LPC11U00::Divider namespace

	namespace ROMAPI
	{
		using namespace ROMUSB;
		using namespace ROMDivider;

		#endif //__cplusplus

		typedef struct LPCAPI
		{
			USBAPI*				usb;
			void*				_reserved[3];
			DividerAPI*			divider;
		} LPCAPI;

		LPCAPI** const API = (LPCAPI**)0x1FFF1FF8;


		typedef void (*LPCFlashProgrammerAPI)(unsigned int* commandArgs, unsigned int* resultArgs);

		enum FlashCommand
		{
			FlashCommand_PrepareSectors			= 50,
			FlashCommand_CopyRAMToFlash			= 51,
			FlashCommand_EraseSectors			= 52,
			FlashCommand_BlankCheckSectors		= 53,
			FlashCommand_ReadPartID				= 54,
			FlashCommand_ReadBootCodeVersion	= 55,
			FlashCommand_Compare				= 56,
			FlashCommand_ReinvokeISP			= 57,
			FlashCommand_ReadUID				= 58,
			FlashCommand_ErasePage				= 59,
			FlashCommand_EEPROMWrite			= 61,
			FlashCommand_EEPROMRead				= 62,
		};

		enum FlashResponse
		{
			FlashResponse_Success								= 0,
			FlashResponse_InvalidCommand						= 1,
			FlashResponse_SrcAddrError							= 2,
			FlashResponse_DstAddrError							= 3,
			FlashResponse_SrcAddrNotMapped						= 4,
			FlashResponse_DstAddrNotMapped						= 5,
			FlashResponse_CountError							= 6,
			FlashResponse_InvalidSector							= 7,
			FlashResponse_SectorNotBlank						= 8,
			FlashResponse_SectorNotPreparedForWriteOperation	= 9,
			FlashResponse_CompareError							= 10,
			FlashResponse_Busy									= 11,
			FlashResponse_ParamError							= 12,
			FlashResponse_AddrError								= 13,
			FlashResponse_AddrNotMapped							= 14,
			FlashResponse_CmdLocked								= 15,
			FlashResponse_InvalidCode							= 16,
			FlashResponse_InvalidBaudRate						= 17,
			FlashResponse_InvalidStopBit						= 18,
			FlashResponse_CodeReadProtectionEnabled				= 19,
		};

		LPCFlashProgrammerAPI const FlashProgrammer = (LPCFlashProgrammerAPI)0x1FFF1FF1;
		
		#ifdef __cplusplus

	}	// c++ LPC11U00::API namespace

}	// c++ LPC11U00 namespace

extern "C"
{

#endif //__cplusplus

	void			interruptsEnabled(void);
	void			interruptsDisabled(void);

#ifdef __cplusplus

}	// c++ extern "C"

#endif //__cplusplus

#endif	//!assembler

#endif //__LPC11U00_SERIES_INCLUDED__
