// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : bsp.c
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#include "bsp.h"

#include <reg_phys.h>
#include <debug.h>

#include <gpio.h>
#include <clock.h>
#include <gic.h>
#include <uart.h>
#include <timer.h>
//#include <wdt.h>
//#include <i2c.h>
//#include <gpsb.h>
//#include <adc.h>
//#include <mbox.h>
#include <pmu.h>
#include <pmio.h>
//#include <dse.h>

#ifdef FMU_DRIVER
    //#include <fmu.h>
#endif
//#include <rtc.h>

/*
***************************************************************************************************
                                         STATIC VARIABLES
***************************************************************************************************
*/

/*
***************************************************************************************************
                                         FUNCTION PROTOTYPES
***************************************************************************************************
*/


/*
***************************************************************************************************
*                                         FUNCTIONS
***************************************************************************************************
*/

void BSP_PreInit
(
    void
)
{
    CLOCK_Init();
    GIC_Init();
    PMU_Init();
    BSP_EnableSYSPower();
}

void BSP_Init
(
    void
)
{

    (void)UART_Init();
    (void)TIMER_Init();

#if 0
    //(void)WDT_Init();
    (void)DSE_Init(DES_SEL_ALL);

//temp    I2C_Init();
    GPSB_Init();
    (void) ADC_Init(ADC_MODE_NORMAL,ADC_MODULE_0);
    (void) ADC_Init(ADC_MODE_NORMAL,ADC_MODULE_1);

#ifdef FMU_DRIVER
    (void)FMU_Init();
#endif
    (void)RTC_Init();

    MBOX_Init();

    PMIO_Init();

#endif

    mcu_printf("\nInitialize System done\n");
    mcu_printf("Welcome to Telechips MCU BSP\n");
}

/* R0 : ARM Exception ID, R1 : Dummy , R2 : Link Register(Return PC)*/
void BSP_UndefAbortExceptionHook
(
    uint32_t                              uiExceptId,
    uint32_t                              uiDummy,
    uint32_t                              uiLinkReg
)
{
    (void)uiExceptId;
    (void)uiDummy;
    (void)uiLinkReg;

}

/*
Warning !!!  After calliing this function. it will never return from exception except H/W reseting.
This function is designed for very restricted use case such as firmware upgrading.
*/
void BSP_SystemHalt
(
    void
)
{
    (void)SAL_CoreCriticalEnter();  // Interrupt disable(I/F)
    ARM_Reserved();                 // move ARM p.c on the sram.
}

void BSP_EnableSYSPower
(
    void
)
{
    /* Enable SYS_3P3 */
    (void) GPIO_Config(SYS_PWR_EN, (uint32_t)(GPIO_FUNC(0U) | GPIO_OUTPUT));
    (void) GPIO_Set(SYS_PWR_EN, 1UL);
}

