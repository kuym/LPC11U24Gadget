#include <LPC11U00.h>
#include <string.h>

#define MEMORY_SRAM_TOP (0x10000000UL + 8192)

#define STACK_TOP (MEMORY_SRAM_TOP - 4)	//@@todo: get from linker script

#define WEAK_IGNORE __attribute__ ((weak, alias("ignoreInterrupt")))
#define STARTUP	__attribute__ ((section(".startup"), used))

extern unsigned int _stext, _sidata, _sdata, _edata, _sbss, _ebss;

typedef void (*Ctor)(void);
extern Ctor __init_start;
extern Ctor __init_end;

extern int __heap_start__;
extern int __heap_end__;

int main(void);

void ignoreInterrupt(void) INTERRUPT;

void _start(void) __attribute__ ((weak, alias("_gaunt_start"), used));
void _gaunt_start(void);
void HardFault_Handler(void) INTERRUPT;
void SVCall_Handler(void) WEAK_IGNORE;
void SysTick_Handler(void) WEAK_IGNORE;


void FLEX_INT0_IRQHandler(void) WEAK_IGNORE;
void FLEX_INT1_IRQHandler(void) WEAK_IGNORE;
void FLEX_INT2_IRQHandler(void) WEAK_IGNORE;
void FLEX_INT3_IRQHandler(void) WEAK_IGNORE;
void FLEX_INT4_IRQHandler(void) WEAK_IGNORE;
void FLEX_INT5_IRQHandler(void) WEAK_IGNORE;
void FLEX_INT6_IRQHandler(void) WEAK_IGNORE;
void FLEX_INT7_IRQHandler(void) WEAK_IGNORE;
void GINT0_IRQHandler(void) WEAK_IGNORE;
void GINT1_IRQHandler(void) WEAK_IGNORE;
void SSP1_IRQHandler(void) WEAK_IGNORE;
void I2C_IRQHandler(void) WEAK_IGNORE;
void TIMER16_0_IRQHandler(void) WEAK_IGNORE;
void TIMER16_1_IRQHandler(void) WEAK_IGNORE;
void TIMER32_0_IRQHandler(void) WEAK_IGNORE;
void TIMER32_1_IRQHandler(void) WEAK_IGNORE;
void SSP0_IRQHandler(void) WEAK_IGNORE;
void UART_IRQHandler(void) WEAK_IGNORE;
void USB_IRQHandler(void) WEAK_IGNORE;
void USB_FIQHandler(void) WEAK_IGNORE;
void ADC_IRQHandler(void) WEAK_IGNORE;
void WDT_IRQHandler(void) WEAK_IGNORE;
void BOD_IRQHandler(void) WEAK_IGNORE;
void FMC_IRQHandler(void) WEAK_IGNORE;
void USBWakeup_IRQHandler(void) WEAK_IGNORE;


typedef void (*IRQVector)(void);

//this is named "null" as a debugging enhancement and because nothing should depend on this symbol
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
//
// low-level assert functions provided for hard-fault handling and so on

static void			UARTWriteStringLengthSync(char const* msg, int length)
{
	while(((length == -1) && *msg) || (length-- > 0))
	{
		while(!(*UARTLineStatus & UARTLineStatus_TxHoldingRegisterEmpty));
		*UARTData = *msg++;
	}
}
static void			UARTWriteStringSync(char const* msg)
{
	UARTWriteStringLengthSync(msg, -1);
}

static void			UARTWriteHexSync(unsigned int v)
{
	static char const* hex = "0123456789ABCDEF";

	for(int i = 0; i < 8; i++)
	{
		while(!(*UARTLineStatus & UARTLineStatus_TxHoldingRegisterEmpty));
		*UARTData = hex[v >> 28];
		v <<= 4;
	}
}

////////////////////////////////////////////////////////////////


static signed int volatile gInterruptCount;	// interrupts initially disabled at boot

void STARTUP __attribute__((naked)) _gaunt_start(void)
{
	__asm__ volatile (
	"cpsid		i						\n"		// disable interrupts
	"ldr		r0,		=_stext			\n"
	"ldr		r1,		[r0, #0]		\n"		// $sp = *(void**)(&_stext)	//load SP from address 0. Yes, the hardware does this, but not
	"mov		sp,		r1				\n"							//  if you manually vector to _start during e.g. debugging or soft-reset
	"mov		r2,		#2				\n"
	"msr		CONTROL, r2				\n"		// switch to Process SP
	"mov		sp,		r1				\n"		// set PSP = MSP
	"msr		CONTROL, r0				\n"		// switch back to Main SP
	"ldr		r0,		=0x40048000		\n"		// there are some odd scenarios (noticed during debugging) where the ROM bootloader/seed doesn't
	"str		r2,		[r0, #0]		\n"		//   set this correctly.
	::: "r0", "r1", "r2");

	// copy the .data segment to SRAM
	memcpy(&_sdata, &_sidata, (&_edata - &_sdata) * sizeof(unsigned int));

	// clear the bss segment in SRAM
	memset(&_sbss, 0, (&_ebss - &_sbss) * sizeof(unsigned int));

	// run each constructor
	Ctor* c = &__init_start;
	while(c < &__init_end)
		(*c++)();					// <= the joy of C

	// initialize heap
	*(unsigned int*)(&__heap_start__) = (&__heap_end__ - &__heap_start__);
	
	*IOConfigPIO0_11 = (*IOConfigPIO0_11 & ~IOConfigPIO_FunctionMask) | IOConfigPIO0_11_Function_PIO;
	*IOConfigPIO0_12 = (*IOConfigPIO0_12 & ~IOConfigPIO_FunctionMask) | IOConfigPIO0_12_Function_PIO;
	*IOConfigPIO0_13 = (*IOConfigPIO0_13 & ~IOConfigPIO_FunctionMask) | IOConfigPIO0_13_Function_PIO;
	*IOConfigPIO0_14 = (*IOConfigPIO0_14 & ~IOConfigPIO_FunctionMask) | IOConfigPIO0_14_Function_PIO;
	
	gInterruptCount = 1;
	
	main();
	
	// if the application returns from main(), that's a hardfault
	HardFault_Handler();
}

void			interruptsEnabled(void)
{
	signed int count = --gInterruptCount;
	if(count < 0)
	{
		UARTWriteStringSync("\nfault! intCnt=");
		UARTWriteHexSync(count);
		__asm__ volatile ("bkpt 0x06"::);	// invalid interrupt-free refcount
	}
	else if (count == 0)
		__asm__ volatile ("cpsie i"::);
}

void			interruptsDisabled(void)
{
	__asm__ volatile ("cpsid i"::);
	gInterruptCount++;
}

void STARTUP INTERRUPT __attribute__((naked)) HardFault_Handler(void)
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
	
	register unsigned int volatile faultPC;
	register unsigned int volatile faultLR;
	
	__asm__ volatile (
	"ldr	%0, 	[sp, #24]			\n"
	"ldr	%1, 	[sp, #20]			\n"
	: "=r" (faultPC), "=r" (faultLR)
	:
	: );
	
	UARTWriteStringSync("\nHardFault: pc=");
	UARTWriteHexSync(faultPC);
	UARTWriteStringSync(", lr=");
	UARTWriteHexSync(faultLR);
	UARTWriteStringSync("\nhalt.");
	
	while(1);

	/*__asm__ volatile (
	"bx		lr							\n"
	);*/
}

/*
void STARTUP INTERRUPT _SVCall(void)
{
}
*/

void STARTUP Sleep(void)
{
	//@@enter PMU state
	__asm__ volatile ("wfi"::);	//chip is sleeping, waiting for the next event.
	//@@exit PMU state
}

void STARTUP Reset(void)
{
	*((unsigned int volatile*)(0xE000ED0C)) = 0x05FA0004;	//invoke a hard reset
}

void STARTUP INTERRUPT ignoreInterrupt(void)
{
	unsigned int activeInterrupt = (*InterruptControl & InterruptControl_ActiveVector__Mask);

	if(activeInterrupt > 16)
		activeInterrupt -= 16;
	UARTWriteStringSync("\nUnhandledInt #");
	UARTWriteHexSync(activeInterrupt);
}

extern void* __attribute__ ((weak, alias("__dso_handle_nostdlib"))) __dso_handle;
void* __dso_handle_nostdlib = 0;

int __aeabi_atexit(void* object, void (*destroyer)(void*), void* dso_handle)
{
	(void)object;
	(void)destroyer;
	(void)dso_handle;

	//destroyer(object);	// ?

	return(0);	// firmware never exits
}

int __aeabi_sidiv(int numerator, int denominator)
{
	return((*LPC11U00ROMAPI)->divider->sidiv(numerator, denominator));
}

unsigned int __aeabi_uidiv(unsigned int numerator, unsigned int denominator)
{
	return((*LPC11U00ROMAPI)->divider->uidiv(numerator, denominator));
}
