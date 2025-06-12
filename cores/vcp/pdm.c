// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : pdm.c
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#define MCU_BSP_SUPPORT_DRIVER_PDM 1

#if (MCU_BSP_SUPPORT_DRIVER_PDM == 1)

#include "bsp.h"

#if (MCU_BSP_SUPPORT_DRIVER_FMU == 1)
#include "fmu.h"
#endif // ( MCU_BSP_SUPPORT_DRIVER_FMU == 1 )

#include "gpio.h"
#include "pdm.h"
#include "pmio.h"

/**************************************************************************************************
 *                                             LOCAL VARIABLES
 ***************************************************************************************************/

static PDMChannelHandler_t PDMHandler[PDM_TOTAL_CHANNELS];

#ifdef PDM_SAFETY_FEATURE_ENABLED
static PMMISRData_t PDMIsrData[PDM_TOTAL_CHANNELS];
static boolean g_safety_enabled = FALSE;
#endif

/* On the TCC70xx, PDM port is configured at PERICHSEL register (0xA0F222B8) */
static PDMPortConfig_t sPdmPort[PDM_OUT_PORT_MAX] =
    {
        /* PDM select channel 0 : GPIO-A */
        {GPIO_PERICH_SEL_PWMSEL_0, GPIO_GPA(10UL), GPIO_FUNC(2UL), GPIO_PERICH_CH0},
        {GPIO_PERICH_SEL_PWMSEL_1, GPIO_GPA(11UL), GPIO_FUNC(2UL), GPIO_PERICH_CH0},
        {GPIO_PERICH_SEL_PWMSEL_2, GPIO_GPA(12UL), GPIO_FUNC(2UL), GPIO_PERICH_CH0},
        {GPIO_PERICH_SEL_PWMSEL_3, GPIO_GPA(13UL), GPIO_FUNC(2UL), GPIO_PERICH_CH0},
        {GPIO_PERICH_SEL_PWMSEL_4, GPIO_GPA(14UL), GPIO_FUNC(2UL), GPIO_PERICH_CH0},
        {GPIO_PERICH_SEL_PWMSEL_5, GPIO_GPA(15UL), GPIO_FUNC(2UL), GPIO_PERICH_CH0},
        {GPIO_PERICH_SEL_PWMSEL_6, GPIO_GPA(16UL), GPIO_FUNC(2UL), GPIO_PERICH_CH0},
        {GPIO_PERICH_SEL_PWMSEL_7, GPIO_GPA(17UL), GPIO_FUNC(2UL), GPIO_PERICH_CH0},
        {GPIO_PERICH_SEL_PWMSEL_8, GPIO_GPA(18UL), GPIO_FUNC(2UL), GPIO_PERICH_CH0},

        /* PDM select channel 1 : GPIO-B */
        {GPIO_PERICH_SEL_PWMSEL_0, GPIO_GPB(6UL), GPIO_FUNC(2UL), GPIO_PERICH_CH1},
        {GPIO_PERICH_SEL_PWMSEL_1, GPIO_GPB(7UL), GPIO_FUNC(2UL), GPIO_PERICH_CH1},
        {GPIO_PERICH_SEL_PWMSEL_2, GPIO_GPB(8UL), GPIO_FUNC(2UL), GPIO_PERICH_CH1},
        {GPIO_PERICH_SEL_PWMSEL_3, GPIO_GPB(9UL), GPIO_FUNC(2UL), GPIO_PERICH_CH1},
        {GPIO_PERICH_SEL_PWMSEL_4, GPIO_GPB(10UL), GPIO_FUNC(2UL), GPIO_PERICH_CH1},
        {GPIO_PERICH_SEL_PWMSEL_5, GPIO_GPB(11UL), GPIO_FUNC(2UL), GPIO_PERICH_CH1},
        {GPIO_PERICH_SEL_PWMSEL_6, GPIO_GPB(26UL), GPIO_FUNC(2UL), GPIO_PERICH_CH1},
        {GPIO_PERICH_SEL_PWMSEL_7, GPIO_GPB(27UL), GPIO_FUNC(2UL), GPIO_PERICH_CH1},
        {GPIO_PERICH_SEL_PWMSEL_8, GPIO_GPB(28UL), GPIO_FUNC(2UL), GPIO_PERICH_CH1},

        /* PDM select channel 2 : GPIO-C */
        {GPIO_PERICH_SEL_PWMSEL_0, GPIO_GPC(6UL), GPIO_FUNC(2UL), GPIO_PERICH_CH2},
        {GPIO_PERICH_SEL_PWMSEL_1, GPIO_GPC(7UL), GPIO_FUNC(2UL), GPIO_PERICH_CH2},
        {GPIO_PERICH_SEL_PWMSEL_2, GPIO_GPC(8UL), GPIO_FUNC(2UL), GPIO_PERICH_CH2},
        {GPIO_PERICH_SEL_PWMSEL_3, GPIO_GPC(9UL), GPIO_FUNC(2UL), GPIO_PERICH_CH2},
        {GPIO_PERICH_SEL_PWMSEL_4, GPIO_GPC(10UL), GPIO_FUNC(2UL), GPIO_PERICH_CH2},
        {GPIO_PERICH_SEL_PWMSEL_5, GPIO_GPC(11UL), GPIO_FUNC(2UL), GPIO_PERICH_CH2},
        {GPIO_PERICH_SEL_PWMSEL_6, GPIO_GPC(12UL), GPIO_FUNC(2UL), GPIO_PERICH_CH2},
        {GPIO_PERICH_SEL_PWMSEL_7, GPIO_GPC(13UL), GPIO_FUNC(2UL), GPIO_PERICH_CH2},
        {GPIO_PERICH_SEL_PWMSEL_8, GPIO_GPC(14UL), GPIO_FUNC(2UL), GPIO_PERICH_CH2},

        /* PDM select channel 3 : GPIO-K */
        {GPIO_PERICH_SEL_PWMSEL_0, GPIO_GPK(8UL), GPIO_FUNC(2UL), GPIO_PERICH_CH3},
        {GPIO_PERICH_SEL_PWMSEL_1, GPIO_GPK(9UL), GPIO_FUNC(2UL), GPIO_PERICH_CH3},
        {GPIO_PERICH_SEL_PWMSEL_2, GPIO_GPK(10UL), GPIO_FUNC(2UL), GPIO_PERICH_CH3},
        {GPIO_PERICH_SEL_PWMSEL_3, GPIO_GPK(11UL), GPIO_FUNC(2UL), GPIO_PERICH_CH3},
        {GPIO_PERICH_SEL_PWMSEL_4, GPIO_GPK(12UL), GPIO_FUNC(2UL), GPIO_PERICH_CH3},
        {GPIO_PERICH_SEL_PWMSEL_5, GPIO_GPK(13UL), GPIO_FUNC(2UL), GPIO_PERICH_CH3},
        {GPIO_PERICH_SEL_PWMSEL_6, GPIO_GPK(14UL), GPIO_FUNC(2UL), GPIO_PERICH_CH3},
        {GPIO_PERICH_SEL_PWMSEL_7, GPIO_GPK(15UL), GPIO_FUNC(2UL), GPIO_PERICH_CH3},
        {GPIO_PERICH_SEL_PWMSEL_8, GPIO_GPK(16UL), GPIO_FUNC(2UL), GPIO_PERICH_CH3}};

/*
***************************************************************************************************
*                                         FUNCTION PROTOTYPES
***************************************************************************************************
*/
static SALRetCode_t PMM_EnableMonitoring(
    uint32 uiChannel,
    uint32 uiConnectionSelect,
    uint32 uiCommissionErrorCheck);

static SALRetCode_t PMM_SetConfig(
    uint32 uiChannel);

static SALRetCode_t PDM_WaitForIdleStatus(
    uint32 uiChannel);

/*
***************************************************************************************************
*                                          PDM_Delay1u
*
* Delay.
*
* @param
* @return
*
* Notes
*
***************************************************************************************************
*/
static void PDM_Delay1u(
    uint32 uiTime)
{
    uint32 uiDelayCnt = 0;

    for (uiDelayCnt = 0; uiDelayCnt < uiTime; uiDelayCnt++)
    {
        BSP_NOP_DELAY();
    }
}

/*
***************************************************************************************************
*                                          PDM_WaitForIdleStatus
*
* Wait for IDLE status
*
* @param    channel number 0 ~ 8.
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t PDM_WaitForIdleStatus(
    uint32 uiChannel)
{
    uint32 uiDelayCnt = 0x3FFFFUL;
    SALRetCode_t ret = SAL_RET_FAILED;

    if (uiChannel < PDM_OUT_CH_MAX)
    {
        while (uiDelayCnt > 0UL) /* QAC */
        {
            uiDelayCnt--;

            if (PDM_GetChannelStatus(uiChannel) == PDM_OFF)
            {
                break;
            }
        }

        ret = SAL_RET_SUCCESS;
    }

    return ret;
}

/******************************************************************************
 * PDM Mornitoring for Safety Features, PMM
 ******************************************************************************/
#ifdef PDM_SAFETY_FEATURE_ENABLED

/*
***************************************************************************************************
*                                           PMM_EnableProcess
*
* Monitoring system enable process

* @param    channel number 0 ~ 8.
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t PMM_EnableProcess(
    uint32 uiChannel)
{
    SALRetCode_t ret = SAL_RET_FAILED;

    if (uiChannel < PDM_OUT_CH_MAX)
    {
        if (PDM_GetChannelStatus(uiChannel) == PDM_ON)
        {
            (void)PDM_Disable(uiChannel, (uint32)PMM_ON);
            (void)PDM_WaitForIdleStatus(uiChannel);
        }

        (void)PMM_SetConfig(uiChannel);

        /* PMM_CNECT : Input Connetion Selection (Internal or External PDM loopback check) */
        ret = PMM_EnableMonitoring(uiChannel, (uint32)PMM_CONNECT_INTERNAL, (uint32)PMM_COMMISSION_ENABLE);
    }

    return ret;
}

/*
***************************************************************************************************
*                                           PMM_SetDutyMarginValue
*
* set noise filter value and duty margine value.

* @param    channel number 0 ~ 8.
* @param    noise filter value
* @param    duty margin value
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t PMM_SetDutyMarginValue(
    uint32 uiChannel,
    uint32 uiNoiseFilterValue,
    uint32 uiDutyMarginValue)
{
    uint32 uiVal = 0;
    uint32 uiReg = 0;
    uint32 uiModuleId = 0;
    uint32 uiChannelId = 0;
    SALRetCode_t ret = SAL_RET_FAILED;

    if (uiChannel < PDM_OUT_CH_MAX)
    {
        uiModuleId = PDMHandler[uiChannel].chModuleId;
        uiChannelId = PDMHandler[uiChannel].chChannelId;

        if ((uiModuleId < PDM_TOTAL_MODULES) && (uiChannelId < PDM_TOTAL_CH_PER_MODULE))
        {
            uiReg = PMM_BASE + (uiModuleId * PMM_MODULE_OFFSET) + PMM_GetPMMDutyMarginReg(uiChannelId);
            uiVal = (uiNoiseFilterValue << PMM_FLT_VAL_OFFSET) | (uiDutyMarginValue << PMM_DUTY_MARGIN_OFFSET);
            SAL_WriteReg(uiVal, uiReg);
            ret = SAL_RET_SUCCESS;
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                           PMM_SetTimeoutValue
*
* set pmm time out value, if the output of pdm is not generated for a certain period of time
* a fault has occured
* @param    channel number 0 ~ 8.
* @param    time-out value
* @return
* Notes     this value is counted on a divided pdm operating clock basis.
*
***************************************************************************************************
*/
static SALRetCode_t PMM_SetTimeoutValue(
    uint32 uiChannel,
    uint32 uiTimeoutValue)
{
    uint32 uiReg = 0;
    uint32 uiModuleId = 0;
    uint32 uiChannelId = 0;
    SALRetCode_t ret = SAL_RET_FAILED;

    if (uiChannel < PDM_OUT_CH_MAX)
    {
        uiModuleId = PDMHandler[uiChannel].chModuleId;
        uiChannelId = PDMHandler[uiChannel].chChannelId;

        if ((uiModuleId < PDM_TOTAL_MODULES) && (uiChannelId < PDM_TOTAL_CH_PER_MODULE))
        {
            uiReg = PMM_BASE + (uiModuleId * PMM_MODULE_OFFSET) + PMM_GetPMMTimeoutValueReg(uiChannelId);
            SAL_WriteReg(uiTimeoutValue, uiReg);
            ret = SAL_RET_SUCCESS;
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                           PMM_EnableMonitoring
*
* Enable PDM Monitoring Module for Safety
*
* @param    channel number 0 ~ 8.
* @param    input connection selection.
* @param    enable commission error check.
* @return
* Notes It is for safety feature.
*
***************************************************************************************************
*/
static SALRetCode_t PMM_EnableMonitoring(
    uint32 uiChannel,
    uint32 uiConnectionSelect,
    uint32 uiCommissionErrorCheck)
{
    uint32 uiVal = 0;
    uint32 uiModuleId = 0;
    uint32 uiChannelId = 0;
    uint32 uiPmmPdmEnReg = 0;
    uint32 uiPmmCtrlReg = 0;
    SALRetCode_t ret = SAL_RET_FAILED;

    if (uiChannel < PDM_OUT_CH_MAX)
    {
        uiModuleId = PDMHandler[uiChannel].chModuleId;
        uiChannelId = PDMHandler[uiChannel].chChannelId;

        if ((uiModuleId < PDM_TOTAL_MODULES) && (uiChannelId < PDM_TOTAL_CH_PER_MODULE))
        {
            /* Set PDM Monitoring Module Enable (PMM_CTRL) */
            uiPmmCtrlReg = PMM_BASE + PMM_CTRL_REG_OFFSET + (uiModuleId * PMM_MODULE_OFFSET);

            uiVal = SAL_ReadReg(uiPmmCtrlReg);

            /* Clear PMM_CNECT & PMM_CM_EN bit */
            uiVal &= ~(0x1UL << PMM_GetPMMCtrlPmmCNECTReg(uiChannelId));
            uiVal &= ~(0x1UL << PMM_GetPMMCtrlPmmCMENReg(uiChannelId));

            /* Set PMM_CNECT , Input connection selection */
            uiVal |= ((uiConnectionSelect > 0UL) ? ((uint32)PMM_ON) : ((uint32)PMM_OFF)) << (uint32)PMM_GetPMMCtrlPmmCNECTReg(uiChannelId);

            /* Set PMM_CM_EN , Commission error check enable */
            uiVal |= ((uiCommissionErrorCheck > 0UL) ? ((uint32)PMM_ON) : ((uint32)PMM_OFF)) << (uint32)PMM_GetPMMCtrlPmmCMENReg(uiChannelId);

            SAL_WriteReg(uiVal, uiPmmCtrlReg);

            /* Set PDM Mornitoring Module Operation Enable (PMM_PDMEN) */
            uiPmmPdmEnReg = PMM_BASE + PMM_PDMEN_REG_OFFSET + (uiModuleId * PMM_MODULE_OFFSET);

            /* Set PMM_EN enable (PMM_CTRL) */
            uiVal = SAL_ReadReg(uiPmmCtrlReg) | ((uint32)0x1UL << (PMM_GetPMMCtrlPmmEnReg(uiChannelId)));
            SAL_WriteReg(uiVal, uiPmmCtrlReg);

            /* Set PMM_PDM_EN enable (PMM_PDMEN) */
            uiVal = SAL_ReadReg(uiPmmPdmEnReg) | ((uint32)0x1UL << (PMM_GetPDMENEnableReg(uiChannelId)));
            SAL_WriteReg(uiVal, uiPmmPdmEnReg);

            /* Set PMM_TRIG enable (PMM_CTRL) */
            uiVal = SAL_ReadReg(uiPmmCtrlReg) | ((uint32)0x1UL << (PMM_GetPMMCtrlPmmTRIGReg(uiChannelId)));
            SAL_WriteReg(uiVal, uiPmmCtrlReg);

            PDMHandler[uiChannel].chPMMEnable = PMM_ON;

            ret = SAL_RET_SUCCESS;
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                           PMM_DisableMonitoring
*
* Disable PDM Monitoring Module.
*
* @param    channel number 0 ~ 8.
* @return
* Notes It is for safety feature.
*
***************************************************************************************************
*/
static SALRetCode_t PMM_DisableMonitoring(
    uint32 uiChannel)
{
    uint32 uiVal = 0;
    uint32 uiModuleId = 0;
    uint32 uiChannelId = 0;
    uint32 uiPmmPdmEnReg = 0;
    uint32 uiPmmCtrlReg = 0;
    SALRetCode_t ret = SAL_RET_FAILED;

    if (uiChannel < PDM_OUT_CH_MAX)
    {
        uiModuleId = PDMHandler[uiChannel].chModuleId;
        uiChannelId = PDMHandler[uiChannel].chChannelId;

        if ((uiModuleId < PDM_TOTAL_MODULES) && (uiChannelId < PDM_TOTAL_CH_PER_MODULE))
        {
            /* clear pmm_ctrl pdm monitoring module enable register */
            uiPmmCtrlReg = PMM_BASE + PMM_CTRL_REG_OFFSET + (uiModuleId * PMM_MODULE_OFFSET);
            uiVal = SAL_ReadReg(uiPmmCtrlReg) & ~((uint32)0x1UL << (PMM_GetPMMCtrlPmmEnReg(uiChannelId)));
            SAL_WriteReg(uiVal, uiPmmCtrlReg);

            /* clear pmm_pdmen pmm operation enable register */
            uiPmmPdmEnReg = PMM_BASE + PMM_PDMEN_REG_OFFSET + (uiModuleId * PMM_MODULE_OFFSET);
            uiVal = SAL_ReadReg(uiPmmPdmEnReg) & ~((uint32)0x1UL << (PMM_GetPDMENEnableReg(uiChannelId)));
            SAL_WriteReg(uiVal, uiPmmPdmEnReg);

            PDMHandler[uiChannel].chPMMEnable = PMM_OFF;

            ret = SAL_RET_SUCCESS;
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                           PMM_SetConfig
*
* Set Configuration value for PMM
*
* @param    channel number 0 ~ 8.
* @return
* Notes     every config value should be same as PDM's.
*
***************************************************************************************************
*/
static SALRetCode_t PMM_SetConfig(
    uint32 uiChannel)
{
    uint32 uiVal = 0;
    uint32 uiModuleId = 0;
    uint32 uiChannelId = 0;

    uint32 uiOpModeReg = 0;
    uint32 uiPstn1Reg = 0;
    uint32 uiPstn2Reg = 0;
    uint32 uiPstn3Reg = 0;
    uint32 uiPstn4Reg = 0;
    uint32 uiOutPtn1Reg = 0;
    uint32 uiOutPtn2Reg = 0;
    uint32 uiOutPtn3Reg = 0;
    uint32 uiOutPtn4Reg = 0;
    uint32 uiMaxCntReg = 0;

    SALRetCode_t ret = SAL_RET_FAILED;

    if (uiChannel < PDM_OUT_CH_MAX)
    {
        uiModuleId = PDMHandler[uiChannel].chModuleId;
        uiChannelId = PDMHandler[uiChannel].chChannelId;

        if ((uiModuleId < PDM_TOTAL_MODULES) && (uiChannelId < PDM_TOTAL_CH_PER_MODULE))
        {
            (void)PMM_SetDutyMarginValue(uiChannel, 0x00U, PDMHandler[uiChannel].chPMMDutyMargin);
            (void)PMM_SetTimeoutValue(uiChannel, PDMHandler[uiChannel].chPMMTimeoutValue);

            uiOpModeReg = PMM_BASE + PDM_OP_MODE_REG_OFFSET + (uiModuleId * PMM_MODULE_OFFSET);
            uiVal = SAL_ReadReg(uiOpModeReg);

            /* PDM configurations and Monitoring Module configurations must be same (BASE + 0x08 ~ 0x93)*/

            /* Set PDMM Mode Register (OP_MODE) */
            /* operation mode (MODE_X) */
            uiVal = (uiVal & ~(0xFUL << PDM_GetOPModeOperationModeReg(uiChannelId))) |
                    (uint32)(PDMHandler[uiChannel].chModeCfgInfo.mcOperationMode << PDM_GetOPModeOperationModeReg(uiChannelId));

            /* Output signal inverse (INV_X) */
            uiVal = ((uiVal & ~(0x1UL << PDM_GetOPModeSignalInverseReg(uiChannelId))) |
                     ((uint32)(PDMHandler[uiChannel].chModeCfgInfo.mcInversedSignal << PDM_GetOPModeSignalInverseReg(uiChannelId))));

            /* Output signal in idle state only for phase mode 1 & 2 (IDLE_X) */
            uiVal = ((uiVal & ~(0x1UL << PDM_GetOPModeOutputIdleReg(uiChannelId))) |
                     ((uint32)(PDMHandler[uiChannel].chOutSignalInIdle << PDM_GetOPModeOutputIdleReg(uiChannelId))));

            /* Input clock divide (DIV_X) */
            uiVal = ((uiVal & ~(0x3UL << PDM_GetOPModeClockDivideReg(uiChannelId))) |
                     (uint32)(PDMHandler[uiChannel].chModeCfgInfo.mcClockDivide << PDM_GetOPModeClockDivideReg(uiChannelId)));

            SAL_WriteReg(uiVal, uiOpModeReg);

            /* Set Output count or pattern value according to PDM_OUTPUT_MODE */
            switch (PDMHandler[uiChannel].chModeCfgInfo.mcOperationMode)
            {
            case PDM_OUTPUT_MODE_PHASE_1:
                uiPstn1Reg = PMM_BASE + (uiModuleId * PMM_MODULE_OFFSET) + PDM_GetPSTN1Reg(uiChannelId);
                uiPstn2Reg = PMM_BASE + (uiModuleId * PMM_MODULE_OFFSET) + PDM_GetPSTN2Reg(uiChannelId);

                SAL_WriteReg(PDMHandler[uiChannel].chModeCfgInfo.mcPosition1, uiPstn1Reg);
                SAL_WriteReg(PDMHandler[uiChannel].chModeCfgInfo.mcPosition2, uiPstn2Reg);
                break;

            case PDM_OUTPUT_MODE_PHASE_2:
                uiPstn1Reg = PMM_BASE + (uiModuleId * PMM_MODULE_OFFSET) + PDM_GetPSTN1Reg(uiChannelId);
                uiPstn2Reg = PMM_BASE + (uiModuleId * PMM_MODULE_OFFSET) + PDM_GetPSTN2Reg(uiChannelId);
                uiPstn3Reg = PMM_BASE + (uiModuleId * PMM_MODULE_OFFSET) + PDM_GetPSTN3Reg(uiChannelId);
                uiPstn4Reg = PMM_BASE + (uiModuleId * PMM_MODULE_OFFSET) + PDM_GetPSTN4Reg(uiChannelId);

                SAL_WriteReg(PDMHandler[uiChannel].chModeCfgInfo.mcPosition1, uiPstn1Reg);
                SAL_WriteReg(PDMHandler[uiChannel].chModeCfgInfo.mcPosition2, uiPstn2Reg);
                SAL_WriteReg(PDMHandler[uiChannel].chModeCfgInfo.mcPosition3, uiPstn3Reg);
                SAL_WriteReg(PDMHandler[uiChannel].chModeCfgInfo.mcPosition4, uiPstn4Reg);
                break;

            case PDM_OUTPUT_MODE_REGISTER_1:
                uiOutPtn1Reg = PMM_BASE + (uiModuleId * PMM_MODULE_OFFSET) + PDM_GetOutPTN1Reg(uiChannelId);

                SAL_WriteReg(PDMHandler[uiChannel].chModeCfgInfo.mcOutPattern1, uiOutPtn1Reg);
                break;

            case PDM_OUTPUT_MODE_REGISTER_2:
                uiOutPtn1Reg = PMM_BASE + (uiModuleId * PMM_MODULE_OFFSET) + PDM_GetOutPTN1Reg(uiChannelId);
                uiOutPtn2Reg = PMM_BASE + (uiModuleId * PMM_MODULE_OFFSET) + PDM_GetOutPTN2Reg(uiChannelId);
                uiOutPtn3Reg = PMM_BASE + (uiModuleId * PMM_MODULE_OFFSET) + PDM_GetOutPTN3Reg(uiChannelId);
                uiOutPtn4Reg = PMM_BASE + (uiModuleId * PMM_MODULE_OFFSET) + PDM_GetOutPTN4Reg(uiChannelId);

                SAL_WriteReg(PDMHandler[uiChannel].chModeCfgInfo.mcOutPattern1, uiOutPtn1Reg);
                SAL_WriteReg(PDMHandler[uiChannel].chModeCfgInfo.mcOutPattern2, uiOutPtn2Reg);
                SAL_WriteReg(PDMHandler[uiChannel].chModeCfgInfo.mcOutPattern3, uiOutPtn3Reg);
                SAL_WriteReg(PDMHandler[uiChannel].chModeCfgInfo.mcOutPattern4, uiOutPtn4Reg);
                break;

            case PDM_OUTPUT_MODE_REGISTER_3:
                uiOutPtn1Reg = PMM_BASE + (uiModuleId * PMM_MODULE_OFFSET) + PDM_GetOutPTN1Reg(uiChannelId);
                uiOutPtn2Reg = PMM_BASE + (uiModuleId * PMM_MODULE_OFFSET) + PDM_GetOutPTN2Reg(uiChannelId);
                uiOutPtn3Reg = PMM_BASE + (uiModuleId * PMM_MODULE_OFFSET) + PDM_GetOutPTN3Reg(uiChannelId);
                uiOutPtn4Reg = PMM_BASE + (uiModuleId * PMM_MODULE_OFFSET) + PDM_GetOutPTN4Reg(uiChannelId);
                uiMaxCntReg = PMM_BASE + PDM_MAX_CNT_REG_OFFSET + (uiModuleId * PMM_MODULE_OFFSET);

                SAL_WriteReg(PDMHandler[uiChannel].chModeCfgInfo.mcOutPattern1, uiOutPtn1Reg);
                SAL_WriteReg(PDMHandler[uiChannel].chModeCfgInfo.mcOutPattern2, uiOutPtn2Reg);
                SAL_WriteReg(PDMHandler[uiChannel].chModeCfgInfo.mcOutPattern3, uiOutPtn3Reg);
                SAL_WriteReg(PDMHandler[uiChannel].chModeCfgInfo.mcOutPattern4, uiOutPtn4Reg);

                uiVal = SAL_ReadReg(uiMaxCntReg);
                uiVal = (uiVal & ~(0xFFUL << PDM_GetMaxCountValueReg(uiChannelId))) |
                        (uint32)(PDMHandler[uiChannel].chModeCfgInfo.mcMaxCount << PDM_GetMaxCountValueReg(uiChannelId));

                SAL_WriteReg(uiVal, uiMaxCntReg);
                break;

            default:
                PDM_Err("PDM Invalid Operation Mode \n");
                ret = SAL_RET_FAILED;
                break;
            }
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                           PMM_PDM0IrqHandler
*
* PMM PDM0 ISR handler
*
* @param    user data argument
* @return
* Notes
*
***************************************************************************************************
*/
static void PMM_PDM0IrqHandler(
    void *pArg)
{
    uint32 uiReg = 0;
    uint32 uiVal = 0;

    (void)pArg;

    uiReg = PMM0_BASE + PMM_FAULT_STS_REG_OFFSET;
    uiVal = SAL_ReadReg(uiReg);
    (void)FMU_IsrClr((FMUFaultid_t)FMU_ID_PDM0);

    if ((uiVal & PMM_FAULT_STS_CH_A_MASK) != (uint32)NULL)
    {
        PDMHandler[0].chPMMErrChannel = TRUE;
        SAL_WriteReg(PMM_FAULT_STS_CH_A_MASK, uiReg);
    }

    if ((uiVal & PMM_FAULT_STS_CH_B_MASK) != (uint32)NULL)
    {
        PDMHandler[1].chPMMErrChannel = TRUE;
        SAL_WriteReg(PMM_FAULT_STS_CH_B_MASK, uiReg);
    }

    if ((uiVal & PMM_FAULT_STS_CH_C_MASK) != (uint32)NULL)
    {
        PDMHandler[2].chPMMErrChannel = TRUE;
        SAL_WriteReg(PMM_FAULT_STS_CH_C_MASK, uiReg);
    }

    if ((uiVal & PMM_FAULT_STS_CH_D_MASK) != (uint32)NULL)
    {
        PDMHandler[3].chPMMErrChannel = TRUE;
        SAL_WriteReg(PMM_FAULT_STS_CH_D_MASK, uiReg);
    }
}

/*
***************************************************************************************************
*                                           PMM_PDM1IrqHandler
*
* PMM PDM1 ISR handler
*
* @param    user data argument
* @return
* Notes
*
***************************************************************************************************
*/
static void PMM_PDM1IrqHandler(
    void *pArg)
{
    uint32 uiReg = 0;
    uint32 uiVal = 0;

    (void)pArg;

    uiReg = PMM1_BASE + PMM_FAULT_STS_REG_OFFSET;
    uiVal = SAL_ReadReg(uiReg);
    (void)FMU_IsrClr((FMUFaultid_t)FMU_ID_PDM1);

    if ((uiVal & PMM_FAULT_STS_CH_A_MASK) != (uint32)NULL)
    {
        PDMHandler[4].chPMMErrChannel = TRUE;
        SAL_WriteReg(PMM_FAULT_STS_CH_A_MASK, uiReg);
    }

    if ((uiVal & PMM_FAULT_STS_CH_B_MASK) != (uint32)NULL)
    {
        PDMHandler[5].chPMMErrChannel = TRUE;
        SAL_WriteReg(PMM_FAULT_STS_CH_B_MASK, uiReg);
    }

    if ((uiVal & PMM_FAULT_STS_CH_C_MASK) != (uint32)NULL)
    {
        PDMHandler[6].chPMMErrChannel = TRUE;
        SAL_WriteReg(PMM_FAULT_STS_CH_C_MASK, uiReg);
    }

    if ((uiVal & PMM_FAULT_STS_CH_D_MASK) != (uint32)NULL)
    {
        PDMHandler[7].chPMMErrChannel = TRUE;
        SAL_WriteReg(PMM_FAULT_STS_CH_D_MASK, uiReg);
    }
}

/*
***************************************************************************************************
*                                           PMM_PDM2IrqHandler
*
* PMM PDM2 ISR handler
*
* @param    user data argument
* @return
* Notes
*
***************************************************************************************************
*/
static void PMM_PDM2IrqHandler(
    void *pArg)
{
    uint32 uiReg = 0;
    uint32 uiVal = 0;

    (void)pArg;

    uiReg = PMM2_BASE + PMM_FAULT_STS_REG_OFFSET;
    uiVal = SAL_ReadReg(uiReg);
    (void)FMU_IsrClr((FMUFaultid_t)FMU_ID_PDM2);

    if ((uiVal & PMM_FAULT_STS_CH_A_MASK) != (uint32)NULL)
    {
        PDMHandler[8].chPMMErrChannel = TRUE;
        SAL_WriteReg(PMM_FAULT_STS_CH_A_MASK, uiReg);
    }

    if ((uiVal & PMM_FAULT_STS_CH_B_MASK) != (uint32)NULL)
    {
        PDMHandler[9].chPMMErrChannel = TRUE;
        SAL_WriteReg(PMM_FAULT_STS_CH_B_MASK, uiReg);
    }

    if ((uiVal & PMM_FAULT_STS_CH_C_MASK) != (uint32)NULL)
    {
        PDMHandler[10].chPMMErrChannel = TRUE;
        SAL_WriteReg(PMM_FAULT_STS_CH_C_MASK, uiReg);
    }

    if ((uiVal & PMM_FAULT_STS_CH_D_MASK) != (uint32)NULL)
    {
        PDMHandler[11].chPMMErrChannel = TRUE;
        SAL_WriteReg(PMM_FAULT_STS_CH_D_MASK, uiReg);
    }
}
#endif

/*
***************************************************************************************************
*                                       INTERNAL FUNCTION
***************************************************************************************************
*/

/*
***************************************************************************************************
*                                           PDM_SetOutputInverse
*
* Enable output signal Inverse

* @param    channel number 0 ~ 8.
* @param    0: normal , 1: inverse
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t PDM_SetOutputInverse(
    uint32 uiChannel,
    uint32 uiOutputInverse)
{
    uint32 uiVal = 0;
    uint32 uiModuleId = 0;
    uint32 uiChannelId = 0;
    uint32 uiOpModeReg = 0;
    SALRetCode_t ret = SAL_RET_FAILED;

    if (uiChannel < PDM_OUT_CH_MAX)
    {
        uiModuleId = PDMHandler[uiChannel].chModuleId;
        uiChannelId = PDMHandler[uiChannel].chChannelId;

        if ((uiModuleId < PDM_TOTAL_MODULES) && (uiChannelId < PDM_TOTAL_CH_PER_MODULE))
        {
            PDMHandler[uiChannel].chModeCfgInfo.mcInversedSignal = uiOutputInverse;

            uiOpModeReg = PDM_BASE + PDM_OP_MODE_REG_OFFSET + (uiModuleId * PDM_MODULE_OFFSET);
            uiVal = SAL_ReadReg(uiOpModeReg);

            /*output_signal, INV_X is set, output signal inversed*/
            uiVal = ((uiVal & ~(0x1UL << PDM_GetOPModeSignalInverseReg(uiChannelId))) |
                     (uiOutputInverse << PDM_GetOPModeSignalInverseReg(uiChannelId)));

            SAL_WriteReg(uiVal, uiOpModeReg);

            ret = SAL_RET_SUCCESS;
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          PDM_SetOutputLoopCount
*
* Set PDM Ouput Loop Count
*
* @param    channel number 0 ~ 8.
* @param    if repetition_loop value is '0' PDM-X operate infinite value mode, defult '0'
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t PDM_SetOutputLoopCount(
    uint32 uiChannel,
    uint32 uiLoopCount)
{
    uint32 uiVal = 0;
    uint32 uiModuleId = 0;
    uint32 uiChannelId = 0;
    uint32 uiOpStepReg = 0;
    SALRetCode_t ret = SAL_RET_FAILED;

    if (uiChannel < PDM_OUT_CH_MAX)
    {
        uiModuleId = PDMHandler[uiChannel].chModuleId;
        uiChannelId = PDMHandler[uiChannel].chChannelId;

        if ((uiModuleId < PDM_TOTAL_MODULES) && (uiChannelId < PDM_TOTAL_CH_PER_MODULE))
        {
            PDMHandler[uiChannel].chModeCfgInfo.mcLoopCount = uiLoopCount;

            uiOpStepReg = PDM_BASE + PDM_OP_STEP_REG_OFFSET + (uiModuleId * PDM_MODULE_OFFSET);
            uiVal = SAL_ReadReg(uiOpStepReg);

            uiVal = uiVal & ~(0xFUL << (PDM_GetOPStepLoopCountReg(uiChannelId)));
            uiVal = uiVal | (uiLoopCount << (PDM_GetOPStepLoopCountReg(uiChannelId)));
            SAL_WriteReg(uiVal, uiOpStepReg);

            ret = SAL_RET_SUCCESS;
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                           PDM_SetOutputInIdleState
*
* Enable output signal in Idle state

* @param    channel number 0 ~ 8.
* @param    0: low , 1: high
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t PDM_SetOutputInIdleState(
    uint32 uiChannel,
    uint32 uiOutputState)
{
    uint32 uiVal = 0;
    uint32 uiModuleId = 0;
    uint32 uiChannelId = 0;
    uint32 uiOpModeReg = 0;
    SALRetCode_t ret = SAL_RET_FAILED;

    if (uiChannel < PDM_OUT_CH_MAX)
    {
        uiModuleId = PDMHandler[uiChannel].chModuleId;
        uiChannelId = PDMHandler[uiChannel].chChannelId;

        if ((uiModuleId < PDM_TOTAL_MODULES) && (uiChannelId < PDM_TOTAL_CH_PER_MODULE))
        {
            PDMHandler[uiChannel].chOutSignalInIdle = uiOutputState;

            uiOpModeReg = PDM_BASE + PDM_OP_MODE_REG_OFFSET + (uiModuleId * PDM_MODULE_OFFSET);
            uiVal = SAL_ReadReg(uiOpModeReg);

            /*output_signal, IDLE_X is set, output signal in Idle state */
            uiVal = ((uiVal & ~(0x1UL << PDM_GetOPModeOutputIdleReg(uiChannelId))) |
                     (uiOutputState << PDM_GetOPModeOutputIdleReg(uiChannelId)));

            SAL_WriteReg(uiVal, uiOpModeReg);

            ret = SAL_RET_SUCCESS;
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                           PDM_SetCfgOutputControl
*
* set output control

* @param    channel number 0 ~ 8.
* @param    output control value
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t PDM_SetCfgOutputControl(
    uint32 uiChannel,
    uint32 uiOutputCtrl)
{
    uint32 uiReg = 0;
    uint32 uiVal = 0;
    uint32 uiModuleId = 0;
    uint32 uiChannelId = 0;
    SALRetCode_t ret = SAL_RET_FAILED;

    if (uiChannel < PDM_OUT_CH_MAX)
    {
        uiModuleId = PDMHandler[uiChannel].chModuleId;
        uiChannelId = PDMHandler[uiChannel].chChannelId;

        if ((uiModuleId < PDM_TOTAL_MODULES) && (uiChannelId < PDM_TOTAL_CH_PER_MODULE))
        {
            uiReg = PDM_CFG_BASE + PDM_GetCfgOutCtrlReg(uiModuleId);
            uiVal = SAL_ReadReg(uiReg);

            uiVal &= ~((0xFFUL) << PDM_GetCfgOutCtrlShift(uiChannelId));
            uiVal |= (uiOutputCtrl << PDM_GetCfgOutCtrlShift(uiChannelId));

            SAL_WriteReg(uiVal, uiReg);

            ret = SAL_RET_SUCCESS;
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                           PDM_GetCfgOutputControl
*
* get output control value

* @param    channel number 0 ~ 8.
* @return
*
* Notes
*
***************************************************************************************************
*/
static uint8 PDM_GetCfgOutputControl(
    uint32 uiChannel)
{
    uint8 ucRet = 0;
    uint32 uiReg = 0;
    uint32 uiVal = 0;
    uint32 uiModuleId = 0;
    uint32 uiChannelId = 0;

    if (uiChannel < PDM_OUT_CH_MAX)
    {
        uiModuleId = PDMHandler[uiChannel].chModuleId;
        uiChannelId = PDMHandler[uiChannel].chChannelId;

        if ((uiModuleId < PDM_TOTAL_MODULES) && (uiChannelId < PDM_TOTAL_CH_PER_MODULE))
        {
            uiReg = PDM_CFG_BASE + PDM_GetCfgOutCtrlReg(uiModuleId);
            uiVal = SAL_ReadReg(uiReg);
            ucRet = (uint8)((uiVal >> PDM_GetCfgOutCtrlShift(uiChannelId)) & 0xFU);
        }
    }

    return ucRet;
}

/*
***************************************************************************************************
*                                          PDM_SetRegisterMode
*
* Set register mode configuration according to mode number (pattern value, clock divide, MAX value)
*
* @param    channel number 0 ~ 8.
* @param    Operation Mode Configure Information.
* @return
*
* Notes set register mode, use 4 register, (4 * 32) bits in each PDM clock cycles,
        PDM generates PDM output signal by reading all bits of 1 register.
*
***************************************************************************************************
*/
static SALRetCode_t PDM_SetRegisterMode(
    uint32 uiChannel,
    const PDMModeConfig_t *pModeConfig)
{
    uint32 uiVal = 0;
    uint32 uiModuleId = 0;
    uint32 uiChannelId = 0;
    uint32 uiOpModeVal = 0;
    uint32 uiOpModeReg = 0;
    uint32 uiOutPtn1Reg = 0;
    uint32 uiOutPtn2Reg = 0;
    uint32 uiOutPtn3Reg = 0;
    uint32 uiOutPtn4Reg = 0;
    uint32 uiMaxCntReg = 0;
    uint32 uiClkDivide = 0;
    uint32 uiOutPtnValue1 = 0;
    uint32 uiOutPtnValue2 = 0;
    uint32 uiOutPtnValue3 = 0;
    uint32 uiOutPtnValue4 = 0;
    uint32 uiMaxCnt = 0;
    SALRetCode_t ret = SAL_RET_SUCCESS;

    if (PDM_OUT_CH_MAX <= uiChannel)
    {
        PDM_Err("Invalid channel number \n");
        ret = SAL_RET_FAILED;
    }
    else if (pModeConfig == (PDMModeConfig_t *)NULL)
    {
        PDM_Err("Invalid configuration pointer \n");
        ret = SAL_RET_FAILED;
    }
    else
    {
        uiModuleId = PDMHandler[uiChannel].chModuleId;
        uiChannelId = PDMHandler[uiChannel].chChannelId;

        if ((uiModuleId < PDM_TOTAL_MODULES) && (uiChannelId < PDM_TOTAL_CH_PER_MODULE))
        {

            uiOpModeReg = PDM_BASE + PDM_OP_MODE_REG_OFFSET + (uiModuleId * PDM_MODULE_OFFSET);

            uiOutPtn1Reg = PDM_BASE + (uiModuleId * PDM_MODULE_OFFSET) + PDM_GetOutPTN1Reg(uiChannelId);
            uiOutPtn2Reg = PDM_BASE + (uiModuleId * PDM_MODULE_OFFSET) + PDM_GetOutPTN2Reg(uiChannelId);
            uiOutPtn3Reg = PDM_BASE + (uiModuleId * PDM_MODULE_OFFSET) + PDM_GetOutPTN3Reg(uiChannelId);
            uiOutPtn4Reg = PDM_BASE + (uiModuleId * PDM_MODULE_OFFSET) + PDM_GetOutPTN4Reg(uiChannelId);
            uiMaxCntReg = PDM_BASE + PDM_MAX_CNT_REG_OFFSET + (uiModuleId * PDM_MODULE_OFFSET);

            uiVal = SAL_ReadReg(uiOpModeReg);

            uiOpModeVal = pModeConfig->mcOperationMode;
            uiClkDivide = pModeConfig->mcClockDivide;
            uiOutPtnValue1 = pModeConfig->mcOutPattern1;
            uiOutPtnValue2 = pModeConfig->mcOutPattern2;
            uiOutPtnValue3 = pModeConfig->mcOutPattern3;
            uiOutPtnValue4 = pModeConfig->mcOutPattern4;
            uiMaxCnt = pModeConfig->mcMaxCount;

            if ((((uiVal >> PDM_GetOPModeOperationModeReg(uiChannelId)) & 0xFU) != (uiOpModeVal)) &&
                (PDMHandler[uiChannel].chEnable == PDM_ON))
            {
                (void)PDM_Disable(uiChannel, PMM_ON);
                (void)PDM_WaitForIdleStatus(uiChannel);
            }
            PDMHandler[uiChannel].chModeCfgInfo.mcOperationMode = uiOpModeVal;
            PDMHandler[uiChannel].chModeCfgInfo.mcClockDivide = uiClkDivide;
            PDMHandler[uiChannel].chModeCfgInfo.mcOutPattern1 = uiOutPtnValue1;
            PDMHandler[uiChannel].chModeCfgInfo.mcOutPattern2 = uiOutPtnValue2;
            PDMHandler[uiChannel].chModeCfgInfo.mcOutPattern3 = uiOutPtnValue3;
            PDMHandler[uiChannel].chModeCfgInfo.mcOutPattern4 = uiOutPtnValue4;
            PDMHandler[uiChannel].chModeCfgInfo.mcMaxCount = uiMaxCnt;

            /* operation mode */
            uiVal = (uiVal & ~((uint32)0xFU << PDM_GetOPModeOperationModeReg(uiChannelId))) |
                    ((uint32)uiOpModeVal << PDM_GetOPModeOperationModeReg(uiChannelId));

            /* clk divide */
            uiVal = ((uiVal & ~((uint32)0x3U << PDM_GetOPModeClockDivideReg(uiChannelId))) |
                     (uiClkDivide << PDM_GetOPModeClockDivideReg(uiChannelId)));

            SAL_WriteReg(uiVal, uiOpModeReg);

            /* output pattern register */
            SAL_WriteReg(uiOutPtnValue1, uiOutPtn1Reg);
            SAL_WriteReg(uiOutPtnValue2, uiOutPtn2Reg);
            SAL_WriteReg(uiOutPtnValue3, uiOutPtn3Reg);
            SAL_WriteReg(uiOutPtnValue4, uiOutPtn4Reg);

            /* output data maximum count register, MAX_CNT, 7bits */
            uiVal = SAL_ReadReg(uiMaxCntReg);

            uiVal = (uiVal & ~((uint32)0xFFU << PDM_GetMaxCountValueReg(uiChannelId))) |
                    (uiMaxCnt << PDM_GetMaxCountValueReg(uiChannelId));

            SAL_WriteReg(uiVal, uiMaxCntReg);
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          PDM_SetPhaseMode1
*
* set phase mode 1, PSTN_1 and PSTN_2
*
* @param    channel number 0 ~ 8.
* @param    clock divide value 0 ~ 3
* @param    PSTN_1 value.
* @param    PSTN_2 value.
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t PDM_SetPhaseMode1(
    uint32 uiChannel,
    uint32 uiClockDivide,
    uint32 uiPositionValue1,
    uint32 uiPositionValue2)
{
    uint32 uiVal = 0;
    uint32 uiModuleId = 0;
    uint32 uiChannelId = 0;
    uint32 uiOpModeReg = 0;
    uint32 uiPstn1Reg = 0; /* low position */
    uint32 uiPstn2Reg = 0; /* high position */
    SALRetCode_t ret = SAL_RET_FAILED;

    if (uiChannel < PDM_OUT_CH_MAX)
    {
        uiModuleId = PDMHandler[uiChannel].chModuleId;
        uiChannelId = PDMHandler[uiChannel].chChannelId;

        if ((uiModuleId < PDM_TOTAL_MODULES) && (uiChannelId < PDM_TOTAL_CH_PER_MODULE))
        {
            uiOpModeReg = PDM_BASE + PDM_OP_MODE_REG_OFFSET + (uiModuleId * PDM_MODULE_OFFSET);
            uiPstn1Reg = PDM_BASE + (uiModuleId * PDM_MODULE_OFFSET) + PDM_GetPSTN1Reg(uiChannelId);
            uiPstn2Reg = PDM_BASE + (uiModuleId * PDM_MODULE_OFFSET) + PDM_GetPSTN2Reg(uiChannelId);
            uiVal = SAL_ReadReg(uiOpModeReg);

            if ((((uiVal >> PDM_GetOPModeOperationModeReg(uiChannelId)) & 0xFU) != PDM_OUTPUT_MODE_PHASE_1) &&
                (PDMHandler[uiChannel].chEnable == PDM_ON))
            {
                (void)PDM_Disable(uiChannel, PMM_ON);
                (void)PDM_WaitForIdleStatus(uiChannel);
            }

            PDMHandler[uiChannel].chModeCfgInfo.mcClockDivide = uiClockDivide;
            PDMHandler[uiChannel].chModeCfgInfo.mcOperationMode = PDM_OUTPUT_MODE_PHASE_1;
            PDMHandler[uiChannel].chModeCfgInfo.mcPosition1 = uiPositionValue1;
            PDMHandler[uiChannel].chModeCfgInfo.mcPosition2 = uiPositionValue2;

            /* operation mode phase 1 */
            uiVal = (uiVal & ~((uint32)0xFU << PDM_GetOPModeOperationModeReg(uiChannelId))) |
                    ((uint32)PDM_OUTPUT_MODE_PHASE_1 << PDM_GetOPModeOperationModeReg(uiChannelId));

            SAL_WriteReg(uiVal, uiOpModeReg);

            /* divide clock,  default value by 2 */
            uiVal = SAL_ReadReg(uiOpModeReg);
            uiVal = ((uiVal & ~((uint32)0x3U << PDM_GetOPModeClockDivideReg(uiChannelId))) |
                     (uiClockDivide << PDM_GetOPModeClockDivideReg(uiChannelId)));

            SAL_WriteReg(uiVal, uiOpModeReg);

            /* pahse mode position register */
            SAL_WriteReg((uint32)uiPositionValue1, uiPstn1Reg);
            SAL_WriteReg((uint32)uiPositionValue2, uiPstn2Reg);

            ret = SAL_RET_SUCCESS;
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          PDM_SetPhaseMode2
*
* set phase mode 1, PSTN_1, PSTN_2, PSTN_3 and PSTN_4
*
* @param    channel number 0 ~ 8.
* @param    clock divide value 0 ~ 3
* @param    PSTN_1 value.
* @param    PSTN_2 value.
* @param    PSTN_3 value.
* @param    PSTN_4 value.
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t PDM_SetPhaseMode2(
    uint32 uiChannel,
    uint32 uiClockDivide,
    uint32 uiPositionValue1,
    uint32 uiPositionValue2,
    uint32 uiPositionValue3,
    uint32 uiPositionValue4)
{
    uint32 uiVal = 0;
    uint32 uiModuleId = 0;
    uint32 uiChannelId = 0;
    uint32 uiOpModeReg = 0;
    uint32 uiPstn1Reg = 0; /* low position */
    uint32 uiPstn2Reg = 0; /* high position */
    uint32 uiPstn3Reg = 0; /* low position-2*/
    uint32 uiPstn4Reg = 0; /* high position-2 */
    SALRetCode_t ret = SAL_RET_FAILED;

    if (uiChannel < PDM_OUT_CH_MAX)
    {
        uiModuleId = PDMHandler[uiChannel].chModuleId;
        uiChannelId = PDMHandler[uiChannel].chChannelId;

        if ((uiModuleId < PDM_TOTAL_MODULES) && (uiChannelId < PDM_TOTAL_CH_PER_MODULE))
        {
            uiOpModeReg = PDM_BASE + PDM_OP_MODE_REG_OFFSET + (uiModuleId * PDM_MODULE_OFFSET);
            uiPstn1Reg = PDM_BASE + (uiModuleId * PDM_MODULE_OFFSET) + PDM_GetPSTN1Reg(uiChannelId);
            uiPstn2Reg = PDM_BASE + (uiModuleId * PDM_MODULE_OFFSET) + PDM_GetPSTN2Reg(uiChannelId);
            uiPstn3Reg = PDM_BASE + (uiModuleId * PDM_MODULE_OFFSET) + PDM_GetPSTN3Reg(uiChannelId);
            uiPstn4Reg = PDM_BASE + (uiModuleId * PDM_MODULE_OFFSET) + PDM_GetPSTN4Reg(uiChannelId);
            uiVal = SAL_ReadReg(uiOpModeReg);

            if ((((uiVal >> PDM_GetOPModeOperationModeReg(uiChannelId)) & 0xFU) != PDM_OUTPUT_MODE_PHASE_2) &&
                (PDMHandler[uiChannel].chEnable == PDM_ON))
            {
                (void)PDM_Disable(uiChannel, PMM_ON);
                (void)PDM_WaitForIdleStatus(uiChannel);
            }

            PDMHandler[uiChannel].chModeCfgInfo.mcClockDivide = uiClockDivide;
            PDMHandler[uiChannel].chModeCfgInfo.mcOperationMode = PDM_OUTPUT_MODE_PHASE_2;
            PDMHandler[uiChannel].chModeCfgInfo.mcPosition1 = uiPositionValue1;
            PDMHandler[uiChannel].chModeCfgInfo.mcPosition2 = uiPositionValue2;
            PDMHandler[uiChannel].chModeCfgInfo.mcPosition3 = uiPositionValue3;
            PDMHandler[uiChannel].chModeCfgInfo.mcPosition4 = uiPositionValue4;

            /* operation mode phase 2 */
            uiVal = (uiVal & ~((uint32)0xFU << PDM_GetOPModeOperationModeReg(uiChannelId))) |
                    ((uint32)PDM_OUTPUT_MODE_PHASE_2 << PDM_GetOPModeOperationModeReg(uiChannelId));
            SAL_WriteReg(uiVal, uiOpModeReg);

            /* divide by 2 : default value */
            uiVal = SAL_ReadReg(uiOpModeReg);
            uiVal = ((uiVal & ~((uint32)0x3U << PDM_GetOPModeClockDivideReg(uiChannelId))) |
                     (uiClockDivide << PDM_GetOPModeClockDivideReg(uiChannelId)));
            SAL_WriteReg(uiVal, uiOpModeReg);

            /* pahse mode position register */
            SAL_WriteReg((uint32)uiPositionValue1, uiPstn1Reg);
            SAL_WriteReg((uint32)uiPositionValue2, uiPstn2Reg);
            SAL_WriteReg((uint32)uiPositionValue3, uiPstn3Reg);
            SAL_WriteReg((uint32)uiPositionValue4, uiPstn4Reg);

            ret = SAL_RET_SUCCESS;
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                           PDM_ConfigPhase1Pstn
*
* Calculate the number of clock of duty and period for phase mode 1
* @param    channel number 0 ~ 8.
* @param    operation clock frequence of PDM
* @param    period1 ns
* @param    duty1 ns
* @return
*
***************************************************************************************************
*/
static SALRetCode_t PDM_ConfigPhase1Pstn(
    uint32 uiChannel,
    uint32 uiClockFreq,
    uint32 uiDutyNanoSec1,
    uint32 uiPeriodNanoSec1)
{
    uint32 uiClockDivide = 0;
    uint32 uiClockTns = 0;
    uint32 uiPeriodTotalns = 0;
    uint32 uiPeriodTotalnum = 0;
    uint32 uiPosition1 = 0;
    uint32 uiPosition2 = 0;
    SALRetCode_t ret = SAL_RET_SUCCESS;

    if (uiClockFreq == 0UL)
    {
        PDM_Err("Invalid clock frequency. Check the congiguration \n");
        ret = SAL_RET_FAILED;
    }
    else if ((uiPeriodNanoSec1 == 0UL) || (uiDutyNanoSec1 > uiPeriodNanoSec1))
    {
        PDM_Err("Invalid duty and period. Check the congiguration \n");
        ret = SAL_RET_FAILED;
    }
    else
    {
        for (uiClockDivide = 0UL; uiClockDivide <= PDM_DIVID_MAX; uiClockDivide++)
        {
            if ((PDM_ONE_SECOND_TO_NANO % (uiClockFreq >> (uiClockDivide + 1UL))) == 0UL)
            {
                break;
            }
        }

        uiClockDivide = (uiClockDivide > PDM_DIVID_MAX) ? 0UL : uiClockDivide;
        uiClockFreq = (uiClockFreq >> (uiClockDivide + 1UL));
        uiClockTns = (PDM_ONE_SECOND_TO_NANO / uiClockFreq);

        uiPeriodTotalns = uiPeriodNanoSec1;

        for (; uiClockDivide <= PDM_DIVID_MAX; uiClockDivide++)
        {
            uiPeriodTotalnum = (uiPeriodTotalns / uiClockTns);

            /* Check max value, output count value in phase mode : pstnX + '2' (X = 1, 2) */
            if (uiPeriodTotalnum <= (PDM_PERIOD_MAX_VALUE - PDM_HW_LIMIT_VALUE_4))
            {
                break;
            }

            if ((PDM_REG_MAX_VALUE >> 1UL) > uiClockTns)
            {
                uiClockTns = (uiClockTns << 1UL);
                uiClockFreq = (uiClockFreq >> 1UL);
            }
        }

        if ((PDM_ONE_SECOND_TO_NANO % uiClockFreq) != 0UL)
        {
            PDM_D("There is difference in calculation. It may affect the output pulse\n");
        }

        uiPosition1 = ((uiPeriodNanoSec1 - uiDutyNanoSec1) / uiClockTns); // LOW
        uiPosition2 = (uiDutyNanoSec1 / uiClockTns);                      // HIGH

        uiPosition1 = (uiPosition1 > PDM_HW_LIMIT_VALUE_2) ? (uiPosition1 - PDM_HW_LIMIT_VALUE_2) : 0UL;
        uiPosition2 = (uiPosition2 > PDM_HW_LIMIT_VALUE_2) ? (uiPosition2 - PDM_HW_LIMIT_VALUE_2) : 0UL;

        ret = PDM_SetPhaseMode1(uiChannel, uiClockDivide, uiPosition1, uiPosition2);
    }

    return ret;
}

/*
***************************************************************************************************
*                                           PDM_ConfigPhase2Pstn
*
* Calculate the number of clock of duty and period for phase mode 2

* @param    channel number 0 ~ 8.
* @param    operation clock frequence of PDM
* @param    duty1 ns
* @param    period1 ns
* @param    duty1 ns
* @param    period2 ns
* @return
*
* Notes     if period2 is '0', pdm mode will be phase mode1, otherwise phase mode2
*           nano second scale.
*
***************************************************************************************************
*/
static SALRetCode_t PDM_ConfigPhase2Pstn(
    uint32 uiChannel,
    uint32 uiClockFreq,
    uint32 uiDutyNanoSec1,
    uint32 uiPeriodNanoSec1,
    uint32 uiDutyNanoSec2,
    uint32 uiPeriodNanoSec2)
{
    uint32 uiClockDivide = 0;
    uint32 uiClockTns = 0;
    uint32 uiPeriodTotalns = 0;
    uint32 uiPeriodTotalnum = 0;
    uint32 uiPosition1 = 0;
    uint32 uiPosition2 = 0;
    uint32 uiPosition3 = 0;
    uint32 uiPosition4 = 0;
    SALRetCode_t ret = SAL_RET_SUCCESS;

    if (uiClockFreq == 0UL)
    {
        PDM_Err("Invalid clock frequency. Check the congiguration \n");
        ret = SAL_RET_FAILED;
    }
    else if ((uiPeriodNanoSec1 == 0UL) || (uiDutyNanoSec1 > uiPeriodNanoSec1) || (uiDutyNanoSec2 > uiPeriodNanoSec2))
    {
        PDM_Err("Invalid duty and period (uiDutyNanoSec1 > uiPeriodNanoSec1) or (uiDutyNanoSec2 > uiPeriodNanoSec2)\n");
        ret = SAL_RET_FAILED;
    }
    else
    {
        for (uiClockDivide = 0UL; uiClockDivide <= PDM_DIVID_MAX; uiClockDivide++)
        {
            if ((PDM_ONE_SECOND_TO_NANO % (uiClockFreq >> (uiClockDivide + 1UL))) == 0UL)
            {
                break;
            }
        }

        uiClockDivide = (uiClockDivide > PDM_DIVID_MAX) ? 0UL : uiClockDivide;
        uiClockFreq = (uiClockFreq >> (uiClockDivide + 1UL));
        uiClockTns = (PDM_ONE_SECOND_TO_NANO / uiClockFreq);

        uiPeriodTotalns = uiPeriodNanoSec1 + uiPeriodNanoSec2;

        for (; uiClockDivide <= PDM_DIVID_MAX; uiClockDivide++)
        {
            uiPeriodTotalnum = (uiPeriodTotalns / uiClockTns);

            /* Check max value, output count value in phase mode : pstnX + '2' (X = 1, 2) */
            if (uiPeriodTotalnum <= (PDM_PERIOD_MAX_VALUE - PDM_HW_LIMIT_VALUE_4))
            {
                break;
            }

            if ((PDM_REG_MAX_VALUE >> 1UL) > uiClockTns)
            {
                uiClockTns = (uiClockTns << 1UL);
                uiClockFreq = (uiClockFreq >> 1UL);
            }
        }

        if ((PDM_ONE_SECOND_TO_NANO % uiClockFreq) != 0UL)
        {
            PDM_D("There is difference in calculation. It may affect the output pulse\n");
        }

        uiPosition1 = ((uiPeriodNanoSec1 - uiDutyNanoSec1) / uiClockTns); // LOW1
        uiPosition2 = (uiDutyNanoSec1 / uiClockTns);                      // HIGH1

        uiPosition3 = ((uiPeriodNanoSec2 - uiDutyNanoSec2) / uiClockTns); // LOW2
        uiPosition4 = (uiDutyNanoSec2 / uiClockTns);                      // HIGH2

        uiPosition1 = (uiPosition1 > PDM_HW_LIMIT_VALUE_2) ? (uiPosition1 - PDM_HW_LIMIT_VALUE_2) : 0UL;
        uiPosition2 = (uiPosition2 > PDM_HW_LIMIT_VALUE_2) ? (uiPosition2 - PDM_HW_LIMIT_VALUE_2) : 0UL;
        uiPosition3 = (uiPosition3 > PDM_HW_LIMIT_VALUE_2) ? (uiPosition3 - PDM_HW_LIMIT_VALUE_2) : 0UL;
        uiPosition4 = (uiPosition4 > PDM_HW_LIMIT_VALUE_2) ? (uiPosition4 - PDM_HW_LIMIT_VALUE_2) : 0UL;

        ret = PDM_SetPhaseMode2(uiChannel, uiClockDivide, uiPosition1, uiPosition2, uiPosition3, uiPosition4);
    }

    return ret;
}

/*
***************************************************************************************************
*                                          PDM_ConfigPhaseIdle
*
* set IDLE state in phase mode 1 & 2
*
* @param    channel number 0 ~ 8.
* @param    Operation Mode Configure Information.
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t PDM_ConfigPhaseIdle(
    uint32 uiChannel,
    const PDMModeConfig_t *pModeConfig)
{
    SALRetCode_t ret = SAL_RET_FAILED;

    if (uiChannel < PDM_OUT_CH_MAX)
    {
        PDM_D("Transit to IDLE State \n");

        ret = PDM_SetOutputInIdleState(uiChannel, pModeConfig->mcOutSignalInIdle);

        if (ret == SAL_RET_SUCCESS)
        {
            switch (pModeConfig->mcOperationMode)
            {
            case PDM_OUTPUT_MODE_PHASE_1:
                PDMHandler[uiChannel].chIdleState = PDM_ON;
                ret = PDM_SetPhaseMode1(uiChannel, 0UL, PDM_OUTPUT_PATTERN_LOW, PDM_OUTPUT_PATTERN_HIGH);
                break;

            case PDM_OUTPUT_MODE_PHASE_2:
                PDMHandler[uiChannel].chIdleState = PDM_ON;
                ret = PDM_SetPhaseMode2(uiChannel, 0UL,
                                        PDM_OUTPUT_PATTERN_LOW, PDM_OUTPUT_PATTERN_HIGH,
                                        PDM_OUTPUT_PATTERN_LOW, PDM_OUTPUT_PATTERN_HIGH);
                break;
            default:
                PDM_Err("Invalid Operation mode\n");
                break;
            }
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                           PDM_ConfigPhaseMode
*
* Check whether Idle pattern or not in Phase mode

* @param    channel number 0 ~ 8.
* @param    Operation Mode Configure Information.
* @return
*
* Notes     if period2 is '0', pdm mode will be phase mode1, otherwise phase mode2
*           nano second scale.
*
***************************************************************************************************
*/
static SALRetCode_t PDM_ConfigPhaseMode(
    uint32 uiChannel,
    const PDMModeConfig_t *pModeConfig)
{
    uint32 uiClkFreq = 0;
    SALRetCode_t ret = SAL_RET_SUCCESS;

    if (pModeConfig == (PDMModeConfig_t *)NULL)
    {
        PDM_Err("Invalid configuration pointer \n");
        ret = SAL_RET_FAILED;
    }
    else
    {
        uiClkFreq = CLOCK_GetPeriRate((sint32)CLOCK_PERI_PWM0);
        switch (pModeConfig->mcOperationMode)
        {
        case PDM_OUTPUT_MODE_PHASE_1:
            if (((pModeConfig->mcPosition1 == PDM_OUTPUT_PATTERN_LOW) && (pModeConfig->mcPosition2 == PDM_OUTPUT_PATTERN_HIGH)) ||
                ((pModeConfig->mcPosition1 == PDM_OUTPUT_PATTERN_HIGH) && (pModeConfig->mcPosition2 == PDM_OUTPUT_PATTERN_LOW)))
            {
                ret = PDM_ConfigPhaseIdle(uiChannel, pModeConfig);
            }
            else
            {
                PDMHandler[uiChannel].chIdleState = PDM_OFF;
                ret = PDM_ConfigPhase1Pstn(uiChannel, uiClkFreq, pModeConfig->mcDutyNanoSec1, pModeConfig->mcPeriodNanoSec1);
            }
            break;

        default:
            if (((pModeConfig->mcPosition1 == PDM_OUTPUT_PATTERN_LOW) && (pModeConfig->mcPosition2 == PDM_OUTPUT_PATTERN_HIGH) &&
                 (pModeConfig->mcPosition3 == PDM_OUTPUT_PATTERN_LOW) && (pModeConfig->mcPosition4 == PDM_OUTPUT_PATTERN_HIGH)) ||
                ((pModeConfig->mcPosition1 == PDM_OUTPUT_PATTERN_HIGH) && (pModeConfig->mcPosition2 == PDM_OUTPUT_PATTERN_LOW) &&
                 (pModeConfig->mcPosition3 == PDM_OUTPUT_PATTERN_HIGH) && (pModeConfig->mcPosition4 == PDM_OUTPUT_PATTERN_LOW)))
            {
                ret = PDM_ConfigPhaseIdle(uiChannel, pModeConfig);
            }
            else
            {
                PDMHandler[uiChannel].chIdleState = PDM_OFF;
                ret = PDM_ConfigPhase2Pstn(uiChannel, uiClkFreq,
                                           pModeConfig->mcDutyNanoSec1, pModeConfig->mcPeriodNanoSec1,
                                           pModeConfig->mcDutyNanoSec2, pModeConfig->mcPeriodNanoSec2);
            }
            break;
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          PDM_ConfigPort
*
* Set Port Selection Register
*
* @param    channel number 0 ~ 8 (9 ~ 11 are not available, Can't be selected by PERISEL Register)
* @param    port number.
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t PDM_ConfigPort(
    uint32 uiChannel,
    uint32 uiPortNum)
{
    uint32 uiIdx = 0;
    SALRetCode_t ret = SAL_RET_FAILED;

    if (uiChannel < PDM_OUT_CH_MAX)
    {
        for (uiIdx = 0; uiIdx < PDM_OUT_PORT_MAX; uiIdx++)
        {
            if (uiPortNum == sPdmPort[uiIdx].pcPortSelCh)
            {
                if ((uiIdx + uiChannel) < PDM_OUT_PORT_MAX)
                {
                    if (sPdmPort[uiIdx].pcPortSelCh == (uint32)GPIO_PERICH_CH3)
                    {
                        /* If the port is GPIO_K, have to set PMGPIO_FS register */
                        PMIO_SetGpk(PMIO_GPK((uiChannel + 8UL)));
                    }

                    (void)GPIO_Config(sPdmPort[uiIdx + uiChannel].pcPortNum, (sPdmPort[uiIdx + uiChannel].pcPortFs | GPIO_OUTPUT | GPIO_DS(0x3UL)));

                    (void)GPIO_PerichSel(sPdmPort[uiIdx + uiChannel].pcPdmCh, sPdmPort[uiIdx + uiChannel].pcPortSelCh);

                    PDMHandler[uiChannel].chModeCfgInfo.mcPortNumber = uiPortNum;

                    ret = SAL_RET_SUCCESS;

                    break;
                }
            }
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          PDM_ConfigOutput
*
* Set Port Selection Register
*
* @param    channel number 0 ~ 8 (9 ~ 11 are not available, Can't be selected by PERISEL Register)
* @param    port number.
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t PDM_ConfigOutput(
    uint32 uiChannel,
    PDMModeConfig_t *pModeConfig)
{
    uint32 ret = 0UL;

    /* Check vaildation of Configuration value */
    if (1UL < pModeConfig->mcInversedSignal)
    {
        PDM_D("WARNING! Inavild InversedSignal value. Set InversedSignal to '0'\n");
        pModeConfig->mcInversedSignal = 0UL;
    }

    if (0x0FUL < pModeConfig->mcLoopCount)
    {
        PDM_D("WARNING! Inavild LoopCount value. Set LoopCount to '0'\n");
        pModeConfig->mcLoopCount = 0UL;
    }

    if (1UL < pModeConfig->mcOutSignalInIdle)
    {
        PDM_D("WARNING! Inavild OutSignalInIdle value. Set OutSignalInIdle to '0'\n");
        pModeConfig->mcOutSignalInIdle = 0UL;
    }

    if (0x0FUL < pModeConfig->mcOutputCtrl)
    {
        PDM_D("WARNING! Inavild OutputCtrl value. Set OutputCtrl to '0'\n");
        pModeConfig->mcOutputCtrl = 0UL;
    }

    if ((pModeConfig->mcOutputCtrl & 0x01UL) != ((pModeConfig->mcOutputCtrl & 0x04UL) >> 2UL))
    {
        PDM_D("WARNING! Inavild OutputCtrl value. DO_Sel and OEN_Sel should be same. Set OutputCtrl to '0'\n");
        pModeConfig->mcOutputCtrl = 0UL;
    }

    ret |= (uint32)PDM_ConfigPort(uiChannel, pModeConfig->mcPortNumber);
    ret |= (uint32)PDM_SetOutputInverse(uiChannel, pModeConfig->mcInversedSignal);
    ret |= (uint32)PDM_SetOutputLoopCount(uiChannel, pModeConfig->mcLoopCount);
    ret |= (uint32)PDM_SetOutputInIdleState(uiChannel, pModeConfig->mcOutSignalInIdle);
    ret |= (uint32)PDM_SetCfgOutputControl(uiChannel, pModeConfig->mcOutputCtrl);

    return (SALRetCode_t)ret;
}

/*
***************************************************************************************************
*                                       USER API FUNCTION
***************************************************************************************************
*/

/*
***************************************************************************************************
*                                           PDM_CfgSetWrPw
*
* write password

* @param
* @return
*
* Notes
*
***************************************************************************************************
*/
void PDM_CfgSetWrPw(
    void)
{
    uint32 uiReg = 0;

    uiReg = PDM_CFG_BASE + PDM_PCFG_WR_PW_REG_OFFSET;

    SAL_WriteReg(PCFG_WR_PW, uiReg);
}

/*
***************************************************************************************************
*                                           PMM_SetCfgWrLock
*
* write lock

* @param    lock(1U) or unlock(0U)
* @return
*
* Notes
*
***************************************************************************************************
*/
void PDM_CfgSetWrLock(
    uint32 uiLock)
{
    uint32 uiReg = 0;

    uiReg = PDM_CFG_BASE + PDM_PCFG_WR_LOCK_REG_OFFSET;

    SAL_WriteReg(uiLock, uiReg);
}

/*
***************************************************************************************************
*                                          PDM_EnableClock
*
* Enable PDM peripheral clock.
* Notes
*
***************************************************************************************************
*/
void PDM_EnableClock(
    void)
{
    (void)CLOCK_SetPeriRate((sint32)CLOCK_PERI_PWM0, PDM_PERI_CLOCK);
    (void)CLOCK_EnablePeri((sint32)CLOCK_PERI_PWM0);
    (void)CLOCK_SetPeriRate((sint32)CLOCK_PERI_PWM1, PDM_PERI_CLOCK);
    (void)CLOCK_EnablePeri((sint32)CLOCK_PERI_PWM1);
    (void)CLOCK_SetPeriRate((sint32)CLOCK_PERI_PWM2, PDM_PERI_CLOCK);
    (void)CLOCK_EnablePeri((sint32)CLOCK_PERI_PWM2);

    PDM_Delay1u(1000UL);

    PDM_D("\n PDM ch0-peri clk >> %dHz \n", CLOCK_GetPeriRate((sint32)CLOCK_PERI_PWM0));
    PDM_D("\n PDM ch1-peri clk >> %dHz \n", CLOCK_GetPeriRate((sint32)CLOCK_PERI_PWM1));
    PDM_D("\n PDM ch2-peri clk >> %dHz \n", CLOCK_GetPeriRate((sint32)CLOCK_PERI_PWM2));
}

/*
***************************************************************************************************
*                                          PDM_DisableClock
*
* Disable PDM peripheral clock.
* Notes
*
***************************************************************************************************
*/
void PDM_DisableClock(
    void)
{
    (void)CLOCK_DisablePeri((sint32)CLOCK_PERI_PWM0);
    (void)CLOCK_DisablePeri((sint32)CLOCK_PERI_PWM1);
    (void)CLOCK_DisablePeri((sint32)CLOCK_PERI_PWM2);
}

/*
***************************************************************************************************
*                                          PDM_Init
*
* Initialize variable for register map and setup FMU for error Handling.
*
* Notes
*
***************************************************************************************************
*/
void PDM_Init(void)
{
    uint32 uiChannel = 0;

    for (uiChannel = 0UL; uiChannel < PDM_OUT_CH_MAX; uiChannel++)
    {
        /* Initialize pdm handle */
        PDMHandler[uiChannel].chModuleId = uiChannel / PDM_TOTAL_CH_PER_MODULE;
        PDMHandler[uiChannel].chChannelId = uiChannel % PDM_TOTAL_CH_PER_MODULE;
        PDMHandler[uiChannel].chEnable = PDM_OFF;
        PDMHandler[uiChannel].chIdleState = PDM_OFF;
        PDMHandler[uiChannel].chOutSignalInIdle = PDM_OFF;
        PDMHandler[uiChannel].chIdleState = PDM_OFF;

        PDMHandler[uiChannel].chModeCfgInfo.mcPortNumber = PDM_OFF;
        PDMHandler[uiChannel].chModeCfgInfo.mcOperationMode = PDM_OFF;
        PDMHandler[uiChannel].chModeCfgInfo.mcClockDivide = PDM_OFF;
        PDMHandler[uiChannel].chModeCfgInfo.mcLoopCount = PDM_OFF;
        PDMHandler[uiChannel].chModeCfgInfo.mcInversedSignal = PDM_OFF;
        PDMHandler[uiChannel].chModeCfgInfo.mcPeriodNanoSec1 = PDM_OFF;
        PDMHandler[uiChannel].chModeCfgInfo.mcDutyNanoSec1 = PDM_OFF;
        PDMHandler[uiChannel].chModeCfgInfo.mcPeriodNanoSec2 = PDM_OFF;
        PDMHandler[uiChannel].chModeCfgInfo.mcDutyNanoSec2 = PDM_OFF;
        PDMHandler[uiChannel].chModeCfgInfo.mcPosition1 = 0UL;
        PDMHandler[uiChannel].chModeCfgInfo.mcPosition2 = 0UL;
        PDMHandler[uiChannel].chModeCfgInfo.mcPosition3 = 0UL;
        PDMHandler[uiChannel].chModeCfgInfo.mcPosition4 = 0UL;

        PDMHandler[uiChannel].chModeCfgInfo.mcOutPattern1 = 0UL;
        PDMHandler[uiChannel].chModeCfgInfo.mcOutPattern2 = 0UL;
        PDMHandler[uiChannel].chModeCfgInfo.mcOutPattern3 = 0UL;
        PDMHandler[uiChannel].chModeCfgInfo.mcOutPattern4 = 0UL;
        PDMHandler[uiChannel].chModeCfgInfo.mcMaxCount = 0UL;

#ifdef PDM_SAFETY_FEATURE_ENABLED
        PDMHandler[uiChannel].chPMMEnable = PMM_ON;
        PDMHandler[uiChannel].chPMMInputCnect = PMM_OFF;
        PDMHandler[uiChannel].chPMMFaultStatus = 0UL;
        PDMHandler[uiChannel].chPMMTimeoutValue = 0xFFFFFFFFUL;
        PDMHandler[uiChannel].chPMMDutyMargin = 0xFFUL;
        PDMHandler[uiChannel].chPMMErrChannel = FALSE;
#endif
    }

    PDM_EnableClock();

#ifdef PDM_SAFETY_FEATURE_ENABLED
    if (g_safety_enabled == FALSE)
    {
        (void)FMU_IsrHandler(FMU_ID_PDM0, FMU_SVL_HIGH, (FMUIntFnctPtr)&PMM_PDM0IrqHandler, NULL_PTR);
        (void)FMU_IsrHandler(FMU_ID_PDM1, FMU_SVL_HIGH, (FMUIntFnctPtr)&PMM_PDM1IrqHandler, NULL_PTR);
        (void)FMU_IsrHandler(FMU_ID_PDM2, FMU_SVL_HIGH, (FMUIntFnctPtr)&PMM_PDM2IrqHandler, NULL_PTR);

        (void)FMU_Set((FMUFaultid_t)FMU_ID_PDM0);
        (void)FMU_Set((FMUFaultid_t)FMU_ID_PDM1);
        (void)FMU_Set((FMUFaultid_t)FMU_ID_PDM2);

        g_safety_enabled = TRUE;
    }
#endif

    return;
}

/*
***************************************************************************************************
*                                          PDM_Deinit
*
* De-initialize PDM descriptor.
*
* Notes
*
***************************************************************************************************
*/
void PDM_Deinit(
    void)
{
    PDM_DisableClock();
    (void)SAL_MemSet(PDMHandler, 0, sizeof(PDMHandler) * PDM_TOTAL_CHANNELS);

#ifdef PDM_SAFETY_FEATURE_ENABLED
    (void)SAL_MemSet(PDMIsrData, 0, sizeof(PDMIsrData) * PDM_TOTAL_CHANNELS);
#endif
}

/*
***************************************************************************************************
*                                          PDM_Enable
*
* Enable pdm module, setup OP_EN register, and Trigger output signal
*
* @param    channel number 0 ~ 8.
* @param    Enable PMM monitoring.
* @return
*
* Notes To operate PDM correctly MUST
        set all configuration and enable first
        set VUP to 1 to announce the end of register update
        set TRIG to 1, PDM output will be generated correctly.
*
***************************************************************************************************
*/
SALRetCode_t PDM_Enable(
    uint32 uiChannel,
    uint32 uiMonitoring)
{
    uint32 uiReg = 0;
    uint32 uiVal = 0;
    uint32 uiModuleId = 0;
    uint32 uiChannelId = 0;
    SALRetCode_t ret = SAL_RET_SUCCESS;

    if (PDM_OUT_CH_MAX <= uiChannel)
    {
        PDM_Err("Invalid channel number \n");
        ret = SAL_RET_FAILED;
    }
    else if (PMM_ON < uiMonitoring)
    {
        PDM_Err("Invalid uiMonitoring option \n");
        ret = SAL_RET_FAILED;
    }
    else
    {
        uiModuleId = PDMHandler[uiChannel].chModuleId;
        uiChannelId = PDMHandler[uiChannel].chChannelId;

        if ((uiModuleId < PDM_TOTAL_MODULES) && (uiChannelId < PDM_TOTAL_CH_PER_MODULE))
        {
#ifdef PDM_SAFETY_FEATURE_ENABLED
            if (uiMonitoring == PMM_ON)
            {
                (void)PMM_EnableProcess(uiChannel);
            }
#endif
            uiReg = PDM_BASE + PDM_OP_EN_REG_OFFSET + (uiModuleId * PDM_MODULE_OFFSET);

            uiVal = SAL_ReadReg(uiReg) | ((uint32)0x1U << (PDM_GetOPENEnableReg(uiChannelId)));
            SAL_WriteReg(uiVal, uiReg);

            /* announce update PDM-A/B/C/D */
            uiVal = SAL_ReadReg(uiReg) | ((uint32)0x1U << (PDM_GetOPENValueUpReg(uiChannelId)));
            SAL_WriteReg(uiVal, uiReg);

            /* At least 1 Cycle delay required based on 'Peripheral_Clock/DIV_x' in 12.5.1 Safety Feature */
            PDM_Delay1u(1UL);

            if (PDM_GetChannelStatus(uiChannel) != PDM_ON)
            {
                /* triggering output generation PDM-A/B/C/D */
                uiVal = SAL_ReadReg(uiReg) | ((uint32)0x1U << (PDM_GetOPENTrigReg(uiChannelId)));
                SAL_WriteReg(uiVal, uiReg);
            }

            PDMHandler[uiChannel].chEnable = PDM_ON;

            ret = SAL_RET_SUCCESS;
        }
    }

    return ret; // success
}

/*
***************************************************************************************************
*                                          PDM_Disable
*
* Disable pdm module
*
* @param    channel number 0 ~ 8.
* @param    Disable PMM monitoring.
* @return
*
* Notes
*
***************************************************************************************************
*/
SALRetCode_t PDM_Disable(
    uint32 uiChannel,
    uint32 uiMonitoring)
{
    uint32 uiReg = 0;
    uint32 uiVal = 0;
    uint32 uiModuleId = 0;
    uint32 uiChannelId = 0;
    SALRetCode_t ret = SAL_RET_FAILED;

    if (PDM_OUT_CH_MAX <= uiChannel)
    {
        PDM_Err("Invalid channel number \n");
        ret = SAL_RET_FAILED;
    }
    else if (PMM_ON < uiMonitoring)
    {
        PDM_Err("Invalid uiMonitoring option \n");
        ret = SAL_RET_FAILED;
    }
    else
    {
        uiModuleId = PDMHandler[uiChannel].chModuleId;
        uiChannelId = PDMHandler[uiChannel].chChannelId;

        if ((uiModuleId < PDM_TOTAL_MODULES) && (uiChannelId < PDM_TOTAL_CH_PER_MODULE))
        {
#ifdef PDM_SAFETY_FEATURE_ENABLED
            if (uiMonitoring == PMM_ON)
            {
                (void)PMM_DisableMonitoring(uiChannel);
            }
#endif
            uiReg = PDM_BASE + PDM_OP_EN_REG_OFFSET + (uiModuleId * PDM_MODULE_OFFSET);

            /* clear enable bit PDM-A/B/C/D  */
            uiVal = SAL_ReadReg(uiReg) & ~((uint32)0x1U << (PDM_GetOPENEnableReg(uiChannelId)));
            SAL_WriteReg(uiVal, uiReg);

            PDMHandler[uiChannel].chEnable = PDM_OFF;

            ret = SAL_RET_SUCCESS;
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          PDM_GetChannelStatus
*
* Get channel status
*
* @param    channel number 0 ~ 8.
* @return   return value 1 means PDMn-X is busy status
*
* Notes
*
***************************************************************************************************
*/
uint32 PDM_GetChannelStatus(
    uint32 uiChannel)
{
    uint32 uiReg = 0;
    uint32 uiVal = 0;
    uint32 uiModuleId = 0;
    uint32 uiChannelId = 0;
    uint32 ret = PDM_REG_MAX_VALUE;

    if (uiChannel < PDM_OUT_CH_MAX)
    {
        uiModuleId = PDMHandler[uiChannel].chModuleId;
        uiChannelId = PDMHandler[uiChannel].chChannelId;

        if ((uiModuleId < PDM_TOTAL_MODULES) && (uiChannelId < PDM_TOTAL_CH_PER_MODULE))
        {
            uiReg = PDM_BASE + PDM_STATUS_REG_OFFSET + (uiModuleId * PDM_MODULE_OFFSET);
            uiVal = SAL_ReadReg(uiReg);
            ret = ((uiVal >> PDM_GetBusyStatusReg(uiChannelId)) & 0x01UL);
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          PDM_SetConfig
*
* Set Configuration value for PMM
* User API
*
* @param    channel number 0 ~ 8.
* @param    Operation Mode Configure Information.
* @return
*
* Notes
*
***************************************************************************************************
*/
SALRetCode_t PDM_SetConfig(
    uint32 uiChannel,
    PDMModeConfig_t *pModeConfig)
{
    SALRetCode_t ret = SAL_RET_SUCCESS;

    if (PDM_OUT_CH_MAX <= uiChannel)
    {
        PDM_Err("Invalid channel number \n");
        ret = SAL_RET_FAILED;
    }
    else if (pModeConfig == (PDMModeConfig_t *)NULL)
    {
        PDM_Err("Invalid configuration pointer \n");
        ret = SAL_RET_FAILED;
    }
    else
    {
        ret = PDM_ConfigOutput(uiChannel, pModeConfig);

        if (ret != SAL_RET_SUCCESS)
        {
            PDM_Err("ERROR! PDM Configuration FAIL!\n");
        }
        else
        {
            if ((pModeConfig->mcOperationMode == PDM_OUTPUT_MODE_PHASE_1) ||
                (pModeConfig->mcOperationMode == PDM_OUTPUT_MODE_PHASE_2))
            {
                ret = PDM_ConfigPhaseMode(uiChannel, pModeConfig);
            }
            else if ((pModeConfig->mcOperationMode == PDM_OUTPUT_MODE_REGISTER_1) ||
                     (pModeConfig->mcOperationMode == PDM_OUTPUT_MODE_REGISTER_2) ||
                     (pModeConfig->mcOperationMode == PDM_OUTPUT_MODE_REGISTER_3))
            {
                ret = PDM_SetRegisterMode(uiChannel, pModeConfig);
            }
            else
            {
                PDM_Err("Invalid PDM Operation Mode \n");
                ret = SAL_RET_FAILED;
            }
        }
    }

    return ret;
}

#endif // ( MCU_BSP_SUPPORT_DRIVER_PDM == 1 )
