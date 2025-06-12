// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : gpsb.h
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#ifndef MCU_BSP_GPSB_HEADER
#define MCU_BSP_GPSB_HEADER

#ifdef __cplusplus
extern "C" {
#endif

//hsj #if ( MCU_BSP_SUPPORT_DRIVER_GPSB == 1 )

/*
***************************************************************************************************
*                                             INCLUDE FILES
***************************************************************************************************
*/

#include <clock.h>
#include <bsp.h>
#include <debug.h>
#include <gpio.h>
#include <gic.h>
#include <sal_internal.h>
#include "Arduino.h"

/* GPSB Debug message control */
#ifdef DEBUG_ENABLE
    #define GPSB_D(fmt, args...)        {LOGD(DBG_TAG_GPSB, fmt, ## args)}
    #define GPSB_E(fmt, args...)        {LOGE(DBG_TAG_GPSB, fmt, ## args)}
#else
    #define GPSB_D(fmt, args...)
    #define GPSB_E(fmt, args...)
#endif

#define GPSB_FAILED                     (-1)
#define GPSB_PASSED                     (0U)

#define GPSB_MASTER_MODE                (0x0UL)
#define GPSB_SLAVE_MODE                 (0x1UL)
#define UINT_MAX_VALUE                  (0xffffffffUL)

#define GPSB_CH_PORT_NUM                (uint32)(3UL)

/* SDM & RVC External port */
#define SDM_RVC_PORT_MASK               (uint32)(0x3fUL)
#define SDM_PORT_SHIFT                  (uint32)(16UL)
#define RVC_PORT_SHIFT                  (uint32)(24UL)

/* GPSB master transfer mode */
#define GPSB_XFER_PIO                   (uint32)(0x01UL)    /* PIO mode */
#define GPSB_XFER_DMA                   (uint32)(0x02UL)    /* DMA mode*/

/* GPSB clocking mode */
#define GPSB_CPHA                       (uint32)(0x01UL)
#define GPSB_CPOL                       (uint32)(0x02UL)

#define GPSB_MODE_0                     ((uint32)0 | (uint32)0)
#define GPSB_MODE_1                     ((uint32)0 | (uint32)GPSB_CPHA)
#define GPSB_MODE_2                     ((uint32)GPSB_CPOL | (uint32)0)
#define GPSB_MODE_3                     ((uint32)GPSB_CPOL | (uint32)GPSB_CPHA)

#define GPSB_CS_HIGH                    (uint32)(0x04UL)
#define GPSB_LSB_FIRST                  (uint32)(0x08UL)
#define GPSB_LOOP                       (uint32)(0x10UL)

#define GPSB_SLAVE                      (uint32)(0x20UL)

/* GPSB Event */
#define GPSB_EVENT_COMPLETE             (uint32)(0x0000UL)    /* Transfer complete */
#define GPSB_EVENT_ERR_ROR              (uint32)(0x0100UL)    /* Read FIFO over-run error */
#define GPSB_EVENT_ERR_WUR              (uint32)(0x0200UL)    /* Write FIFO under-run error */
#define GPSB_EVENT_ERR_RUR              (uint32)(0x0400UL)    /* Read FIFO under-run error */
#define GPSB_EVENT_ERR_WOR              (uint32)(0x0800UL)    /* Write FIFO over-run error */

/* State of GPSB transfer */
#define GPSB_XFER_MODE_WITHOUT_INTERRUPT    (uint32)(0x01UL)  /* w/o interrupt mode */
#define GPSB_XFER_MODE_WITH_INTERRUPT       (uint32)(0x02UL)  /* w/ interrupt mode */
#define GPSB_XFER_MODE_DMA                  (uint32)(0x10UL)  /* DMA mode */
#define GPSB_XFER_MODE_PIO                  (uint32)(0x20UL)  /* Non-DMA mode */
#define GPSB_XFER_MODE_WITHOUT_CTF          (uint32)(0x40UL)  /* w/o GPSB CTF mode */

#define GPSB_XFER_MODE_WITH_INTR_WITH_DMA   (GPSB_XFER_MODE_WITH_INTERRUPT | GPSB_XFER_MODE_DMA)
#define GPSB_XFER_MODE_WITH_INTR_WITH_PIO   (GPSB_XFER_MODE_WITH_INTERRUPT | GPSB_XFER_MODE_PIO)

#define GPSB_XFER_STATE_DISABLED        (uint8)(0x00UL)    /* GPSB is disabled */
#define GPSB_XFER_STATE_IDLE            (uint8)(0x01UL)    /* GPSB is in idle state */
#define GPSB_XFER_STATE_RUNNING         (uint8)(0x02UL)    /* GPSB is in blocking transfer */
#define GPSB_XFER_STATE_MASK            (GPSB_XFER_STATE_DISABLED | GPSB_XFER_STATE_IDLE | GPSB_XFER_STATE_RUNNING)

/* Get Physical address */
#define GPSB_GET_PHY_ADDR(v_addr)       ((uint32)(v_addr))

/* Get Non-cacheable memory address */
#define GPSB_DMA_BUF_ADDR(x)            ((uint32)(x))

#define GPSB_POLL_TIMEOUT               (uint32)(1000000UL)

#define GPSB_PORT_SEL_MASK              (uint32)(0x3FUL)

/* IO monitor modes */
#define GPSB_SM_MON_PROTOCOL            (uint32)(0UL)
#define GPSB_SM_MON_STUCK_HIGH          (uint32)(1UL)
#define GPSB_SM_MON_STUCK_LOW           (uint32)(2UL)
#define GPSB_SM_MON_STUCK_BOTH          (uint32)(3UL)

typedef void                            (*GPSBCallback)(uint32 uiCh, uint32 iEvent, void *pArg);

typedef struct GPSBXferComplete
{
    GPSBCallback                        xcCallback;
    void *                              xcArg;
} GPSBXferComplite_t;

typedef struct GPSBDmaBuff
{
    uint32 *                            dbAddr;
    uint32                              dbSize;
} GPSBDmaBuff_t;

typedef struct GPSBDev
{
    uint32                              dChannel;           /* GPSB channel */
    uint32                              dBase;              /* GPSB base address */
    uint32                              dCfgAddr;           /* GPSB Port Configuration address */
    uint32                              dPeriName;          /* Peri. clock number */
    sint32                              dIobusName;         /* IOBUS number */
    uint32                              dIsSlave;           /* GPSB mode (Master or Slave)*/
    uint32                              dSdo;               /* GPIO information  */
    uint32                              dSdi;
    uint32                              dSclk;
    uint32                              dPort;
    uint32                              dIntrPriority;      /* Interrupt priority */
    uint32                              dSupportDma;

    GPSBDmaBuff_t                       dTxDma;
    GPSBDmaBuff_t                       dRxDma;

    const uint32 *                      dAsyncTxBuf;
    uint32 *                            dAsyncRxBuf;
    uint32                              dAsyncTxDataSize;
    uint32                              dAsyncRxDataSize;

    uint8                               dState;             /* State of GPSB */
    GPSBXferComplite_t                  dComplete;          /* Notify the completion of non-blocking transfer */
} GPSBDev_t;

typedef struct GPSBSafetyParam
{
    uint32                              spCh;               /* channel number */
    uint32                              spLoopCh;           /* master channel for loopback */
    uint32                              spMonitorMode;      /* I/O monitor mode */
    uint16                              spWidth;            /* counter comparing bit width */
} GPSBSafetyParam_t;

typedef struct GPSBSafetyBase
{
    uint32                              sbBaseAddr;
} GPSBSafetyBase_t;

typedef struct GPSBOpenParam
{
    uint32                              uiSdo;
    uint32                              uiSdi;
    uint32                              uiSclk;
    uint32 *                            pDmaAddrTx;
    uint32 *                            pDmaAddrRx;
    uint32                              uiDmaBufSize;
    GPSBCallback                        fbCallback;
    void *                              pArg;
	uint32								uiIsSlave;
} GPSBOpenParam_t;

/*
***************************************************************************************************
*                                         FUNCTION PROTOTYPES
***************************************************************************************************
*/


/*
***************************************************************************************************
*                                          GPSB_SetSpeed
*
* Function to set GPSB operattion clock speed. (= SCLK)
*
* @param    uiCh [in]       : Value of channel to control
* @param    uiSpeedInHz [in]: SCLK speed
* @return   SAL_RET_SUCCESS or SAL_RET_FAILED
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_SetSpeed
(
    uint32                              uiCh,
    uint32                              uiSpeedInHz
);

/*
***************************************************************************************************
*                                          GPSB_SetBpw
*
* Function to set transfer bpw value.
*
* @param    uiCh [in]       : Value of channel to control
* @param    ucBpw [in]      : Bit per word
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_SetBpw
(
    uint32                               uiCh,
    uint32                               uiBpw
);

/*
***************************************************************************************************
*                                          GPSB_SetBpwNoSwap
*
* Function to set transfer bpw value.
* @param    uiCh [in]       : Value of channel to control
* @param    ucBpw [in]      : Bit per word
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*           If you don't have a specific reason, please use tcc_GPSB_master_set_bpw().
*           In some cases, need to send/receive the data without half-word and byte swapping.
*
***************************************************************************************************
*/

SALRetCode_t GPSB_SetBpwNoSwap
(
    uint32                               uiCh,
    uint32                               uiBpw
);

/*
***************************************************************************************************
*                                          GPSB_SetMode
*
* Function to set gpsb operate mode.
*
* @param    uiCh [in]       : Value of channel to control
* @param    uiMode [in]     : Mode
* @return
* Notes
*
***************************************************************************************************
*/

void GPSB_SetMode
(
    uint32                              uiCh,
    uint32                              uiMode
);

/*
***************************************************************************************************
*                                          GPSB_CsAlloc
*
* Function to config CS GPIO.
*
* @param    uiCh [in]       : Value of channel to control
* @param    iCs [in]        : CS GPIO index
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_CsAlloc
(
    uint32                              uiCh,
    uint32                              uiCs
);

/*
***************************************************************************************************
*                                          GPSB_CsInit
*
* Function to initialize CS GPIO to default config.
*
* @param    uiCh [in]       : Value of channel to control
* @param    iCs [in]        : CS GPIO index
* @param    ucCsHigh [in]   : default high or low
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_CsInit
(
    uint32                              uiCh,
    uint32                              uiCs,
    uint32                              uiCsHigh
);

/*
***************************************************************************************************
*                                          GPSB_CsActivate
* Function to set active CS GPIO.
*
* @param    uiCh [in]       : Value of channel to control
* @param    iCs [in]        : CS GPIO index
* @param    ucCsHigh        : Default High or Low
* @return
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_CsActivate
(
    uint32                              uiCh,
    uint32                              uiCs,
    uint32                              uiCsHigh
);

/*
***************************************************************************************************
*                                          GPSB_CsDeactivate
*
* Function to set active CS GPIO.
*
* @param    uiCh [in]       : Value of channel to control
* @param    iCs [in]        : CS GPIO index
* @param    ucCsHigh [in]   : Default High or Low
* @return
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_CsDeactivate
(
    uint32                              uiCh,
    uint32                              uiCs,
    uint32                              uiCsHigh
);

/*
***************************************************************************************************
*                                          GPSB_AsyncXfer
*
* Function to transfer data. this function can be called from application.
*
* @param    uiCh [in]       : Value of channel to control
* @param    pTx [in]        : TX data buffer
* @param    pRx [out]       : RX data buffer
* @param    uiDataSize [in] : Write/Read data size
* @param    ucXferMode [in] : Transfer mode (DMA/PIO , Interrupt/Non-Interrupt)
* @return   SAl_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_AsyncXfer
(
    uint32                              uiCh,
    const uint32 *                      pTx,
    uint32 *                            pRx,
    uint32                              uiDataSize,
    uint32                              uiXferMode
);

/*
***************************************************************************************************
*                                          GPSB_Xfer
*
* Function to transfer data. this function can be called from application.
*
* @param    uiCh [in]       : Value of channel to control
* @param    pTx [in]        : TX data buffer
* @param    pRx [out]       : RX data buffer
* @param    uiDataSize [in] : Write/Read data size
* @param    ucXferMode [in] : Transfer mode (DMA/PIO , Interrupt/Non-Interrupt)
* @return   SAl_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_Xfer
(
    uint32                              uiCh,
    const uint8 *                       pTx,
    uint8 *                             pRx,
    uint32                              uiDataSize,
    uint32                              uiXferMode
);

/*
***************************************************************************************************
*                                          GPSB_Open
*
* Function to initialize GPSB channel.
*
* @param    uiCh [in]           : Value of channel to control
* @param    OpenParam [in]      : Parameter of GPSB configuration
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_Open
(
    uint32                              uiCh,
    GPSBOpenParam_t                     OpenParam
);

/*
***************************************************************************************************
*                                          GPSB_Close
*
* Function to de-init GPSB channel.
*
* @param    uiCh [in]       : Value of channel to control
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_Close
(
    uint32                              uiCh
);

/*
***************************************************************************************************
*                                          GPSB_Init
*
* Function to register interrupt handle and enable interrupt.
*
* @return
* Notes
*
***************************************************************************************************
*/

void GPSB_Init
(
    void
);

/*
***************************************************************************************************
*                                          GPSB_Deinit
*
* Function to disable interrupt.
*
* @return
* Notes
*
***************************************************************************************************
*/

void GPSB_Deinit
(
    void
);

/*
***************************************************************************************************
*                                          GPSB_Reset
*
* Function to reset gpsb channel.
*
* @param    uiCh [in]       : Value of channel to control
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*           Reset GPSB block
*
***************************************************************************************************
*/

SALRetCode_t GPSB_Reset
(
    uint32                              uiCh
);

/*
***************************************************************************************************
*                                          GPSB_SetSdmChannel
*
* Function to set sdm channel.
*
* @param    uiCh [in]       : Value of channel to control
* @return   GPSB_PASSED or GPSB_FAILED
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_SetSdmChannel
(
    uint32                              uiCh
);

/*
***************************************************************************************************
*                                          GPSB_SetRvcChannel
*
* Function to set rvc channel.
*
* @param    uiCh [in]       : Value of channel to control
* @return   GPSB_PASSED or GPSB_FAILED
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_SetRvcChannel
(
    uint32                              uiCh
);

/*
***************************************************************************************************
*                                          GPSB_SetLbCfg
*
* Function to set channel to enable loopback.
*
* @param    ucMstCh [in]    : Master Channel index
* @param    ucLoopCh [in]   : Loopback Channel index
* @param    ucEnable [in]   : Loopback enable
* @return   GPSB_PASSED or GPSB_FAILED
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_SetLbCfg
(
    uint32                              uiMstCh,
    uint32                              uiLoopCh,
    uint32                              uiEnable
);

/*
***************************************************************************************************
*                                          GPSB_ClearLbCfg
*
* Function to unset channel to disable loopback.
*
* @return   GPSB_PASSED
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_ClearLbCfg
(
    void
);

/*
***************************************************************************************************
*                                          GPSB_SetSdmExtPort
*
* Function to set sdm external hw port info.
*
* @param    iPort [in]      : Port number connected sdm.
* @return   GPSB_PASSED or GPSB_FAILED
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_SetSdmExtPort
(
    uint32                              uiPort
);

/*
***************************************************************************************************
*                                          GPSB_SetRvcExtPort
*
* Function to set rvc external hw port info.
*
* @param    iPort [in]      : Port number connected rvc.
* @return   GPSB_PASSED or GPSB_FAILED
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_SetRvcExtPort
(
    uint32                              uiPort
);

/*
***************************************************************************************************
*                                          GPSB_SafetyEnableLock
*
* Function to enable write lock.
*
* @return
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_SafetyEnableLock
(
    void
);

/*
***************************************************************************************************
*                                          GPSB_SafetyDisableLock
*
* Function to disable write lock.
*
* @return
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_SafetyDisableLock
(
    void
);

/*
***************************************************************************************************
*                                          GPSB_SafetyEnableLoopback
*
* Function to enable loopback between master & slave channel.
*
* @param    uiMstCh [in] : Master channel index
* @param    uiSlvCh [in] : Slave channel index
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_SafetyEnableLoopback
(
    uint32                              uiMstCh,
    uint32                              uiSlvCh
);

/*
***************************************************************************************************
*                                          GPSB_SafetyDisableLoopback
*
* Function to disable loopback between master & slave channel.
*
* @param    uiMstCh [in] : Master channel index
* @param    uiSlvCh [in] : Slave channel index
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_SafetyDisableLoopback
(
    uint32                              uiMstCh,
    uint32                              uiSlvCh
);

/*
***************************************************************************************************
*                                          GPSB_SafetyEnableMonitor
*
* Function to enable safety monitor.
*
* @param    uiCh [in]       : Value of channel to control
* @param    uiMonMode[in]   : Select Protocol/Data stuck monitor
* @param    uiMonWidth      : Value of width of monitor
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_SafetyEnableMonitor
(
    uint32                              uiCh,
    uint32                              uiMonMode,
    uint32                              uiMonWidth
);

/*
***************************************************************************************************
*                                          GPSB_SafetyDisableMonitor
*
* Function to disable safety monitor.
*
* @param    uiCh [in]       : Value of channel to control
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_SafetyDisableMonitor
(
    uint32                              uiCh
);

/*
***************************************************************************************************
*                                          GPSB_SafetyGetStatus
* Function to get safety monitor status.
*
* @param    uiCh [in]       : Value of channel to control
* @return   GPSB_SM_ERR_NONE or GPSB_SM_ERR
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_SafetyGetStatus
(
    uint32                              uiCh
);

/*
***************************************************************************************************
*                                          GPSB_FillSlaveTXData
* Function to fill tx data in slave mode.
*
* @param    uiCh [in]       : Value of channel to control
* @param    pBuff [in]      : Buffer pointer
* @param    uiSize [in]     : Data size
* @return   none
* Notes
*
***************************************************************************************************
*/

void GPSB_FillSlaveTXData
(
    uint32          uiCh,
    const void *    pBuff,
    uint32          uiSize
);

/*
***************************************************************************************************
*                                          GPSB_SetSlaveDMAMode
* Function to set dma mode in slave mode.
*
* @param    uiCh [in]       : Value of channel to control
* @param    pTx [in]        : Phsical memory address for slave mode Tx DMA
* @param    pRx [in]        : Phsical memory address for slave mode Rx DMA
* @param    uiDataSize [in] : Data size
* @return   void
* Notes
*
***************************************************************************************************
*/

void GPSB_SetSlaveDMAMode
(
    uint32                              uiCh,
    const void *                        pTx,
    void *                              pRx,
    uint32                              uiDataSize
);

/*
***************************************************************************************************
*                                          GPSB_GetSlaveRXData
* Function to get rx data in slave mode.
*
* @param    uiCh [in]       : Value of channel to control
* @return   GPSB_SM_ERR_NONE or GPSB_SM_ERR
* Notes
*
***************************************************************************************************
*/

uint32 GPSB_GetSlaveRXData
(
    uint32                              uiCh
);

/*
***************************************************************************************************
*                                          GPSB_GetStatus
* Function to get status register value.
*
* @param    uiCh [in]       : Value of channel to control
* @return   rx data
* Notes
*
***************************************************************************************************
*/

uint32 GPSB_GetStatus
(
    uint32                              uiCh
);

//hsj #endif  // ( MCU_BSP_SUPPORT_DRIVER_GPSB == 1 )

#ifdef __cplusplus
}
#endif

#endif  // MCU_BSP_GPSB_HEADER

