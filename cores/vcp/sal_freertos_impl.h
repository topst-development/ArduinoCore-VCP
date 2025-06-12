// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : sal_freertos_impl.h
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#if !defined(SAL_FREERTOS_IMPL_HEADER)
#define SAL_FREERTOS_IMPL_HEADER

/*
***************************************************************************************************
*                                             [INCLUDE FILES]
***************************************************************************************************
*/

#include <sal_internal.h>

/*
***************************************************************************************************
*                                             [DEFINITIONS]
***************************************************************************************************
*/

#ifdef __cplusplus
extern "C"
{
#endif

/*
***************************************************************************************************
*                                         [FUNCTION PROTOTYPES]
***************************************************************************************************
*/
/* Defined on ASM */
extern void CPU_DSB
(
    void
);

extern uint32_t CPU_SR_Save
(
    void
);

extern void CPU_SR_Restore
(
    uint32_t uiStatus
);

extern void CPU_WaitForEvent
(
    void
);

#ifdef __cplusplus
}
#endif

#endif // SAL_FREERTOS_IMPL_HEADER

