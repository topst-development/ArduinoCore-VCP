// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : timer.c
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#include "timer.h"

#include <reg_phys.h>
#include <bsp.h>
#include <gic.h>
#include <debug.h>

/*
***************************************************************************************************
*                                             DEFINITIONS
***************************************************************************************************
*/
#if (DEBUG_ENABLE)
// Deviation Record - MISRA C-2012 Rule 20.10, Preprocessing Directives
#   include <debug.h>
#   define TMR_D(fmt, args...)          {LOGD(DBG_TAG_TIMER, fmt, ## args)}
#   define TMR_E(fmt, args...)          {LOGE(DBG_TAG_TIMER, fmt, ## args)}
#else
#   define TMR_D(fmt, args...)
#   define TMR_E(fmt, args...)
#endif


/* Register Map */
#define TMR_OP_EN_CFG                   (0x000UL)
#define TMR_MAIN_CNT_LVD                (0x004UL)
#define TMR_CMP_VALUE0                  (0x008UL)
#define TMR_CMP_VALUE1                  (0x00CUL)
#define TMR_PSCL_CNT                    (0x010UL)
#define TMR_MAIN_CNT                    (0x014UL)
#define TMR_IRQ_CTRL                    (0x018UL)

/* Configuration Value */
#define TMR_OP_EN_CFG_LDM0_ON           (1UL << 28UL)
#define TMR_OP_EN_CFG_LDM1_ON           (1UL << 29UL)
#define TMR_OP_EN_CFG_OPMODE_FREE_RUN   (0UL << 26UL)
#define TMR_OP_EN_CFG_OPMODE_ONE_SHOT   (1UL << 26UL)
#define TMR_OP_EN_CFG_LDZERO_OFFSET     (25UL)
#define TMR_OP_EN_CFG_CNT_EN            (1UL << 24UL)

/* 0: Reading this register to be cleared, 1: Writing a non-zero value to MASKED_IRQ_STATUS to be cleared */
#define TMR_IRQ_CLR_CTRL_WRITE          (1UL << 31UL)
#define TMR_IRQ_CLR_CTRL_READ           (0UL << 31UL)
#define TMR_IRQ_MASK_ALL                (0x1FUL)
#define TMR_IRQ_CTRL_IRQ_EN0            (1UL << 16UL)
#define TMR_IRQ_CTRL_IRQ_EN1            (2UL << 16UL)
#define TMR_IRQ_CTRL_IRQ_EN2            (4UL << 16UL)
#define TMR_IRQ_CTRL_IRQ_EN3            (8UL << 16UL)
#define TMR_IRQ_CTRL_IRQ_EN4            (16UL << 16UL)
#define TMR_IRQ_CTRL_IRQ_ALLEN          ((TMR_IRQ_CTRL_IRQ_EN0)         \
                                        | (TMR_IRQ_CTRL_IRQ_EN1)        \
                                        | (TMR_IRQ_CTRL_IRQ_EN2)        \
                                        | (TMR_IRQ_CTRL_IRQ_EN3)        \
                                        | (TMR_IRQ_CTRL_IRQ_EN4))

#define TMR_PRESCALE                    (11UL)
#define TMR_CLK_RATE                    ((12UL) * (1000UL) * (1000UL))

typedef struct TIMERResourceTable
{
    TIMERChannel_t                      rtChannel;
    bool                             rtUsed;
    TIMERHandler                        rtHandler;
    void *                              rtArgs;
    
} TIMERResource_t;

/*
***************************************************************************************************
*                                             VARIABLES
***************************************************************************************************
*/
static bool                          gTimerInitialized = FALSE;

static TIMERResource_t                  gTimerRes[TIMER_CH_MAX];

/*
***************************************************************************************************
*                                         FUNCTION PROTOTYPES
***************************************************************************************************
*/
static void TIMER_Handler
(
    void *                              pArgs
);

static SALRetCode_t TIMER_EnableMode
(
    const TIMERConfig_t *               pCfg
);

static void TIMER_SetEnableCoreReg
(
    const TIMERConfig_t *               pCfg,
    const uint32_t                        uiCmpVal0,
    const uint32_t                        uiCmpVal1,
    uint32_t                              uiCfgVal,
    const uint32_t                        uiIrqVal
);

static SALRetCode_t TIMER_EnableComp0
(
    const TIMERConfig_t *               pCfg
);

static SALRetCode_t TIMER_EnableComp1
(
    const TIMERConfig_t *               pCfg
);

static SALRetCode_t TIMER_EnableSmallComp
(
    const TIMERConfig_t *               pCfg
);

/*
***************************************************************************************************
*                                         FUNCTIONS
***************************************************************************************************
*/

static void TIMER_Handler
(
    // Deviation Record - MISRA C-2012 Rule 8.13, Declarations and Definitions
    void *                              pArgs
)
{
    TIMERResource_t *   timer = NULL_PTR;
    uint32_t              reg;

    (void)SAL_MemCopy(&timer, (const void *)&pArgs, sizeof(TIMERResource_t *));

    if (timer != NULL_PTR)
    {
        reg = MCU_BSP_TIMER_BASE + ((uint32_t)timer->rtChannel * 0x100UL) + TMR_IRQ_CTRL;

        // Deviation Record - CERT INT36-C, CERT-C Integers
        if (((SAL_ReadReg(reg) & TMR_IRQ_CTRL_IRQ_ALLEN) != 0UL) && (timer->rtUsed == TRUE))
        {
            (void)TIMER_InterruptClear(timer->rtChannel);

            if (timer->rtHandler != NULL_PTR)
            {
                (void)timer->rtHandler(timer->rtChannel, timer->rtArgs);
            }
        }
    }
}

/*
***************************************************************************************************
*                                          TIMER_InterruptClear
*
* This function clears the interrupt signal that is occurred when meeting the conditions.
*
* @param    uiChannel [in] The specific channel for TIMER module
* @return
*
* Notes
*   It is called automatically with dealing with TIMER handler.
*   Therefore, user don't have chance to call this function.
*
***************************************************************************************************
*/
SALRetCode_t TIMER_InterruptClear
(
    TIMERChannel_t                      uiChannel
)
{
    uint32_t  reg;
    uint32_t  clr_ctl;

    reg     = MCU_BSP_TIMER_BASE + ((uint32_t)uiChannel * 0x100U) + TMR_IRQ_CTRL;
    // Deviation Record - CERT INT36-C, CERT-C Integers
    clr_ctl = SAL_ReadReg(reg);

    if ((clr_ctl & TMR_IRQ_CLR_CTRL_WRITE) != 0U)
    {
        // Deviation Record - CERT INT36-C, CERT-C Integers
        SAL_WriteReg(clr_ctl | TMR_IRQ_MASK_ALL, reg);
    }
    else    // TMR_IRQ_CLR_CTRL_READ
    {
        // Deviation Record - CERT INT36-C, CERT-C Integers
        SAL_ReadReg(reg);
    }

    return SAL_RET_SUCCESS;
}

/*
***************************************************************************************************
*                                          TIMER_Enable
*
* This function enables the specific TIMER module with TIMER_OP_FREERUN, TIMER_START_ZERO,
* TIMER_COUNTER_COMP0
*
* @param    uiChannel [in] The specific channel for TIMER module
* @param    uiUSec [in] The micro-second that user want to set for interrupt signal
* @param    fnHandler [in] The external handler which user needs to register
* @param    pArgs [in] The pointer of configuration structure table
* @return
*
* Notes
*
***************************************************************************************************
*/
// Deviation Record - HIS metric violation (HIS_CALLING)
SALRetCode_t TIMER_Enable
(
    TIMERChannel_t                      uiChannel,
    uint32_t                              uiUSec,
    TIMERHandler                        fnHandler,
    void *                              pArgs
)
{
    return TIMER_EnableWithMode(uiChannel, uiUSec, TIMER_OP_FREERUN, fnHandler, pArgs);
}

/*
***************************************************************************************************
*                                          TIMER_EnableWithMode
*
* This function enables the specific TIMER module with TIMER_START_ZERO, TIMER_COUNTER_COMP0.
*
* @param    uiChannel [in] The specific channel for TIMER module
* @param    uiUSec [in] The micro-second that user want to set for interrupt signal
* @param    uiOpMode [in] Operation mode, Free running or One-time running
* @param    fnHandler [in] The external handler which user needs to register
* @param    pArgs [in] The pointer of configuration structure table
* @return
*
* Notes
*
***************************************************************************************************
*/
// Deviation Record - HIS metric violation (HIS_CALLING)
SALRetCode_t TIMER_EnableWithMode
(
    TIMERChannel_t                      uiChannel,
    uint32_t                              uiUSec,
    TIMEROpMode_t                       uiOpMode,
    TIMERHandler                        fnHandler,
    void *                              pArgs
)
{
    TIMERConfig_t cfg;

    cfg.ctChannel       = uiChannel;
    cfg.ctStartMode     = TIMER_START_ZERO;
    cfg.ctOpMode        = uiOpMode;
    cfg.ctCounterMode   = TIMER_COUNTER_COMP0;
    cfg.ctMainValueUsec = 0;
    cfg.ctCmp0ValueUsec = uiUSec;
    cfg.ctCmp1ValueUsec = 0;
    cfg.fnHandler       = fnHandler;
    cfg.pArgs           = pArgs;

    return TIMER_EnableWithCfg(&cfg);
}

/*
***************************************************************************************************
*                                          TIMER_EnableWithCfg
*
* This function enables the specific TIMER module with configuration structure
*
* @param    pCfg [in] Configuration structure with TIMER module information
* @return
*
* Notes
*
***************************************************************************************************
*/
SALRetCode_t TIMER_EnableWithCfg
(
    const TIMERConfig_t *               pCfg
)
{
    SALRetCode_t ret = SAL_RET_SUCCESS;
    
    // MISRA 14.7 : A function shall have a single point of exit at the end of the function
    if (gTimerInitialized == FALSE)
    {
        ret = SAL_DbgReportError
        (
            SAL_DRVID_TMR,
            TIMER_API_ENABLE_CFG,
            TIMER_ERR_NOT_INITIALIZED,
            __FUNCTION__
        );
    }
    else if (pCfg == NULL_PTR)
    {
        ret = SAL_DbgReportError
        (
            SAL_DRVID_TMR,
            TIMER_API_ENABLE_CFG,
            TIMER_ERR_INVALID_PARAM,
            __FUNCTION__
        );
    }
    else if (TIMER_CH_MAX <= pCfg->ctChannel)
    {
        ret = SAL_DbgReportError
        (
            SAL_DRVID_TMR,
            TIMER_API_ENABLE_CFG,
            TIMER_ERR_OUTOF_RANGE_CHANNEL,
            __FUNCTION__
        );
    }
    else
    {
        ret = TIMER_EnableMode(pCfg);
        
        if (ret == SAL_RET_SUCCESS)
        {
            gTimerRes[pCfg->ctChannel].rtUsed       = TRUE;
            gTimerRes[pCfg->ctChannel].rtHandler    = pCfg->fnHandler;
            gTimerRes[pCfg->ctChannel].rtArgs       = pCfg->pArgs;

            if (pCfg->ctChannel != TIMER_CH_0)    // timer 0 specially used by os timer
            {
                (void)GIC_IntVectSet
                (
                    (uint32_t)GIC_TIMER_0 + (uint32_t)pCfg->ctChannel,
                    GIC_PRIORITY_NO_MEAN,
                    GIC_INT_TYPE_LEVEL_HIGH,
                    (GICIsrFunc)&TIMER_Handler,
                    (void*)&gTimerRes[pCfg->ctChannel]
                );
            }       // (ch != TIMER_CH_0)

            TMR_D("Channel (%d) is enabled\n", (uint32_t)pCfg->ctChannel);
        }       // (ret == SAL_RET_SUCCESS)
    }

    return ret;
}

static SALRetCode_t TIMER_EnableMode
(
    const TIMERConfig_t *               pCfg
)
{
    SALRetCode_t    ret = SAL_RET_SUCCESS;

    switch (pCfg->ctCounterMode)
    {
        case TIMER_COUNTER_COMP0 :
        {
            ret = TIMER_EnableComp0(pCfg);

            break;
        }

        case TIMER_COUNTER_COMP1 :
        {
            ret = TIMER_EnableComp1(pCfg);

            break;
        }

        case TIMER_COUNTER_SMALL_COMP :
        {
            ret = TIMER_EnableSmallComp(pCfg);
            
            break;
        }

        default : //TIMER_COUNTER_MAIN
        {
            TIMER_SetEnableCoreReg(pCfg, 0x0UL, 0x0UL, 0x0UL, TMR_IRQ_CTRL_IRQ_EN2);
            
            break;
        }
    }

    return ret;
}

static void TIMER_SetEnableCoreReg
(
    const TIMERConfig_t *               pCfg,
    const uint32_t                        uiCmpVal0,
    const uint32_t                        uiCmpVal1,
    uint32_t                              uiCfgVal,
    const uint32_t                        uiIrqVal
)
{
    uint32_t          mainval = 0x0UL;
    uint32_t          tmpval = 0x0UL;
    uint32_t          reg = MCU_BSP_TIMER_BASE + ((uint32_t)pCfg->ctChannel * 0x100UL);
    uint32_t          rate_factor  = (TMR_CLK_RATE / 1000UL) / ((TMR_PRESCALE + 1UL) * 1000UL);
    
    mainval = (pCfg->ctMainValueUsec == 0UL) ? 0xFFFFFFFFUL : ((pCfg->ctMainValueUsec * rate_factor) - 1UL);
    
    //reset main cnt load value
    // Deviation Record - CERT INT36-C, CERT-C Integers
    SAL_WriteReg(mainval, (uint32_t)(reg + TMR_MAIN_CNT_LVD));
    SAL_WriteReg(uiCmpVal0, (uint32_t)(reg + TMR_CMP_VALUE0));
    SAL_WriteReg(uiCmpVal1, (uint32_t)(reg + TMR_CMP_VALUE1));

    uiCfgVal |= (TMR_PRESCALE
                | TMR_OP_EN_CFG_CNT_EN
                | ((uint32_t)pCfg->ctStartMode << TMR_OP_EN_CFG_LDZERO_OFFSET));

    if (pCfg->ctOpMode == TIMER_OP_ONESHOT)
    {
        uiCfgVal |= TMR_OP_EN_CFG_OPMODE_ONE_SHOT;
    }

    // Deviation Record - CERT INT36-C, CERT-C Integers
    tmpval = SAL_ReadReg((uint32_t)(reg + TMR_IRQ_CTRL));

    // Deviation Record - CERT INT36-C, CERT-C Integers
    SAL_WriteReg(uiCfgVal, (uint32_t)(reg + TMR_OP_EN_CFG));
    SAL_WriteReg((tmpval | uiIrqVal), (uint32_t)(reg + TMR_IRQ_CTRL));
}

static SALRetCode_t TIMER_EnableComp0
(
    const TIMERConfig_t *               pCfg
)
{
    uint32_t          tmpval = 0x0UL;
    uint32_t          rate_factor;
    uint32_t          mainval = 0x0UL;
    uint32_t          cmpval0 = 0x0UL;
    uint32_t          cmpval1 = 0x0UL;
    uint32_t          reg_cfgval = 0x0UL;
    uint32_t          reg_irqval = TMR_IRQ_CTRL_IRQ_EN2;
    SALRetCode_t    ret = SAL_RET_SUCCESS;

    rate_factor = (TMR_CLK_RATE / 1000UL) / ((TMR_PRESCALE + 1UL) * 1000UL);

    // CERT-C Integers (CERT INT30-C) : Ensure that unsigned integer operations do not wrap
    if ((SAL_MAX_INT_VAL / rate_factor) < pCfg->ctCmp0ValueUsec)
    {
        ret = SAL_DbgReportError
        (
            SAL_DRVID_TMR,
            TIMER_API_ENABLE_CFG,
            TIMER_ERR_OUTOF_COMPARATOR_0,
            __FUNCTION__
        );
    }
    else
    {
	    mainval = (pCfg->ctMainValueUsec == 0UL) ? 0xFFFFFFFFUL : ((pCfg->ctMainValueUsec * rate_factor) - 1UL);
        tmpval = (pCfg->ctCmp0ValueUsec * rate_factor) - 1UL;

        if ((pCfg->ctStartMode == TIMER_START_MAINCNT)
            && (((0xFFFFFFFFUL - tmpval) == 0xFFFFFFFFUL) || (mainval > (0xFFFFFFFFUL - tmpval))))
        {
            ret = SAL_DbgReportError
            (
                SAL_DRVID_TMR,
                TIMER_API_ENABLE_CFG,
                TIMER_ERR_OUTOF_COMPARATOR_0,
                __FUNCTION__
            );
        }
        else
        {
            cmpval0     = (pCfg->ctStartMode == TIMER_START_ZERO) ? tmpval : (mainval + tmpval);
            reg_cfgval  = TMR_OP_EN_CFG_LDM0_ON;
            reg_irqval  |= TMR_IRQ_CTRL_IRQ_EN0;

            TIMER_SetEnableCoreReg(pCfg, cmpval0, cmpval1, reg_cfgval, reg_irqval);
        }
    }
    
    return ret;
}

static SALRetCode_t TIMER_EnableComp1
(
    const TIMERConfig_t *               pCfg
)
{
    uint32_t          tmpval = 0x0UL;
    uint32_t          rate_factor;
    uint32_t          mainval;
    uint32_t          cmpval0 = 0x0UL;
    uint32_t          cmpval1 = 0x0UL;
    uint32_t          reg_cfgval = 0x0UL;
    uint32_t          reg_irqval = TMR_IRQ_CTRL_IRQ_EN2;
    SALRetCode_t    ret = SAL_RET_SUCCESS;

    rate_factor = (TMR_CLK_RATE / 1000UL) / ((TMR_PRESCALE + 1UL) * 1000UL);

    // CERT-C Integers (CERT INT30-C) : Ensure that unsigned integer operations do not wrap
    if ((SAL_MAX_INT_VAL / rate_factor) < pCfg->ctCmp1ValueUsec)
    {
        ret = SAL_DbgReportError
        (
            SAL_DRVID_TMR,
            TIMER_API_ENABLE_CFG,
            TIMER_ERR_OUTOF_COMPARATOR_1,
            __FUNCTION__
        );
    }
    else
    {
        mainval = (pCfg->ctMainValueUsec == 0UL) ? 0xFFFFFFFFUL : ((pCfg->ctMainValueUsec * rate_factor) - 1UL);
        tmpval = (pCfg->ctCmp1ValueUsec * rate_factor) - 1UL;

        if ((pCfg->ctStartMode == TIMER_START_MAINCNT)
            && (((0xFFFFFFFFUL - tmpval) == 0xFFFFFFFFUL) || (mainval > (0xFFFFFFFFUL - tmpval))))
        {
            ret = SAL_DbgReportError
            (
                SAL_DRVID_TMR,
                TIMER_API_ENABLE_CFG,
                TIMER_ERR_OUTOF_COMPARATOR_1,
                __FUNCTION__
            );
        }
        else
        {
            cmpval1     = (pCfg->ctStartMode == TIMER_START_ZERO) ? tmpval : (mainval + tmpval);
            reg_cfgval  = TMR_OP_EN_CFG_LDM1_ON;
            reg_irqval  |= TMR_IRQ_CTRL_IRQ_EN1;

            TIMER_SetEnableCoreReg(pCfg, cmpval0, cmpval1, reg_cfgval, reg_irqval);
        }
    }
    
    return ret;
}

static SALRetCode_t TIMER_EnableSmallComp
(
    const TIMERConfig_t *               pCfg
)
{
    uint32_t          rate_factor;
    uint32_t          tmpval0 = 0x0UL;
    uint32_t          tmpval1 = 0x0UL;
    uint32_t          mainval = 0x0UL;
    uint32_t          cmpval0 = 0x0UL;
    uint32_t          cmpval1 = 0x0UL;
    uint32_t          reg_cfgval = 0x0UL;
    uint32_t          reg_irqval = TMR_IRQ_CTRL_IRQ_EN2;
    SALRetCode_t    ret = SAL_RET_SUCCESS;

    rate_factor = (TMR_CLK_RATE / 1000UL) / ((TMR_PRESCALE + 1UL) * 1000UL);
    mainval = (pCfg->ctMainValueUsec == 0UL) ? 0xFFFFFFFFUL : ((pCfg->ctMainValueUsec * rate_factor) - 1UL);

    // CERT-C Integers (CERT INT30-C) : Ensure that unsigned integer operations do not wrap
    if ((SAL_MAX_INT_VAL / rate_factor) < pCfg->ctCmp0ValueUsec)
    {
        ret = SAL_DbgReportError
        (
            SAL_DRVID_TMR,
            TIMER_API_ENABLE_CFG,
            TIMER_ERR_OUTOF_COMPARATOR_0,
            __FUNCTION__
        );
    }
    else if ((SAL_MAX_INT_VAL / rate_factor) < pCfg->ctCmp1ValueUsec)
    {
        ret = SAL_DbgReportError
        (
            SAL_DRVID_TMR,
            TIMER_API_ENABLE_CFG,
            TIMER_ERR_OUTOF_COMPARATOR_1,
            __FUNCTION__
        );
    }
    else
    {
        tmpval0 = (pCfg->ctCmp0ValueUsec * rate_factor) - 1UL;
        tmpval1 = (pCfg->ctCmp1ValueUsec * rate_factor) - 1UL;
    
        if (pCfg->ctStartMode == TIMER_START_MAINCNT)
        {
            // Less value is selected
            if (tmpval0 <= tmpval1)
            {
                // CERT-C Integers (CERT INT30-C) : Ensure that unsigned integer operations do not wrap
                if ((SAL_MAX_INT_VAL - mainval) <= tmpval0)
                {
                    ret = SAL_DbgReportError
                    (
                        SAL_DRVID_TMR,
                        TIMER_API_ENABLE_CFG,
                        TIMER_ERR_OUTOF_COMPARATOR_0,
                        __FUNCTION__
                    );
                }
                else
                {
                    cmpval0 = mainval + tmpval0;
                    cmpval1 = SAL_MAX_INT_VAL;
                }
            }
            else
            {
                if ((SAL_MAX_INT_VAL - mainval) <= tmpval1)
                {
                    ret = SAL_DbgReportError
                    (
                        SAL_DRVID_TMR,
                        TIMER_API_ENABLE_CFG,
                        TIMER_ERR_OUTOF_COMPARATOR_1,
                        __FUNCTION__
                    );
                }
                else
                {
                    cmpval0 = SAL_MAX_INT_VAL;
                    cmpval1 = mainval + tmpval1;
                }
            }
        }
        else
        {
            cmpval0 = tmpval0;
            cmpval1 = tmpval1;
        }

        if (ret == SAL_RET_SUCCESS)
        {
            reg_cfgval  = (TMR_OP_EN_CFG_LDM0_ON | TMR_OP_EN_CFG_LDM1_ON);
            reg_irqval  |= (TMR_IRQ_CTRL_IRQ_EN0 | TMR_IRQ_CTRL_IRQ_EN1);

            TIMER_SetEnableCoreReg(pCfg, cmpval0, cmpval1, reg_cfgval, reg_irqval);
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          TIMER_Disable
*
* This function disables the specific TIMER module.
*
* @param    uiChannel [in] The specific channel for TIMER module
* @return
*
* Notes
*
***************************************************************************************************
*/
// Deviation Record - HIS metric violation (HIS_CALLING)
SALRetCode_t TIMER_Disable
(
    TIMERChannel_t                      uiChannel
)
{
    SALRetCode_t    ret;
    uint32_t          reg;
    uint32_t          tmpval;

    ret = SAL_RET_SUCCESS;
    reg = MCU_BSP_TIMER_BASE + ((uint32_t)uiChannel * 0x100UL);
    tmpval = 0x0UL;

    // MISRA 14.7 : A function shall have a single point of exit at the end of the function
    if (gTimerInitialized == FALSE)
    {
        ret = SAL_DbgReportError
        (
            SAL_DRVID_TMR,
            TIMER_API_DISABLE,
            TIMER_ERR_NOT_INITIALIZED,
            __FUNCTION__
        );
    }
    else
    {
        // Deviation Record - CERT INT36-C, CERT-C Integers
        tmpval = SAL_ReadReg((uint32_t)(reg + TMR_IRQ_CTRL));

        // Deviation Record - CERT INT36-C, CERT-C Integers
        SAL_WriteReg((tmpval & (~TMR_OP_EN_CFG_CNT_EN)), (uint32_t)(reg + TMR_OP_EN_CFG));
        SAL_WriteReg(0U, (uint32_t)(reg + TMR_IRQ_CTRL));

        gTimerRes[uiChannel].rtUsed     = FALSE;
        gTimerRes[uiChannel].rtHandler  = NULL;
        gTimerRes[uiChannel].rtArgs     = NULL_PTR;

        TMR_D("Channel (%d) is disabled\n", (uint32_t)uiChannel);
    }

    return ret;
}

/*
***************************************************************************************************
*                                          TIMER_Init
*
* This function initializes clock and registers for all channels
*
* @return
*
* Notes
*
***************************************************************************************************
*/
SALRetCode_t TIMER_Init
(
    void
)
{
    uint32_t          reg;
    uint32_t          resIndex;
    uint32_t          reg_val;
    SALRetCode_t    ret;

    ret = SAL_RET_SUCCESS;

    for (resIndex = 0 ; resIndex < (uint32_t)TIMER_CH_MAX ; resIndex++)
    {
        reg = MCU_BSP_TIMER_BASE + (resIndex * 0x100UL);

        gTimerRes[resIndex].rtChannel  = (TIMERChannel_t)resIndex;
        gTimerRes[resIndex].rtUsed     = FALSE;
        gTimerRes[resIndex].rtHandler  = NULL_PTR;
        gTimerRes[resIndex].rtArgs     = NULL_PTR;

        // Deviation Record - CERT INT36-C, CERT-C Integers
        SAL_WriteReg(0x7FFFU, (uint32_t)(reg + TMR_OP_EN_CFG));
        SAL_WriteReg(0x0U,    (uint32_t)(reg + TMR_MAIN_CNT_LVD));
        SAL_WriteReg(0x0U,    (uint32_t)(reg + TMR_CMP_VALUE0));
        SAL_WriteReg(0x0U,    (uint32_t)(reg + TMR_CMP_VALUE1));

        reg_val = TMR_IRQ_CLR_CTRL_WRITE | TMR_IRQ_MASK_ALL;    // = TMR_IRQ_CLR_CTRL_READ | TMR_IRQ_MASK_ALL;

        SAL_WriteReg(reg_val, (uint32_t)(reg + TMR_IRQ_CTRL));
    }

    gTimerInitialized = TRUE;

    return ret;
}

