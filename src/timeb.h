/**
 *  \file timeb.h
 *  \brief TIME AND CLOCK FUNCTIONS
 *  \author Behzad Seyfi
 */
#ifndef TIMEB
#define TIMEB

#ifdef __cplusplus
extern "C" {
#endif
     
#include <stdint.h>
#include "stm32f103xb.h"
    
    void initializeDWTtimer(void);
    void delayUS_DWT(uint32_t  us);

#ifdef __cplusplus
}  /* extern "C" */
#endif
#endif // !TIMEB
