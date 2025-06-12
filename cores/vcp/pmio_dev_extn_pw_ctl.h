// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : pmio_dev_extn_pw_ctl.h
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#ifndef  MCU_BSP_PMIO_DEV_EXTN_PW_CTL_HEADER
#define  MCU_BSP_PMIO_DEV_EXTN_PW_CTL_HEADER
/*
***************************************************************************************************
*                                             INCLUDE FILES
***************************************************************************************************
*/
#include <sal_internal.h>
#include <gic_enum.h>

/*
***************************************************************************************************
*                                             DEFINITIONS
***************************************************************************************************
*/
#define PMIO_EXTN_VA_INTERRUPT_SRC_PMIC_RST  ((uint32_t)(GIC_EXT3))
#define PMIO_EXTN_VA_INTERRUPT_PRIO_PMIC_RST  (1UL)


//Sample Power Pin for D3
#define PMIO_EXTN_VA_STR_MODE_PIN           15UL
#define PMIO_EXTN_VA_STR_MODE_GPIO          (GPIO_GPK(PMIO_EXTN_VA_STR_MODE_PIN))

#define PMIO_EXTN_VA_PWR_EN_PIN             10UL
#define PMIO_EXTN_VA_PWR_EN_GPIO            (GPIO_GPK(PMIO_EXTN_VA_PWR_EN_PIN))

#define PMIO_EXTN_VA_ALIVE_PWR_CON_PIN      4UL
#define PMIO_EXTN_VA_ALIVE_PWR_CON_GPIO     (GPIO_GPB(PMIO_EXTN_VA_ALIVE_PWR_CON_PIN))

#define PMIO_EXTN_VA_PMIC_RESET_OUT_PIN     15UL
#define PMIO_EXTN_VA_PMIC_RESET_OUT_GPIO    (GPIO_GPA(PMIO_EXTN_VA_PMIC_RESET_OUT_PIN))

#define PMIO_EXTN_VA_TCC_RESET_PIN          19UL
#define PMIO_EXTN_VA_TCC_RESET_GPIO         (GPIO_GPA(PMIO_EXTN_VA_TCC_RESET_PIN))
/*
***************************************************************************************************
*                                             LOCAL VARIABLES
***************************************************************************************************
*/

typedef struct
{
    uint32_t uiPwrEnPin;
    uint32_t uiAlivePwrConPin;
    uint32_t uiStrModePin;
    uint32_t uiPmicRstPin;
    uint32_t uiTccRstPin;
}PMIOEXTNExtPwrPins_t;

/*
***************************************************************************************************
*                                         FUNCTION PROTOTYPES
***************************************************************************************************
*/
void PMIO_EXTN_Init
(
    PMIOEXTNExtPwrPins_t tExtPwrPins
);

void PMIO_EXTN_SetStr
(
    uint8_t ucEn
);

void PMIO_EXTN_SetPower
(
    uint8_t ucEn
);

uint32_t PMIO_EXTN_GetUsingGpk32
(
    void
);


#endif /*MCU_BSP_PMIO_DEV_EXTN_PW_CTL_HEADER*/
