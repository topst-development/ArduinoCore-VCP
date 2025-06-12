// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : gpsb_reg.h
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#if !defined(GPSB_TCC805X_HEADER)

#define GPSB_TCC805X_HEADER
/*
***************************************************************************************************
*                                             INCLUDE FILES
***************************************************************************************************
*/

#include <gpsb.h>
#include <clock_dev.h>

/*
***************************************************************************************************
*                                             DEFINITIONS
***************************************************************************************************
*/
#define GPSB_SCLK                       (uint32)(0UL)
#define GPSB_CS                         (uint32)(1UL)
#define GPSB_SDO                        (uint32)(2UL)
#define GPSB_SDI                        (uint32)(3UL)
#define GPSB_FUNC                       (uint32)(4UL)
#define GPSB_PERISEL                    (uint32)(5UL)
#define GPSB_PERICH                     (uint32)(6UL)

#define GPSB_MFIO_PERISEL               (uint32)(5UL)
#define GPSB_MFIO_CHSEL                 (uint32)(6UL)
#define GPSB_MFIO_TYPE                  (uint32)(7UL)
#define GPSB_MFIO_CHNUM                 (uint32)(8UL)

#define DEDICATED_DMA_NUM               (uint32)(5UL)

#define GPSB_CH_0                       (0UL)
#define GPSB_CH_1                       (1UL)
#define GPSB_CH_2                       (2UL)
#define GPSB_CH_3                       (3UL)
#define GPSB_CH_4                       (4UL)

/* GPSB register address */
#define GPSB0_BASE                      ((uint32)MCU_BSP_GPSB_BASE + (uint32)0x00000UL)
#define GPSB1_BASE                      ((uint32)MCU_BSP_GPSB_BASE + (uint32)0x10000UL)
#define GPSB2_BASE                      ((uint32)MCU_BSP_GPSB_BASE + (uint32)0x20000UL)
#define GPSB3_BASE                      ((uint32)MCU_BSP_GPSB_BASE + (uint32)0x30000UL)
#define GPSB4_BASE                      ((uint32)MCU_BSP_GPSB_BASE + (uint32)0x40000UL)
#define GPSB5_BASE                      ((uint32)MCU_BSP_GPSB_BASE + (uint32)0x50000UL)

#define GPSB_PCFG_BASE                  ((uint32)MCU_BSP_GPSB_BASE + (uint32)0xF0000UL)
#define GPSB_IO_MONITOR_BASE            ((uint32)MCU_BSP_GPSB_BASE + (uint32)0xE0000UL)
//#define GPSB_PORT_CFG(x)                (GPSB_PCFG_BASE + (0x4UL * (x)))
#define GPSB_CH_BASE(x)                 (MCU_BSP_GPSB_BASE + ((x) * 0x10000UL))

/* GPSB Control register */
#define GPSB_PORT                       (uint32)(0x00UL) /* Data port */
#define GPSB_STAT                       (uint32)(0x04UL) /* Status register */
#define GPSB_INTEN                      (uint32)(0x08UL) /* Interrupt enable */
#define GPSB_MODE                       (uint32)(0x0CUL) /* Mode register */
#define GPSB_CTRL                       (uint32)(0x10UL) /* Control register */
#define GPSB_EVTCTRL                    (uint32)(0x14UL) /* Counter and Ext. Event Control */
#define GPSB_CCV                        (uint32)(0x18UL) /* Counter current value */
#define GPSB_TXBASE                     (uint32)(0x20UL) /* TX base address register */
#define GPSB_RXBASE                     (uint32)(0x24UL) /* RX base address register */
#define GPSB_PACKET                     (uint32)(0x28UL) /* Packet register */
#define GPSB_DMACTR                     (uint32)(0x2CUL) /* DMA control register */
#define GPSB_DMASTR                     (uint32)(0x30UL) /* DMA status register */
#define GPSB_DMAICR                     (uint32)(0x34UL) /* DMA interrupt control register */

/* GPSB Port Configuration */
#define GPSB_LB_CFG                     (uint32)(0x00UL)
#define GPSB_PCFG_WR_PW                 (uint32)(0x04UL)
#define GPSB_PCFG_WR_LOCK               (uint32)(0x08UL)
#define GPSB_PCFG_WR_EN_BIT             (BSP_BIT_00)

/* GPSB IO Monitor */
#define GPSB0_IOMON                     (uint32)(0x00UL)
#define GPSB1_IOMON                     (uint32)(0x04UL)
#define GPSB2_IOMON                     (uint32)(0x08UL)
#define GPSB3_IOMON                     (uint32)(0x0CUL)
#define GPSB4_IOMON                     (uint32)(0x10UL)
#define GPSB5_IOMON                     (uint32)(0x14UL)
#define GPSB_IOMON_CLR                  (uint32)(0x18UL)

/* IO monitor errors */
#define GPSB_SM_ERR_NONE                (uint32)(0x0UL)
#define GPSB_SM_ERR_STUCK               (uint32)(0x1UL)
#define GPSB_SM_ERR_PROTOCOL            (uint32)(0x2UL)
#define GPSB_SM_ERR_ALL                 (uint32)(0x3UL)

#define GPSB_PORT_PSWD                  (uint32)(0x5AFEACE5UL)

#define GPSB_CH_NUM                     (uint32)(5UL)
#define GPSB_PORT_NUM                   (uint32)(14UL)

#define GPSB_INTERFACE_SDM              (uint32)(0x00UL)
#define GPSB_INTERFACE_RVC              (uint32)(0x01UL)

/* Write Lock */
#define GPSB_BIT_WR_LOCK                (uint32)(0x1UL)

/* GPSB Tx/Rx FIFO Threshold for irq/dma req. */
#define GPSB_CFGWTH                     (uint32)(1UL)
#define GPSB_CFGRTH                     (uint32)(0UL)

#define GPSB_WCNT_MASK					(uint32)(0xFF000000)
#define GPSB_RCNT_MASK					(uint32)(0x00FF0000)
#define GPSB_RCNT_SHIFT					(uint32)(16)

/* Default clock divider load value */
#define GPSB_DEF_DIVLDV                 (uint32)(0UL)

extern GPSBDev_t                gpsb[GPSB_CH_NUM + 1UL];
extern const GPSBSafetyBase_t   gpsb_sm[GPSB_CH_NUM + 1UL];
extern const uint32             gpsbport[GPSB_PORT_NUM][9];

#endif


