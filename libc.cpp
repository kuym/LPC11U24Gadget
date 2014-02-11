extern "C" void exit(int reason)
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
		src = ((unsigned char*)src) + 1;
	}
}

extern "C" unsigned long strlen(char const* str)
{
	unsigned long len = 0;
	while(*str++ != '\0')
		len++;
	return(len);
}

extern "C" int __clzsi2(int a)
{
	unsigned int x = (unsigned int)a;
	int t = ((x & 0xFFFF0000) == 0) << 4;  /* if (x is small) t = 16 else 0 */
	x >>= 16 - t;      /* x = [0 - 0xFFFF] */
	unsigned int r = t;       /* r = [0, 16] */
	t = ((x & 0xFF00) == 0) << 3;
	x >>= 8 - t;       /* x = [0 - 0xFF] */
	r += t;            /* r = [0, 8, 16, 24] */
	t = ((x & 0xF0) == 0) << 2;
	x >>= 4 - t;       /* x = [0 - 0xF] */
	r += t;            /* r = [0, 4, 8, 12, 16, 20, 24, 28] */
	t = ((x & 0xC) == 0) << 1;
	x >>= 2 - t;       /* x = [0 - 3] */
	r += t;            /* r = [0 - 30] and is even */
    return(r + ((2 - x) & -((x & 2) == 0)));
}


////////////////////////////////////////////////////////////////
// Memory management


static const unsigned int kHeapFlagMask = 0xFF000000;
static const unsigned int kHeapAllocated = 0x80000000;
extern int __heap_start__;
extern int __heap_end__;

static inline unsigned int*	sp(void)
{
	unsigned int* e;

	__asm__ volatile
	(
		"mov	%0, 	sp		\n"
		: "=r" (e)
		:
		:
	);

	return(e);
}

void*			operator new(unsigned int size)
{
	if(size == 0)
		return((void*)~0);

	//InterruptFreeEnter();
	unsigned int* m = (unsigned int*)(&__heap_start__);
	unsigned int* e = sp() - 8;	//leave a 32-byte margin

	//allocations are (4 + size) bytes, rounded up to the next 4-byte boundary
	unsigned int s = (unsigned int)((size + 7) >> 2);
	
	while(m < e)
	{
		unsigned int bs = *m & ~kHeapFlagMask;
		//look for an unallocated block big enough to fit this allocation
		if(!(*m & kHeapAllocated) && (s < bs))
		{
			if((bs - s) > 1)		//if there's any meaningful remainder,
				m[s] = (bs - s);	//	mark the remainder as free
			else
				s = bs;	//expand allocation
			
			*m = s | kHeapAllocated;	//allocate
			
			for(unsigned int i = 1; i < s; i++)	//zero memory
				m[i] = 0;
			
			//InterruptFreeLeave();
			return(m + 1);
		}
		m += bs;
	}
	__asm volatile("bkpt 7"::);	//@@ Out-of-memory exception
	
	//effectively unreachable code:
	//InterruptFreeLeave();
	return((void*)~0);
}

void			operator delete(void* allocation)
{
	unsigned int* a = (unsigned int*)allocation;
	unsigned int* e = sp() - 8;	//leave a 32-byte margin
	
	//InterruptFreeEnter();
	if((a < (unsigned int*)(&__heap_start__)) || (a >= e) || !(a[-1] & kHeapAllocated))
	{
		//InterruptFreeLeave();
		return;	//@@throw?
	}
	
	a--;	//unwrap
	*a &= ~kHeapAllocated;	//free block

	unsigned int cbs = (*a & ~kHeapFlagMask);	//current block size
	
	for(unsigned int i = 1; i < cbs; i++)	//mark memory as free
		a[i] = 0xfeeefeee;
	
	//defragment heap
	unsigned int* n = a + cbs;				//next block
	if(!(*n & kHeapAllocated))				//if the next block is free,
		cbs += (*n & ~kHeapFlagMask);		//	account for it when freeing
	
	for(unsigned int* p = (unsigned int*)(&__heap_start__); p < a;)
	{
		unsigned int pbs = *p & ~kHeapFlagMask;
		if(((p + pbs) == a) && !(*p & kHeapAllocated))	//predecessor is free
		{
			a = p;
			cbs += pbs;
		}
		p += pbs;
	}
	
	*a = cbs;	//merge free blocks if contiguous
	//InterruptFreeLeave();
}
