// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : pmio_dev_extn_pw_ctl.c
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#include <pmio_dev_extn_pw_ctl.h>
#include <pmio_dev.h>
#include <gpio.h>
#include <gic.h>
#include <debug.h>

/*************************************************************************************************
 *                                             DEFINITIONS
 *************************************************************************************************/

static PMIOEXTNExtPwrPins_t gtExtPwrPins;

static uint8_t gucTccRstIrqStatus = 0U;
static uint32_t guiGpk32 = 0UL;

/************************************************************************************************/
/*                                             STATIC FUNCTION                                  */
/************************************************************************************************/

static bool PMIO_EXTN_IsGpk
(
    uint32_t uiVa
);

static void PMIO_EXTN_PmicResetInterruptHandler
(
    void *                              pArg
);
/*************************************************************************************************/
/*                                             Implementation                                    */
/* ***********************************************************************************************/
static bool PMIO_EXTN_IsGpk
(
    uint32_t uiVa
)
{
    bool bRet = FALSE;

    if(GPIO_PORT_K == (uiVa & GPIO_PORT_K))
    {
        bRet = TRUE;
    }

    return bRet;
}

static uint32_t PMIO_EXTN_GetGpkNum
(
    uint32_t uiVa
)
{
    uint32_t uiGpkNum = (uiVa & 0x1FUL);

    //Store using gpk
    guiGpk32 |= (1UL << uiGpkNum);

    return uiGpkNum;
}

static void PMIO_EXTN_PmicResetInterruptHandler
(
    void *                              pArg
)
{
    (void)pArg;

    if(gucTccRstIrqStatus == 1U)
    {
        gucTccRstIrqStatus = 0U;
        (void)GIC_IntSrcDis(PMIO_EXTN_VA_INTERRUPT_SRC_PMIC_RST);
    }
    (void)GPIO_Set(gtExtPwrPins.uiTccRstPin, 1UL);
    mcu_printf("extn power reset %d\n", gucTccRstIrqStatus);
}

void PMIO_EXTN_Init
(
    PMIOEXTNExtPwrPins_t tExtPwrPins
)
{
    uint32_t uiGpkNum;
    uint32_t uiCheckParam;

    uiGpkNum = 0UL;
    uiCheckParam = tExtPwrPins.uiPwrEnPin + \
                   tExtPwrPins.uiAlivePwrConPin + \
                   tExtPwrPins.uiStrModePin + \
                   tExtPwrPins.uiPmicRstPin + \
                   tExtPwrPins.uiTccRstPin;


    if(uiCheckParam > 0UL)
    {
        gtExtPwrPins.uiPwrEnPin          = tExtPwrPins.uiPwrEnPin      ;
        gtExtPwrPins.uiAlivePwrConPin    = tExtPwrPins.uiAlivePwrConPin;
        gtExtPwrPins.uiStrModePin        = tExtPwrPins.uiStrModePin    ;
        gtExtPwrPins.uiPmicRstPin        = tExtPwrPins.uiPmicRstPin    ;
        gtExtPwrPins.uiTccRstPin         = tExtPwrPins.uiTccRstPin     ;
    }
    else
    {
        gtExtPwrPins.uiPwrEnPin          = PMIO_EXTN_VA_STR_MODE_GPIO      ;
        gtExtPwrPins.uiAlivePwrConPin    = PMIO_EXTN_VA_PWR_EN_GPIO        ;
        gtExtPwrPins.uiStrModePin        = PMIO_EXTN_VA_ALIVE_PWR_CON_GPIO ;
        gtExtPwrPins.uiPmicRstPin        = PMIO_EXTN_VA_PMIC_RESET_OUT_GPIO;
        gtExtPwrPins.uiTccRstPin         = PMIO_EXTN_VA_TCC_RESET_GPIO     ;
    }

    /*STR_MODE*/
    if(PMIO_EXTN_IsGpk(gtExtPwrPins.uiStrModePin) == TRUE)
    {
        uiGpkNum = PMIO_EXTN_GetGpkNum(gtExtPwrPins.uiStrModePin);
        PMIO_REG_PMGPIO_FS  &=  (~(1UL<< uiGpkNum)); //0: pin controlled by PMGPIO
        PMIO_REG_PMGPIO_EN  |=    (1UL<< uiGpkNum);  //1: out
    }
    else
    {
        mcu_printf("Warnning! STR pin must be GPIO_K port.\n");
    }

    /*PWR_EN*/
    if(PMIO_EXTN_IsGpk(gtExtPwrPins.uiPwrEnPin) == TRUE)
    {
        uiGpkNum = PMIO_EXTN_GetGpkNum(gtExtPwrPins.uiPwrEnPin);
        PMIO_REG_PMGPIO_FS  |=  (1UL<< uiGpkNum ); //1: pin controlled by GPIO
        PMIO_REG_PMGPIO_EN  |=  (1UL<< uiGpkNum );  //1: out
    }
    ( void ) GPIO_Config(
            gtExtPwrPins.uiPwrEnPin,
            ( uint32_t ) ( GPIO_FUNC( 0U ) | GPIO_OUTPUT )
            );


    /*ALIVE_PWR_CON*/
    if(PMIO_EXTN_IsGpk(gtExtPwrPins.uiAlivePwrConPin) == TRUE)
    {
        uiGpkNum = PMIO_EXTN_GetGpkNum(gtExtPwrPins.uiAlivePwrConPin);
        PMIO_REG_PMGPIO_FS  |=  (1UL<< uiGpkNum ); //1: pin controlled by GPIO
        PMIO_REG_PMGPIO_EN  |=  (1UL<< uiGpkNum );  //1: out
    }
    ( void ) GPIO_Config(
            gtExtPwrPins.uiAlivePwrConPin,
            ( uint32_t ) ( GPIO_FUNC( 0U ) | GPIO_OUTPUT )
            );


    /*PMIC_RESET_OUT*/
    if(PMIO_EXTN_IsGpk(gtExtPwrPins.uiPmicRstPin) == TRUE)
    {
        uiGpkNum = PMIO_EXTN_GetGpkNum(gtExtPwrPins.uiPmicRstPin);
        PMIO_REG_PMGPIO_FS  |=    (1UL<< uiGpkNum ); //1: pin controlled by GPIO
        PMIO_REG_PMGPIO_EN  &=  (~(1UL<< uiGpkNum ));  //0: in
    }
    ( void ) GPIO_Config(
            gtExtPwrPins.uiPmicRstPin,
            ( uint32_t ) ( GPIO_FUNC( 0U ) | GPIO_INPUT|GPIO_INPUTBUF_EN )
            );

    /*TCC_RESET*/
    if(PMIO_EXTN_IsGpk(gtExtPwrPins.uiTccRstPin) == TRUE)
    {
        uiGpkNum = PMIO_EXTN_GetGpkNum(gtExtPwrPins.uiTccRstPin);
        PMIO_REG_PMGPIO_FS  |=  (1UL<< uiGpkNum ); //1: pin controlled by GPIO
        PMIO_REG_PMGPIO_EN  |=  (1UL<< uiGpkNum );  //1: out
    }
    ( void ) GPIO_Config(
            gtExtPwrPins.uiTccRstPin,
            ( uint32_t ) ( GPIO_FUNC( 0U ) | GPIO_OUTPUT )
            );
    (void)GPIO_Set(gtExtPwrPins.uiTccRstPin, 0UL);


    /*check PMIC_RESET_OUT*/
    (void)GPIO_IntExtSet(PMIO_EXTN_VA_INTERRUPT_SRC_PMIC_RST, gtExtPwrPins.uiPmicRstPin);
    (void)GIC_IntVectSet( \
            PMIO_EXTN_VA_INTERRUPT_SRC_PMIC_RST, \
            PMIO_EXTN_VA_INTERRUPT_PRIO_PMIC_RST, \
            GIC_INT_TYPE_LEVEL_HIGH, \
            &PMIO_EXTN_PmicResetInterruptHandler, \
            NULL_PTR );

    //(void)GIC_IntSrcEn(PMIO_EXTN_VA_INTERRUPT_SRC_PMIC_RST);
}

void PMIO_EXTN_SetStr
(
    uint8_t ucEn
)
{
    uint32_t uiGpkNum = 0UL;

    if(PMIO_EXTN_IsGpk(gtExtPwrPins.uiStrModePin) == TRUE)
    {
        uiGpkNum = PMIO_EXTN_GetGpkNum(gtExtPwrPins.uiStrModePin);

        if(ucEn == 0U)
        {
            PMIO_REG_PMGPIO_DAT &= (~(1UL<< uiGpkNum )); //0: Low
        }
        else
        {
            PMIO_REG_PMGPIO_DAT |=   (1UL<< uiGpkNum ); //1: High
        }
    }
    else
    {
        mcu_printf("Warnning! STR pin must be GPIO_K port.\n");
    }
}

void PMIO_EXTN_SetPower
(
    uint8_t ucEn
)
{
    uint32_t uiGpkNum = 0UL;

    if(ucEn == 0U)
    {
        uiGpkNum = PMIO_EXTN_GetGpkNum(gtExtPwrPins.uiStrModePin);

        if((PMIO_REG_PMGPIO_DAT & (1UL<< uiGpkNum )) == 0x0UL)
        {
            (void)GPIO_Set(gtExtPwrPins.uiAlivePwrConPin, 0UL);
        }
        else
        {
            (void)GPIO_Set(gtExtPwrPins.uiAlivePwrConPin, 1UL);
        }
        (void)GPIO_Set(gtExtPwrPins.uiPwrEnPin, 0UL);

        if(gucTccRstIrqStatus == 1U)
        {
            gucTccRstIrqStatus = 0U;
            (void)GIC_IntSrcDis(PMIO_EXTN_VA_INTERRUPT_SRC_PMIC_RST);
        }
    }
    else
    {
        (void)GPIO_Set(gtExtPwrPins.uiAlivePwrConPin, 1UL);
        (void)GPIO_Set(gtExtPwrPins.uiPwrEnPin, 1UL);

        if(gucTccRstIrqStatus == 0U)
        {
            gucTccRstIrqStatus = 1U;
            (void)GIC_IntSrcEn(PMIO_EXTN_VA_INTERRUPT_SRC_PMIC_RST);
        }
    }
}

uint32_t PMIO_EXTN_GetUsingGpk32
(
    void
)
{
    return guiGpk32;
}


