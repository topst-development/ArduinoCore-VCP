// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : pmu.c
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#include <pmu.h>
#include <pmu_reg.h>
#include <debug.h>
#include <stdbool.h>

/*************************************************************************************************
 *                                             DEFINITIONS
 *************************************************************************************************/

#if defined(DEBUG_ENABLE)
    #define PMU_D(fmt, args...)        {mcu_printf("[PMU][%s:%d] " fmt, \
                                                __func__, __LINE__, ## args);}
#else
    #define PMU_D(fmt, args...)
#endif

#define PMU_E(fmt, args...)        {mcu_printf("[PMU][%s:%d] Error ! " fmt, \
                                            __func__, __LINE__, ## args);}


#define PMU_VA_WR_PASS                  (0x5AFEACE5UL)

typedef enum
{
    PMU_FMU_NOTHING                     = 0,

    PMU_FMU_XIN_PVT_DISABLE             = 1,
    PMU_FMU_XIN_PVT_EDGE                = 2,
    PMU_FMU_XIN_PVT_LEVEL               = 3,

    PMU_FMU_PWR_PVT_DISABLE             = 4,
    PMU_FMU_PWR_PVT_EDGE                = 5,
    PMU_FMU_PWR_PVT_LEVEL               = 6,

    PMU_FMU_MAX                         = 7
}PMUFmu_t;


static bool gIsPOR = (FALSE);

/************************************************************************************************/
/*                                             STATIC FUNCTION                                  */
/************************************************************************************************/

static void PMU_FmuCtrl
(
    PMUFmu_t                            tFmu
);

static void PMU_ResetCtrl
(
    PMUSrc_t                            tRst,
    uint32_t                              uiEn
);

static void PMU_MonCtrl
(
    PMUSrc_t                            tMon,
    uint32_t                              uiEn
);

/*************************************************************************************************/
/*                                             Implementation                                    */
/* ***********************************************************************************************/

static void PMU_FmuCtrl
(
    PMUFmu_t                            tFmu
)
{
    uint32_t uiFieldOffset;
    uint32_t uiEn;

    uiFieldOffset = 0UL;
    uiEn = 0UL;

    if((tFmu > PMU_FMU_NOTHING) && (tFmu < PMU_FMU_MAX))
    {
        switch(tFmu)
        {
            case PMU_FMU_XIN_PVT_DISABLE:
                {
                    uiFieldOffset = PMU_ADDR_PVT_SM_CFG_FIELD_XIN_PERIODIC_EN;
                    uiEn = 0UL;
                    break;
                }
            case PMU_FMU_XIN_PVT_EDGE:
                {
                    uiFieldOffset = PMU_ADDR_PVT_SM_CFG_FIELD_XIN_PERIODIC_EN;
                    uiEn = 2UL;
                    break;
                }
            case PMU_FMU_XIN_PVT_LEVEL:
                {
                    uiFieldOffset = PMU_ADDR_PVT_SM_CFG_FIELD_XIN_PERIODIC_EN;
                    uiEn = 3UL;
                    break;
                }
            case PMU_FMU_PWR_PVT_DISABLE:
                {
                    uiFieldOffset = PMU_ADDR_PVT_SM_CFG_FIELD_POWER_PERIODIC_EN;
                    uiEn = 0UL;
                    break;
                }
            case PMU_FMU_PWR_PVT_EDGE:
                {
                    uiFieldOffset = PMU_ADDR_PVT_SM_CFG_FIELD_POWER_PERIODIC_EN;
                    uiEn = 2UL;
                    break;
                }
            case PMU_FMU_PWR_PVT_LEVEL:
                {
                    uiFieldOffset = PMU_ADDR_PVT_SM_CFG_FIELD_POWER_PERIODIC_EN;
                    uiEn = 3UL;
                    break;
                }
            default:
                {
                    break;
                }
        }

        PMU_REG_SET(
                PMU_VA_WR_PASS,
                PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW,
                PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW_MASK,
                PMU_ADDR_PMU_WR_PW);

        PMU_REG_APPEND(
                uiEn,
                uiFieldOffset,
                PMU_VA_MASK_2 << uiFieldOffset,
                PMU_ADDR_PVT_SM_CFG);
    }
    else
    {
        PMU_E("Unknown reset type %d\n", (uint32_t)tFmu);
    }
}

static void PMU_ResetCtrl
(
    PMUSrc_t                            tRst,
    uint32_t                              uiEn
)
{
    uint32_t uiFieldOffset;

    uiFieldOffset = 0xFFFFFFFFUL;

    if((tRst > PMU_SRC_NOTHING) && (tRst < PMU_SRC_MAX))
    {
        if(tRst == PMU_SRC_WDT)
        {
            uiFieldOffset = PMU_ADDR_RST_ENABLE_FIELD_WDT_RST_EN;
        }
        else if(tRst == PMU_SRC_PMU_WDT)
        {
            uiFieldOffset = PMU_ADDR_RST_ENABLE_FIELD_PMU_WDT_RST_EN;
        }
        else if(tRst == PMU_SRC_HSM_WDT)
        {
            uiFieldOffset = PMU_ADDR_RST_ENABLE_FIELD_HSM_WDT_RST_EN;
        }
        else if(tRst == PMU_SRC_HSM_WARM)
        {
            uiFieldOffset = PMU_ADDR_RST_ENABLE_FIELD_HSM_WARM_RST_EN;
        }
        else if(tRst == PMU_SRC_HSM_COLD)
        {
            uiFieldOffset = PMU_ADDR_RST_ENABLE_FIELD_HSM_COLD_RST_EN;
        }
        else if(tRst == PMU_SRC_FMU_FAULT)
        {
            uiFieldOffset = PMU_ADDR_RST_ENABLE_FIELD_FMU_FAULT_RST_EN;
        }
        else if(tRst == PMU_SRC_XIN_PVT)
        {
            uiFieldOffset = PMU_ADDR_RST_ENABLE_FIELD_XIN_PVT_RST_EN;
            if(uiEn == 1UL)
            {
                PMU_FmuCtrl(PMU_FMU_XIN_PVT_DISABLE);
                PMU_MonCtrl(tRst, 1);
            }
            else
            {
                PMU_MonCtrl(tRst, 0);
                PMU_FmuCtrl(PMU_FMU_XIN_PVT_LEVEL);
            }
        }
        else if(tRst >= PMU_SRC_PWR0_PVT)
        {
            uiFieldOffset = PMU_ADDR_RST_ENABLE_FIELD_PWR0_PVT_RST_EN + ((uint32_t)tRst - PMU_SRC_PWR0_PVT);
            if(uiEn == 1UL)
            {
                PMU_FmuCtrl(PMU_FMU_PWR_PVT_DISABLE);
                PMU_MonCtrl(tRst, 1);
            }
            else
            {
                PMU_MonCtrl(tRst, 0);
                PMU_FmuCtrl(PMU_FMU_PWR_PVT_LEVEL);
            }
        }
        else
        {
            PMU_E("Unknown reset type %d\n", (uint32_t)tRst);
        }

        if(uiFieldOffset < 0xFFFFFFFFUL)
        {
            PMU_REG_SET(
                    PMU_VA_WR_PASS,
                    PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW,
                    PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW_MASK,
                    PMU_ADDR_PMU_WR_PW);

            PMU_REG_APPEND(
                    uiEn,
                    uiFieldOffset,
                    PMU_VA_MASK_1 << uiFieldOffset,
                    PMU_ADDR_RST_ENABLE);
        }
    }
    else
    {
        PMU_E("Unknown reset type %d\n", (uint32_t)tRst);
    }

}

static void PMU_MonCtrl
(
    PMUSrc_t                            tMon,
    uint32_t                              uiEn
)
{
    uint32_t uiAddr;
    uint32_t uiFieldOffset;

    uiAddr = 0UL;
    uiFieldOffset = 0UL;

    if((tMon >= PMU_SRC_XIN_PVT) && (tMon < PMU_SRC_MAX))
    {
        if(tMon == PMU_SRC_XIN_PVT)
        {
            uiAddr = PMU_ADDR_PVT4_CFG;
            uiFieldOffset = PMU_ADDR_PVT4_CFG_FIELD_XIN_MON_EN;
        }
        else
        {
            uiAddr = PMU_ADDR_PVT0_CFG;
            uiFieldOffset = PMU_ADDR_PVT0_CFG_FIELD_PWR0_MON_EN + ((uint32_t)tMon - (uint32_t)PMU_SRC_PWR0_PVT);

            PMU_REG_SET(
                    PMU_VA_WR_PASS,
                    PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW,
                    PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW_MASK,
                    PMU_ADDR_PMU_WR_PW);

            PMU_REG_APPEND(
                    2UL,
                    PMU_ADDR_PVT3_CFG_FIELD_CFG_PWR_MON_THR,
                    PMU_ADDR_PVT3_CFG_FIELD_CFG_PWR_MON_THR_MASK,
                    PMU_ADDR_PVT3_CFG);
        }

        PMU_REG_SET(
                PMU_VA_WR_PASS,
                PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW,
                PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW_MASK,
                PMU_ADDR_PMU_WR_PW);

        PMU_REG_APPEND(
                uiEn,
                uiFieldOffset,
                PMU_VA_MASK_1 << uiFieldOffset,
                uiAddr);

        PMU_ClearPvtError(tMon);
    }
    else
    {
        PMU_E("Unknown monitoring type %d\n", (uint32_t)tMon);
    }
}

void PMU_Init
(
    void
)
{
    if(PMU_REG_GET(0, PMU_ADDR_COMMON_FIELD_FULL_MASK, PMU_ADDR_RST_STATUS0) == 0UL)
    {
        gIsPOR = TRUE;
    }

    PMU_REG_SET(
            PMU_VA_WR_PASS,
            PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW,
            PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW_MASK,
            PMU_ADDR_PMU_WR_PW);

    PMU_REG_SET(
            0xFFFFFFFF,
            0,
            PMU_ADDR_COMMON_FIELD_FULL_MASK,
            PMU_ADDR_RST_STATUS0);

    PMU_REG_SET(
            PMU_VA_WR_PASS,
            PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW,
            PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW_MASK,
            PMU_ADDR_PMU_WR_PW);

    PMU_REG_SET(
            0xFFFFFFFF,
            0,
            PMU_ADDR_COMMON_FIELD_FULL_MASK,
            PMU_ADDR_RST_STATUS1);

    PMU_REG_SET(
            PMU_VA_WR_PASS,
            PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW,
            PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW_MASK,
            PMU_ADDR_PMU_WR_PW);

    PMU_REG_SET(
            0x0,
            0x0,
            PMU_ADDR_COMMON_FIELD_FULL_MASK,
            PMU_ADDR_HSM_RSTN_MSK);

    PMU_REG_SET(
            PMU_VA_WR_PASS,
            PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW,
            PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW_MASK,
            PMU_ADDR_PMU_WR_PW);

    PMU_REG_APPEND(
            0x0,
            PMU_ADDR_PVT4_CFG_FIELD_XIN_MON_EN,
            PMU_ADDR_PVT4_CFG_FIELD_XIN_MON_EN_MASK,
            PMU_ADDR_PVT4_CFG);

    PMU_REG_SET(
            PMU_VA_WR_PASS,
            PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW,
            PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW_MASK,
            PMU_ADDR_PMU_WR_PW);

    PMU_REG_SET(
            0x0,
            0x0,
            PMU_ADDR_COMMON_FIELD_FULL_MASK,
            PMU_ADDR_PVT0_CFG);

    PMU_REG_SET(
            PMU_VA_WR_PASS,
            PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW,
            PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW_MASK,
            PMU_ADDR_PMU_WR_PW);

    PMU_REG_SET(
            0x0,
            0x0,
            PMU_ADDR_COMMON_FIELD_FULL_MASK,
            PMU_ADDR_RST_ENABLE);

    PMU_FmuCtrl(PMU_FMU_XIN_PVT_LEVEL);
    PMU_FmuCtrl(PMU_FMU_PWR_PVT_LEVEL);
}

bool PMU_GetResetOccur
(
    PMUSrc_t                             tRst
)
{
    bool bRet;
    uint32_t uiAddr;
    uint32_t uiFieldOffset;
    uint32_t uiFieldVa;

    bRet = FALSE;
    uiAddr = 0UL;
    uiFieldOffset = 0UL;
    uiFieldVa = 0UL;

    if(gIsPOR == FALSE)
    {
        if((tRst > PMU_SRC_NOTHING) && (tRst < PMU_SRC_MAX))
        {
            if(tRst < PMU_SRC_XIN_PVT)
            {
                uiAddr = PMU_ADDR_RST_STATUS0;
                uiFieldOffset = (((uint32_t)tRst) * 2UL);
            }
            else
            {
                uiAddr = PMU_ADDR_RST_STATUS1;
                uiFieldOffset = (((uint32_t)tRst - PMU_SRC_XIN_PVT) * 2UL);
            }

            uiFieldOffset += 1; //Get old status

            uiFieldVa = PMU_REG_GET(uiFieldOffset, PMU_VA_MASK_1 << uiFieldOffset, uiAddr);

            if(uiFieldVa == 0UL)
            {
                bRet = TRUE;
            }
        }
        else
        {
            PMU_E("Unknown reset type %d\n", (uint32_t)tRst);
        }
    }

    return bRet;
}

void PMU_EnableResetMonitor
(
    PMUMonSrc_t                            tRst
)
{
    if( (tRst >= PMU_MON_SRC_HSM_COLD) && (tRst < PMU_MON_SRC_MAX) )
    {
        PMU_ResetCtrl((PMUSrc_t)tRst, 1UL);
    }
    else
    {
        PMU_E("Monitoring source is out of range. %d\n", tRst);
    }
}

void PMU_DisableResetMonitor
(
    PMUMonSrc_t                            tRst
)
{
    if( (tRst >= PMU_MON_SRC_HSM_COLD) && (tRst < PMU_MON_SRC_MAX) )
    {
        PMU_ResetCtrl((PMUSrc_t)tRst, 0UL);
    }
    else
    {
        PMU_E("Monitoring source is out of range. %d\n", tRst);
    }
}

void PMU_ColdReset
(
    void
)
{
    PMU_REG_SET(
            PMU_VA_WR_PASS,
            PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW,
            PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW_MASK,
            PMU_ADDR_PMU_WR_PW);

    PMU_REG_SET(
            0x1,
            PMU_ADDR_COLD_RST_REQ_FIELD_COLD_RST_REQ,
            PMU_ADDR_COLD_RST_REQ_FIELD_COLD_RST_REQ_MASK,
            PMU_ADDR_COLD_RST_REQ);
}

void PMU_WarmReset
(
    void
)
{
    PMU_REG_SET(
            PMU_VA_WR_PASS,
            PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW,
            PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW_MASK,
            PMU_ADDR_PMU_WR_PW);

    PMU_REG_SET(
            0x1,
            PMU_ADDR_WARM_RST_REQ_FIELD_WARM_RST_REQ,
            PMU_ADDR_WARM_RST_REQ_FIELD_WARM_RST_REQ_MASK,
            PMU_ADDR_WARM_RST_REQ);
}

void PMU_ClearPvtError
(
    PMUPvtSrc_t                             tPvt
)
{
    uint32_t uiAddr;
    uint32_t uiField;

    uiAddr = 0UL;
    uiField = 0UL;

    if(tPvt >= PMU_PVT_SRC_XIN_PVT && tPvt < PMU_PVT_SRC_MAX)
    {
        if(tPvt == PMU_PVT_SRC_XIN_PVT)
        {
            uiAddr = PMU_ADDR_PVT4_CFG;
            uiField = PMU_ADDR_PVT4_CFG_FIELD_XIN_OURANGE;
        }
        else
        {
            uiAddr = PMU_ADDR_PVT1_CFG;
            uiField = tPvt - PMU_PVT_SRC_PWR0_PVT;
        }

        PMU_REG_SET(
                PMU_VA_WR_PASS,
                PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW,
                PMU_ADDR_PMU_WR_PW_FIELD_PMU_WR_PW_MASK,
                PMU_ADDR_PMU_WR_PW);

        PMU_REG_APPEND(
                0x1,
                uiField,
                (PMU_VA_MASK_1 << uiField),
                uiAddr);
    }
    else
    {
        PMU_E("Pvt source is out of range. %d\n", tPvt);
    }
}

