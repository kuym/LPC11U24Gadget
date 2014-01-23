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
