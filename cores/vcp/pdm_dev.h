// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : pdm_dev.h
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#ifndef MCU_BSP_PDM_DEV_HEADER
#define MCU_BSP_PDM_DEV_HEADER

#define MCU_BSP_SUPPORT_DRIVER_PDM 1

#if ( MCU_BSP_SUPPORT_DRIVER_PDM == 1 )

/*
***************************************************************************************************
*                                             DEFINITIONS
***************************************************************************************************
*/

/* Write Lock Password for 805x */
#define PCFG_WR_PW                      (0x5AFEACE5UL)

/*-----------------------------------------------------------------------*/

/*--------------------------------PDM Register---------------------------*/

/*-----------------------------------------------------------------------*/

/****************************************************************************
 * Define - PDM Register Map BASE Addr.
****************************************************************************/
#define PDM_BASE                        (MCU_BSP_PWM_BASE)
#define PDM0_BASE                       (PDM_BASE)
#define PDM1_BASE                       (PDM_BASE + 0x10000UL)
#define PDM2_BASE                       (PDM_BASE + 0x20000UL)

/****************************************************************************
 * Define -  OFFSET of the PDM 0/1/2
****************************************************************************/
#define PDM_MODULE_OFFSET               (0x10000UL)


/****************************************************************************
 * Define -  OFFSET of PDM STATUS Register from PDM_BASE
****************************************************************************/
#define PDM_STATUS_REG_OFFSET           (0x00UL)

/****************************************************************************
 * Define -  OFFSET of PDM OP_EN Register from PDM_BASE
****************************************************************************/
#define PDM_OP_EN_REG_OFFSET            (0x04UL)

/****************************************************************************
 * Define -  OFFSET of PDM OP_MODE Register from PDM_BASE
****************************************************************************/
#define PDM_OP_MODE_REG_OFFSET          (0x08UL)

/****************************************************************************
 * Define -  OFFSET of PDM OP_STEP Register from PDM_BASE
****************************************************************************/
#define PDM_OP_STEP_REG_OFFSET          (0x0CUL)

/****************************************************************************
 * Define -  OFFSET of PDM X_PSTN1 Register from PDM_BASE
             (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PDM_GetPSTN1Reg(X)              ((0x10UL) + ((X) * (0x10UL)))

/****************************************************************************
 * Define -  OFFSET of PDM X_PSTN2 Register from PDM_BASE
             (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PDM_GetPSTN2Reg(X)              ((0x14UL) + ((X) * (0x10UL)))

/****************************************************************************
 * Define -  OFFSET of PDM X_PSTN3 Register from PDM_BASE
             (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PDM_GetPSTN3Reg(X)              ((0x18UL) + ((X) * (0x10UL)))

/****************************************************************************
 * Define -  OFFSET of PDM X_PSTN4 Register from PDM_BASE
             (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PDM_GetPSTN4Reg(X)              ((0x1CUL) + ((X) * (0x10UL)))

/****************************************************************************
 * Define -  OFFSET of PDM X_OUT_PTN1 Register from PDM_BASE
             (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PDM_GetOutPTN1Reg(X)            ((0x50UL) + ((X) * (0x10UL)))

/****************************************************************************
 * Define -  OFFSET of PDM X_OUT_PTN2 Register from PDM_BASE
             (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PDM_GetOutPTN2Reg(X)            ((0x54UL) + ((X) * (0x10UL)))

/****************************************************************************
 * Define -  OFFSET of PDM X_OUT_PTN3 Register from PDM_BASE
             (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PDM_GetOutPTN3Reg(X)            ((0x58UL) + ((X) * (0x10UL)))

/****************************************************************************
 * Define -  OFFSET of PDM X_OUT_PTN4 Register from PDM_BASE
             (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PDM_GetOutPTN4Reg(X)            ((0x5CUL) + ((X) * (0x10UL)))

/****************************************************************************
 * Define -  OFFSET of PDM MAX_CNT Register from PDM_BASE
****************************************************************************/
#define PDM_MAX_CNT_REG_OFFSET          (0x90UL)

/****************************************************************************
 * Define -  SHIFT Value for PDMn_BUSY_A/B/C/D Channel
             (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PDM_GetBusyStatusReg(X)         ((0UL) + ((X) * (0x1UL)))

/****************************************************************************
 * Define -  SHIFT Value for PDMn_EN_VUP_TRIG_A/B/C/D Channel
             (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PDM_GetOPENEnableReg(X)         ((0UL) + ((X) * (0x1UL)))
#define PDM_GetOPENValueUpReg(X)        ((4UL) + ((X) * (0x1UL)))
#define PDM_GetOPENTrigReg(X)           ((16UL) + ((X) * (0x1UL)))

/****************************************************************************
 * Define -  SHIFT Value for PDMn_MODE_INV_IDLE_A/B/C/D Channel
             (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PDM_GetOPModeOperationModeReg(X)    ((0UL) + ((X) * (0x4UL)))
#define PDM_GetOPModeSignalInverseReg(X)    ((16UL) + ((X) * (0x1UL)))
#define PDM_GetOPModeOutputIdleReg(X)        ((20UL) + ((X) * (0x1UL)))
#define PDM_GetOPModeClockDivideReg(X)      ((24UL) + ((X) * (0x2UL)))

/****************************************************************************
 * Define -  SHIFT Value for PDMn_MODE_INV_IDLE_A/B/C/D Channel
             (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PDM_GetOPStepLoopCountReg(X)    ((0UL) + ((X) * (0x4UL)))

/****************************************************************************
 * Define -  SHIFT Value for PDMn_MAX_CNT_A/B/C/D Channel
             (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PDM_GetMaxCountValueReg(X)      ((0UL) + ((X) * (0x8UL)))

/*-------------------------------------------------------------------------------*/

/*---------------------------Configuration Register------------------------------*/

/*-------------------------------------------------------------------------------*/

/****************************************************************************
 * Define - PDM CONFIG Register Map BASE Addr.
****************************************************************************/
#define PDM_CFG_BASE                    (MCU_BSP_PWM_CFG_BASE)

/****************************************************************************
 * Define -  OFFSET of the PDM_0/1/2_OUT_CTRL from PDM_CFG_BASE
            (X is PDM0 : 0, PDM1: 1, PDM2 : 2)
****************************************************************************/
#define PDM_GetCfgOutCtrlReg(X)         ((0UL) + ((X) * (0x04UL)))

/****************************************************************************
 * Define -  SHIFT Value for PDMn_A/B/C/D Channel_OUT_CTRL
             (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PDM_GetCfgOutCtrlShift(X)       ((0UL) + ((X) * (0x8UL)))

/****************************************************************************
 * Define -  OFFSET of PDM_PCFG_WR_PW Register from PDM_CFG_BASE
****************************************************************************/
#define PDM_PCFG_WR_PW_REG_OFFSET       (0x10UL)

/****************************************************************************
 * Define -  OFFSET of PDM_PCFG_WR_LOCK Register from PDM_CFG_BASE
****************************************************************************/
#define PDM_PCFG_WR_LOCK_REG_OFFSET     (0x14UL)

/****************************************************************************
 * Define -  WRITE LOCK PASSWORD
****************************************************************************/
#define WRITE_LOCK_PASSWORD             (0x5AFEACE5UL)

/*----------------------------------------------------------------------------*/

/*---------------------PMM Register for Safety Features-----------------------*/

/*----------------------------------------------------------------------------*/

/****************************************************************************
 * Define - PMM Register Map BASE Addr.for
 Safey Feature
****************************************************************************/
#define PMM_BASE                        (PDM_BASE + 0x80000UL)
#define PMM0_BASE                       (PMM_BASE)
#define PMM1_BASE                       (PMM_BASE + 0x10000UL)
#define PMM2_BASE                       (PMM_BASE + 0x20000UL)

/****************************************************************************
 * Define -  OFFSET of the PMM 0/1/2
****************************************************************************/
#define PMM_MODULE_OFFSET               (0x10000UL)


/****************************************************************************
 * Define -  OFFSET of PMM Operation Enable Register from PMM_BASE
****************************************************************************/
#define PMM_PDMEN_REG_OFFSET            (0x04UL)

/****************************************************************************
 * Define -  SHIFT Value for PMMn_PDMEN_A/B/C/D Channel
             (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PMM_GetPDMENEnableReg(X)        ((0UL) + ((X) * (0x1UL)))

/****************************************************************************
 * PMMCFG is the same as PDM (PMMn_BASE + 0x08) ~ (PMMn_BASE + 0x93)
****************************************************************************/


/****************************************************************************
 * Define -  OFFSET of PMM Operation Enable Register from PMM_BASE
****************************************************************************/
#define PMM_CTRL_REG_OFFSET             (0xA0UL)

/****************************************************************************
 * Define -  SHIFT Value for PMMn_EN_CM_EN_CNECT_TRIG_A/B/C/D Channel
             (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PMM_GetPMMCtrlPmmEnReg(X)       ((0UL) + ((X) * (0x1UL)))
#define PMM_GetPMMCtrlPmmCMENReg(X)     ((4UL) + ((X) * (0x1UL)))
#define PMM_GetPMMCtrlPmmCNECTReg(X)    ((8UL) + ((X) * (0x1UL)))
#define PMM_GetPMMCtrlPmmTRIGReg(X)     ((16UL) + ((X) * (0x1UL)))

/****************************************************************************
 * Define -  OFFSET of PMM Fault State Register from PMM_BASE
****************************************************************************/
#define PMM_FAULT_STS_REG_OFFSET        (0xA4UL)

#define PMM_FAULT_STS_CH_A_MASK         (0x1UL)
#define PMM_FAULT_STS_CH_B_MASK         (0x2UL)
#define PMM_FAULT_STS_CH_C_MASK         (0x4UL)
#define PMM_FAULT_STS_CH_D_MASK         (0x8UL)

/****************************************************************************
 * Define -  SHIFT Value for PMMn_FLT_STS_A/B/C/D Channel
             (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PMM_GetPMMFaultStatusReg(X)     ((0UL) + ((X) * (0x1UL)))

/****************************************************************************
 * Define -  OFFSET of the PMMn_DUTY_MRG from PMM_BASE
            (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PMM_GetPMMDutyMarginReg(X)      ((0xA8UL) + ((X) * (0x04UL)))

/****************************************************************************
 * Define -  SHIFT Value for PMMn_DUTY_MRG_FLT_VAL Channel
 ***************************************************************************/
#define PMM_DUTY_MARGIN_OFFSET          (0x0UL)
#define PMM_FLT_VAL_OFFSET              (0x10UL)

/****************************************************************************
 * Define -  OFFSET of the PMMn_TO VAL from PMM_BASE
            (X is CH-A : 0, CH-B: 1, CH-C: 2, CH-D: 3)
****************************************************************************/
#define PMM_GetPMMTimeoutValueReg(X)    ((0xB8UL) + ((X) * (0x04UL)))


/*-----------------------------------------------------------------------*/

/*-------------------------------GPIO Register---------------------------*/

/*-----------------------------------------------------------------------*/

/****************************************************************************
 * Define - GPIO Peripheral Select Register Map BASE Addr.
****************************************************************************/
#define PDM_PERICH_SEL_BASE             (MCU_BSP_GPIO_BASE + 0x2B8UL)

/****************************************************************************
 * Define -  OFFSET of the PDM_PERICH_SEL_BASE
****************************************************************************/
#define PDM_GetPeriSelectionShift(X)    ((10UL) + ((X) * (0x02UL)))

#endif  // ( MCU_BSP_SUPPORT_DRIVER_PDM == 1 )

#endif  // MCU_BSP_PDM_DEV_HEADER

