#ifndef MGENERAL_H
#define MGENERAL_H

//-----------------------------------------------------------------------------
// Useful pre-compile constants
//-----------------------------------------------------------------------------

#define ERROR	0
#define SUCCESS	1
#ifndef NULL
#define NULL ((void *)0)
#endif

//-----------------------------------------------------------------------------
// Bit manipulation and validation on registers:
//-----------------------------------------------------------------------------

//#define set(reg,bit)		(reg |=  (bit))
//#define clear(reg,bit)		(reg &= ~(bit))
//#define toggle(reg,bit)		(reg ^=  (bit))
//#define check(reg,bit)		(reg & (bit))
//#define clearset(reg, clearmask, setmask)  (reg = reg & (~(clearmask)) | (setmask))


// -----------------------------------------------------------------------------
// Wait for a specified number of clock cycles: 
// -----------------------------------------------------------------------------
// val can between 1 to (2^32-1) 
// and the number clock cycles wasted will be 4 * val
#define mWait(val)   do{register uint32_t i;for(i=0;i<val;++i)\
		__asm__ __volatile__ ("nop\n\t":::"memory");} while(0);
#define mWaitus(val) do{register uint32_t i;for(i=0;i<(val*18);++i)\
		__asm__ __volatile__ ("nop\n\t":::"memory");} while(0);
#define mWaitms(val) do{register uint32_t i;for(i=0;i<(val*18000);++i)\
		__asm__ __volatile__ ("nop\n\t":::"memory");} while(0);						

#endif // MGENERAL_H