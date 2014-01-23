#include <LPC11U00.h>

#define MEMORY_SRAM_TOP (0x10000000UL + 8192)

#define STACK_TOP (MEMORY_SRAM_TOP - 4)	//@@todo: get from linker script

#define WEAK_IGNORE __attribute__ ((weak, alias("ignoreInterrupt")))
#define STARTUP	__attribute__ ((section(".startup"), used))

extern "C" int main(void);

extern "C" void ignoreInterrupt(void) INTERRUPT;

extern "C" void _start(void) __attribute__ ((weak, alias("_gaunt_start"), used));
extern "C" void _gaunt_start(void);
extern "C" void HardFault_Handler(void) INTERRUPT;
extern "C" void SVCall_Handler(void) WEAK_IGNORE;
extern "C" void SysTick_Handler(void) WEAK_IGNORE;


extern "C" void FLEX_INT0_IRQHandler(void) WEAK_IGNORE;
extern "C" void FLEX_INT1_IRQHandler(void) WEAK_IGNORE;
extern "C" void FLEX_INT2_IRQHandler(void) WEAK_IGNORE;
extern "C" void FLEX_INT3_IRQHandler(void) WEAK_IGNORE;
extern "C" void FLEX_INT4_IRQHandler(void) WEAK_IGNORE;
extern "C" void FLEX_INT5_IRQHandler(void) WEAK_IGNORE;
extern "C" void FLEX_INT6_IRQHandler(void) WEAK_IGNORE;
extern "C" void FLEX_INT7_IRQHandler(void) WEAK_IGNORE;
extern "C" void GINT0_IRQHandler(void) WEAK_IGNORE;
extern "C" void GINT1_IRQHandler(void) WEAK_IGNORE;
extern "C" void SSP1_IRQHandler(void) WEAK_IGNORE;
extern "C" void I2C_IRQHandler(void) WEAK_IGNORE;
extern "C" void TIMER16_0_IRQHandler(void) WEAK_IGNORE;
extern "C" void TIMER16_1_IRQHandler(void) WEAK_IGNORE;
extern "C" void TIMER32_0_IRQHandler(void) WEAK_IGNORE;
extern "C" void TIMER32_1_IRQHandler(void) WEAK_IGNORE;
extern "C" void SSP0_IRQHandler(void) WEAK_IGNORE;
extern "C" void UART_IRQHandler(void) WEAK_IGNORE;
extern "C" void USB_IRQHandler(void) WEAK_IGNORE;
extern "C" void USB_FIQHandler(void) WEAK_IGNORE;
extern "C" void ADC_IRQHandler(void) WEAK_IGNORE;
extern "C" void WDT_IRQHandler(void) WEAK_IGNORE;
extern "C" void BOD_IRQHandler(void) WEAK_IGNORE;
extern "C" void FMC_IRQHandler(void) WEAK_IGNORE;
extern "C" void USBWakeup_IRQHandler(void) WEAK_IGNORE;


typedef void (*IRQVector)(void);

//this is named "null" as a debugging enhancement and because nothing should depend on this symbol
extern "C"
IRQVector const null[] __attribute__ ((section(".isr_vector"), used)) =
{
	(IRQVector)STACK_TOP,	// Initial stack pointer value
	&_start,				// Reset vector
	0,						// NMI (not present on this chip)
	&HardFault_Handler,		// Hard fault (bus fault etc.)
	0,						// Reserved
	0,						//  "
	0,						//  "
	(IRQVector)0xFFFFFFFF,	// Checksum, populated by post-build step
	0,						// Reserved
	0,						//  "
	0,						//  "
	&SVCall_Handler,		// Supervisor call
	0,						// Reserved
	0,						//  "
	0,						// Pending supervisor call
	&SysTick_Handler,		// Systick interrupt
	
	
	&FLEX_INT0_IRQHandler,
	&FLEX_INT1_IRQHandler,
	&FLEX_INT2_IRQHandler,
	&FLEX_INT3_IRQHandler,
	&FLEX_INT4_IRQHandler,
	&FLEX_INT5_IRQHandler,
	&FLEX_INT6_IRQHandler,
	&FLEX_INT7_IRQHandler,
	&GINT0_IRQHandler,
	&GINT1_IRQHandler,
	0,
	0,
	0,
	0,
	&SSP1_IRQHandler,
	&I2C_IRQHandler,
	&TIMER16_0_IRQHandler,
	&TIMER16_1_IRQHandler,
	&TIMER32_0_IRQHandler,
	&TIMER32_1_IRQHandler,
	&SSP0_IRQHandler,
	&UART_IRQHandler,
	&USB_IRQHandler,
	&USB_FIQHandler,
	&ADC_IRQHandler,
	&WDT_IRQHandler,
	&BOD_IRQHandler,
	&FMC_IRQHandler,
	0,
	0,
	&USBWakeup_IRQHandler,
	0	
};

////////////////////////////////////////////////////////////////
// ROM divider

struct idivResult
{
	int				quotient;
	int				remainder;
};
struct uidivResult
{
	unsigned int	quotient;
	unsigned int	remainder;
};

struct DividerAPI
{
	int				(*sidiv)(int numerator, int denominator);
	unsigned int	(*uidiv)(unsigned int numerator, unsigned int denominator);
	idivResult		(*sidivmod)(int numerator, int denominator);
	uidivResult 	(*uidivmod)(unsigned int numerator, unsigned int denominator);
};

struct USBAPI;

struct LPCROMAPI
{
	USBAPI*				usb;
	void*				_reserved[3];
	DividerAPI*			divider;
};

LPCROMAPI** const ROMAPI = (LPCROMAPI**)0x1FFF1FF8;

////////////////////////////////////////////////////////////////


extern "C" void STARTUP __attribute__((naked)) _gaunt_start(void)
{
	__asm__ volatile (
	"cpsid		i				\n"		//disable interrupts
	"mov		r0,		#0		\n"
	"ldr		r1,		[r0, #0]\n"		//$sp = *(void**)0	//load SP from address 0. Yes, the hardware does this, but not
	"mov		sp,		r1		\n"							//  if you manually vector to _start during e.g. debugging or soft-reset
	"mov		r2,		#2		\n"
	"msr		CONTROL, r2		\n"		//switch to Process SP
	"mov		sp,		r1		\n"		//set PSP = MSP
	"msr		CONTROL, r0		\n"		//switch back to Main SP
	"ldr		r0,		=0x40048000\n"	//there are some odd scenarios (noticed during debugging) where the ROM bootloader/seed doesn't
	"str		r2,		[r0, #0]\n"		//  set this correctly.
	
	//annoyingly, I couldn't get this to generate correct asm, hence the inline asm version below
	// void (**p)(void) = __init_end;
	// while(p-- != __init_start)
	//   (*p)();
	
	"ldr		r4,		=__init_end		\n"
	"ldr		r5,		=__init_start	\n"
	"._start_next_ctor:					\n"
	"cmp		r4, r5					\n"
	"beq		._start_ctors_done		\n"
	"sub		r4,		#4				\n"
	"ldr		r0, [r4, #0]			\n"
	"blx		r0						\n"		//r4 and r5 are chosen above because ATPCS guarantees they won't be trampled here
	"b			._start_next_ctor		\n"
	"._start_ctors_done:				\n"
	::: "r0", "r1", "r2", "r4", "r5");
	
	main();
	HardFault_Handler();
}

void	writeSync(char const* msg, int length = -1)
{
	while(((length == -1) && *msg) || (length-- > 0))
	{
		while(!(*LPC11U00::UARTLineStatus & LPC11U00::UARTLineStatus_TxHoldingRegisterEmpty));
		*LPC11U00::UARTData = *msg++;
	}
}
void	writeSync(unsigned int v)
{
	static char const* hex = "0123456789ABCDEF";

	for(int i = 0; i < 8; i++)
	{
		while(!(*LPC11U00::UARTLineStatus & LPC11U00::UARTLineStatus_TxHoldingRegisterEmpty));
		*LPC11U00::UARTData = hex[v >> 28];
		v <<= 4;
	}
}

extern "C" void STARTUP INTERRUPT __attribute__((naked)) HardFault_Handler(void)
{
	//according to the ARM ARMv7-M A.R.M sec. B1-22, the NVIC stores the following
	//  values in these places when vectoring to a handler:
	//
	//  $sp[0] = $r0
	//  $sp[1] = $r1
	//  $sp[2] = $r2
	//  $sp[3] = $r3
	//  $sp[4] = $r12
	//  $sp[5] = $lr
	//  $sp[6] = (returnAddress)	<- in "precise" cases, the faulting address, else one instruction past it*
	//  $sp[7] = $cpsr
	
	//*(im)precision due to the nature of the instruction, execution pipeline and bus activity
	
	//moreover, these values are important:
	//  $sp = ($sp_at_fault - 32)
	//  $cpsr & 0xFF = (fault code)
	//  $lr = 0xFFFFFFF9, or in some cases 0xFFFFFFF1 or 0xFFFFFFFD
	
	//a fault occurred at or near $sp[6]
	
	unsigned int volatile register faultPC;
	unsigned int volatile register faultLR;
	
	__asm__ volatile (
	"ldr	%0, 	[sp, #24]			\n"
	"ldr	%1, 	[sp, #20]			\n"
	: "=r" (faultPC), "=r" (faultLR)
	:
	: );
	
	writeSync("\nHardFault: pc=");
	writeSync(faultPC);
	writeSync(", lr=");
	writeSync(faultLR);
	writeSync("\nhalt.");
	
	while(1);

	/*__asm__ volatile (
	"bx		lr							\n"
	);*/
}

/*
extern "C" void STARTUP INTERRUPT _SVCall(void)
{
}
*/

extern "C" void STARTUP Sleep(void)
{
	//@@enter PMU state
	__asm__ volatile ("wfi"::);	//chip is sleeping, waiting for the next event.
	//@@exit PMU state
}

extern "C" void STARTUP Reset(void)
{
	*((unsigned int volatile*)(0xE000ED0C)) = 0x05FA0004;	//invoke a hard reset
}

extern "C" void STARTUP INTERRUPT ignoreInterrupt(void)
{
}

extern void* __attribute__ ((weak, alias("__dso_handle_nostdlib"))) __dso_handle;
void* __dso_handle_nostdlib = 0;

extern "C" int __aeabi_atexit(void* object, void (*destroyer)(void*), void* dso_handle)
{
	return(0);	//firmware never exits
}

extern "C" int __aeabi_sidiv(int numerator, int denominator)
{
	return((*ROMAPI)->divider->sidiv(numerator, denominator));
}

extern "C" unsigned int __aeabi_uidiv(unsigned int numerator, unsigned int denominator)
{
	return((*ROMAPI)->divider->uidiv(numerator, denominator));
}
