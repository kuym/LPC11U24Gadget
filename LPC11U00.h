#ifndef __LPC11U00_SERIES_INCLUDED__
#define __LPC11U00_SERIES_INCLUDED__

	#ifndef __ASSEMBLER__
	
		#ifdef __cplusplus
			namespace LPC11U00{
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

	#define						_BIT(n)		(1 << (n))
	
	#define REGISTER				u32 volatile* const
	#define REGISTER_ADDRESS(x)		((u32 volatile*)(x))

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
			
			IOConfigPIO_OpenDrain				=	(0x01 << 10),
		};

	REGISTER IOConfigPIO0_0		= REGISTER_ADDRESS(0x40044000);
		enum IOConfigPIO0_0
		{
			IOConfigPIO0_0_Function_nReset		=	(0x00 << 0),
			IOConfigPIO0_0_Function_PIO			=	(0x01 << 0),

			IOConfigPIO0_0_Supported			=	(0x3F | IOConfigPIO_OpenDrain)
		};

	REGISTER IOConfigPIO0_1		= REGISTER_ADDRESS(0x40044004);
		enum IOConfigPIO0_1
		{
			IOConfigPIO0_1_Function_PIO			=	(0x00 << 0),
			IOConfigPIO0_1_Function_ClockOutput	=	(0x01 << 0),
			IOConfigPIO0_1_Function_T2_M2		=	(0x02 << 0),
			IOConfigPIO0_1_Function_USB_Toggle	=	(0x03 << 0),

			IOConfigPIO0_1_Supported			=	(0x3F | IOConfigPIO_OpenDrain)
		};
		
	REGISTER IOConfigPIO0_3		= REGISTER_ADDRESS(0x40044010);
		enum IOConfigPIO0_3
		{
			IOConfigPIO0_3_Function_PIO			=	(0x00 << 0),
			IOConfigPIO0_3_Function_VBUS		=	(0x01 << 0),
			IOConfigPIO0_3_Function_IOH_1		=	(0x02 << 0),

			IOConfigPIO0_3_Supported			=	(0x3F | IOConfigPIO_OpenDrain)
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
		
	REGISTER IOConfigPIO0_18	= REGISTER_ADDRESS(0x40044048);
		enum IOConfigPIO0_18
		{
			IOConfigPIO0_18_Function_PIO		=	(0x00 << 0),
			IOConfigPIO0_18_Function_RXD		=	(0x01 << 0),
			IOConfigPIO0_18_Function_T2_M0		=	(0x02 << 0),
			
			IOConfigPIO0_18_Supported			=	(0x3F | IOConfigPIO_OpenDrain)
		};

	REGISTER IOConfigPIO0_19	= REGISTER_ADDRESS(0x4004404C);
		enum IOConfigPIO0_19
		{
			IOConfigPIO0_19_Function_PIO		=	(0x00 << 0),
			IOConfigPIO0_19_Function_TXD		=	(0x01 << 0),
			IOConfigPIO0_19_Function_T2_M1		=	(0x02 << 0),
			
			IOConfigPIO0_19_Supported			=	(0x3F)
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

	REGISTER USBDeviceCommandStatus = REGISTER_ADDRESS(0x40080000);
	
	////////////////////////////////////////////////////////////////
	
	REGISTER GPIO0 = REGISTER_ADDRESS(0x50001000UL);
	REGISTER GPIO0DIR = REGISTER_ADDRESS(0x50002000UL);
	REGISTER GPIO0RAW = REGISTER_ADDRESS(0x50002100UL);

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
	#ifdef __cplusplus
	}	// c++ ns
	#endif //__cplusplus

	extern "C" void HardFault_Handler(void);
	extern "C" void SVCall_Handler(void);
	extern "C" void SysTick_Handler(void);

	extern "C" void FLEX_INT0_IRQHandler(void);
	extern "C" void FLEX_INT1_IRQHandler(void);
	extern "C" void FLEX_INT2_IRQHandler(void);
	extern "C" void FLEX_INT3_IRQHandler(void);
	extern "C" void FLEX_INT4_IRQHandler(void);
	extern "C" void FLEX_INT5_IRQHandler(void);
	extern "C" void FLEX_INT6_IRQHandler(void);
	extern "C" void FLEX_INT7_IRQHandler(void);
	extern "C" void GINT0_IRQHandler(void);
	extern "C" void GINT1_IRQHandler(void);
	extern "C" void SSP1_IRQHandler(void);
	extern "C" void I2C_IRQHandler(void);
	extern "C" void TIMER16_0_IRQHandler(void);
	extern "C" void TIMER16_1_IRQHandler(void);
	extern "C" void TIMER32_0_IRQHandler(void);
	extern "C" void TIMER32_1_IRQHandler(void);
	extern "C" void SSP0_IRQHandler(void);
	extern "C" void UART_IRQHandler(void);
	extern "C" void USB_IRQHandler(void);
	extern "C" void USB_FIQHandler(void);
	extern "C" void ADC_IRQHandler(void);
	extern "C" void WDT_IRQHandler(void);
	extern "C" void BOD_IRQHandler(void);
	extern "C" void FMC_IRQHandler(void);
	extern "C" void USBWakeup_IRQHandler(void);


	typedef void (*IRQVector)(void);
	
	#endif	//!assembler

#endif //__LPC11U00_SERIES_INCLUDED__
