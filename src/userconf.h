

#ifndef _USERCONF_H_
#define _USERCONF_H_

#ifdef __cplusplus
extern "C"
{
#endif

//! MICRO TYPE
#include "stm32f1xx_hal.h"
#include "core_cm3.h"

//! INCLUDES
#include <stdint.h>
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

#include "timeb.h"
#include "lcd_5110.h"

//!GLOBALS
//@brief System Clock in MHz
    static uint_fast32_t SYSCLOCKFREQ;

    
//!define which spi handle is dedicated to lcd
#define LCD_SPI_Handler hspi1

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif
