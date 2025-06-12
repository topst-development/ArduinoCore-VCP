// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : uart_drv.h
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/
#ifndef UART_DRV_HEADER
#define UART_DRV_HEADER

#include <stdbool.h>

// UART Settings
#define UART_BUFF_SIZE                  (0x100UL)   // 256

#define UART_MODE_TX                    (0UL)
#define UART_MODE_RX                    (1UL)

#define UART_PORT_CFG_MAX               (16U)
#define UART_PORT_TBL_SIZE              (UART_PORT_CFG_MAX)

// GDMA Address
#define GDMA_ADDRESS_OFFSET             (0UL)
#define GDMA_ADDRESS_UNIT               (GDMA_BUFF_SIZE + GDMA_BUFF_MARGIN)
#define GDMA_ADDRESS_UNIT_CH_RX(ch)     ((uint32_t)GDMA_ADDRESS_OFFSET + ((GDMA_ADDRESS_UNIT * 2UL) * (ch)))
#define GDMA_ADDRESS_UNIT_CH_TX(ch)     (((uint32_t)GDMA_ADDRESS_OFFSET + ((GDMA_ADDRESS_UNIT * 2UL) * (ch))) + GDMA_ADDRESS_UNIT)

// channel number : 0, 1, 2, 3
// baudrate : 9600, 14400, 19200, 38400, 57600, 115200, 230400, 460800, 921600 etc.
// flow control. Enable : 1, Disable : 0
// mode : UART_POLLING_MODE, UART_INTR_MODE, UART_DMA_MODE

#define TCC_GPNONE                      (0xFFFFUL)
#define TCC_GPSD0(x)                    (TCC_GPNONE)
#define TCC_GPSD1(x)                    (TCC_GPNONE)
#define TCC_GPSD2(x)                    (TCC_GPNONE)

// UART FIFO buffer size
#define UART_TX_FIFO_SIZE               (8UL)
#define UART_RX_FIFO_SIZE               (12UL)

// UART Base address
#define UART_GET_BASE(n)                (MCU_BSP_UART_BASE + (0x10000UL * (n)))

// Raw Interrupt Status Register (RIS) Fields
// Masked Interrupt Status Register (MIS) Fields
// Interrupt Clear Register (ICR) Fields
#define UART_INT_OEIS                   (1UL << 10)   // Overrun error interrupt
#define UART_INT_BEIS                   (1UL << 9)    // Break error interrupt
#define UART_INT_PEIS                   (1UL << 8)    // Parity error interrupt
#define UART_INT_FEIS                   (1UL << 7)    // Framing error interrupt
#define UART_INT_RTIS                   (1UL << 6)    // Receive timeout interrupt
#define UART_INT_TXIS                   (1UL << 5)    // Transmit interrupt
#define UART_INT_RXIS                   (1UL << 4)    // Receive interrupt

// DMA Control Register (DMACR) Fields
#define UART_DMACR_DMAONERR             (1UL << 2)    // DMA on error
#define UART_DMACR_TXDMAE               (1UL << 1)    // Transmit DMA enable
#define UART_DMACR_RXDMAE               (1UL << 0)    // Receive DMA enable

// UART Interrupt mode
typedef struct UART_INTERRUPT_DATA
{
    uint8_t *                             iXmitBuf;
    int32_t                              iHead;
    int32_t                              iTail;
    int32_t                              iSize;
} UartInterruptData_t;

typedef struct UART_BOARD_PORT
{
    uint32_t                              bPortCfg;   // Config port ID
    uint32_t                              bPortTx;    // UT_TXD GPIO
    uint32_t                              bPortRx;    // UT_RXD GPIO
    uint32_t                              bPortRts;   // UT_RTS GPIO
    uint32_t                              bPortCts;   // UT_CTS GPIO
    uint32_t                              bPortFs;    // UART function select
    uint32_t                              bPortCH;    // Channel
} UartBoardPort_t;

typedef struct UART_STATUS
{
    bool                             sIsProbed;
    uint32_t                              sBase;      // UART Controller base address
    uint8_t                               sCh;        // UART Channel
    uint8_t                               sOpMode;    // Operation Mode
    uint8_t                               sCtsRts;    // CTS and RTS
    uint8_t                               sWordLength;    // Word Length
    uint8_t                               s2StopBit;   // 1: two stop bits are transmitted
    uint8_t                               sParity;    // 0:disable, 1:enable
    UartBoardPort_t                     sPort;      // GPIO Port Infomation
    GDMAInformation_t                   sRxDma;     // Rx DMA
    GDMAInformation_t                   sTxDma;     // Tx DMA
    UartInterruptData_t                 sRxIntr;    // Rx Interrupt
    UartInterruptData_t                 sTxIntr;    // Tx Interrupt
} UartStatus_t;

#endif /* _PLATFORM_TCC_UART_H_ */

