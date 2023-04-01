#include "userconf.h"

void initializeDWTtimer(void)
{
	//Initiaize DWT Timer  
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	//DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	// Unchanagable means you should not change frequency at execution time unless call this function at each frequency change.
	SYSCLOCKFREQ = (uint32_t) HAL_RCC_GetSysClockFreq() / 999999UL;
}


void delayUS_DWT(uint32_t  us) {
	DWT->CYCCNT = 0;
	uint_fast32_t cycles = DWT->CYCCNT+SYSCLOCKFREQ*us;
	//cycles += 
	do {} while (DWT->CYCCNT < cycles);
}

