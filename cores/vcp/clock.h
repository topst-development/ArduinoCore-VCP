// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : clock.h
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#ifndef MCU_BSP_CLOCK_HEADER
#define MCU_BSP_CLOCK_HEADER

#include <sal_internal.h>
#include <stdint.h>
#include <stdbool.h>

/*
***************************************************************************************************
*                                             DEFINITIONS
***************************************************************************************************
*/

#define CKC_MHZ(x)                      (uint32_t)((x) * 1000000)

/*
***************************************************************************************************
*                                         FUNCTION PROTOTYPES
***************************************************************************************************
*/

void CLOCK_Init
(
    void
);

int32_t CLOCK_SetPllRate
(
    int32_t                              iId,
    uint32_t                              uiRate
);

uint32_t CLOCK_GetPllRate
(
    int32_t                              iId
);

int32_t CLOCK_SetPllDiv
(
    int32_t                              iId,
    uint32_t                              uiPllDiv
);

int32_t CLOCK_SetClkCtrlRate
(
    int32_t                              iId,
    uint32_t                              uiRate
);

uint32_t CLOCK_GetClkCtrlRate
(
    int32_t                              iId
);

int32_t CLOCK_IsPeriEnabled
(
    int32_t                              iId
);

int32_t CLOCK_EnablePeri
(
    int32_t                              iId
);

int32_t CLOCK_DisablePeri
(
    int32_t                              iId
);

uint32_t CLOCK_GetPeriRate
(
    int32_t                              iId
);

int32_t CLOCK_SetPeriRate
(
    int32_t                              iId,
    uint32_t                              uiRate
);

int32_t CLOCK_IsIobusPwdn
(
    int32_t                              iId
);

int32_t CLOCK_EnableIobus
(
    int32_t                              iId,
    bool                             bEn
);

int32_t CLOCK_SetIobusPwdn
(
    int32_t                              iId,
    bool                             bEn
);

int32_t CLOCK_SetSwReset
(
    int32_t                              iId,
    bool                             bReset
);

#endif /* __MCU_BSP_CLOCK_HEADER__ */

