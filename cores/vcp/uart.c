// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : uart.c
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#include <bsp.h>
#include "reg_phys.h"
#include "gdma.h"
#include "uart_drv.h"
#include "uart.h"
#include "clock.h"
#include "clock_dev.h"
#include "gpio.h"
#include "mpu.h" //mem_get_dma_base

/*
***************************************************************************************************
*                                             DEFINITIONS
***************************************************************************************************
*/

#if (DEBUG_ENABLE)
#   include <debug.h>
#   define UART_D(fmt, args...)          {LOGD(DBG_TAG_UART, fmt, ## args)}
#   define UART_E(fmt, args...)          {LOGE(DBG_TAG_UART, fmt, ## args)}
#else
#   define UART_D(fmt, args...)
#   define UART_E(fmt, args...)
#endif

static UartStatus_t uart[UART_CH_MAX];

static uint8_t uart_buff0[2][UART_BUFF_SIZE];
static uint8_t uart_buff1[2][UART_BUFF_SIZE];
static uint8_t uart_buff2[2][UART_BUFF_SIZE];
static uint8_t uart_buff3[2][UART_BUFF_SIZE];
static uint8_t uart_buff4[2][UART_BUFF_SIZE];
static uint8_t uart_buff5[2][UART_BUFF_SIZE];

static uint8_t *uart_buff[UART_CH_MAX][2] =
{
    { uart_buff0[0], uart_buff0[1] },
    { uart_buff1[0], uart_buff1[1] },
    { uart_buff2[0], uart_buff2[1] },
    { uart_buff3[0], uart_buff3[1] },
    { uart_buff4[0], uart_buff4[1] },
    { uart_buff5[0], uart_buff5[1] }
};

/*
***************************************************************************************************
                                     STATIC FUNCTIONS
***************************************************************************************************
*/

static uint32_t UART_RegRead
(
    uint8_t                               ucCh,
    uint32_t                              uiAddr
);

static void UART_RegWrite
(
    uint8_t                               ucCh,
    uint32_t                              uiAddr,
    uint32_t                              uiSetValue
);

static SALRetCode_t UART_Reset
(
    uint8_t                               ucCh
);

static int32_t UART_SetGpio
(
    uint8_t                               ucCh,
    const UartBoardPort_t *             psInfo
);

static int32_t UART_SetPortConfig
(
    uint8_t                               ucCh,
    uint32_t                              uiPort
);

static SALRetCode_t UART_ClearGpio
(
    uint8_t                               ucCh
);

static int32_t UART_SetChannelConfig
(
    uint8_t                               ucCh,
    uint32_t                              uiBaud,
    uint8_t                               ucWordLength,
    uint8_t                               ucFIFO,
    uint8_t                               uc2StopBit,
    uint8_t                               ucParity
);

static int32_t UART_SetBaudRate
(
    uint8_t                               ucCh,
    uint32_t                              uiBaud
);

static void UART_StatusInit
(
    uint8_t                               ucCh
);

static int32_t UART_Probe
(
    uint8_t                               ucCh,
    uint32_t                              uiPriority,
    uint32_t                              uiBaudrate,
    uint8_t                               ucMode,
    uint8_t                               ucCtsRts,
    uint8_t                               ucPortCfg,
    uint8_t                               ucWordLength,
    uint8_t                               ucFIFO,
    uint8_t                               uc2StopBit,
    uint8_t                               ucParity,
    GICIsrFunc                          fnCallback
);

static int32_t UART_Rx
(
    uint8_t                               ucCh
);

static int32_t UART_TxChar
(
    uint8_t                               ucCh,
    uint8_t                               cChar
);

static int32_t UART_Tx
(
    uint8_t                               ucCh
);

static void UART_DmaRxIrq
(
    uint8_t                               ucCh
);

static void UART_EnableInterrupt
(
    uint8_t                               ucCh,
    uint32_t                              uiPriority,
    uint8_t                               ucFIFO,
    GICIsrFunc                          FnCallback
);

static void UART_DisableInterrupt
(
    uint8_t                               ucCh
);

static void UART_InterruptTxProbe
(
    uint8_t                               ucCh
);

static void UART_InterruptRxProbe
(
    uint8_t                               ucCh
);

static int32_t UART_InterruptWrite
(
    uint8_t                               ucCh,
    const uint8_t *                       pucBuf,
    uint32_t                              uiSize
);

static int32_t UART_InterruptRead
(
    uint8_t                               ucCh,
    uint8_t *                             pucBuf,
    uint32_t                              uiSize
);

static void UART_DmaTxProbe
(
    uint8_t                               ucCh,
    uint32_t *                            puiAddr
);

static void UART_DmaRxProbe
(
    uint8_t                               ucCh,
    uint32_t *                            puiAddr
);

static int32_t UART_DmaTxEnable
(
    uint8_t                               ucCh,
    uint32_t                              uiSize,
    const GDMAInformation_t *           psDmacon
);

static int32_t UART_DmaRxEnable
(
    uint8_t                               ucCh,
    uint32_t                              uiSize,
    const GDMAInformation_t *           psDmacon
);

static int32_t UART_DmaWrite
(
    uint8_t                               ucCh,
    const uint8_t *                       pucBuf,
    uint32_t                              uiSize
);

static int32_t UART_DmaRead
(
    uint8_t                               ucCh,
    uint8_t *                             pucBuf,
    uint32_t                              uiSize
);

static int32_t UART_DmaRxTriggerDma
(
    uint8_t                               ucCh
);


static void UART_DmaRxISR
(
    void * pArg
)
;


/*
***************************************************************************************************
*                                         FUNCTION PROTOTYPES
***************************************************************************************************
*/

static uint32_t UART_RegRead
(
    uint8_t                               ucCh,
    uint32_t                              uiAddr
)
{
    uint32_t uiRet;
    uint32_t uiRegAddr;

    uiRet       = 0;
    if(uart[ucCh].sBase == 0)
    {
        uart[ucCh].sBase = UART_GET_BASE(ucCh);
    }
    uiRegAddr   = uart[ucCh].sBase + uiAddr;
    uiRet       = SAL_ReadReg(uiRegAddr);

    return uiRet;

}

static void UART_RegWrite
(
    uint8_t                               ucCh,
    uint32_t                              uiAddr,
    uint32_t                              uiSetValue
)
{
    uint32_t uiRegAddr;

    if(uart[ucCh].sBase == 0)
    {
        uart[ucCh].sBase = UART_GET_BASE(ucCh);
    }
    uiRegAddr   = uart[ucCh].sBase + uiAddr;
    SAL_WriteReg(uiSetValue, uiRegAddr);

    return;
}

/*
***************************************************************************************************
*                                          UART_Reset
*
* Function to reset UART Channel.
*
* @param    ucCh [in]       : Value of channel to control
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

static SALRetCode_t UART_Reset
(
    uint8_t                               ucCh
)
{
    int32_t  ret;
    int32_t  iClkBusId;

    ret = 0;
    iClkBusId   = (int32_t)CLOCK_IOBUS_UART0 + (int32_t)ucCh;

    /* SW reset */
    ret = CLOCK_SetSwReset(iClkBusId, TRUE);

    if(ret != (int32_t)NULL)
    {
        return SAL_RET_FAILED;
    }

    /* Bit Clear */
    ret = CLOCK_SetSwReset(iClkBusId, FALSE);

    if(ret != (int32_t)NULL)
    {
        return SAL_RET_FAILED;
    }

    return SAL_RET_SUCCESS;
}

static int32_t UART_SetGpio
(
    uint8_t                               ucCh,
    const UartBoardPort_t *             psInfo
)
{
    int32_t          ret;
    SALRetCode_t    ret1;
    SALRetCode_t    ret2;
    SALRetCode_t    ret3;
    SALRetCode_t    ret4;
    SALRetCode_t    retCfg;

    ret = -2;

    if (psInfo != NULL_PTR)
    {
        /* set port controller, channel */
        switch(ucCh)
        {
            case UART_CH0:
                retCfg = GPIO_PerichSel(GPIO_PERICH_SEL_UARTSEL_0, psInfo->bPortCH);
                break;
            case UART_CH1:
                retCfg = GPIO_PerichSel(GPIO_PERICH_SEL_UARTSEL_1, psInfo->bPortCH);
                break;
            case UART_CH2:
                retCfg = GPIO_PerichSel(GPIO_PERICH_SEL_UARTSEL_2, psInfo->bPortCH);
                break;
            case UART_CH3:
                retCfg = GPIO_MfioCfg(GPIO_MFIO_CFG_PERI_SEL0, GPIO_MFIO_UART3, GPIO_MFIO_CFG_CH_SEL0, psInfo->bPortCH);
                break;
            case UART_CH4:
                retCfg = GPIO_MfioCfg(GPIO_MFIO_CFG_PERI_SEL1, GPIO_MFIO_UART4, GPIO_MFIO_CFG_CH_SEL1, psInfo->bPortCH);
                break;
            case UART_CH5:
                retCfg = GPIO_MfioCfg(GPIO_MFIO_CFG_PERI_SEL2, GPIO_MFIO_UART5, GPIO_MFIO_CFG_CH_SEL2, psInfo->bPortCH);
                break;
            default:
                break;
        }

        if (retCfg != SAL_RET_FAILED)
        {
            /* set debug port */
            ret1 = GPIO_Config(psInfo->bPortTx, (psInfo->bPortFs)); // TX
            ret2 = GPIO_Config(psInfo->bPortRx, (psInfo->bPortFs | GPIO_INPUT | GPIO_INPUTBUF_EN)); // RX

            uart[ucCh].sPort.bPortCfg = psInfo->bPortCfg;
            uart[ucCh].sPort.bPortTx = psInfo->bPortTx;
            uart[ucCh].sPort.bPortRx = psInfo->bPortRx;
            uart[ucCh].sPort.bPortFs = psInfo->bPortFs;

            if (uart[ucCh].sCtsRts != 0UL)
            {
                ret3 = GPIO_Config(psInfo->bPortRts, psInfo->bPortFs); // RTS
                ret4 = GPIO_Config(psInfo->bPortCts, psInfo->bPortFs); // CTS

                if((ret1 != SAL_RET_SUCCESS) || (ret2 != SAL_RET_SUCCESS) || (ret3 != SAL_RET_SUCCESS) || (ret4 != SAL_RET_SUCCESS))
                {
                    ret = -1;
                }
                else
                {
                    uart[ucCh].sPort.bPortCts = psInfo->bPortCts;
                    uart[ucCh].sPort.bPortRts = psInfo->bPortRts;
                }
            }
            else if((ret1 != SAL_RET_SUCCESS) || (ret2 != SAL_RET_SUCCESS))
            {
                ret = -1;
            }
            else
            {
                ret = 0;
            }
        }
    }
    else
    {
        ret = -1;
    }

    return ret;
}

static int32_t UART_SetPortConfig
(
    uint8_t                               ucCh,
    uint32_t                              uiPort
)
{
    uint32_t  idx;
    int32_t  ret;
    static const UartBoardPort_t board_serial[UART_PORT_TBL_SIZE] =
    {
        { 0UL,  GPIO_GPA(28UL),   GPIO_GPA(29UL),   TCC_GPNONE,       TCC_GPNONE,       GPIO_FUNC(1UL),   GPIO_PERICH_CH0 },  // CTL_0, CH_0
        { 1UL,  GPIO_GPC(16UL),   GPIO_GPC(17UL),   GPIO_GPC(18UL),   GPIO_GPC(19UL),   GPIO_FUNC(2UL),   GPIO_PERICH_CH1 },  // CTL_0, CH_1

        { 2UL,  GPIO_GPB(8UL),    GPIO_GPB(9UL),    GPIO_GPB(10UL),   GPIO_GPB(11UL),   GPIO_FUNC(1UL),   GPIO_PERICH_CH0 },  // CTL_1, CH_0
        { 3UL,  GPIO_GPA(6UL),    GPIO_GPA(7UL),    GPIO_GPA(8UL),    GPIO_GPA(9UL),    GPIO_FUNC(2UL),   GPIO_PERICH_CH1 },  // CTL_1, CH_1

        { 4UL,  GPIO_GPB(25UL),   GPIO_GPB(26UL),   GPIO_GPB(27UL),   GPIO_GPB(28UL),   GPIO_FUNC(1UL),   GPIO_PERICH_CH0 },  // CTL_2, CH_0
        { 5UL,  GPIO_GPC(0UL),    GPIO_GPC(1UL),    GPIO_GPC(2UL),    GPIO_GPC(3UL),    GPIO_FUNC(2UL),   GPIO_PERICH_CH1 },  // CTL_2, CH_1

        { 6UL,  GPIO_GPA(16UL),   GPIO_GPA(17UL),   GPIO_GPA(18UL),   GPIO_GPA(19UL),   GPIO_FUNC(3UL),   GPIO_MFIO_CH0 },    // CTL_3, CH_0
        { 7UL,  GPIO_GPB(0UL),    GPIO_GPB(1UL),    GPIO_GPB(2UL),    GPIO_GPB(3UL),    GPIO_FUNC(3UL),   GPIO_MFIO_CH1 },    // CTL_3, CH_1
        { 8UL,  GPIO_GPC(4UL),    GPIO_GPC(5UL),    GPIO_GPC(6UL),    GPIO_GPC(7UL),    GPIO_FUNC(3UL),   GPIO_MFIO_CH2 },    // CTL_3, CH_2
        { 9UL,  GPIO_GPK(11UL),   GPIO_GPK(12UL),   GPIO_GPK(13UL),   GPIO_GPK(14UL),   GPIO_FUNC(3UL),   GPIO_MFIO_CH3 },    // CTL_3, CH_3

        { 10UL, GPIO_GPA(20UL),   GPIO_GPA(21UL),   GPIO_GPA(22UL),   GPIO_GPA(23UL),   GPIO_FUNC(3UL),   GPIO_MFIO_CH0 },    // CTL_4, CH_0
        { 11UL, GPIO_GPB(4UL),    GPIO_GPB(5UL),    GPIO_GPB(6UL),    GPIO_GPB(7UL),    GPIO_FUNC(3UL),   GPIO_MFIO_CH1 },    // CTL_4, CH_1
        { 12UL, GPIO_GPC(8UL),    GPIO_GPC(9UL),    GPIO_GPC(10UL),   GPIO_GPC(11UL),   GPIO_FUNC(3UL),   GPIO_MFIO_CH2 },    // CTL_4, CH_2

        { 13UL, GPIO_GPA(24UL),   GPIO_GPA(25UL),   GPIO_GPA(26UL),   GPIO_GPA(27UL),   GPIO_FUNC(3UL),   GPIO_MFIO_CH0 },    // CTL_5, CH_0
        { 14UL, GPIO_GPB(8UL),    GPIO_GPB(9UL),    GPIO_GPB(10UL),   GPIO_GPB(11UL),   GPIO_FUNC(3UL),   GPIO_MFIO_CH1 },    // CTL_5, CH_1
        { 15UL, GPIO_GPC(12UL),   GPIO_GPC(13UL),   GPIO_GPC(14UL),   GPIO_GPC(15UL),   GPIO_FUNC(3UL),   GPIO_MFIO_CH2 },    // CTL_5, CH_2
    };

    ret = 0;

    for(idx = 0UL ; idx <= UART_PORT_CFG_MAX ; idx++)
    {
        if(board_serial[idx].bPortCfg == uiPort)
        {
            ret = UART_SetGpio(ucCh, &board_serial[idx]);
            break;
        }
    }

    return ret;
}

static SALRetCode_t UART_ClearGpio
(
    uint8_t                               ucCh
)
{
    uint32_t          gpio_Tx = 0;
    uint32_t          gpio_Rx = 0;
    uint32_t          gpio_Cts = 0;
    uint32_t          gpio_Rts = 0;
    SALRetCode_t    ret;

    ret = SAL_RET_SUCCESS;

    if(ucCh >= UART_CH_MAX)
    {
        return SAL_RET_FAILED;
    }

    gpio_Tx = uart[ucCh].sPort.bPortTx;
    gpio_Rx = uart[ucCh].sPort.bPortRx;

    /* Reset gpio */
    ret = GPIO_Config(gpio_Tx, GPIO_FUNC(0UL));

    if(ret != SAL_RET_SUCCESS)
    {
        return SAL_RET_FAILED;
    }

    ret = GPIO_Config(gpio_Rx, GPIO_FUNC(0UL));

    if(ret != SAL_RET_SUCCESS)
    {
        return SAL_RET_FAILED;
    }

    if(uart[ucCh].sCtsRts == ON)
    {
        gpio_Cts = uart[ucCh].sPort.bPortCts;
        gpio_Rts = uart[ucCh].sPort.bPortRts;

        ret = GPIO_Config(gpio_Cts, GPIO_FUNC(0UL));

        if(ret != SAL_RET_SUCCESS)
        {
            return SAL_RET_FAILED;
        }

        ret = GPIO_Config(gpio_Rts, GPIO_FUNC(0UL));

        if(ret != SAL_RET_SUCCESS)
        {
            return SAL_RET_FAILED;
        }
    }

    /* Reset MFIO Configuration */
    switch(ucCh)
    {
        case UART_CH3:
            GPIO_MfioCfg(GPIO_MFIO_CFG_PERI_SEL0, GPIO_MFIO_DISABLE, GPIO_MFIO_CFG_CH_SEL0, GPIO_MFIO_CH0);
            break;
        case UART_CH4:
            GPIO_MfioCfg(GPIO_MFIO_CFG_PERI_SEL1, GPIO_MFIO_DISABLE, GPIO_MFIO_CFG_CH_SEL1, GPIO_MFIO_CH0);
            break;
        case UART_CH5:
            GPIO_MfioCfg(GPIO_MFIO_CFG_PERI_SEL2, GPIO_MFIO_DISABLE, GPIO_MFIO_CFG_CH_SEL2, GPIO_MFIO_CH0);
            break;
        default:
            break;
    }

    return SAL_RET_SUCCESS;
}

static int32_t UART_SetChannelConfig
(
    uint8_t                               ucCh,
    uint32_t                              uiBaud,
    uint8_t                               ucWordLength,
    uint8_t                               ucFIFO,
    uint8_t                               uc2StopBit,
    uint8_t                               ucParity
)
{
    uint32_t  cr_data = 0;
    uint32_t  lcr_data = 0;
    int32_t  ret;
    int32_t  iClkBusId;
    int32_t  iClkPeriId;

    /* Enable the UART controller peri clock */
    iClkBusId   = (int32_t)CLOCK_IOBUS_UART0 + (int32_t)ucCh;
    (void)CLOCK_SetIobusPwdn(iClkBusId, SALDisabled);
    iClkPeriId  = (int32_t)CLOCK_PERI_UART0 + (int32_t)ucCh;
    ret         = CLOCK_SetPeriRate(iClkPeriId, UART_DEBUG_CLK);
    (void)CLOCK_EnablePeri(iClkPeriId);

    if (ret == 0)
    {
        (void)UART_SetBaudRate(ucCh, uiBaud);
        // line control setting
        // Word Length
        lcr_data = UART_LCRH_WLEN(ucWordLength);

        // Enables FIFOs
        if (ucFIFO == ENABLE_FIFO)
        {
            lcr_data |= UART_LCRH_FEN;
        }

        // Two Stop Bits
        if (uc2StopBit == ON)
        {
            lcr_data |= UART_LCRH_STP2;
        }

        // Parity Enable
        switch (ucParity)
        {
            case PARITY_SPACE:
                lcr_data &= ~(UART_LCRH_PEN);
                break;
            case PARITY_EVEN:
                lcr_data |= ((UART_LCRH_PEN | UART_LCRH_EPS) & ~(UART_LCRH_SPS));
                break;
            case PARITY_ODD:
                lcr_data |= ((UART_LCRH_PEN & ~(UART_LCRH_EPS)) & ~(UART_LCRH_SPS));
                break;
            case PARITY_MARK:
                lcr_data |= ((UART_LCRH_PEN & ~(UART_LCRH_EPS)) | UART_LCRH_SPS);
                break;
            default:
                break;
        }

        UART_RegWrite(ucCh, UART_REG_LCRH, lcr_data);

        // control register setting
        cr_data = UART_CR_EN;
        cr_data |= UART_CR_TXE;
        cr_data |= UART_CR_RXE;

        if (uart[ucCh].sCtsRts != 0UL)
        { //brace
            cr_data |= (UART_CR_RTSEN | UART_CR_CTSEN);
        }

        UART_RegWrite(ucCh, UART_REG_CR, cr_data);
    }

    return ret;
}

static int32_t UART_SetBaudRate
(
    uint8_t                               ucCh,
    uint32_t                              uiBaud
) // (uint32_t => int32_t)return type mismatched
{
    uint32_t  u32_div;
    uint32_t  mod;
    uint32_t  brd_i;
    uint32_t  brd_f;
    uint32_t  pclk;
    int32_t  ret;

    if (ucCh >= UART_CH_MAX)
    {
        (void)SAL_DbgReportError(SAL_DRVID_UART, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
        ret = -1;
    }
    else
    {
        // Read the peri clock
        pclk = CLOCK_GetPeriRate((int32_t)CLOCK_PERI_UART0 + (int32_t)ucCh);

        if (pclk == 0UL)
        {
            ret = -1;
        }
        else
        {
            // calculate integer baud rate divisor
            u32_div = 16UL * uiBaud;
            brd_i   = pclk / u32_div;
            UART_RegWrite(ucCh, UART_REG_IBRD, brd_i);

            // calculate faction baud rate divisor
            // NOTICE : fraction maybe need sampling
            mod     = pclk % (16UL * uiBaud);
            u32_div = ((((1UL << 3UL) * 16UL) * mod) / (16UL * uiBaud));
            brd_f   = u32_div / 2UL;
            UART_RegWrite(ucCh, UART_REG_FBRD, brd_f);
            ret = (int32_t)SAL_RET_SUCCESS;
        }
    }
    return ret;
}

static void UART_StatusInit
(
    uint8_t                               ucCh
)
{
    //uart[ucCh].sIsProbed                = 0;
    uart[ucCh].sBase                    = UART_GET_BASE(ucCh);
    uart[ucCh].sCh                      = ucCh;
    uart[ucCh].sOpMode                  = UART_POLLING_MODE;
    uart[ucCh].sCtsRts                  = 0;
    uart[ucCh].sWordLength              = 0;
    uart[ucCh].s2StopBit                = 0;
    uart[ucCh].sParity                  = 0;

    /* Interupt mode init */
    uart[ucCh].sTxIntr.iXmitBuf         = NULL;
    uart[ucCh].sTxIntr.iHead            = -1;
    uart[ucCh].sTxIntr.iTail            = -1;
    uart[ucCh].sTxIntr.iSize            = 0;
    uart[ucCh].sRxIntr.iXmitBuf         = NULL;
    uart[ucCh].sRxIntr.iHead            = -1;
    uart[ucCh].sRxIntr.iTail            = -1;
    uart[ucCh].sRxIntr.iSize            = 0;

    /* DMA mode init */
    uart[ucCh].sTxDma.iCon              = 0xFF;
    uart[ucCh].sTxDma.iCh               = 0xFF;
    uart[ucCh].sTxDma.iSrcAddr          = NULL;
    uart[ucCh].sTxDma.iDestAddr         = NULL;
    uart[ucCh].sTxDma.iBufSize          = GDMA_BUFF_SIZE;
    uart[ucCh].sTxDma.iTransSize        = 0;
    uart[ucCh].sRxDma.iCon              = 0xFF;
    uart[ucCh].sRxDma.iCh               = 0xFF;
    uart[ucCh].sRxDma.iSrcAddr          = NULL;
    uart[ucCh].sRxDma.iDestAddr         = NULL;
    uart[ucCh].sRxDma.iBufSize          = GDMA_BUFF_SIZE;
    uart[ucCh].sRxDma.iTransSize        = 0;
}

static int32_t UART_Probe
(
    uint8_t                               ucCh,
    uint32_t                              uiPriority,
    uint32_t                              uiBaudrate,
    uint8_t                               ucMode,
    uint8_t                               ucCtsRts,
    uint8_t                               ucPortCfg,
    uint8_t                               ucWordLength,
    uint8_t                               ucFIFO,
    uint8_t                               uc2StopBit,
    uint8_t                               ucParity,
    GICIsrFunc                          fnCallback
)
{
    static uint32_t * dma_tx_buf;
    static uint32_t * dma_rx_buf;
    uint32_t          uiDmaRxAddr;
    uint32_t          uiDmaTxAddr;
    int32_t          ret;

    ret = -1;

    if ((ucCh < UART_CH_MAX) && (uart[ucCh].sIsProbed == OFF))
    {
        uart[ucCh].sOpMode = ucMode;
        uart[ucCh].sCtsRts = ucCtsRts;
        uart[ucCh].sWordLength = ucWordLength;
        uart[ucCh].s2StopBit = uc2StopBit;
        uart[ucCh].sParity = ucParity;

        // Set port config
        ret = UART_SetPortConfig(ucCh, ucPortCfg);
        if (ret != -1)
        {
            ret = UART_SetChannelConfig(ucCh, uiBaudrate, ucWordLength, ucFIFO, uc2StopBit, ucParity);

            if (ret != -1)
            {
                // Configure for interrupt mode
                if (uart[ucCh].sOpMode == UART_INTR_MODE)
                {
                    UART_InterruptTxProbe(ucCh);
                    UART_InterruptRxProbe(ucCh);
                    UART_EnableInterrupt(ucCh, uiPriority, ucFIFO, fnCallback);
                }
                else if (uart[ucCh].sOpMode == UART_DMA_MODE)
                {
                    uiDmaRxAddr = MPU_GetDMABaseAddress() + GDMA_ADDRESS_UNIT_CH_RX(ucCh);
                    uiDmaTxAddr = MPU_GetDMABaseAddress() + GDMA_ADDRESS_UNIT_CH_TX(ucCh);
                    dma_rx_buf  = (uint32_t *)(uiDmaRxAddr);
                    dma_tx_buf  = (uint32_t *)(uiDmaTxAddr);

                    (void)SAL_MemSet((void *)dma_rx_buf, 0, GDMA_BUFF_SIZE);
                    (void)SAL_MemSet((void *)dma_tx_buf, 0, GDMA_BUFF_SIZE);
                    UART_DmaTxProbe(ucCh, dma_tx_buf);
                    UART_DmaRxProbe(ucCh, dma_rx_buf);
                    UART_EnableInterrupt(ucCh, uiPriority, ucFIFO, fnCallback);
                }
                else
                {
	                //UART_POLLING_MODE
                }

                uart[ucCh].sIsProbed = ON;
            }
        }
    }

    return ret;
}

static int32_t UART_Rx
(
    uint8_t                               ucCh
)
{
    uint32_t  status;
    uint32_t  max_count;
    uint32_t  data;
    uint8_t * buf;
    int32_t  ret;

    max_count   = UART_BUFF_SIZE;
    buf         = NULL;

    if (ucCh >= UART_CH_MAX)
    {
        (void)SAL_DbgReportError(SAL_DRVID_UART, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
        ret = -1;
    }
    else
    {
        buf = uart[ucCh].sRxIntr.iXmitBuf;

        while(max_count > 0UL)
        {
            status = UART_RegRead(ucCh, UART_REG_FR);

            if((status & UART_FR_RXFE) != 0UL)
            {
                break;
            }

            data = UART_RegRead(ucCh, UART_REG_DR);

            buf[uart[ucCh].sRxIntr.iHead] = (uint8_t)(data & 0xFFUL);
            uart[ucCh].sRxIntr.iHead++;

            if(uart[ucCh].sRxIntr.iHead >= uart[ucCh].sRxIntr.iSize)
            {
                uart[ucCh].sRxIntr.iHead = 0;
            }

            max_count--;
        }

        ret = (int32_t)SAL_RET_SUCCESS;
    }

    return ret;
}

static int32_t UART_TxChar
(
    uint8_t                               ucCh,
    uint8_t                               cChar
)
{
    int32_t  ret;

    if (ucCh >= UART_CH_MAX)
    {
        (void)SAL_DbgReportError(SAL_DRVID_UART, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
        ret = -1;
    }
    else
    {
        if ((UART_RegRead(ucCh, UART_REG_FR) & UART_FR_TXFF) != 0UL)
        {
            ret = -1;
        }
        else
        {
            UART_RegWrite(ucCh, UART_REG_DR, cChar);
            ret = (int32_t)SAL_RET_SUCCESS;
        }
    }

    return ret;
}

static int32_t UART_Tx
(
    uint8_t                               ucCh
)
{
    uint8_t * buf;
    int32_t  ret;

    buf = NULL;

    if (ucCh >= UART_CH_MAX)
    {
        (void)SAL_DbgReportError(SAL_DRVID_UART, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
        ret = -1;
    }
    else
    {
        buf = uart[ucCh].sTxIntr.iXmitBuf;

        // xmit buffer is empty
        if (uart[ucCh].sTxIntr.iHead == uart[ucCh].sTxIntr.iTail)
        {
            UART_RegWrite(ucCh, UART_REG_ICR, UART_INT_TXIS);
        }
        else
        {
            do
            {
                if (UART_TxChar(ucCh, buf[(uint32_t)uart[ucCh].sTxIntr.iTail]) != 0)
                {
                    break;
                }

                uart[ucCh].sTxIntr.iTail++;

                if (uart[ucCh].sTxIntr.iTail >= uart[ucCh].sTxIntr.iSize)
                {
                    uart[ucCh].sTxIntr.iTail = 0;
                }
            } while (uart[ucCh].sTxIntr.iHead != uart[ucCh].sTxIntr.iTail);
        }

        ret = (int32_t)SAL_RET_SUCCESS;
    }

    return ret;
}

static void UART_DmaRxIrq
(
    uint8_t                               ucCh
)
{
    uint32_t  dmacr;

    if (ucCh < UART_CH_MAX)
    {
        dmacr   = UART_RegRead(ucCh, UART_REG_DMACR);
        dmacr   &= ~UART_DMACR_RXDMAE;
        UART_RegWrite(ucCh, UART_REG_DMACR, dmacr);

        UART_RegWrite(ucCh, UART_REG_ICR, UART_INT_OEIS | UART_INT_BEIS | UART_INT_PEIS | UART_INT_FEIS);
        (void)UART_DmaRxTriggerDma(ucCh);
    }
}

static void UART_EnableInterrupt
(
    uint8_t                               ucCh,
    uint32_t                              uiPriority,
    uint8_t                               ucFIFO,
    GICIsrFunc                          fnCallback
)
{
    uint32_t  i;
    uint32_t  im = 0UL;

    if (ucCh < UART_CH_MAX)
    {
        (void)GIC_IntVectSet((uint32_t)GIC_UART0 + ucCh, uiPriority, GIC_INT_TYPE_LEVEL_HIGH, fnCallback, &(uart[ucCh]));
        (void)GIC_IntSrcEn((uint32_t)GIC_UART0 + ucCh);

        UART_RegWrite(ucCh, UART_REG_ICR, UART_INT_RXIS | UART_INT_TXIS | UART_INT_RTIS);

        for (i = 0UL ; i < (UART_RX_FIFO_SIZE * 2UL) ; ++i)
        {
            if ((UART_RegRead(ucCh, UART_REG_FR) & UART_FR_RXFF) != 0UL)
            {
                break;
            }

            (void)UART_RegRead(ucCh, UART_REG_DR);
        }

        if (ucFIFO == ENABLE_FIFO)
        {
            im = UART_INT_RTIS;
        }

        im |= UART_INT_RXIS;

        UART_RegWrite(ucCh, UART_REG_IMSC, im);
    }
}

static void UART_DisableInterrupt
(
    uint8_t                               ucCh
)
{
    if (ucCh < UART_CH_MAX)
    {
        (void)GIC_IntVectSet((uint32_t)GIC_UART0 + ucCh, GIC_PRIORITY_NO_MEAN, GIC_INT_TYPE_LEVEL_HIGH, NULL_PTR, NULL_PTR);
        (void)GIC_IntSrcDis((uint32_t)GIC_UART0 + ucCh);

        UART_RegWrite(ucCh, UART_REG_ICR, 0x7FF);
    }
}

static void UART_InterruptTxProbe
(
    uint8_t                               ucCh
)
{
    if (ucCh < UART_CH_MAX)
    {
        uart[ucCh].sTxIntr.iXmitBuf = uart_buff[ucCh][UART_MODE_TX];
        uart[ucCh].sTxIntr.iSize    = (int32_t)UART_BUFF_SIZE;
        uart[ucCh].sTxIntr.iHead    = 0;
        uart[ucCh].sTxIntr.iTail    = 0;
    }
}

static void UART_InterruptRxProbe
(
    uint8_t                               ucCh
)
{
    if (ucCh < UART_CH_MAX)
    {
        uart[ucCh].sRxIntr.iXmitBuf = uart_buff[ucCh][UART_MODE_RX];
        uart[ucCh].sRxIntr.iSize    = (int32_t)UART_BUFF_SIZE;
        uart[ucCh].sRxIntr.iHead    = 0;
        uart[ucCh].sRxIntr.iTail    = 0;
    }
}

static int32_t UART_InterruptWrite
(
    uint8_t                               ucCh,
    const uint8_t *                       pucBuf,
    uint32_t                              uiSize
)
{
    uint32_t  i;
    uint32_t  imsc;
    int32_t  ret;

    if (pucBuf != NULL_PTR)
    {
        // copy user buffer to tx buffer
        for(i = 0 ; i < uiSize ; i++)
        {
            uart[ucCh].sTxIntr.iXmitBuf[uart[ucCh].sTxIntr.iHead] = pucBuf[i];
            uart[ucCh].sTxIntr.iHead++;

            if(uart[ucCh].sTxIntr.iHead >= uart[ucCh].sTxIntr.iSize)
            {
                uart[ucCh].sTxIntr.iHead = 0;
            }
        }

        ret = UART_Tx(ucCh);

        if (ret == (int32_t)SAL_RET_SUCCESS)
        {
            imsc = UART_RegRead(ucCh, UART_REG_IMSC);
            imsc |= UART_INT_TXIS;
            UART_RegWrite(ucCh, UART_REG_IMSC, imsc);
        }
    }
    else
    {
        ret = -1;
    }

    return ret;
}

static int32_t UART_InterruptRead
(
    uint8_t                               ucCh,
    uint8_t *                             pucBuf,
    uint32_t                              uiSize
)
{
    int32_t  cnt;
    int32_t  i;
    int32_t  ret;

    if (ucCh >= UART_CH_MAX)
    {
        (void)SAL_DbgReportError(SAL_DRVID_UART, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
        ret = -1;
    }
    else
    {
        if(uart[ucCh].sRxIntr.iHead > uart[ucCh].sRxIntr.iTail)
        {
            cnt = uart[ucCh].sRxIntr.iHead - uart[ucCh].sRxIntr.iTail;
        }
        else if (uart[ucCh].sRxIntr.iHead < uart[ucCh].sRxIntr.iTail)
        {
            cnt = uart[ucCh].sRxIntr.iSize - uart[ucCh].sRxIntr.iTail;
        }
        else
        {
            cnt = 0;
        }

        if (cnt > (int32_t)uiSize)
        {
            cnt = (int32_t)uiSize;
        }

        // copy rx buffer to user buffer
        for(i = 0 ; i < cnt ; i++)
        {
            pucBuf[i] = uart[ucCh].sRxIntr.iXmitBuf[uart[ucCh].sRxIntr.iTail];
            uart[ucCh].sRxIntr.iTail++;

            if(uart[ucCh].sRxIntr.iTail >= uart[ucCh].sRxIntr.iSize)
            {
                uart[ucCh].sRxIntr.iTail = 0;
            }
        }

        ret = cnt;
    }

    return ret;
}

static void UART_DmaTxProbe
(
    uint8_t                               ucCh,
    uint32_t *                            puiAddr
)
{
    uint32_t regData;

    if (ucCh < UART_CH_MAX)
    {
        uart[ucCh].sTxDma.iCon          = ucCh;
        uart[ucCh].sTxDma.iCh           = (uint32_t)GDMA_PERI_TX;
        uart[ucCh].sTxDma.iSrcAddr      = (uint8_t *)(puiAddr);
        uart[ucCh].sTxDma.iDestAddr     = (uint8_t *)(uart[ucCh].sBase);
        uart[ucCh].sTxDma.iBufSize      = GDMA_BUFF_SIZE;
        uart[ucCh].sTxDma.iTransSize    = 0;

        (void)GDMA_Init(&uart[ucCh].sTxDma, GIC_PRIORITY_NO_MEAN);

        // Enable Transmit DMA
        regData = (UART_RegRead(ucCh, UART_REG_DMACR) | UART_DMACR_DMAONERR | UART_DMACR_TXDMAE);
        UART_RegWrite(ucCh, UART_REG_DMACR, regData);

        GDMA_SetFlowControl(&uart[ucCh].sTxDma, GDMA_FLOW_TYPE_M2P);
        GDMA_SetAddrIncrement(&uart[ucCh].sTxDma, GDMA_NO_INC, GDMA_INC);
        GDMA_SetBurstSize(&uart[ucCh].sTxDma, GDMA_BURST_SIZE_1, GDMA_BURST_SIZE_1);

        GDMA_SetPeri(&uart[ucCh].sTxDma, (uint8_t)GDMA_PERI_REQ_PORT_UART1_TX, 0U);

        (void)UART_DmaTxEnable(ucCh, uart[ucCh].sTxDma.iBufSize, (const GDMAInformation_t *)&uart[ucCh].sTxDma);
    }
}

static void UART_DmaRxProbe
(
    uint8_t                               ucCh,
    uint32_t *                            puiAddr
)
{
    uint32_t regData;

    if (ucCh < UART_CH_MAX)
    {
        uart[ucCh].sRxDma.iCon          = ucCh;
        uart[ucCh].sRxDma.iCh           = (uint32_t)GDMA_PERI_RX;
        uart[ucCh].sRxDma.iSrcAddr      = (uint8_t *)(uart[ucCh].sBase);
        uart[ucCh].sRxDma.iDestAddr     = (uint8_t *)(puiAddr);
        uart[ucCh].sRxDma.iBufSize      = GDMA_BUFF_SIZE;
        uart[ucCh].sRxDma.iTransSize    = 0UL;
        uart[ucCh].sRxDma.fpIsrCallbackForComplete    = UART_DmaRxISR;

        (void)GDMA_Init(&uart[ucCh].sRxDma, GIC_PRIORITY_NO_MEAN);

        // Enable Receive DMA
        regData = (UART_RegRead(ucCh, UART_REG_DMACR) | UART_DMACR_DMAONERR | UART_DMACR_RXDMAE);
        UART_RegWrite(ucCh, UART_REG_DMACR, regData);

        GDMA_SetFlowControl(&uart[ucCh].sRxDma, GDMA_FLOW_TYPE_P2M);
        GDMA_SetAddrIncrement(&uart[ucCh].sRxDma, GDMA_INC, GDMA_NO_INC);
        GDMA_SetBurstSize(&uart[ucCh].sRxDma, GDMA_BURST_SIZE_1, GDMA_BURST_SIZE_1);

        GDMA_SetPeri(&uart[ucCh].sRxDma, 0U, (uint8_t)GDMA_PERI_REQ_PORT_UART1_RX);

        (void)UART_DmaRxEnable(ucCh, uart[ucCh].sRxDma.iBufSize, (const GDMAInformation_t *)&uart[ucCh].sRxDma);
    }
}

static int32_t UART_DmaTxEnable
(
    uint8_t                               ucCh,
    uint32_t                              uiSize,
    const GDMAInformation_t *           psDmacon
)
{
    int32_t  ret;

    if (ucCh >= UART_CH_MAX)
    {
        (void)SAL_DbgReportError(SAL_DRVID_UART, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
        ret = -1;
    }
    else
    {
        uart[ucCh].sTxDma.iSrcAddr  = (uint8_t *)(psDmacon->iSrcAddr);
        uart[ucCh].sTxDma.iDestAddr = (uint8_t *)(psDmacon->iDestAddr);
        GDMA_SetSrcAddr(&uart[ucCh].sTxDma, (uint32_t)(uart[ucCh].sTxDma.iSrcAddr));
        GDMA_SetDestAddr(&uart[ucCh].sTxDma, (uint32_t)(uart[ucCh].sTxDma.iDestAddr));
        GDMA_InterruptEnable(&uart[ucCh].sTxDma);
        GDMA_SetTransferWidth(&uart[ucCh].sTxDma, GDMA_TRANSFER_SIZE_BYTE, GDMA_TRANSFER_SIZE_BYTE);
        GDMA_SetTransferSize(&uart[ucCh].sTxDma, uiSize);
        ret = (int32_t)SAL_RET_SUCCESS;
    }

    return ret;
}

static int32_t UART_DmaRxEnable
(
    uint8_t                               ucCh,
    uint32_t                              uiSize,
    const GDMAInformation_t *           psDmacon
)
{
    int32_t  ret;

    if (ucCh >= UART_CH_MAX)
    {
        (void)SAL_DbgReportError(SAL_DRVID_UART, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
        ret = -1;
    }
    else
    {
        uart[ucCh].sRxDma.iSrcAddr  = (uint8_t *)(psDmacon->iSrcAddr);
        uart[ucCh].sRxDma.iDestAddr = (uint8_t *)(psDmacon->iDestAddr);
        GDMA_SetSrcAddr(&uart[ucCh].sRxDma, (uint32_t)(uart[ucCh].sRxDma.iSrcAddr));
        GDMA_SetDestAddr(&uart[ucCh].sRxDma, (uint32_t)(uart[ucCh].sRxDma.iDestAddr));
        GDMA_InterruptEnable(&uart[ucCh].sRxDma);
        GDMA_SetTransferWidth(&uart[ucCh].sRxDma, GDMA_TRANSFER_SIZE_BYTE, GDMA_TRANSFER_SIZE_BYTE);
        GDMA_SetTransferSize(&uart[ucCh].sRxDma, uiSize);

        // Run DMA
        GDMA_ChannelEnable(&uart[ucCh].sRxDma);
        ret = (int32_t)SAL_RET_SUCCESS;
    }

    return ret;
}

static int32_t UART_DmaWrite
(
    uint8_t                               ucCh,
    const uint8_t *                       pucBuf,
    uint32_t                              uiSize
)
{
    uint32_t  i;
    int32_t  ret;

    if (ucCh >= UART_CH_MAX)
    {
        (void)SAL_DbgReportError(SAL_DRVID_UART, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
        ret = -1;
    }
    else
    {
        if (uiSize > GDMA_BUFF_SIZE)
        {
            ret = -1;
        }
        else
        {
            for (i = 0; i < uiSize; i++)
            {
                uart[ucCh].sTxDma.iSrcAddr[i] = pucBuf[i];
            }

            GDMA_SetSrcAddr(&uart[ucCh].sTxDma, (uint32_t)(uart[ucCh].sTxDma.iSrcAddr));
            GDMA_SetDestAddr(&uart[ucCh].sTxDma, (uint32_t)(uart[ucCh].sTxDma.iDestAddr));
            GDMA_InterruptEnable(&uart[ucCh].sTxDma);
            GDMA_SetTransferWidth(&uart[ucCh].sTxDma, GDMA_TRANSFER_SIZE_BYTE, GDMA_TRANSFER_SIZE_BYTE);
            GDMA_SetTransferSize(&uart[ucCh].sTxDma, uiSize);

            // Run DMA
            GDMA_ChannelEnable(&uart[ucCh].sTxDma);
            ret = (int32_t)SAL_RET_SUCCESS;
        }
    }

    return ret;
}

static int32_t UART_DmaRead
(
    uint8_t                               ucCh,
    uint8_t *                             pucBuf,
    uint32_t uiSize
)
{
    uint32_t  cnt;
    uint32_t  post_cnt;
    uint32_t  i;
    uint32_t  num_brige;
    uint32_t  prev_buf;
    uint32_t  c_size;
    uint32_t  u_size;
    int32_t  ret;

    cnt         = 0;
    post_cnt    = 0;
    num_brige   = 0;
    prev_buf    = 0;
    c_size      = 0;
    u_size      = uiSize;

    if (ucCh >= UART_CH_MAX)
    {
        (void)SAL_DbgReportError(SAL_DRVID_UART, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
        ret = -1;
    }
    else
    {
        c_size = GDMA_GetTransferSize(&uart[ucCh].sRxDma);

        if (uart[ucCh].sRxDma.iTransSize != c_size)
        {
            if(uart[ucCh].sRxDma.iTransSize > c_size)
            {
                cnt         = uart[ucCh].sRxDma.iTransSize - c_size;
                prev_buf    = uart[ucCh].sRxDma.iBufSize - uart[ucCh].sRxDma.iTransSize;

                if(u_size >= cnt)
                {
                    for (i = 0 ; i < cnt ; i++)
                    {
                        pucBuf[i] = (uint8_t)(uart[ucCh].sRxDma.iDestAddr[prev_buf+i]);
                    }

                    uart[ucCh].sRxDma.iTransSize = c_size;
                }
                else //else if(u_size < cnt)
                {
                    for (i = 0 ; i < u_size ; i++)
                    {
                        pucBuf[i] = (uint8_t)(uart[ucCh].sRxDma.iDestAddr[prev_buf+i]);
                    }

                    uart[ucCh].sRxDma.iTransSize -=  u_size;
                    cnt = u_size;
                }
                ret = (int32_t)cnt;
            }
            else if(c_size > uart[ucCh].sRxDma.iTransSize)
            {
                prev_buf    = uart[ucCh].sRxDma.iBufSize - uart[ucCh].sRxDma.iTransSize;
                cnt         = uart[ucCh].sRxDma.iBufSize - c_size;
                post_cnt    = uart[ucCh].sRxDma.iTransSize;

                if(u_size > post_cnt)
                {
                    u_size  = u_size - post_cnt;

                    for(i= 0UL ; i < uart[ucCh].sRxDma.iTransSize ; i++)
                    {
                        pucBuf[i] = (uint8_t)(uart[ucCh].sRxDma.iDestAddr[prev_buf + i]);
                        num_brige=i+1UL;
                    }

                    if(u_size >= cnt)
                    {
                        for(i = 0UL ; i < cnt ; i++)
                        {
                            pucBuf[num_brige+i] = (uint8_t)(uart[ucCh].sRxDma.iDestAddr[i]);
                        }

                        uart[ucCh].sRxDma.iTransSize = c_size;
                        cnt += post_cnt;
                    }
                    else //else if(u_size < cnt)
                    {
                        for(i = 0UL ; i < u_size ; i++)
                        {
                            pucBuf[num_brige+i] = (uint8_t)(uart[ucCh].sRxDma.iDestAddr[i]);
                        }

                        uart[ucCh].sRxDma.iTransSize =  uart[ucCh].sRxDma.iBufSize - u_size;
                        cnt = u_size + post_cnt;
                    }
                }
                else
                {
                    for(i= 0UL ; i < u_size ; i++)
                    {
                        pucBuf[i]   = (uint8_t)(uart[ucCh].sRxDma.iDestAddr[prev_buf + i]);
                        num_brige   = i + 1UL;
                    }

                    uart[ucCh].sRxDma.iTransSize -= u_size;
                    cnt = u_size;
                }

                ret = (int32_t)cnt;
            }
            else
            {
                ret = 0;// empty statement
            }
        }
        else
        {
            ret = 0;// empty statement
        }
    }

    return ret;
}

static int32_t UART_DmaRxTriggerDma
(
    uint8_t                               ucCh
)
{
    uint32_t  dmacr;
    uint32_t  im;
    int32_t  ret;

    if (ucCh >= UART_CH_MAX)
    {
        (void)SAL_DbgReportError(SAL_DRVID_UART, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
        ret = -1;
    }
    else
    {
        dmacr = UART_RegRead(ucCh, UART_REG_DMACR);
        dmacr |= UART_DMACR_RXDMAE;
        UART_RegWrite(ucCh, UART_REG_DMACR, dmacr);

        im = UART_RegRead(ucCh, UART_REG_IMSC);
        im |= ~UART_INT_RXIS;
        UART_RegWrite(ucCh, UART_REG_IMSC, im);

        ret = (int32_t)SAL_RET_SUCCESS;
    }

    return ret;
}

static void UART_DmaRxISR(void * pArg)
{
    GDMAInformation_t * dmacon = (GDMAInformation_t *)pArg;

    GDMA_SetDestAddr(dmacon, (uint32_t)dmacon->iDestAddr);
    GDMA_InterruptEnable(dmacon);
    GDMA_SetTransferSize(dmacon, dmacon->iBufSize);

    GDMA_ChannelEnable(dmacon);
}


/*
***************************************************************************************************
*                                         INTERFACE FUNCTIONS
***************************************************************************************************
*/

int32_t UART_Open
(
    uint8_t                               ucCh,
    uint32_t                              uiPriority,     // Interrupt Priority
    uint32_t                              uiBaudrate,     // Baudrate
    uint8_t                               ucMode,         // polling or interrupt or udma
    uint8_t                               ucCtsRts,       // on/off
    uint8_t                               ucPortCfg,      // port selection
    uint8_t                               ucWordLength,   // 5~6 bits
    uint8_t                               ucFIFO,         // on/off
    uint8_t                               uc2StopBit,     // on/off
    uint8_t                               ucParity,       // space, even, odd, mark
    GICIsrFunc                          fnCallback      // callback function
)
{
    int32_t          ret = -1;

    UART_StatusInit(ucCh);

    if (ucPortCfg <= UART_PORT_CFG_MAX)
    {
        ret = UART_Probe(ucCh, uiPriority, uiBaudrate, ucMode, ucCtsRts, ucPortCfg,
                        ucWordLength, ucFIFO, uc2StopBit, ucParity, fnCallback);
    }
    else
    {
        (void)SAL_DbgReportError(SAL_DRVID_UART, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }

    return ret;
}

void UART_Close
(
    uint8_t                              ucCh
)
{
    int32_t          iClkBusId;
    SALRetCode_t    ret;

    if (ucCh < UART_CH_MAX)
    {
        /* Disable the UART Interrupt */
        UART_DisableInterrupt(ucCh);

        /* Disable the UART controller Bus clock */
        iClkBusId   = (int32_t)CLOCK_IOBUS_UART0 + (int32_t)ucCh;
        (void)CLOCK_SetIobusPwdn(iClkBusId, TRUE);

        if (uart[ucCh].sOpMode == UART_DMA_MODE)
        {
            /* Disable the UDMA controller Bus clock */
            iClkBusId   = (int32_t)CLOCK_IOBUS_DMA_CON0 + (int32_t)ucCh;
            (void)CLOCK_SetIobusPwdn(iClkBusId, TRUE);
        }

        ret = UART_ClearGpio(ucCh);

        if (ret != SAL_RET_SUCCESS)
        {
            UART_D("%s ch %d failed to reset port\n", __func__, ucCh);
        }

        /* Disable the UART ch */
        SAL_WriteReg(NULL, MCU_BSP_UART_BASE + (0x10000UL * (ucCh)) + UART_REG_CR);

        /* Disable the UART GDMA */
        if (uart[ucCh].sOpMode == UART_DMA_MODE)
        {
            GDMA_ChannelDisable(&uart[ucCh].sRxDma);
            GDMA_ChannelDisable(&uart[ucCh].sTxDma);
        }

        /* Initialize UART Structure */
        SAL_MemSet(&uart[ucCh], 0, sizeof(UartStatus_t));

        /* UART SW Reset */
        UART_Reset(ucCh);
    }
    else
    {
        (void)SAL_DbgReportError(SAL_DRVID_UART, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }
}

int32_t UART_Read
(
    uint8_t                               ucCh,
    uint8_t *                             pucBuf,
    uint32_t                              uiSize
)
{
    uint32_t  i;
    int32_t  ret;
    int8_t   getc_err;

    ret = -1;

    if(pucBuf != NULL_PTR)
    {
        if(OFF != uart[ucCh].sIsProbed)
        {
            if (ucCh >= UART_CH_MAX)
            {
                (void)SAL_DbgReportError(SAL_DRVID_UART, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
            }

            if (uart[ucCh].sOpMode == UART_DMA_MODE)
            {
                ret = UART_DmaRead(ucCh, pucBuf, uiSize);
            }
            else if (uart[ucCh].sOpMode == UART_INTR_MODE)
            {
                ret = UART_InterruptRead(ucCh, pucBuf, uiSize);
            }
            else
            {
                for (i = 0 ; i < uiSize ; i++)
                {
                    ret = UART_GetChar(ucCh, 0, (int8_t *)&getc_err);

                    if (ret >= 0)
                    {
                        pucBuf[i] = (uint8_t)((uint32_t)ret & 0xFFUL);
                    }
                    else
                    {
                        ret = (int32_t)i;
                        break;
                    }
                }
            }
        }
    }

    return ret;
}

int32_t UART_Write
(
    uint8_t                               ucCh,
    const uint8_t *                       pucBuf,
    uint32_t                              uiSize
)
{
    uint32_t  i;
    int32_t  ret;

    ret = -1;

    if (pucBuf != NULL_PTR)
    {
        if(OFF != uart[ucCh].sIsProbed)
        {
            if (ucCh >= UART_CH_MAX)
            {
                (void)SAL_DbgReportError(SAL_DRVID_UART, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
            }
            else
            {
                if (uart[ucCh].sOpMode == UART_DMA_MODE)
                {
                    ret = UART_DmaWrite(ucCh, pucBuf, uiSize);
                }
                else if (uart[ucCh].sOpMode == UART_INTR_MODE)
                {
                    ret = UART_InterruptWrite(ucCh, pucBuf, uiSize);
                }
                else
                {
                    for (i = 0; i < uiSize; i++)
                    {
                        ret = UART_PutChar(ucCh, pucBuf[i]);
                    }
                }
            }
        }
    }

    return ret;
}

uint32_t UART_GetData
(
    uint8_t                               ucCh,
    int32_t                              iWait,
    int8_t *                             pcErr
)
{
    /* Use for Lin */
    uint32_t  data;
    uint32_t  ret;
    int32_t  timeout;

    data    = 0;
    ret     = 0;
    timeout = 0;

    if (pcErr != NULL_PTR)
    {
        if ((ucCh >= UART_CH_MAX) || (OFF == uart[ucCh].sIsProbed))
        {
            (void)SAL_DbgReportError(SAL_DRVID_UART, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
            *pcErr = (int8_t)-1;
        }
        else
        {
            while ((UART_RegRead(ucCh, UART_REG_FR) & UART_FR_RXFE) != 0UL)
            {
                timeout++;

                if( timeout > iWait )
                {
                    *pcErr = (int8_t)-1;
                    break;
                }
            }

            if ((UART_RegRead(ucCh, UART_REG_FR) & UART_FR_RXFE) == 0UL)
            {
                data    = UART_RegRead(ucCh, UART_REG_DR);
                ret     = data;
            }
        }
    }

    return ret;
}

int32_t UART_GetChar
(
    uint8_t                               ucCh,
    int32_t                              iWait,
    int8_t *                             pcErr
)
{
    uint32_t  data;
    int32_t  ret;

    ret = -1;

    if (pcErr != NULL_PTR)
    {
        if ((ucCh >= UART_CH_MAX) || (OFF == uart[ucCh].sIsProbed))
        {
            (void)SAL_DbgReportError(SAL_DRVID_UART, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
            *pcErr = (int8_t)-1;
        }
        else
        {
            if(iWait == 1)
            {
                while ((UART_RegRead(ucCh, UART_REG_FR) & UART_FR_RXFE) != 0UL)
                {
                    if ((UART_RegRead(ucCh, UART_REG_FR) & UART_FR_RXFE) == 0UL)
                    {
                        break;
                    }
                }
                data    = UART_RegRead(ucCh, UART_REG_DR);
                ret     = (int32_t) data;
            }
            else
            {
                if((UART_RegRead(ucCh, UART_REG_FR) & UART_FR_RXFE) != 0UL)
                {
                    *pcErr = (int8_t)(-1);
                }
                else
                {
                    data    = UART_RegRead(ucCh, UART_REG_DR);
                    ret     = (int32_t) data;
                }
            }
        }
    }

    return ret;
}

int32_t UART_PutChar
(
    uint8_t                               ucCh,
    uint8_t                               ucChar
)
{
    int32_t  ret;

    ret = -1;

    if ((ucCh >= UART_CH_MAX) || (OFF == uart[ucCh].sIsProbed))
    {
        (void)SAL_DbgReportError(SAL_DRVID_UART, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }
    else
    {
        while ((UART_RegRead(ucCh, UART_REG_FR) & UART_FR_TXFF) != 0UL)
        {
            if ((UART_RegRead(ucCh, UART_REG_FR) & UART_FR_TXFF) == 0UL)
            {
                break;
            }
        }

        UART_RegWrite(ucCh, UART_REG_DR, ucChar);
        ret = (int32_t)SAL_RET_SUCCESS;
    }

    return ret;
}

void UART_SetLineControlReg
(
    uint8_t                               ucCh,
    uint32_t                              uiBits,
    uint8_t                               uiEnabled
)
{
    uint32_t  uiLcrh;

    uiLcrh = UART_RegRead(ucCh, UART_REG_LCRH);

    if (uiEnabled == 1U)
    {
        uiLcrh = uiLcrh | uiBits;
    }
    else //(uiEnabled == 0U)
    {
        uiLcrh = uiLcrh & ~(uiBits);
    }

    UART_RegWrite(ucCh, UART_REG_LCRH, uiLcrh);
}

void UART_SetInterruptClearReg
(
    uint8_t                               ucCh,
    uint32_t                              uiSetValue
)
{
    UART_RegWrite(ucCh, UART_REG_ICR, uiSetValue);
}

void UART_SetErrorClearReg
(
    uint8_t                               ucCh,
    uint32_t                              uiSetValue
)
{
    UART_RegWrite(ucCh, UART_REG_ECR, uiSetValue);
}

uint32_t UART_GetReceiveStatusReg
(
    uint8_t                               ucCh
)
{
    uint32_t  uiRet;
    uiRet = UART_RegRead(ucCh, UART_REG_RSR);

    return uiRet;
}

uint32_t UART_GetRawInterruptStatusReg
(
    uint8_t                               ucCh
)
{
    uint32_t  uiRet;
    uiRet = UART_RegRead(ucCh, UART_REG_RIS);

    return uiRet;
}

void UART_Init
(
    void
)
{
    // For LOG output
    (void)UART_Close(UART_DEBUG_CH);
    (void)UART_Open(UART_DEBUG_CH, GIC_PRIORITY_NO_MEAN, 115200UL, UART_POLLING_MODE, UART_CTSRTS_OFF,
                    2U, WORD_LEN_8, DISABLE_FIFO, TWO_STOP_BIT_OFF, PARITY_SPACE, NULL_PTR);
}

void UART_ISR
(
    void *                              pArg
)
{
    UartStatus_t *  uartStat;
    uint32_t          status;
    uint32_t          imsc;
    int32_t          max_count;

    uartStat    = pArg;
    max_count   = (int32_t)UART_BUFF_SIZE;

    if (uartStat != NULL_PTR)
    {
        imsc    = UART_RegRead(uartStat->sCh, UART_REG_IMSC);
        status  = UART_RegRead(uartStat->sCh, UART_REG_MIS) & imsc;

        if (status != 0UL)
        {
            do
            {
                UART_RegWrite(uartStat->sCh, UART_REG_ICR, status & ~(UART_INT_RXIS | UART_INT_RTIS | UART_INT_TXIS));

                if((status & (UART_INT_RTIS | UART_INT_RXIS)) != 0UL)
                {
                    if (uart[uartStat->sCh].sOpMode == UART_DMA_MODE)
                    {
                        UART_DmaRxIrq(uartStat->sCh);
                    }
                    else
                    {
                        (void)UART_Rx(uartStat->sCh);
                    }

                    UART_RegWrite(uartStat->sCh, UART_REG_ICR, UART_INT_RXIS | UART_INT_RTIS);
                }

                if((status & UART_INT_TXIS) != 0UL)
                {
                    (void)UART_Tx(uartStat->sCh);
                }

                if (max_count > 0)
                {
                    break;
                }

                status = UART_RegRead(uartStat->sCh, UART_REG_MIS) & imsc;
                max_count--;
            } while(status != 0UL);
        }
    }
}

