// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : uart.h
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#ifndef UART_HEADER
#define UART_HEADER

#ifdef __cplusplus
extern "C" {
#endif


#include <sal_internal.h>
#include <gic.h>

#ifndef ON
#define ON                              (1U)
#define OFF                             (0U)
#endif

// UART Channels
#define UART_CH0                        (0U)
#define UART_CH1                        (1U)
#define UART_CH2                        (2U)
#define UART_CH3                        (3U)
#define UART_CH4                        (4U)
#define UART_CH5                        (5U)
#define UART_CH_MAX                     (6U)

#define UART_POLLING_MODE               (0U)
#define UART_INTR_MODE                  (1U)
#define UART_DMA_MODE                   (2U)

#define UART_CTSRTS_ON                  (1U)
#define UART_CTSRTS_OFF                 (0U)

/*
***************************************************************************************************
*                                         UART DEFAULT SETTING
***************************************************************************************************
*/

// Set for using UART0 channel
#define UART_DEBUG_CH                   (UART_CH1)
#define UART_DEBUG_CLK                  (48000000UL)    // 48MHz

/*
***************************************************************************************************
*                                         REGISTERS
***************************************************************************************************
*/

// UART Register (BASE Address + Offset)
#define UART_REG_DR                     (0x00UL)    // Data register
#define UART_REG_RSR                    (0x04UL)    // Receive Status register
#define UART_REG_ECR                    (0x04UL)    // Error Clear register
#define UART_REG_FR                     (0x18UL)    // Flag register
#define UART_REG_ILPR                   (0x20UL)    // IrDA Low-power Counter register
#define UART_REG_IBRD                   (0x24UL)    // Integer Baud rate register
#define UART_REG_FBRD                   (0x28UL)    // Fractional Baud rate register
#define UART_REG_LCRH                   (0x2cUL)    // Line Control register
#define UART_REG_CR                     (0x30UL)    // Control register
#define UART_REG_IFLS                   (0x34UL)    // Interrupt FIFO Level status register
#define UART_REG_IMSC                   (0x38UL)    // Interrupt Mask Set/Clear register
#define UART_REG_RIS                    (0x3cUL)    // Raw Interrupt Status register
#define UART_REG_MIS                    (0x40UL)    // Masked Interrupt Status register
#define UART_REG_ICR                    (0x44UL)    // Interrupt Clear register
#define UART_REG_DMACR                  (0x48UL)    // DMA Control register

// UART Flag Register(FR) Fields
#define UART_FR_TXFE                    (1UL << 7)    // Transmit FIFO empty
#define UART_FR_RXFF                    (1UL << 6)    // Receive FIFO full
#define UART_FR_TXFF                    (1UL << 5)    // Transmit FIFO full
#define UART_FR_RXFE                    (1UL << 4)    // Receive FIFO empty
#define UART_FR_BUSY                    (1UL << 3)    // UART busy
#define UART_FR_CTS                     (1UL << 3)    // Clear to send

// UART Line Control Register (LCR_H) Fields
#define UART_LCRH_SPS                   (1UL << 7)    // Stick parity select
#define UART_LCRH_WLEN(x)               ((x) << 5)    // Word length
#define UART_LCRH_FEN                   (1UL << 4)    // Enable FIFOs
#define UART_LCRH_STP2                  (1UL << 3)    // Two stop bits select
#define UART_LCRH_EPS                   (1UL << 2)    // Even parity select
#define UART_LCRH_PEN                   (1UL << 1)    // Parity enable
#define UART_LCRH_BRK                   (1UL << 0)    // Send break

// UART Control Register (CR) Fields
#define UART_CR_CTSEN                   (1UL << 15)   // CTS hardware flow control enable
#define UART_CR_RTSEN                   (1UL << 14)   // RTS hardware flow control enable
#define UART_CR_RTS                     (1UL << 11)   // Request to send
#define UART_CR_RXE                     (1UL << 9)    // Receive enable
#define UART_CR_TXE                     (1UL << 8)    // Transmit enable
#define UART_CR_LBE                     (1UL << 7)    // Loopback enable
#define UART_CR_EN                      (1UL << 0)    // UART enable

#define ENABLE_FIFO                     (1U)
#define DISABLE_FIFO                    (0U)

#define TWO_STOP_BIT_ON                 (1U)
#define TWO_STOP_BIT_OFF                (0U)

enum
{
    WORD_LEN_5 = 0,
    WORD_LEN_6,
    WORD_LEN_7,
    WORD_LEN_8
};

enum
{
    PARITY_SPACE = 0,
    PARITY_EVEN,
    PARITY_ODD,
    PARITY_MARK
};

/*
***************************************************************************************************
*                                         EXTERN FUNCTIONS
***************************************************************************************************
*/

sint32 UART_Open
(
    uint8                               ucCh,
    uint32                              uiPriority,     // Interrupt priority
    uint32                              uiBaudrate,     // Baudrate
    uint8                               ucMode,         // polling or interrupt or udma
    uint8                               ucCtsRts,       // on/off
    uint8                               ucPortCfg,      // port selection
    uint8                               ucWordLength,   // 5~6 bits
    uint8                               ucFIFO,         // on/off
    uint8                               uc2StopBit,     // on/off
    uint8                               ucParity,       // space, even, odd, mark
    GICIsrFunc                          fnCallback      // callback function
);

void UART_Close
(
    uint8                               ucCh
);

sint32 UART_Read
(
    uint8                               ucCh,
    uint8                               *pucBuf,
    uint32                              uiSize
);

sint32 UART_Write
(
    uint8                               ucCh,
    const uint8                         *pucBuf,
    uint32                              uiSize
);

uint32 UART_GetData
(
    uint8                               ucCh,
    sint32                              iWait,
    sint8                               *pcErr
);

sint32 UART_GetChar
(
    uint8                               ucCh,
    sint32                              iWait,
    sint8                               *pcErr
);

sint32 UART_PutChar
(
    uint8                               ucCh,
    uint8                               ucChar
);

void UART_SetLineControlReg
(
    uint8                               ucCh,
    uint32                              uiBits,
    uint8                               uiEnabled
);

void UART_SetInterruptClearReg
(
    uint8                               ucCh,
    uint32                              uiSetValue
);

void UART_SetErrorClearReg
(
    uint8                               ucCh,
    uint32                              uiSetValue
);

uint32 UART_GetReceiveStatusReg
(
    uint8                               ucCh
);

uint32 UART_GetRawInterruptStatusReg
(
    uint8                               ucCh
);

void UART_Init
(
    void
);

void UART_ISR
(
    void *                              pArg
);

#ifdef __cplusplus
}
#endif

#endif /* _PLATFORM_TCC_UART_H_ */

