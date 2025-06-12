// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : adc.h
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#ifndef MCU_BSU_ADC_HEADER
#define MCU_BSU_ADC_HEADER

#define MCU_BSP_SUPPORT_DRIVER_ADC 1

#if ( MCU_BSP_SUPPORT_DRIVER_ADC == 1 )

#include <sal_internal.h>

#if (DEBUG_ENABLE)
    #include "debug.h"

    #define ADC_D(fmt,args...)          {LOGD(DBG_TAG_ADC, fmt, ## args)}
    #define ADC_E(fmt,args...)          {LOGE(DBG_TAG_ADC, fmt, ## args)}
#else
    #define ADC_D(fmt,args...)          {}
    #define ADC_E(fmt,args...)          {}
#endif

/*
***************************************************************************************************
*                                            GLOBAL DEFINITIONS
***************************************************************************************************
*/
#define ADC_MODULE_0                    (0U)
#define ADC_MODULE_1                    (1U)
#define ADC_CONV_TIMEOUT                (2UL)

#define ADC_ERR_INVALID_MODULE          (-1)


typedef enum ADCChannel
{
    ADC_CHANNEL_0                       = 0,
    ADC_CHANNEL_1                       = 1,
    ADC_CHANNEL_2                       = 2,
    ADC_CHANNEL_3                       = 3,
    ADC_CHANNEL_4                       = 4,
    ADC_CHANNEL_5                       = 5,
    ADC_CHANNEL_6                       = 6,
    ADC_CHANNEL_7                       = 7,
    ADC_CHANNEL_8                       = 8,
    ADC_CHANNEL_9                       = 9,
    ADC_CHANNEL_10                      = 10,
    ADC_CHANNEL_11                      = 11,
    ADC_CHANNEL_12                      = 12,
    ADC_CHANNEL_13                      = 13,
    ADC_CHANNEL_MAX                     = 14
} ADCChannel_t;

typedef enum ADCMode
{
    ADC_MODE_NORMAL                     = 0,
    ADC_MODE_IRQ                        = 1,
    ADC_MODE_DMA                        = 2
} ADCMode_t;

/*
***************************************************************************************************
*                                         FUNCTION PROTOTYPES
***************************************************************************************************
*/

/*
***************************************************************************************************
*                                          ADC_Init
*
* @param    Type [in]
* @param    ucModule [in]
*
* Notes
*
***************************************************************************************************
*/

void ADC_Init
(
    uint8                               Type,
    uint8                               ucModule
);

/*
***************************************************************************************************
*                                          ADC_Read
*
* @param    Channel [in]
* @param    ucModule [in]
* @return
*
* Notes
*
***************************************************************************************************
*/

uint32 ADC_Read
(
    ADCChannel_t                        Channel,
    uint8                               ucModule,
    int32                               siWatCh
);

/*
***************************************************************************************************
*                                          ADC_AutoScan
*
* @param    ucModule [in]
*
* @return
*
* Notes
*
***************************************************************************************************
*/

uint32* ADC_AutoScan
(
    uint8                               ucModule,
    int32                               siWatCh
);

/*
***************************************************************************************************
*                                          ADC_DeInit
*
* @param    ucModule [in]
*
* Notes
*
***************************************************************************************************
*/

void ADC_DeInit
(
    uint8                               ucModule
);

#endif  // ( MCU_BSP_SUPPORT_DRIVER_ADC == 1 )

#endif  // MCU_BSU_ADC_HEADER

