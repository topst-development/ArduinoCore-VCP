// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : pdm.h
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#ifndef MCU_BSP_PDM_HEADER
#define MCU_BSP_PDM_HEADER

#define MCU_BSP_SUPPORT_DRIVER_PDM 1

#if ( MCU_BSP_SUPPORT_DRIVER_PDM == 1 )

/****************************************************************************************************
*                                             INCLUDE FILES
****************************************************************************************************/
#include "clock.h"
#include "clock_dev.h"
#include "sal_com.h"
#include "pdm_dev.h"

/*
***************************************************************************************************
*                                             DEFINITIONS
***************************************************************************************************
*/
#if (DEBUG_ENABLE)
    #include "debug.h"

    #define PDM_D(fmt, args...)         {LOGD(DBG_TAG_PDM, fmt, ## args)}
    #define PDM_Err(fmt, args...)       {LOGE(DBG_TAG_PDM, fmt, ## args)}
#else
    #define PDM_D(fmt, args...)
    #define PDM_Err(fmt, args...)
#endif

//#define PDM_SAFETY_FEATURE_ENABLED

#define PDM_ON                          (1UL)
#define PDM_OFF                         (0UL)

#define PMM_ON                          (1UL)
#define PMM_OFF                         (0UL)

/* PDM MAX clk 125Mhz */
#define PDM_PERI_CLOCK                  (125UL * 1000UL * 1000UL)

/* Clock divide max value 3(divide 16) */
#define PDM_DIVID_MAX                   (3UL)

#define PDM_STATE_BUSY                  (1UL)
#define PDM_STATE_IDLE                  (0UL)

#define PDM_OUTPUT_PATTERN_HIGH         (0xFFFFFFFFUL)
#define PDM_OUTPUT_PATTERN_LOW          (0x00000000UL)
#define PDM_PERIOD_MAX_VALUE            (0x0FFFFFFFUL)
#define PDM_REG_MAX_VALUE               (0xFFFFFFFFUL)

#define PMM_CONNECT_INTERNAL            (0x00000000UL)
#define PMM_CONNECT_EXTERNAL            (0x00000001UL)

#define PMM_COMMISSION_ENABLE           (0x00000001UL)
#define PMM_COMMISSION_DISABLE          (0x00000000UL)

#define PDM_HW_LIMIT_VALUE_2            (2UL)       /* In PHASE_MODE, output count value : pstnX + '2' */
#define PDM_HW_LIMIT_VALUE_4            (4UL)

#define PDM_TOTAL_MODULES               (3UL)
#define PDM_TOTAL_CH_PER_MODULE         (4UL)
#define PDM_TOTAL_CHANNELS              (PDM_TOTAL_MODULES * PDM_TOTAL_CH_PER_MODULE)

#define PDM_CH_MAX                      (12UL)
#define PDM_OUT_CH_MAX                  (9UL)
#define PDM_OUT_PORT_MAX                (PDM_OUT_CH_MAX * 4UL) /* Max output channel : 9(0~8), port channel : 4(GPIO-A~C & K) */

/* Operation Mode Phase 1 & 2 */
#define PDM_OUTPUT_MODE_PHASE_1         (0x0001UL) /* 0b0001UL => green hills compiler */
#define PDM_OUTPUT_MODE_PHASE_2         (0x0009UL) /* 0b1001UL => green hills compiler */

/* Operation Mode Register 1 & 2 */
#define PDM_OUTPUT_MODE_REGISTER_1      (0x0002UL) /* 0b0010UL => green hills compiler */
#define PDM_OUTPUT_MODE_REGISTER_2      (0x0004UL) /* 0b0100UL => green hills compiler */
#define PDM_OUTPUT_MODE_REGISTER_3      (0x0006UL) /* 0b0110UL => green hills compiler */

#define PDM_ONE_SECOND_TO_NANO          (1000UL * 1000UL * 1000UL)  // express 1 second to nano-second

/****************************************************************************
 * struct
 ****************************************************************************/

enum PDM_CHANNEL
{
    PDM_CHANNEL_0 = 0,
    PDM_CHANNEL_1,
    PDM_CHANNEL_2,
    PDM_CHANNEL_3,
    PDM_CHANNEL_4,
    PDM_CHANNEL_5,
    PDM_CHANNEL_6,
    PDM_CHANNEL_7,
    PDM_CHANNEL_8,
    PDM_CHANNEL_9,      /* not available */
    PDM_CHANNEL_10,     /* not available */
    PDM_CHANNEL_11      /* not available */
};

#ifdef PDM_SAFETY_FEATURE_ENABLED
typedef struct PMMISRData
{
    uint32 idChannel;
    uint32 idPmmIrqState;
} PMMISRData_t;
#endif

typedef struct PDMModeConfig
{
    uint32                              mcPortNumber;           /* output port number           */
    uint32                              mcClockDivide;          /* input clock divide           */
    uint32                              mcOutSignalInIdle;      /* output signal in IDLE state (Only Phase Mode 1 & 2) */
    uint32                              mcInversedSignal;       /* output signal inverse        */
    uint32                              mcOperationMode;        /* operation mode               */
    uint32                              mcOutputCtrl;           /* output control (DO, OEN)     */
    uint32                              mcLoopCount;            /* step repetition value        */

    uint32                              mcPeriodNanoSec1;       /* period       in nanosecond   */
    uint32                              mcDutyNanoSec1;         /* duty cycle   in nanosecond   */
    uint32                              mcPeriodNanoSec2;       /* period       in nanosecond   */
    uint32                              mcDutyNanoSec2;         /* duty cycle   in nanosecond   */

    uint32                              mcPosition1;            /* pstn1 for Phase mode 1,2     */
    uint32                              mcPosition2;            /* pstn2 for Phase mode 1,2     */
    uint32                              mcPosition3;            /* pstn3 for Phase mode 2       */
    uint32                              mcPosition4;            /* pstn4 for Phase mode 2       */

    uint32                              mcOutPattern1;          /* output pattern1 for register out mode 1,2,3  */
    uint32                              mcOutPattern2;          /* output pattern2 for register out mode 2,3    */
    uint32                              mcOutPattern3;          /* output pattern3 for register out mode 2,3    */
    uint32                              mcOutPattern4;          /* output pattern4 for register out mode 2,3    */
    uint32                              mcMaxCount;             /* max count for register mode3                 */
} PDMModeConfig_t;

typedef struct PDMChannelHandler
{
    uint32                              chModuleId;             /* pdm module 0/1/2             */
    uint32                              chChannelId;            /* channel A/B/C/D              */
    uint32                              chEnable;               /* output enable                */
    uint32                              chOutSignalInIdle;
    uint32                              chIdleState;
    PDMModeConfig_t                     chModeCfgInfo;

#ifdef PDM_SAFETY_FEATURE_ENABLED
    boolean                             chPMMErrChannel;        /* last error channel           */
    uint32                              chPMMEnable;            /* monitoring module enable     */
    uint32                              chPMMInputCnect;        /* monitoring module input connection (internal or external) */
    uint32                              chPMMFaultStatus;       /* pdm fault status             */
    uint32                              chPMMTimeoutValue;      /* for test Timeout value       */
    uint32                              chPMMDutyMargin;        /* for test duty margin         */
#endif
} PDMChannelHandler_t;

typedef struct PDMPortConfig
{
    uint32                              pcPdmCh;                /* pdm channel                      */
    uint32                              pcPortNum;              /* pdm output port number           */
    uint32                              pcPortFs;               /* pdm output port function select  */
    uint32                              pcPortSelCh;            /* pdm output port select channel : CH0(GPIO-A), CH1(GPIO-B), CH2(GPIO-C), CH3(GPIO-K) */
} PDMPortConfig_t;

/*
***************************************************************************************************
*                                         FUNCTION PROTOTYPES
***************************************************************************************************
*/

/*
***************************************************************************************************
*                                          PDM_CfgSetWrPw
*
* Function to enter 'write lock password'.
* Notes
*
***************************************************************************************************
*/

void PDM_CfgSetWrPw
(
    void
);

/*
***************************************************************************************************
*                                          PDM_CfgSetWrLock
*
* Function to lock the register.
* Notes
*
***************************************************************************************************
*/

void PDM_CfgSetWrLock
(
    uint32                              uiLock
);

/*
***************************************************************************************************
*                                          PDM_EnableClock
*
* Enable PDM peripheral clock.
* Notes
*
***************************************************************************************************
*/

void PDM_EnableClock
(
    void
);

/*
***************************************************************************************************
*                                          PDM_DisableClock
*
* Disable PDM peripheral clock.
* Notes
*
***************************************************************************************************
*/

void PDM_DisableClock
(
    void
);

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

void PDM_Init
(
    void
);

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

void PDM_Deinit
(
    void
);

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

SALRetCode_t PDM_Enable
(
    uint32                              uiChannel,
    uint32                              uiMonitoring
);

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

SALRetCode_t PDM_Disable
(
    uint32                              uiChannel,
    uint32                              uiMonitoring
);

/*
***************************************************************************************************
*                                          PDM_GetChannelStauts
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

uint32 PDM_GetChannelStatus
(
    uint32                              uiChannel
);


/*
***************************************************************************************************
*                                          PDM_SetConfig
*
* Set Configuration value for PMM
*
* @param    channel number 0 ~ 8.
* @param    Operation Mode Configure Information.
* @return
*
* Notes
*
***************************************************************************************************
*/

SALRetCode_t PDM_SetConfig
(
    uint32                              uiChannel,
    PDMModeConfig_t *                   pModeConfig
);

#endif  // ( MCU_BSP_SUPPORT_DRIVER_PDM == 1 )

#endif  // MCU_BSP_PDM_HEADER

