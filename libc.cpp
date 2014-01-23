#define STARTUP

/*
#define INTERRUPT			__attribute__ ((interrupt ("IRQ")))		//interrupt type is ignored on ARMv6-M

extern "C" int main(void);

extern "C" void _start(void) __attribute__ ((weak, alias("_gaunt_start"), used));
extern "C" void _gaunt_start(void);
extern "C" void HardFault_Handler(void) INTERRUPT;

extern "C" void STARTUP __attribute__((naked)) _gaunt_start(void)
{
	__asm__ volatile (
	"cpsid		i					\n"	//disable interrupts
	"ldr		r0,	=0x00014000		\n"
	"ldr		r1,	[r0, #0]		\n"	//$sp = *startAddress : load SP. Yes, the hardware does this, but not
	"movs		r2,	#2				\n"	//  if you manually vector to _start during e.g. debugging or soft-reset
	"msr		CONTROL, r2			\n"	//switch to Process SP
	"mov		sp,	r1				\n"
	"ldr		r0, =0x40000524 	\n"	//*RAMON |= 0xF
	"ldr		r2, [r0, #0]		\n"
	"mov		r1, #0xF			\n"
	"orr		r2, r1				\n"
	"str		r2, [r0, #0]		\n"	// end *RAMON |= 0xF
	//"isb.w						\n"	//invoke an instruction sync barrier to ensure subsequent stack usage is correct
	//"dsb.w						\n"	//a data sync barrier ensures stack-based execution and IO relative to the stack is ok
	
	//annoyingly, I couldn't get this to generate correct asm, hence the inline asm version below
	// void (**p)(void) = __init_end;
	// while(p-- != __init_start)
	//   (*p)();
	
	"ldr		r4,	=__preinit_array_end		\n"
	"ldr		r5,	=__preinit_array_start   	\n"
	"._start_next_ctor:					\n"
	"cmp		r4, r5					\n"
	"beq		._start_ctors_done		\n"
	"sub		r4,	#4					\n"
	"ldr		r0, [r4, #0]			\n"
	"blx		r0						\n"	//r4 and r5 are chosen above because ATPCS guarantees they won't be trampled here
	"b			._start_next_ctor		\n"
	"._start_ctors_done:				\n"
	::: "r0", "r1", "r2", "r4", "r5");
	
	main();
	HardFault_Handler();
}

/*
extern "C" void STARTUP INTERRUPT __attribute__((naked)) _HardFault(void)
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
	"ldr	%0,	[sp, #24]			\n"
	"ldr	%1,	[sp, #20]			\n"
	: "=r" (faultPC), "=r" (faultLR)
	:
	: );
	
	(void)faultPC;
	(void)faultLR;
	
	__asm__ volatile (
	"bx	lr							\n"
	);
}

extern "C" void STARTUP Sleep(void)
{
	//@@enter PMU state
	__asm__ volatile ("wfi"::); //chip is sleeping, waiting for the next event.
	//@@exit PMU state
}
*/

extern "C" void STARTUP exit(int reason)
{
	*((unsigned int volatile*)(0xE000ED0C)) = 0x05FA0004;   //invoke a hard reset
	while(1);
}

extern "C" void	memset(void* p, int value, unsigned long length)
{
	while((((unsigned int)p) & 0x3) && (length > 0))
	{
		*(unsigned char*)p = (unsigned char)value;
		p = (void*)(((unsigned char*)p) + 1);
		length--;
	}
	
	if(length >= 4)
	{
		unsigned int value4 = (unsigned char)value;
		value4 |= (value4 << 8);
		value4 |= (value4 << 16);
		
		do
		{
			*(unsigned int*)p = value4;
			p = (void*)(((unsigned int*)p) + 1);
			length -= 4;
		}
		while(length >= 4);
	}
	
	while(length--)
	{
		*(unsigned char*)p = (unsigned char)value;
		p = (void*)(((unsigned char*)p) + 1);
	}
}


extern "C" void	memcpy(void* dest, void const* src, unsigned long length)
{
	while(length--)
	{
		*(unsigned char*)dest = *(unsigned char*)src;
		dest = ((unsigned char*)dest) + 1;
		src = ((unsigned char*)dest) + 1;
	}
}

extern "C" unsigned long strlen(char const* str)
{
	unsigned long len = 0;
	while(*str++ != '\0')
		len++;
	return(len);
}
