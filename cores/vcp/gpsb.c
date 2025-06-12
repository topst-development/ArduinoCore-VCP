// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : gpsb.c
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

//hsj #if ( MCU_BSP_SUPPORT_DRIVER_GPSB == 1 )

#include <gpsb_reg.h>
#define GPIO_GP_MAX 32
/*
***************************************************************************************************
*                                         DEFINITIONS
***************************************************************************************************
*/


/*************************************************************************************************/
/*                                      LOCAL VARIABLES                                          */
/*************************************************************************************************/
static uint32                           dma_tx_dummy[8] = {0};
static uint32                           dma_rx_dummy[8] = {0};

/*
***************************************************************************************************
*                                      FUNCTION PROTOTYPES
***************************************************************************************************
*/

static void GPSB_BitCSet
(
    uint32                              uiAddr,
    uint32                              uiCmask,
    uint32                              uiSmask
);

static void GPSB_BitSClear
(
    uint32                              uiAddr,
    uint32                              uiSmask,
    uint32                              uiCmask
);

static void GPSB_DmaStart
(
    uint32                              uiCh
);

static void GPSB_DmaStop
(
    uint32                              uiCh
);

static SALRetCode_t GPSB_DmaSetAddr
(
    uint32                              uiCh,
    const uint32 *                      uiTx,
    const uint32 *                      uiRx
);

static SALRetCode_t GPSB_SetPort
(
    uint32                              uiCh,
    uint32                              uiSdo,
    uint32                              uiSdi,
    uint32                              uiSclk
);

static void GPSB_SetXferState
(
    uint32                              uiCh,
    uint32                              uiState
);

static uint32 GPSB_FifoWrite
(
    uint32                              uiCh,
    const uint32 *                      puiTxBuf,
    uint32                              uiTxBufSize
);

static uint32 GPSB_FifoRead
(
    uint32                              uiCh,
    uint32 *                            puiRxBuf,
    uint32                              uiRxBufSize
);

static uint32 GPSB_GetBpw
(
    uint32                              uiCh
);

static SALRetCode_t GPSB_SetCs
(
    int32                               iCs,
    uint32                              uiEnable,
    uint32                              uiCsHigh
);

static SALRetCode_t GPSB_WaitRxFifoValid
(
    uint32                              uiCh
);

static SALRetCode_t GPSB_FifoSyncXfer8
(
    uint32                              uiCh,
    const uint8 *                       pTx,
    uint8 *                             pRx,
    uint32                              uiDataSize
);

static SALRetCode_t GPSB_FifoSyncXfer16
(
    uint32                              uiCh,
    const uint8 *                       pTx,
    uint8 *                             pRx,
    uint32                              uiDataSize
);

static SALRetCode_t GPSB_FifoSyncXfer32
(
    uint32                              uiCh,
    const uint8 *                       pTx,
    uint8 *                             pRx,
    uint32                              uiDataSize
);

static SALRetCode_t GPSB_FiFoAsyncXfer
(
    uint32                              uiCh,
    const uint32 *                      pTx,
    uint32 *                            pRx,
    uint32                              uiDataSize
);

static SALRetCode_t GPSB_DmaAsyncXfer
(
    uint32                              uiCh,
    const uint32 *                      pTx,
    uint32 *                            pRx,
    uint32                              uiDataSize
);

static void GPSB_HwInit
(
    uint32                              uiCh
);

static uint32 GPSB_CheckIsrStatus
(
    uint32 uiCh,
    uint32 status
);

static void GPSB_IsrError
(
    uint32                              uiCh,
    uint32                              uiStatus
);

static void GPSB_IsrAsyncFifo
(
    uint32                              uiCh
);

static void GPSB_Isr
(
    uint32                              uiCh
);

static void GPSB_Isr0
(
    void *                              pArg
);

static void GPSB_Isr1
(
    void *                              pArg
);

static void GPSB_Isr2
(
    void *                              pArg
);

static void GPSB_Isr3
(
    void *                              pArg
);

static void GPSB_DmaIsr0
(
    void *                              pArg
);

static void GPSB_DmaIsr1
(
    void *                              pArg
);

static void GPSB_DmaIsr2
(
    void *                              pArg
);

static void GPSB_DmaIsr3
(
    void *                              pArg
);

static uint32 GPSB_IsMaster
(
    uint32                              uiCh
);

static void GPSB_Enable
(
    uint32                              uiCh
);

static void GPSB_Disable
(
    uint32                              uiCh
);

static void GPSB_EnableIntr
(
    uint32                              uiCh
);

static void GPSB_DisableIntr
(
    uint32                              uiCh
);

#ifdef GPSB_DEBUG
static void GPSB_RegDump
(
    uint32                              uiCh
);
#endif

/*
***************************************************************************************************
*                                          GPSB_RegDump
*
* Function to dump all of GPSB register about selected channel.
*
* @param    uiCh [in]       : Value of channel to control
* @return
* Notes
*
***************************************************************************************************
*/

#ifdef GPSB_DEBUG
void GPSB_RegDump
(
    uint32                              uiCh
)
{
    GPSB_D("##### GPSB REG DUMP CH %d (@0x%X)#####\n", uiCh, gpsb[uiCh].dBase);
    GPSB_D("STAT    0x%08X\n", SAL_ReadReg(gpsb[uiCh].dBase + GPSB_STAT));
    GPSB_D("INTEN   0x%08X\n", SAL_ReadReg(gpsb[uiCh].dBase + GPSB_INTEN));
    GPSB_D("MODE    0x%08X\n", SAL_ReadReg(gpsb[uiCh].dBase + GPSB_MODE));
    GPSB_D("CTRL    0x%08X\n", SAL_ReadReg(gpsb[uiCh].dBase + GPSB_CTRL));
    GPSB_D("TXBASE  0x%08X\n", SAL_ReadReg(gpsb[uiCh].dBase + GPSB_TXBASE));
    GPSB_D("RXBASE  0x%08X\n", SAL_ReadReg(gpsb[uiCh].dBase + GPSB_RXBASE));
    GPSB_D("PACKET  0x%08X\n", SAL_ReadReg(gpsb[uiCh].dBase + GPSB_PACKET));
    GPSB_D("DMACTR  0x%08X\n", SAL_ReadReg(gpsb[uiCh].dBase + GPSB_DMACTR));
    GPSB_D("DMASTAT 0x%08X\n", SAL_ReadReg(gpsb[uiCh].dBase + GPSB_DMASTR));
    GPSB_D("DMAICR  0x%08X\n", SAL_ReadReg(gpsb[uiCh].dBase + GPSB_DMAICR));
}
#endif

/*
***************************************************************************************************
*                                          GPSB_BitCSet
*
* Function to clear & set bit in GPSB register.
*
* @param    addr [in]       : Value of Register Address
*           cmask [in]      : Value of clear bit mask
*           smask [in]      : Value of set bit mask
* @return
* Notes
*
***************************************************************************************************
*/

static void GPSB_BitCSet
(
    uint32                              uiAddr,
    uint32                              uiCmask,
    uint32                              uiSmask
)
{
    uint32  val;

    val = (uint32)SAL_ReadReg(uiAddr);
    BSP_BitClr32(val, uiCmask);
    BSP_BitSet32(val, uiSmask);
    SAL_WriteReg(val, uiAddr);

    return;
}

/*
***************************************************************************************************
*                                          GPSB_BitSClear
*
* Function to set & clear bit in GPSB register.
*
* @param    addr [in]       : Value of Register Address
*           smask [in]      : Value of set bit mask
*           cmask [in]      : Value of clear bit mask*
* @return
* Notes
*
***************************************************************************************************
*/

static void GPSB_BitSClear
(
    uint32                              uiAddr,
    uint32                              uiSmask,
    uint32                              uiCmask
)
{
    uint32  val;

    val = (uint32)SAL_ReadReg(uiAddr);
    BSP_BitSet32(val, uiSmask);
    BSP_BitClr32(val, uiCmask);
    SAL_WriteReg(val, uiAddr);

    return;
}

/*
***************************************************************************************************
*                                          GPSB_IsMaster
*
* Function to check gpsb chanel is master.
*
* @param    uiCh [in]       : Value of GPSB Channel.
* @return   TRUE or FALSE
* Notes
*
***************************************************************************************************
*/

static uint32 GPSB_IsMaster
(
    uint32                              uiCh
)
{
    uint32 ret;

    if(gpsb[uiCh].dIsSlave == (uint32)FALSE)
    {
        ret = 1UL;
    }
    else
    {
        ret = 0UL;
    }

    return ret;
}

/*
***************************************************************************************************
*                                          GPSB_Enable
*
* Function to check gpsb chanel is master.
*
* @param    uiCh [in]       : Value of GPSB Channel.
* @return   TRUE or FALSE
* Notes
*
***************************************************************************************************
*/

static void GPSB_Enable
(
    uint32                              uiCh
)
{
    uint32 paddr;
    uint32 reg;

    paddr = 0UL;
    reg = 0UL;

    if(gpsb[(uiCh)].dBase < (UINT_MAX_VALUE - GPSB_MODE))
    {
        /* Enable GPSB operation */
        paddr = (uint32)gpsb[(uiCh)].dBase + GPSB_MODE;
        reg = SAL_ReadReg(paddr) | (BSP_BIT_03);
        SAL_WriteReg(reg, paddr);
    }

    return;
}

/*
***************************************************************************************************
*                                          GPSB_Disable
*
* Function to disable gpsb chanel.
*
* @param    uiCh [in]       : Value of GPSB Channel.
* @return   TRUE or FALSE
* Notes
*
***************************************************************************************************
*/

static void GPSB_Disable
(
    uint32                              uiCh
)
{
    uint32 paddr = 0;
    uint32 reg;

    reg = 0UL;

    if(gpsb[(uiCh)].dBase < (UINT_MAX_VALUE - GPSB_MODE))
    {
        paddr = (uint32)gpsb[(uiCh)].dBase + GPSB_MODE;

        /* Disable GPSB operation */
        reg = SAL_ReadReg(paddr) & (~BSP_BIT_03);
        SAL_WriteReg(reg, paddr);
    }

    return;
}

/*
***************************************************************************************************
*                                          GPSB_EnableIntr
*
* Function to enable interrupt for gpsb chanel.
*
* @param    uiCh [in]       : Value of GPSB Channel.
* @return   TRUE or FALSE
* Notes
*
***************************************************************************************************
*/

static void GPSB_EnableIntr
(
    uint32                              uiCh
)
{
    uint32 paddr;
    uint32 reg;

    paddr = 0UL;
    paddr = 0UL;

    if(gpsb[(uiCh)].dBase < (UINT_MAX_VALUE - GPSB_INTEN))
    {
        /* Enable GPSB Interrupt */
        paddr = (uint32)gpsb[(uiCh)].dBase + GPSB_MODE;
        reg = SAL_ReadReg(paddr) | (BSP_BIT_00);
        SAL_WriteReg(reg, paddr);
    }

    return;
}

/*
***************************************************************************************************
*                                          GPSB_DisableIntr
*
* Function to disable interrupt for gpsb chanel.
*
* @param    uiCh [in]       : Value of GPSB Channel.
* @return   TRUE or FALSE
* Notes
*
***************************************************************************************************
*/

static void GPSB_DisableIntr
(
    uint32                              uiCh
)
{
    uint32 paddr;
    uint32 reg;

    paddr = 0UL;
    reg = 0UL;

    if(gpsb[(uiCh)].dBase < (UINT_MAX_VALUE - GPSB_INTEN))
    {
        paddr = (uint32)gpsb[(uiCh)].dBase + GPSB_INTEN;

        /* Disable GPSB Interrupt */
        reg = SAL_ReadReg(paddr) & (~BSP_BIT_00);
        SAL_WriteReg(reg, paddr);
    }

    return;
}

/*
***************************************************************************************************
*                                          GPSB_DmaStart
*
* Function to start dma.
*
* @param    uiCh [in]       : Value of channel to control
* @return
* Notes
*           Now we support dma operation for channel 0 to 4 only
*           Start dma operation
*
***************************************************************************************************
*/

static void GPSB_DmaStart
(
    uint32                              uiCh
)
{
    uint32 addr;
    uint32 reg;

    addr = 0UL;
    reg  = 0UL;

    /* Setup GPSB DMA */
    if(uiCh < DEDICATED_DMA_NUM)
    {
        /* Set GPSB DMA Address mode - Multiple address mode */
        if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_INTEN))
        {
            /* Enable DMA request for TX/RX FIFO */
            addr = (uint32)gpsb[(uiCh)].dBase + GPSB_INTEN;
            reg = SAL_ReadReg(addr) | (BSP_BIT_31 | BSP_BIT_30);
            SAL_WriteReg(reg, addr);
        }

        if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_DMACTR))
        {
            /* Enable DMA Tx and Rx request */
            addr = (uint32)gpsb[(uiCh)].dBase + GPSB_DMACTR;
            reg = SAL_ReadReg(addr) | (BSP_BIT_31 | BSP_BIT_30);
            SAL_WriteReg(reg, addr);
        }

        if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_DMAICR))
        {
            /* Enbale DMA done interrupt and Disable DMA packet interrupt */
            GPSB_BitSClear((uint32)gpsb[uiCh].dBase + GPSB_DMAICR, BSP_BIT_17, BSP_BIT_16);

            /* Set DMA Rx interrupt */
            addr = gpsb[uiCh].dBase + GPSB_DMAICR;
            reg = SAL_ReadReg(addr) & ~(BSP_BIT_20);
            SAL_WriteReg(reg, addr);
        }

        if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_DMACTR))
        {
            /* Enable GPSB DMA operation */
            addr = (uint32)gpsb[(uiCh)].dBase + GPSB_DMACTR;
            reg = SAL_ReadReg(addr) | (BSP_BIT_00);
            SAL_WriteReg(reg, addr);
        }
    }

    return;
}

/*
***************************************************************************************************
*                                          GPSB_DmaStop
*
* Function to Stop dma.
*
* @param    uiCh [in]       : Value of channel to control
* @return
* Notes
*           Stop dma operation and clear interrupt status
*
***************************************************************************************************
*/

static void GPSB_DmaStop
(
    uint32                              uiCh
)
{
    uint32  addr;
    uint32  reg;

    addr = 0UL;
    reg = 0UL;

    if(uiCh < DEDICATED_DMA_NUM)
    {
        if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_INTEN))
        {
            /* Disable DMA request for TX/RX FIFO */
            addr = gpsb[uiCh].dBase + GPSB_INTEN;
            reg = SAL_ReadReg(addr) & ~(BSP_BIT_31 | BSP_BIT_30);
            SAL_WriteReg(reg, addr);
        }

        if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_DMACTR))
        {
            /* Disable DMA Tx and Rx request */
            addr = gpsb[uiCh].dBase + GPSB_DMACTR;
            reg = SAL_ReadReg(addr) & ~(BSP_BIT_31 | BSP_BIT_30);
            SAL_WriteReg(reg, addr);
        }

        if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_DMAICR))
        {
            /* Clear DMA done and packet interrupt status */
            addr = (uint32)gpsb[(uiCh)].dBase + GPSB_DMAICR;
            reg = SAL_ReadReg(addr) | (BSP_BIT_29 | BSP_BIT_28);
            SAL_WriteReg(reg, addr);
        }

        if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_DMACTR))
        {
            /* Disable GPSB DMA operation */
            addr = gpsb[uiCh].dBase + GPSB_DMACTR;
            reg = SAL_ReadReg(addr) & (~BSP_BIT_00);
            SAL_WriteReg(reg, addr);
        }
    }
    else
    {
        /* Nothing to do */
    }

    return;
}

/*
***************************************************************************************************
*                                          GPSB_DmaSetAddr
*
* Function to set dma buffer address.
*
* @param    uiCh [in]       : Value of channel to control
* @param    uiTx [in]       : TX dma buffer address
* @param    uiRx [in]       : RX dma buffer address
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

static SALRetCode_t GPSB_DmaSetAddr
(
    uint32                              uiCh,
    const uint32 *                      uiTx,
    const uint32 *                      uiRx
)
{
    const uint32 * tx = uiTx;
    const uint32 * rx = uiRx;
    const uint32 * rx_dummy;
    SALRetCode_t    ret;
    uint32          addr;
    uint32          reg;

    addr    = 0UL;
    reg     = 0UL;
    ret     = SAL_RET_SUCCESS;

    if(uiCh > DEDICATED_DMA_NUM)
    {
        GPSB_D("ch %x is not support dma operation\n", uiCh);

        ret = SAL_RET_FAILED;
    }
    else
    {
        /* Set TXBASE and RXBASE */
        if(uiTx != NULL_PTR)
        {
            /* Set multiple packet address mode */
            tx = uiTx;

            if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_DMACTR))
            {
                addr = gpsb[uiCh].dBase + GPSB_DMACTR;
                reg = SAL_ReadReg(addr) & ~(BSP_BIT_17 | BSP_BIT_16);
                SAL_WriteReg(reg, addr);
            }
        }
        else
        {
            /* Set fixed address mode */
            rx_dummy   = dma_tx_dummy;
            tx      = rx_dummy;

            if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_DMACTR))
            {
                GPSB_BitCSet(gpsb[uiCh].dBase + GPSB_DMACTR, BSP_BIT_17 | BSP_BIT_16, BSP_BIT_16);
            }
        }

        if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_TXBASE))
        {
            SAL_WriteReg((uint32)tx, (uint32)(gpsb[uiCh].dBase + GPSB_TXBASE));
        }

        if(uiRx != NULL_PTR)
        {
            /* Set multiple packet address mode */
            rx = uiRx;
            addr = gpsb[uiCh].dBase + GPSB_DMACTR;
            reg = SAL_ReadReg(addr) & ~(BSP_BIT_15 | BSP_BIT_14);
            SAL_WriteReg(reg, addr);
        }
        else
        {
            /* Set fixed address mode */
            rx_dummy   = dma_rx_dummy;
            rx      = rx_dummy;
            GPSB_BitCSet(gpsb[uiCh].dBase + GPSB_DMACTR, BSP_BIT_15 | BSP_BIT_14, BSP_BIT_14);
        }

        SAL_WriteReg((uint32)rx, (uint32)(gpsb[uiCh].dBase + GPSB_RXBASE));

        /* Set Tx and Rx FIFO threshold for interrupt/DMA request */
        GPSB_BitCSet((gpsb[uiCh].dBase + (uint32)GPSB_INTEN), ((uint32)0x7UL << 16UL),
                (((uint32)GPSB_CFGRTH & (uint32)0x7UL) << 16UL));
        GPSB_BitCSet((gpsb[uiCh].dBase + (uint32)GPSB_INTEN), ((uint32)0x7UL << 20UL),
                (((uint32)GPSB_CFGWTH & (uint32)0x7UL) << 20UL));

        ret = SAL_RET_SUCCESS;

    }

    return ret;
}

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
)
{
    sint32  err;
    SALRetCode_t ret;

    err = 0;

    ret = SAL_RET_SUCCESS;

    /* SW reset */
    err = CLOCK_SetSwReset((sint32)(gpsb[uiCh].dIobusName), TRUE);

    if(err != 0)
    {
        ret = SAL_RET_FAILED;
    }
    else
    {
        err = CLOCK_SetSwReset((sint32)(gpsb[uiCh].dIobusName), FALSE);

        if(err != 0)
        {
            ret = SAL_RET_FAILED;
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          GPSB_SetPort
*
* Function to set hw port info.
*
* @param    uiCh [in]       : Value of channel to control
* @param    iSdo [in]       : SDO GPIO index (= MOSI)
* @param    iSdi [in]       : SDI GPIO index (= MISO)
* @param    iSclk [in]      : SCLK GPIO index
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

static SALRetCode_t GPSB_SetPort
(
    uint32                              uiCh,
    uint32                              uiSdo,
    uint32                              uiSdi,
    uint32                              uiSclk
)
{

    SALRetCode_t    ret;
    uint32          i;
    uint32          match_port;
    uint32          port;

    ret         = SAL_RET_SUCCESS;
    i           = 0xffUL;
    match_port  = 0;
    port        = 0;

    for (i = 0 ; i < GPSB_PORT_NUM ; i++)
    {
        if (uiSclk == gpsbport[i][GPSB_SCLK])
        {
            if (uiSdo == gpsbport[i][GPSB_SDO])
            {
                if (uiSdi == gpsbport[i][GPSB_SDI])
                {
                    match_port  = 1;
                    port        = i;
                    break;
                }
            }
        }
    }

    if (match_port == 1UL)
    {
        gpsb[uiCh].dPort = port;
        GPSB_D("%s: GPSB port 0x%x\n", __func__, gpsb[uiCh].dPort);
    }
    else
    {
        GPSB_D("%s: GPSB gpsb port is wrong\n", __func__);

        return SAL_RET_FAILED;
    }

    for (i = 0 ; i < GPSB_CH_NUM ; i++)
    {
        if ((i != uiCh) && (gpsb[i].dPort == port) && (gpsb[i].dPort != (uint32)NULL))
        {
            GPSB_D("%s: 0x%x port is already used by ch 0x%x\n", __func__, port, i);
            return SAL_RET_FAILED;
        }
    }

    ret = (SALRetCode_t)GPIO_Config((uint32)uiSdo, (uint32)(GPIO_FUNC(gpsbport[port][GPSB_FUNC]) | GPIO_OUTPUT));

    if(ret != SAL_RET_SUCCESS)
    {
        return SAL_RET_FAILED;
    }

    ret = (SALRetCode_t)GPIO_Config((uint32)uiSdi, (uint32)((GPIO_FUNC(gpsbport[port][GPSB_FUNC]) | GPIO_INPUT) | GPIO_INPUTBUF_EN));

    if(ret != SAL_RET_SUCCESS)
    {
        return SAL_RET_FAILED;
    }

    if(gpsb[uiCh].dIsSlave == TRUE)
    {
        ret = (SALRetCode_t)GPIO_Config((uint32)uiSclk,
                                    (uint32)(GPIO_FUNC(gpsbport[port][GPSB_FUNC]) | GPIO_INPUT | GPIO_INPUTBUF_EN));

        if(ret != SAL_RET_SUCCESS)
        {
            return SAL_RET_FAILED;
        }
        ret = (SALRetCode_t)GPIO_Config((uint32)gpsbport[port][GPSB_CS],
                                    (uint32)(GPIO_FUNC(gpsbport[port][GPSB_FUNC]) | GPIO_INPUT | GPIO_INPUTBUF_EN));

        if(ret != SAL_RET_SUCCESS)
        {
            return SAL_RET_FAILED;
        }
    }
    else
    {
        ret = (SALRetCode_t)GPIO_Config((uint32)uiSclk, (uint32)(GPIO_FUNC(gpsbport[port][GPSB_FUNC]) | GPIO_OUTPUT));

        if(ret != SAL_RET_SUCCESS)
        {
            return SAL_RET_FAILED;
        }
        ret = (SALRetCode_t)GPIO_Config((uint32)gpsbport[port][GPSB_CS], (uint32)(GPIO_FUNC(gpsbport[port][GPSB_FUNC]) | GPIO_OUTPUT));

        if(ret != SAL_RET_SUCCESS)
        {
            return SAL_RET_FAILED;
        }
    }

    if(gpsbport[port][GPSB_FUNC] == GPIO_FUNC(3UL))
    {
        GPIO_MfioCfg(gpsbport[port][GPSB_MFIO_PERISEL], gpsbport[port][GPSB_MFIO_TYPE], gpsbport[port][GPSB_MFIO_CHSEL], gpsbport[port][GPSB_MFIO_CHNUM]);
    }
    else
    {
        GPIO_PerichSel(gpsbport[port][GPSB_PERISEL], gpsbport[port][GPSB_PERICH]);
    }

    return SAL_RET_SUCCESS;
}

/*
***************************************************************************************************
*                                          GPSB_SetXferState
*
* Function to change xfer state.
*
* @param    uiCh [in]       : Value of channel to control
* @param    uiState [in]    : Xfer state (GPSB_XFER_STATE_DISABLED/IDLE/RUNNING)
* @return
* Notes
*
***************************************************************************************************
*/

static void GPSB_SetXferState
(
    uint32                              uiCh,
    uint32                              uiState
)
{
    switch(uiState)
    {
        case GPSB_XFER_STATE_DISABLED:
        case GPSB_XFER_STATE_IDLE:
        case GPSB_XFER_STATE_RUNNING:
        {
            gpsb[uiCh].dState = (uint8)uiState;
            break;
        }

        default:
        {
            GPSB_E("%s: state is not supported!\n", __func__);
            break;
        }
    }

    return;
}

/*
***************************************************************************************************
*                                          GPSB_FifoWrite
*
* Function to write data to PORT regitster with interrupt xfer mode.
*
* @param    uiCh [in]       : Value of channel to control
* @param    puiTxBuf [in]   : TX data buff
* @param    uiTxBufSize [in]: TX data size
* @return   write data size
* Notes
*
***************************************************************************************************
*/

static uint32 GPSB_FifoWrite
(
    uint32                              uiCh,
    const uint32 *                      puiTxBuf,
    uint32                              uiTxBufSize
)
{
    uint32          i;
    uint32          j;
    uint32          word_size;
    const uint32 *  tx;
    uint32          cnt;
    uint32          write_data;
    uint32          ret;

    word_size   = 0;
    tx          = NULL;
    cnt         = 0;
    write_data  = 0;
    ret         = 0;

    if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_MODE))
    {
        word_size   = (uint32)((SAL_ReadReg((uint32)(gpsb[uiCh].dBase + GPSB_MODE)) >> 8UL) & 0x1FUL) + 1UL;
    }

    word_size   = word_size / 8UL;

    if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_STAT))
    {
        /* Get empty fifo count */
        cnt = (SAL_ReadReg((uint32)(gpsb[uiCh].dBase + GPSB_STAT)) >> 24UL) & 0xFUL;
    }

    if(word_size > 2UL)
    {
        /*
         * With BPW=32 and higher speed, ROR error occurs.
         * In order to prevent ROR error, we limit FIFO size (8->4).
         * We confirmed operation at speed up to 25 MHz.
         * We recommend DMA transfer at high speed.
         */
        cnt = 4UL - cnt;
    }
    else
    {
        cnt = 8UL - cnt;
    }

    tx = puiTxBuf;

    for(i = 0 ; i < cnt ; i++)
    {
        write_data  = 0;

        if(tx != (uint32 *)NULL_PTR)
        {
            if(word_size <= 1UL)
            {
                write_data = tx[0];
            }
            else if(word_size <= 2UL)
            {
                write_data = (((uint32)tx[1] << 8UL) | (uint32)tx[0]);
            }
            else
            {
                write_data = (((uint32)tx[3] << 24UL) | ((uint32)tx[2] << 16UL)
                               |((uint32)tx[1] << 8UL) | ((uint32)tx[0] << 0));
            }

            for(j = 0 ; j < word_size ; j++)
            {
                tx++;
            }
        }

        if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_PORT))
        {
            SAL_WriteReg(write_data, (uint32)(gpsb[uiCh].dBase + GPSB_PORT));
        }
        ret += word_size;

        if(ret >= uiTxBufSize)
        {
            break;
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          GPSB_FifoRead
*
* Function to read data to PORT regitster with interrupt xfer mode.
*
* @param    uiCh [in]       : Value of channel to control
* @param    puiRxBuf [out]  : RX data buff
* @param    uiRxBufSize [in]: RX data size
* @return   read data size
* Notes
*
***************************************************************************************************
*/

static uint32 GPSB_FifoRead
(
    uint32                              uiCh,
    uint32 *                            puiRxBuf,
    uint32                              uiRxBufSize
)
{
    uint32      i;
    uint32      j;
    uint32      word_size;
    uint32 *    rx;
    uint32      cnt;
    uint32      read_data;
    uint32      ret;

    word_size   = 0;
    rx          = NULL_PTR;
    cnt         = 0;
    read_data   = 0;
    ret         = 0;

    /* Get empty fifo count */
    if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_STAT))
    {
        cnt         = (SAL_ReadReg((uint32)(gpsb[uiCh].dBase + GPSB_STAT)) >> 16UL) & 0xFUL;
    }

    if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_MODE))
    {
        word_size   = (((SAL_ReadReg((uint32)(gpsb[uiCh].dBase + GPSB_MODE)) >> 8UL) & 0x1FUL) + 1UL);
    }

    word_size   = word_size / (uint32)8UL;
    rx          = puiRxBuf;

    for(i = 0 ; i < cnt ; i++)
    {
        if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_PORT))
        {
            read_data = SAL_ReadReg((uint32)(gpsb[uiCh].dBase + GPSB_PORT));
        }

        if(rx != (uint32 *)NULL_PTR)
        {
            if(word_size <= (uint32)1UL)
            {
                rx[0] = (uint32)(read_data & 0xFFUL);
            }
            else if(word_size <= (uint32)2UL)
            {
                rx[0] = (uint32)(read_data & 0xFFUL);
                rx[1] = (uint32)((read_data >> 8UL) & 0xFFUL);
            }
            else
            {
                rx[0] = (uint32)(read_data & 0xFFUL);
                rx[1] = (uint32)((read_data >> 8UL) & 0xFFUL);
                rx[2] = (uint32)((read_data >> 16UL) & 0xFFUL);
                rx[3] = (uint32)((read_data >> 24UL) & 0xFFUL);
            }

            for(j = 0 ; j < word_size ; j++)
            {
                rx++;
            }
        }

        ret += word_size;

        if(ret >= uiRxBufSize)
        {
            break;
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          GPSB_SetSpeed
*
* Function to set GPSB operattion clock speed. (= SCLK)
*
* @param    uiCh [in]       : Value of channel to control
* @param    uiSpeedInHz [in]: SCLK speed
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_SetSpeed
(
    uint32                              uiCh,
    uint32                              uiSpeedInHz
)
{
    uint32  pclk;
    uint32  divldv;
    sint32  result;
    SALRetCode_t ret;

    pclk    = 0;
    divldv  = 0;
    result  = 0;
    ret     = SAL_RET_SUCCESS;

    if(gpsb[uiCh].dBase < (UINT_MAX_VALUE -GPSB_MODE))
    {
        divldv  = ((SAL_ReadReg((uint32)(gpsb[uiCh].dBase + (uint32)GPSB_MODE)) >> 24UL) & 0xFFUL);
    }

    if(uiSpeedInHz < ((UINT_MAX_VALUE / 2UL)/(divldv + 1UL)))
    {
        pclk    = uiSpeedInHz * (divldv + 1UL) * 2UL;
    }

    GPSB_D("%s: speed: %d / divldv: %d / pclk: %d\n", __func__, uiSpeedInHz, divldv, pclk);

    if(gpsb[uiCh].dPeriName < (uint32)CLOCK_PERI_MAX)
    {
        result  = CLOCK_SetPeriRate((sint32)gpsb[uiCh].dPeriName, pclk);
    }

    if(result != 0)
    {
        GPSB_D("%s: ch %d failed to set peri. clock\n", __func__, uiCh);

        ret = SAL_RET_FAILED;
    }
    else
    {
        if(gpsb[uiCh].dPeriName < (uint32)CLOCK_PERI_MAX)
        {
            (void)CLOCK_EnablePeri((sint32)(gpsb[uiCh].dPeriName));
        }

        ret = SAL_RET_SUCCESS;
    }

    return ret;
}

/*
***************************************************************************************************
*                                          GPSB_SetBpw
*
* Function to set transfer bpw value.
*
* @param    uiCh [in]       : Value of channel to control
* @param    uiBpw [in]      : Bit per word
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_SetBpw
(
    uint32                              uiCh,
    uint32                              uiBpw
)
{
    SALRetCode_t    ret;
    uint32          addr;
    uint32          reg;

    addr = 0UL;
    reg  = 0UL;
    ret = SAL_RET_SUCCESS;

    if(((uiBpw != (uint32)8UL) && (uiBpw != (uint32)16UL)) && (uiBpw != (uint32)32UL))
    {
        GPSB_D("%s: ch %d bpw is not supported\n", __func__, uiBpw);
        ret = SAL_RET_FAILED;
    }
    else
    {
        if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_MODE))
        {
            GPSB_BitCSet((gpsb[uiCh].dBase + GPSB_MODE) , (0x1FUL << 8UL), (((uiBpw - 1UL) & 0x1FUL) << 8UL));
        }

        /*
         * According to bits per word, Tx/Rx half-word and byte swap should be set
         */

        if(uiBpw == 8UL)
        {
            if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_INTEN))
            {
                addr = gpsb[uiCh].dBase + GPSB_INTEN;
                reg = SAL_ReadReg(addr) & ~(BSP_BIT_24 | BSP_BIT_25 | BSP_BIT_26 | BSP_BIT_27);
                SAL_WriteReg(reg, addr);
            }
        }
        else if(uiBpw == 16UL)
        {
            if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_INTEN))
            {
                GPSB_BitCSet(gpsb[uiCh].dBase + GPSB_INTEN, (BSP_BIT_24 | BSP_BIT_25 | BSP_BIT_26 | BSP_BIT_27), (BSP_BIT_24 | BSP_BIT_26));
            }
        }
        else
        {
            if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_INTEN))
            {
                addr = (uint32)gpsb[(uiCh)].dBase + GPSB_INTEN;
                reg = SAL_ReadReg(addr) | (BSP_BIT_24 | BSP_BIT_25 | BSP_BIT_26 | BSP_BIT_27);
                SAL_WriteReg(reg, addr);
            }
        }
    }

    return ret;
}

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
)
{
    SALRetCode_t    ret;
    uint32          addr;
    uint32          reg;

    ret = SAL_RET_SUCCESS;
    addr = 0UL;
    reg  = 0UL;

    if(((uiBpw != 8UL) && (uiBpw != 16UL)) && (uiBpw != 32UL))
    {
        GPSB_D("%s ch %d bpw is not supported\n", __func__, uiBpw);
        ret = SAL_RET_FAILED;
    }
    else
    {
        if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_MODE))
        {
            GPSB_BitCSet((gpsb[uiCh].dBase + (uint32)GPSB_MODE), 0x1FUL << 8UL, (uint32)(((uiBpw - 1UL) & 0x1FUL)) << 8UL);
        }

        if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_INTEN))
        {
            addr = gpsb[uiCh].dBase + GPSB_INTEN;
            reg = SAL_ReadReg(addr) & ~(BSP_BIT_24 | BSP_BIT_25 | BSP_BIT_26 | BSP_BIT_27);
            SAL_WriteReg(reg, addr);
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          GPSB_GetBpw
*
* Function to set transfer bpw value.
*
* @param    uiCh [in]       : Value of channel to control
* @return   bpw value from register read value.
* Notes
*
***************************************************************************************************
*/

static uint32 GPSB_GetBpw
(
    uint32                              uiCh
)
{
    uint32  bpw;

    bpw = ((SAL_ReadReg((uint32)(gpsb[uiCh].dBase) + GPSB_MODE) >> 8UL) & 0x1FUL);

    return (bpw + 1UL);
}

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
)
{
    uint32  addr;
    uint32  reg;

    addr    = 0UL;
    reg     = 0UL;

    if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_MODE))
    {
        if((uiMode & GPSB_CPOL) != (uint32)NULL)
        {
            addr = (uint32)gpsb[(uiCh)].dBase + GPSB_MODE;
            reg = SAL_ReadReg(addr) | (BSP_BIT_16);
            SAL_WriteReg(reg, addr);
        }
        else
        {
            addr = gpsb[uiCh].dBase + GPSB_MODE;
            reg = SAL_ReadReg(addr) & (~BSP_BIT_16);
            SAL_WriteReg(reg, addr);
        }

        if((uiMode & GPSB_CPHA) != (uint32)NULL)
        {
            addr = (uint32)gpsb[(uiCh)].dBase + GPSB_MODE;
            reg = SAL_ReadReg(addr) | (BSP_BIT_18 | BSP_BIT_17);
            SAL_WriteReg(reg, addr);
        }
        else
        {
            addr = gpsb[uiCh].dBase + GPSB_MODE;
            reg = SAL_ReadReg(addr) & ~(BSP_BIT_18|BSP_BIT_17);
            SAL_WriteReg(reg, addr);
        }

        if((uiMode & GPSB_CS_HIGH) != (uint32)NULL)
        {
            addr = (uint32)gpsb[(uiCh)].dBase + GPSB_MODE;
            reg = SAL_ReadReg(addr) | (BSP_BIT_20 | BSP_BIT_19);
            SAL_WriteReg(reg, addr);
        }
        else
        {
            addr = gpsb[uiCh].dBase + GPSB_MODE;
            reg = SAL_ReadReg(addr) & ~(BSP_BIT_20 | BSP_BIT_19);
            SAL_WriteReg(reg, addr);
        }

        if((uiMode & GPSB_LSB_FIRST) != (uint32)NULL)
        {
            addr = (uint32)gpsb[(uiCh)].dBase + GPSB_MODE;
            reg = SAL_ReadReg(addr) | (BSP_BIT_07);
            SAL_WriteReg(reg, addr);
        }
        else
        {
            addr = gpsb[uiCh].dBase + GPSB_MODE;
            reg = SAL_ReadReg(addr) & (~BSP_BIT_07);
            SAL_WriteReg(reg, addr);
        }

        if((uiMode & GPSB_LOOP) != (uint32)NULL)
        {
            addr = (uint32)gpsb[(uiCh)].dBase + GPSB_MODE;
            reg = SAL_ReadReg(addr) | (BSP_BIT_06);
            SAL_WriteReg(reg, addr);
        }
        else
        {
            addr = gpsb[uiCh].dBase + GPSB_MODE;
            reg = SAL_ReadReg(addr) & (~BSP_BIT_06);
            SAL_WriteReg(reg, addr);
        }

        if((uiMode & GPSB_SLAVE) != (uint32)NULL)
        {
            addr = (uint32)gpsb[(uiCh)].dBase + GPSB_MODE;
            reg = SAL_ReadReg(addr) | (BSP_BIT_02);
            SAL_WriteReg(reg, addr);
        }
        else
        {
            addr = gpsb[uiCh].dBase + GPSB_MODE;
            reg = SAL_ReadReg(addr) & (~BSP_BIT_02);
            SAL_WriteReg(reg, addr);
        }

        GPSB_D("%s: ch %d GPSB MODE 0x%x\n", __func__, uiCh, SAL_ReadReg((uint32)(gpsb[uiCh].dBase + GPSB_MODE)));
    }

    return;
}

/*
***************************************************************************************************
*                                          GPSB_CsAlloc
*
* Function to config CS GPIO.
*
* @param    uiCh [in]       : Value of channel to control
* @param    uiCs [in]       : CS GPIO index
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPSB_CsAlloc
(
    uint32                              uiCh,
    uint32                              uiCs
)
{
    SALRetCode_t    ret;
    sint32          gfb_cs;

    ret     = SAL_RET_SUCCESS;
    gfb_cs  = 0;

    /* Configure CS_GPIO as gpio function */
    if(uiCs > 0UL)
    {
        gfb_cs = (sint32)GPIO_ToNum(uiCs);

        if(gfb_cs == (sint32)SAL_RET_FAILED)
        {
            ret = SAL_RET_FAILED;
        }
        else
        {
            /* Configure CS_GPIO as GFB function and GPSB controls CS */
            ret = GPIO_Config((uint32)uiCs, (uint32)(GPIO_FUNC((uint32)0UL) | GPIO_OUTPUT));

            if(ret != SAL_RET_SUCCESS)
            {
                GPSB_E("%s : GPIO config fail. GPSB channel %d is closed. \n", __func__, uiCh);
                (void)GPSB_Close(uiCh);

                ret = SAL_RET_FAILED;
            }
        }
    }
    else
    {
    }

    return ret;
}

/*
***************************************************************************************************
*                                          GPSB_SetCs
*
* Function to set CS value to high or low.
*
* @param    iCs [in]        : CS GPIO index
* @param    uiEnable [in]   : Active or Deactive
* @param    uiCsHigh [in]   : Default high or low (active high or active low)
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

static SALRetCode_t GPSB_SetCs
(
    int32                               iCs,
    uint32                              uiEnable,
    uint32                              uiCsHigh
)
{
    SALRetCode_t    ret;
    uint32          enable;

    ret     = SAL_RET_SUCCESS;
    enable  = uiEnable;

    if(iCs < 0)
    {
        ret = SAL_RET_FAILED;
    }
    else
    {
        if(uiCsHigh != (uint32)FALSE)
        {
            (void)GPIO_Set((uint32)iCs, enable);
        }
        else
        {
            if(uiEnable > 0UL)
            {
                enable = (uint32)FALSE;
            }
            else
            {
                enable = (uint32)TRUE;
            }

            (void)GPIO_Set((uint32)iCs, enable);
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          GPSB_CsInit
*
* Function to initialize CS GPIO to default config.
*
* @param    uiCh [in]       : Value of channel to control
* @param    uiCs [in]       : CS GPIO index
* @param    uiCsHigh [in]   : default high or low
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
)
{
    SALRetCode_t    ret;

    ret = SAL_RET_SUCCESS;

    /* Configure CS_GPIO as gpio function */
    if(uiCs < GPIO_GP_MAX)
    {
        ret = GPIO_Config((uint32)uiCs, (uint32)(GPIO_FUNC((uint32)0UL) | GPIO_OUTPUT));

        if(ret == SAL_RET_SUCCESS)
        {
            /* Initial state is deactivated state */
            ret = GPSB_SetCs((int32)uiCs, (uint32)FALSE, uiCsHigh);

            if(ret != SAL_RET_SUCCESS)
            {
                GPSB_D("%s: ch %d deactivating cs_gpios(%d) is failed\n", __func__, uiCh, uiCs);
            }
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          GPSB_CsActivate
* Function to set active CS GPIO.
*
* @param    uiCh [in]       : Value of channel to control
* @param    uiCs [in]        : CS GPIO index
* @param    uiCsHigh        : Default High or Low
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
)
{
    SALRetCode_t    ret;

    ret = SAL_RET_SUCCESS;

    if(uiCs < GPIO_GP_MAX)
    {
        ret = GPSB_SetCs((int32)uiCs, (uint32)TRUE, uiCsHigh);

        if(ret != SAL_RET_SUCCESS)
        {
            GPSB_E("%s Fail. GPSB channel %d is closed.\n", __func__, uiCh);
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          GPSB_CsDeactivate
*
* Function to set active CS GPIO.
*
* @param    uiCh [in]       : Value of channel to control
* @param    uiCs [in]       : CS GPIO index
* @param    uiCsHigh [in]   : Default High or Low
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
)
{
    SALRetCode_t    ret;

    ret = SAL_RET_SUCCESS;

    if(uiCs < GPIO_GP_MAX)
    {
        ret = GPSB_SetCs((int32)uiCs, (uint32)FALSE, uiCsHigh);

        if(ret != SAL_RET_SUCCESS)
        {
            GPSB_E("%s Fail. GPSB channel %d is closed.\n", __func__, uiCh);
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          GPSB_WaitRxFifoValid
*
* Function to check RX FIFO count is valid after TX data transfer.
*
* @param    uiCh [in]       : Value of channel to control
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

static SALRetCode_t GPSB_WaitRxFifoValid
(
    uint32                              uiCh
)
{
    uint32  cnt;
    SALRetCode_t ret;

    cnt = 0;
    ret = SAL_RET_SUCCESS;

    if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_STAT))
    {
        while(((SAL_ReadReg((uint32)(gpsb[uiCh].dBase + GPSB_STAT)) & BSP_BIT_00) == 0UL))
        {
            cnt++;

            if(cnt > GPSB_POLL_TIMEOUT)
            {
                GPSB_D("%s ch %d pio polling mode timeout err\n", __func__, uiCh);

                ret = SAL_RET_FAILED;
                break;
            }
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          GPSB_FifoSyncXfer8
*
* Function to Write/Read data with 8 BPW.
*
* @param    uiCh [in]       : Value of channel to control
* @param    pTx [in]        : TX data buffer
* @param    pRx [out]       : RX data buffer
* @param    uiDataSize [in] : Write/Read data size
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

static SALRetCode_t GPSB_FifoSyncXfer8
(
    uint32                              uiCh,
    const uint8 *                       pTx,
    uint8 *                             pRx,
    uint32                              uiDataSize
)
{
    uint32          data_num;
    uint32          i;
    uint32          rx_dummy;
    const uint8 *   tx;
    uint8 *         rx;
    SALRetCode_t    ret;

    tx          = (const uint8 *)pTx;
    rx          = (uint8 *)pRx;
    data_num    = 0;
    rx_dummy       = 0;
    ret         = SAL_RET_SUCCESS;

    if(uiCh >= GPSB_CH_NUM)
    {
        GPSB_D("%s %d channel is wrong\n", __func__, uiCh);
        ret = SAL_RET_FAILED;
    }
    else
    {
        GPSB_Enable(uiCh);
        data_num = uiDataSize;

        for(i = 0 ; i < data_num ; i++)
        {
            /* Write data to TXFIFO */
            if(tx != (uint8 *)NULL_PTR)
            {
                SAL_WriteReg(tx[i],(uint32)(gpsb[uiCh].dBase + GPSB_PORT));
            }

            /* Wait until Read FIFO is valid */
            ret = GPSB_WaitRxFifoValid(uiCh);

            if(ret != SAL_RET_SUCCESS)
            {
                GPSB_D("%s ch %d wait rx fifo valid ret %d\n", __func__, uiCh, ret);
                break;
            }

            /* Read data from RX FIFO */
            rx_dummy = SAL_ReadReg((uint32)(gpsb[uiCh].dBase + GPSB_PORT));

            if(rx != (uint8 *)NULL_PTR)
            {
                rx[i] = (uint8)(rx_dummy & 0xFFUL);
            }
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          GPSB_FifoSyncXfer16
*
* Function to Write/Read data with 16 BPW.
*
* @param    uiCh [in]       : Value of channel to control
* @param    pTx [in]        : TX data buffer
* @param    pRx [out]       : RX data buffer
* @param    uiDataSize [in] : Write/Read data size
* @return   SAL_RET_SUCCESS or SAL_ERR
* @return
* Notes
*
***************************************************************************************************
*/

static SALRetCode_t GPSB_FifoSyncXfer16
(
    uint32                              uiCh,
    const uint8 *                       pTx,
    uint8 *                             pRx,
    uint32                              uiDataSize
)
{
    uint32          i;
    uint32          rx_dummy;
    const uint8 *   tx;
    uint8 *         rx;
    SALRetCode_t    ret;
    uint16          tx_data;

    tx          = pTx;
    rx          = pRx;
    rx_dummy    = 0;
    tx_data     = 0;
    ret         = SAL_RET_SUCCESS;

    if(uiCh >= GPSB_CH_NUM)
    {
        GPSB_D("%s %d channel is wrong\n", __func__, uiCh);
        ret = SAL_RET_FAILED;
    }
    else
    {
        if((uiDataSize % 2UL) != 0UL)
        {
            GPSB_D("%s ch %d data size(%d) is not alligned\n", __func__, uiCh, uiDataSize);
            ret = SAL_RET_FAILED;
        }
        else
        {
            GPSB_Enable(uiCh);

            for(i = 0 ; i < uiDataSize ; i=i+2)
            {
                /* Write data to TXFIFO */
                if(tx != NULL_PTR)
                {
                    tx_data = (uint16)tx[i];
                    tx_data |= (uint16)((tx[i+1UL] & 0xFFUL) << 8);

                    SAL_WriteReg(tx_data, (uint32)(gpsb[uiCh].dBase + GPSB_PORT));
                }

                /* Wait until Read FIFO is valid */
                ret = GPSB_WaitRxFifoValid(uiCh);

                if(ret != SAL_RET_SUCCESS)
                {
                    GPSB_D("%s ch %d wait rx fifo valid ret %d\n", __func__, uiCh, ret);
                    break;
                }

                /* Read data from RX FIFO */
                rx_dummy = SAL_ReadReg((uint32)(gpsb[uiCh].dBase + GPSB_PORT));

                if(rx != NULL_PTR)
                {
                    rx[i] = (uint8)(rx_dummy & 0xFFUL);
                    rx[i+1UL] = (uint8)((rx_dummy & 0xFF00UL) >> 8);
                }
            }
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          GPSB_FifoSyncXfer32
*
* Function to Write/Read data with 32 BPW.
*
* @param    uiCh [in]       : Value of channel to control
* @param    pTx [in]        : TX data buffer
* @param    pRx [out]       : RX data buffer
* @param    uiDataSize [in] : Write/Read data size
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

static SALRetCode_t GPSB_FifoSyncXfer32
(
    uint32                              uiCh,
    const uint8 *                       pTx,
    uint8 *                             pRx,
    uint32                              uiDataSize
)
{
    uint32          i;
    uint32          rx_dummy;
    const uint8 *   tx;
    uint8 *         rx;
    uint32          tx_data;
    SALRetCode_t    ret;

    tx          = pTx;
    rx          = pRx;
    rx_dummy    = 0;
    tx_data     = 0;
    ret         = SAL_RET_SUCCESS;

    if(uiCh >= GPSB_CH_NUM)
    {
        GPSB_D("%s %d channel is wrong\n", __func__, uiCh);

        ret = SAL_RET_FAILED;
    }
    else
    {

        if((uiDataSize % 4UL) != 0UL)
        {
            GPSB_D("%s ch %d data size(%d) is not alligned\n", __func__, uiCh, uiDataSize);

            ret = SAL_RET_FAILED;
        }
        else
        {
            GPSB_Enable(uiCh);

            for(i = 0 ; i < uiDataSize ; i=i+4)
            {
                /* Write data to TXFIFO */
                if(tx != NULL_PTR)
                {
                    tx_data = tx[i];
                    tx_data |= (uint32)((tx[i+1UL] & 0xFFUL) << 8);
                    tx_data |= (uint32)((tx[i+2UL] & 0xFFUL) << 16);
                    tx_data |= (uint32)((tx[i+3UL] & 0xFFUL) << 24);
                    SAL_WriteReg(tx_data, (uint32)(gpsb[uiCh].dBase + GPSB_PORT));
                }
                else
                {
                    SAL_WriteReg(0, (uint32)(gpsb[uiCh].dBase + GPSB_PORT));
                }

                /* Wait until Read FIFO is valid */
                ret = GPSB_WaitRxFifoValid(uiCh);

                if(ret != SAL_RET_SUCCESS)
                {
                    GPSB_D("%s ch %d wait rx fifo valid ret %d\n", __func__, uiCh, ret);
                    break;
                }

                //delay1us(1);

                /* Read data from RX FIFO */
                rx_dummy   = SAL_ReadReg((uint32)(gpsb[uiCh].dBase + GPSB_PORT));

                if(rx != NULL_PTR)
                {
                    rx[i]     = (uint8)(rx_dummy & 0xFFUL);
                    rx[i+1UL] = (uint8)((rx_dummy & 0xFF00UL) >> 8);
                    rx[i+2UL] = (uint8)((rx_dummy & 0xFF0000UL) >> 16);
                    rx[i+3UL] = (uint8)((rx_dummy & 0xFF000000UL) >> 24);
                }
            }
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          GPSB_FiFoAsyncXfer
*
* Function to Async transfer with FIFO(PIO) mode.
*
* @param    uiCh [in]       : Value of channel to control
* @param    pTx [in]        : TX data buffer
* @param    pRx [out]       : RX data buffer
* @param    uiDataSize [in] : Write/Read data size
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

static SALRetCode_t GPSB_FiFoAsyncXfer
(
    uint32                              uiCh,
    const uint32 *                      pTx,
    uint32 *                            pRx,
    uint32                              uiDataSize
)
{
    uint32  ret;
    uint32  i;
    SALRetCode_t ret2;

    ret = 0;
    ret2 = SAL_RET_SUCCESS;

    GPSB_D("%s: ch %d\n", __func__, uiCh);

    if((pTx == NULL_PTR) || (pRx == NULL_PTR))
    {
        ret2 = SAL_RET_FAILED;
    }
    else
    {

        gpsb[uiCh].dAsyncTxBuf      = pTx;
        gpsb[uiCh].dAsyncRxBuf      = pRx;
        gpsb[uiCh].dAsyncTxDataSize = uiDataSize;
        gpsb[uiCh].dAsyncRxDataSize = uiDataSize;

        /* Write data to FIFO */
        ret = GPSB_FifoWrite(uiCh, gpsb[uiCh].dAsyncTxBuf, gpsb[uiCh].dAsyncTxDataSize);

        if(gpsb[uiCh].dAsyncTxBuf != NULL_PTR)
        {
            for(i = 0 ; i < ret ; i++)
            {
                gpsb[uiCh].dAsyncTxBuf++;
            }
        }

        if(gpsb[uiCh].dAsyncTxDataSize > ret)
        {
            gpsb[uiCh].dAsyncTxDataSize -= ret;
        }

        /* Enable operation */
        GPSB_EnableIntr(uiCh);
        GPSB_Enable(uiCh);
    }

    return ret2;
}

/*
***************************************************************************************************
*                                          GPSB_DmaAsyncXfer
*
* Function to transfer data with dma mode.
*
* @param    uiCh [in]       : Value of channel to control
* @param    pTx [in]        : TX data buffer
* @param    pRx [out]       : RX data buffer
* @param    uiDataSize [in] : Write/Read data size
* @return
* Notes
*
***************************************************************************************************
*/

static SALRetCode_t GPSB_DmaAsyncXfer
(
    uint32                              uiCh,
    const uint32 *                      pTx,
    uint32 *                            pRx,
    uint32                              uiDataSize
)
{
    uint32  copy_length;
    uint32  bpw;
    uint32  addr;
    uint32  reg;
    SALRetCode_t ret;

    copy_length = 0;
    bpw         = 0;
    addr        = 0UL;
    reg         = 0UL;
    ret         = SAL_RET_SUCCESS;

    /* Check wrong GPSB channel number */
    if (uiCh >= GPSB_CH_NUM)
    {
        GPSB_D("%d channel is wrong\n", uiCh);

        ret = SAL_RET_FAILED;
    }
    else
    {
        if(gpsb[uiCh].dSupportDma != (uint32)TRUE)
        {
            GPSB_D("%s ch %d does not support dma transfer\n", __func__, uiCh);

            ret = SAL_RET_FAILED;
        }
        else
        {
            /*
             * For channels which is able to use a dedicated dma,
             * set byte endian mode according to bits per word.
             */
            bpw = GPSB_GetBpw(uiCh);

            if(bpw != 32UL)
            {
                addr = (uint32)gpsb[(uiCh)].dBase + GPSB_DMACTR;
                reg = SAL_ReadReg(addr) | (BSP_BIT_28);
                SAL_WriteReg(reg, addr);
            }
            else
            {
                /* When bits per word is equal to 32, disable half-word and byte */
                addr = gpsb[uiCh].dBase + GPSB_INTEN;
                reg = SAL_ReadReg(addr) & ~(BSP_BIT_24 | BSP_BIT_25 | BSP_BIT_26 | BSP_BIT_27);
                SAL_WriteReg(reg, addr);

                addr = gpsb[uiCh].dBase + GPSB_DMACTR;
                reg = SAL_ReadReg(addr) & (~BSP_BIT_28);
                SAL_WriteReg(reg, addr);

            }

            /* Clear packet counter (CH 0-2) */
            addr = (uint32)gpsb[(uiCh)].dBase + GPSB_DMACTR;
            reg = SAL_ReadReg(addr) | (BSP_BIT_02);
            SAL_WriteReg(reg, addr);

            addr = gpsb[uiCh].dBase + GPSB_DMACTR;
            reg = SAL_ReadReg(addr) & (~BSP_BIT_02);
            SAL_WriteReg(reg, addr);

            if(uiDataSize > gpsb[uiCh].dTxDma.dbSize)
            {
                copy_length = gpsb[uiCh].dTxDma.dbSize;
            }
            else
            {
                copy_length = uiDataSize;
            }

            if(pTx != NULL_PTR)
            {
                (void)SAL_MemCopy(gpsb[uiCh].dTxDma.dbAddr, pTx, copy_length);
            }

            /* Set DMA TXBASE and RXBASE */
            (void)GPSB_DmaSetAddr(uiCh, gpsb[uiCh].dTxDma.dbAddr, gpsb[uiCh].dRxDma.dbAddr);

            /* Set Packet size and count */
            SAL_WriteReg((uint32)((copy_length & 0x1FFFUL) << 0UL), (uint32)(gpsb[uiCh].dBase + GPSB_PACKET));

            gpsb[uiCh].dAsyncTxBuf      = pTx;
            gpsb[uiCh].dAsyncRxBuf      = pRx;
            gpsb[uiCh].dAsyncTxDataSize = uiDataSize;
            gpsb[uiCh].dAsyncRxDataSize = 0;

            /* Enable dma operation */
            GPSB_Enable(uiCh);
            GPSB_DmaStart(uiCh);

            ret = SAL_RET_SUCCESS;
        }
    }

    return ret;
}

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
* @param    uiXferMode [in] : Transfer mode (DMA/PIO , Interrupt/Non-Interrupt)
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
)
{
    SALRetCode_t    ret;
    uint32          bpw;
    uint32          word_size;
    uint32          addr;
    uint32          reg;

    ret         = SAL_RET_SUCCESS;
    bpw         = 0;
    word_size   = 0;
    addr        = 0UL;
    reg         = 0UL;

    if (uiCh >= GPSB_CH_NUM)
    {
        GPSB_D("%d channel is wrong\n", uiCh);

        ret = SAL_RET_FAILED;
    }
    else
    {
        if(gpsb[uiCh].dState != GPSB_XFER_STATE_IDLE)
        {
            GPSB_D("%s: ch %d is not in idle state : %d\n", __func__, uiCh, gpsb[uiCh].dState);

            if(gpsb[uiCh].dState != GPSB_XFER_STATE_IDLE)
            {
                ret = SAL_RET_FAILED;
            }
        }


        if(ret == SAL_RET_SUCCESS)
        {
            /* Check data length and bpw */
            if(uiDataSize == 0UL)
            {
                GPSB_D("%s: ch %d data size is zero\n", __func__, uiCh);

                ret = SAL_RET_FAILED;
            }
            else
            {
                bpw = GPSB_GetBpw(uiCh);

                if(bpw <= 8UL)
                {
                    word_size = 1UL;
                }
                else if(bpw <= 16UL)
                {
                    word_size = 2UL;
                }
                else
                {
                    word_size = 4UL;
                }

                if((uiDataSize % word_size) != 0UL)
                {
                    GPSB_D("%s: ch %d data size(%d) is not aligned by word size(%d)\n",
                            __func__, uiCh, uiDataSize, word_size);

                    ret = SAL_RET_FAILED;
                }
                else
                {
                    /*
                     * Set GPSB CTF(Continuous Transfer mode) according to the flag.
                     * In case of transfer without CTF mode, you should configure
                     * GPSB PCFG FRM settings by using tcc_GPSB_master_alloc_cs() before
                     * transfer.
                     * In case of transfer with CTF mode, you can control CS as GPIO
                     * or assign CS to GPSB FRM.
                     */
                    if((uiXferMode & GPSB_XFER_MODE_WITHOUT_CTF) != 0UL)
                    {
                        addr = gpsb[uiCh].dBase + GPSB_MODE;
                        reg = SAL_ReadReg(addr) & (~BSP_BIT_04);
                        SAL_WriteReg(reg, addr);
                    }
                    else
                    {
                        addr = (uint32)gpsb[(uiCh)].dBase + GPSB_MODE;
                        reg = SAL_ReadReg(addr) | (BSP_BIT_04);
                        SAL_WriteReg(reg, addr);
                    }

                    /* Start transfer according to xfer mode */
                    if((uiXferMode & GPSB_XFER_MODE_WITH_INTERRUPT) != 0UL)
                    {
                        /* w/ interrupt mode (support non-bloning mode only) */

                        GPSB_SetXferState(uiCh, GPSB_XFER_STATE_RUNNING);

                        if((uiXferMode & GPSB_XFER_MODE_DMA) != 0UL)
                        {
                            /* DMA mode */
                            ret = GPSB_DmaAsyncXfer(uiCh, pTx, pRx, uiDataSize);
                        }
                        else
                        {
                            /* Non-DMA mode */
                            ret = GPSB_FiFoAsyncXfer(uiCh, pTx, pRx, uiDataSize);
                        }

                    }
                    else
                    {
                        GPSB_D("%s: ch %d unsupported transfer mode 0x%x\n", __func__, uiCh, uiXferMode);

                        ret = SAL_RET_FAILED;
                    }
                }
            }
        }
    }

    return ret;

}
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
* @param    uiXferMode [in] : Transfer mode (DMA/PIO , Interrupt/Non-Interrupt)
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
)
{
    SALRetCode_t    ret;
    uint32          bpw;
    uint32          word_size;
    uint32          addr;
    uint32          reg;

    ret         = SAL_RET_SUCCESS;
    bpw         = 0;
    word_size   = 0;
    addr        = 0UL;
    reg         = 0UL;

    if (uiCh >= GPSB_CH_NUM)
    {
        GPSB_D("%d channel is wrong\n", uiCh);

        ret = SAL_RET_FAILED;
    }
    else
    {
        if(gpsb[uiCh].dState != GPSB_XFER_STATE_IDLE)
        {
            GPSB_D("%s: ch %d is not in idle state : %d\n", __func__, uiCh, gpsb[uiCh].dState);

            if(gpsb[uiCh].dState != GPSB_XFER_STATE_IDLE)
            {
                ret = SAL_RET_FAILED;
            }
        }

        if(ret == SAL_RET_SUCCESS)
        {
            /* Check data length and bpw */
            if(uiDataSize == 0UL)
            {
                GPSB_D("%s: ch %d data size is zero\n", __func__, uiCh);

                ret = SAL_RET_FAILED;
            }
            else
            {

                bpw = GPSB_GetBpw(uiCh);

                if(bpw <= 8UL)
                {
                    word_size = 1UL;
                }
                else if(bpw <= 16UL)
                {
                    word_size = 2UL;
                }
                else
                {
                    word_size = 4UL;
                }

                if((uiDataSize % word_size) != 0UL)
                {
                    GPSB_D("%s: ch %d data size(%d) is not aligned by word size(%d)\n",
                            __func__, uiCh, uiDataSize, word_size);

                    ret = SAL_RET_FAILED;
                }
                else
                {
                    /*
                     * Set GPSB CTF(Continuous Transfer mode) according to the flag.
                     * In case of transfer without CTF mode, you should configure
                     * GPSB PCFG FRM settings by using tcc_GPSB_master_alloc_cs() before
                     * transfer.
                     * In case of transfer with CTF mode, you can control CS as GPIO
                     * or assign CS to GPSB FRM.
                     */
                    if((uiXferMode & GPSB_XFER_MODE_WITHOUT_CTF) != 0UL)
                    {
                        addr = gpsb[uiCh].dBase + GPSB_MODE;
                        reg = SAL_ReadReg(addr) & (~BSP_BIT_04);
                        SAL_WriteReg(reg, addr);
                    }
                    else
                    {
                        addr = (uint32)gpsb[(uiCh)].dBase + GPSB_MODE;
                        reg  = SAL_ReadReg(addr) | (BSP_BIT_04);
                        SAL_WriteReg(reg, addr);
                    }

                    if((uiXferMode & GPSB_XFER_MODE_WITHOUT_INTERRUPT) != 0UL)
                    {
                        GPSB_SetXferState(uiCh, GPSB_XFER_STATE_RUNNING);
                        /* w/o interrupt mode (support blocking mode only) */
                        if(bpw <= 8UL)
                        {
                            ret = GPSB_FifoSyncXfer8(uiCh, pTx, pRx, uiDataSize);
                        }
                        else if(bpw <= 16UL)
                        {
                            ret = GPSB_FifoSyncXfer16(uiCh, pTx, pRx, uiDataSize);
                        }
                        else
                        {
                            ret = GPSB_FifoSyncXfer32(uiCh, pTx, pRx, uiDataSize);
                        }

                        if(ret == SAL_RET_SUCCESS)
                        {
                            GPSB_Disable(uiCh);
                            GPSB_SetXferState(uiCh, GPSB_XFER_STATE_IDLE);
                        }
                    }
                    else
                    {
                        GPSB_D("%s: ch %d unsupported transfer mode 0x%x\n", __func__, uiCh, uiXferMode);

                        ret = SAL_RET_FAILED;
                    }
                }
            }
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          GPSB_HwInit
*
* Function to set default hw settings.
*
* @param    uiCh [in]       : Value of channel to control
* @return
* Notes
*
***************************************************************************************************
*/

static void GPSB_HwInit
(
    uint32                              uiCh
)
{
    sint32  ret;

    ret = 0;

    /* Enable iobus */
    ret = CLOCK_SetIobusPwdn((gpsb[uiCh].dIobusName), FALSE);

    if(ret != 0)
    {
        GPSB_D("%s: ch %d failed to enable iobus pwdn\n", __func__, uiCh);
    }
    else
    {
        /* Disable operation */
        GPSB_Disable(uiCh);

        /* Set GPSB DIVLDV */
        GPSB_BitCSet(gpsb[uiCh].dBase + GPSB_MODE, (0xFFUL << 24UL), ((GPSB_DEF_DIVLDV & 0xFFUL) << 24UL));

        if(gpsb[uiCh].dIsSlave == TRUE)
        {
            /* Set GPSB MODE register */
            GPSB_BitCSet(gpsb[uiCh].dBase + GPSB_MODE, ((BSP_BIT_04|BSP_BIT_02 | BSP_BIT_01) | BSP_BIT_00),
                                                         (BSP_BIT_02));     // Set Slave Mode
        }
        else
        {
            /* Set GPSB MODE register */
            GPSB_BitCSet(gpsb[uiCh].dBase + GPSB_MODE, ((BSP_BIT_02 | BSP_BIT_01) | BSP_BIT_00),  /* Set GPSB master */
                                                         (BSP_BIT_04));                            /* Set CTF mode */
        }

        /* Set interrup for handle error status (for ROR, WUR, RUR, WOR) */
        GPSB_BitCSet(gpsb[uiCh].dBase + GPSB_INTEN, (0x3FFUL),
                       (BSP_BIT_05 | BSP_BIT_06 | BSP_BIT_07 | BSP_BIT_08));


        /* Set GPSB FIFO Theshold for generating interrupt request */
        GPSB_BitCSet(gpsb[uiCh].dBase + GPSB_INTEN, (0x7UL << 20UL), (GPSB_CFGWTH << 20UL));
        GPSB_BitCSet(gpsb[uiCh].dBase + GPSB_INTEN, (0x7UL << 16UL), (GPSB_CFGRTH << 16UL));
    }

}

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
)
{
    SALRetCode_t    ret;

    ret = SAL_RET_SUCCESS;

    if(OpenParam.uiIsSlave == GPSB_SLAVE_MODE)
    {
        gpsb[uiCh].dIsSlave = TRUE;
    }
    else
    {
        gpsb[uiCh].dIsSlave = FALSE;
    }

    /* Check wrong GPSB channel number */
    if(uiCh >= GPSB_CH_NUM)
    {
        GPSB_D("%s: %d channel is wrong\n", __func__, uiCh);

        ret = SAL_RET_FAILED;
    }
    else
    {
        if(gpsb[uiCh].dState != GPSB_XFER_STATE_DISABLED)
        {
            GPSB_D("%s: %d channel is already opened\n", __func__, uiCh);

            ret = SAL_RET_FAILED;
        }
        else
        {
            gpsb[uiCh].dState = GPSB_XFER_STATE_IDLE;

            /* Initialize the gpsb structure */
            gpsb[uiCh].dSdo                     = OpenParam.uiSdo;
            gpsb[uiCh].dSdi                     = OpenParam.uiSdi;
            gpsb[uiCh].dSclk                    = OpenParam.uiSclk;
            gpsb[uiCh].dComplete.xcCallback     = OpenParam.fbCallback;
            gpsb[uiCh].dComplete.xcArg          = OpenParam.pArg;
            gpsb[uiCh].dSupportDma              = (uint32)FALSE;
            gpsb[uiCh].dTxDma.dbAddr            = NULL;
            gpsb[uiCh].dTxDma.dbSize            = 0;

            if((OpenParam.uiDmaBufSize != (uint32)NULL) && ((OpenParam.pDmaAddrTx != (uint32 *)NULL_PTR) || (OpenParam.pDmaAddrRx != (uint32 *)NULL_PTR)))
            {
                gpsb[uiCh].dSupportDma = (uint32)TRUE;

                if(OpenParam.pDmaAddrTx != (uint32 *)NULL_PTR)
                {
                    gpsb[uiCh].dTxDma.dbAddr = OpenParam.pDmaAddrTx;
                    gpsb[uiCh].dTxDma.dbSize = OpenParam.uiDmaBufSize;
                    GPSB_D("%s: tx_dma.paddr: 0x%08X      / size: %d\n",
                            __func__, gpsb[uiCh].dTxDma.dbAddr,
                            gpsb[uiCh].dTxDma.dbSize);
                }

                if(OpenParam.pDmaAddrRx != NULL_PTR)
                {
                    gpsb[uiCh].dRxDma.dbAddr = OpenParam.pDmaAddrRx;
                    gpsb[uiCh].dRxDma.dbSize = OpenParam.uiDmaBufSize;
                    GPSB_D("%s: rx_dma.Addr: 0x%08X\n",
                            __func__, gpsb[uiCh].dRxDma.dbAddr,
                            gpsb[uiCh].dRxDma.dbSize);
                }
            }

            /* Reset GPSB block */
            ret = GPSB_Reset(uiCh);

            if(ret != SAL_RET_SUCCESS)
            {
                GPSB_D("%s: ch %d failed to reset gpsb ret %d\n", __func__, uiCh, ret);
            }
            else
            {
                    /* Set port and gpio configuration */
                    ret = GPSB_SetPort(uiCh, OpenParam.uiSdo, OpenParam.uiSdi, OpenParam.uiSclk);

                if(ret != SAL_RET_SUCCESS)
                {
                    GPSB_D("%s: ch %d gpsb set port failed ret %d\n", __func__, uiCh, ret);
                }
                else
                {
                    /* Stop dma operation */
                    GPSB_DmaStop(uiCh);

                    /* Initialize GPSB for GPSB master*/
                    GPSB_HwInit(uiCh);

				    if(gpsb[uiCh].dIsSlave == TRUE)
				    {
				        GPSB_Enable(uiCh);
				    }

    				GPSB_SetXferState(uiCh, GPSB_XFER_STATE_IDLE);
				}
			}
		}
	}

    return ret;
}

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
)
{
    sint32  err;
    SALRetCode_t ret;

    err     = 0;
    ret     = SAL_RET_SUCCESS;

    /* Check wrong GPSB channel number */
    if(uiCh >= GPSB_CH_NUM)
    {
        GPSB_D("%s %d channel is wrong\n", __func__, uiCh);

        ret = SAL_RET_FAILED;
    }
    else
    {
        (void)SAL_CoreCriticalEnter();

        if(gpsb[uiCh].dState == GPSB_XFER_STATE_DISABLED)
        {
            (void)SAL_CoreCriticalExit();
            GPSB_D("%s %d channel is already closed\n", __func__, uiCh);

            ret = SAL_RET_SUCCESS;
        }
        else
        {
            gpsb[uiCh].dState = GPSB_XFER_STATE_DISABLED;
            (void)SAL_CoreCriticalExit();

            /* Stop the operation */
            GPSB_DmaStop(uiCh);
            GPSB_DisableIntr(uiCh);
            GPSB_Disable(uiCh);

            /* Disable iobus */
            err = CLOCK_SetIobusPwdn(gpsb[uiCh].dIobusName, TRUE);

            if(err != NULL)
            {
                GPSB_D("%s ch %d failed to disable iobus pwdn\n", __func__, uiCh);

                ret = SAL_RET_FAILED;
            }
            else
            {
                gpsb[uiCh].dPort = NULL;

			    GPSB_SetXferState(uiCh, GPSB_XFER_STATE_DISABLED);

			    ret = SAL_RET_SUCCESS;
			}
		}
	}
	return ret;
}

/*
***************************************************************************************************
*                                          GPSB_Isr
*
* Function to process GPSB interrupt.
*
* @param    uiCh [in]       : Value of channel to control
* @return
* Notes
*
***************************************************************************************************
*/

static uint32 GPSB_CheckIsrStatus
(
    uint32 uiCh,
    uint32 status
)
{
    uint32 event;

    event = 0;

    GPSB_D("%s: ch %d error status occur status 0x%08x\n", __func__, uiCh, status);

    if((status & BSP_BIT_05) != 0UL)
    {
        event |= (uint32)GPSB_EVENT_ERR_ROR;
        GPSB_D("%s: ch %d read fifo over-run error\n", __func__, uiCh);
    }

    if((status & BSP_BIT_06) != 0UL)
    {
        event |= (uint32)GPSB_EVENT_ERR_WUR;
        GPSB_D("%s: ch %d write fifo under-run error\n", __func__, uiCh);
    }

    if((status & BSP_BIT_07) != 0UL)
    {
        event |= (uint32)GPSB_EVENT_ERR_RUR;
        GPSB_D("%s: ch %d read fifo under-run error\n", __func__, uiCh);
    }

    if((status & BSP_BIT_08) != 0UL)
    {
        event |= (uint32)GPSB_EVENT_ERR_WOR;
        GPSB_D("%s: ch %d write fifo over-run error\n", __func__, uiCh);
    }

    return event;
}

/*
***************************************************************************************************
*                                          GPSB_IsrDone
*
* Function to process isr complete.
*
* @param    uiCh [in]       : Value of channel to control
* @return
* Notes
*
***************************************************************************************************
*/

static void GPSB_IsrDone(uint32 uiCh)
{
    GPSB_Disable(uiCh);
    GPSB_DmaStop(uiCh);
    GPSB_SetXferState(uiCh, GPSB_XFER_STATE_IDLE);
}

/*
***************************************************************************************************
*                                          GPSB_IsrError
*
* Function to process isr error.
*
* @param    uiCh [in]       : Value of channel to control
* @param    uiStatus [in]   : Value of status register.
* @return
* Notes
*
***************************************************************************************************
*/

static void GPSB_IsrError(uint32 uiCh, uint32 uiStatus)
{
    uint32 event;
    uint32 addr;
    uint32 reg;

    event = GPSB_CheckIsrStatus(uiCh, uiStatus);
    addr = 0UL;
    reg  = 0UL;

    GPSB_IsrDone(uiCh);

    if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_STAT))
    {
        /* Clear error status */
        addr = (uint32)gpsb[(uiCh)].dBase + GPSB_STAT;
        reg  = SAL_ReadReg(addr) | (BSP_BIT_05 | BSP_BIT_06 | BSP_BIT_07 | BSP_BIT_08);
        SAL_WriteReg(reg, addr);
    }

    /* Notify error status */
    if(gpsb[uiCh].dComplete.xcCallback != NULL_PTR)
    {
        gpsb[uiCh].dComplete.xcCallback(uiCh, event, gpsb[uiCh].dComplete.xcArg);
    }
}

/*
***************************************************************************************************
*                                          GPSB_IsrAsyncFifo
*
* Function to process Async fifo transfer.
*
* @param    uiCh [in]       : Value of channel to control
* @return
* Notes
*
***************************************************************************************************
*/

static void GPSB_IsrAsyncFifo
(
    uint32                              uiCh
)
{
    uint32 ret;
    uint32 i;
    uint32 event;

    GPSB_D("%s: ch %d async pio transfer\n", __func__, uiCh);

    ret = GPSB_FifoRead(uiCh, gpsb[uiCh].dAsyncRxBuf, gpsb[uiCh].dAsyncRxDataSize);

    if(gpsb[uiCh].dAsyncRxBuf != NULL_PTR)
    {
        for(i = 0 ; i < ret ; i++)
        {
            gpsb[uiCh].dAsyncRxBuf++;
        }
    }

    if(gpsb[uiCh].dAsyncRxDataSize >= ret)
    {
        gpsb[uiCh].dAsyncRxDataSize -= ret;
    }

    if(gpsb[uiCh].dAsyncTxDataSize > 0UL)
    {
        ret = GPSB_FifoWrite(uiCh, gpsb[uiCh].dAsyncTxBuf, gpsb[uiCh].dAsyncTxDataSize);

        if(gpsb[uiCh].dAsyncTxBuf != NULL_PTR)
        {
            for(i = 0 ; i < ret ; i++)
            {
                gpsb[uiCh].dAsyncTxBuf++;
            }
        }

        if(gpsb[uiCh].dAsyncTxDataSize >= ret)
        {
            gpsb[uiCh].dAsyncTxDataSize -= ret;
        }
    }

    if(gpsb[uiCh].dAsyncRxDataSize <= 0UL)
    {
        GPSB_D("%s: ch %d complete async pio transfer\n", __func__, uiCh);

        gpsb[uiCh].dAsyncTxBuf      = NULL;
        gpsb[uiCh].dAsyncRxBuf      = NULL;
        gpsb[uiCh].dAsyncTxDataSize = 0;

        GPSB_DisableIntr(uiCh);
        GPSB_Disable(uiCh);
        gpsb[uiCh].dState = GPSB_XFER_STATE_IDLE;

        /* Notify the completion of transfer */
        event = GPSB_EVENT_COMPLETE;

        if(gpsb[uiCh].dComplete.xcCallback != NULL_PTR)
        {
            gpsb[uiCh].dComplete.xcCallback(uiCh, event, gpsb[uiCh].dComplete.xcArg);
        }
    }
}

/*
 ***************************************************************************************************
*                                          GPSB_Isr
*
* Function to process GPSB interrupt.
*
* @param    uiCh [in]       : Value of channel to control
* @return
* Notes
*
***************************************************************************************************
*/

static void GPSB_Isr
(
    uint32                              uiCh
)
{
    uint32  dmaicr;
    uint32  status;
    uint32  event;
    uint32  copy_length;
    uint32  data_size;
    uint32  i;
    const void *  buffaddr;

    buffaddr    = (void *)gpsb[uiCh].dRxDma.dbAddr;
    dmaicr      = 0;
    status      = 0;
    event       = 0;
    copy_length = 0;
    data_size   = 0;

    //if((GPSB_IsMaster(uiCh)) > 0UL)
    {
        /* Check error status */
        if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_STAT))
        {
            status = SAL_ReadReg((uint32)(gpsb[uiCh].dBase + GPSB_STAT));
        }

        if((status & (0xFUL << 5UL)) != 0UL)
        {
            GPSB_IsrError(uiCh, status);
        }
        else
        {
            if((status & BSP_BIT_00) != 0UL)
            {
                GPSB_IsrAsyncFifo(uiCh);
            }

            if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_DMAICR))
            {
                dmaicr = SAL_ReadReg((uint32)(gpsb[uiCh].dBase + GPSB_DMAICR));
            }

            if((dmaicr & BSP_BIT_29) != 0UL)
            {
                GPSB_D("%s: ch %d dma done interrupt\n", __func__, uiCh);

                if(gpsb[uiCh].dBase < (UINT_MAX_VALUE - GPSB_DMAICR))
                {
                    SAL_WriteReg(dmaicr, (uint32)(gpsb[uiCh].dBase + GPSB_DMAICR));
                }

                data_size = gpsb[uiCh].dAsyncTxDataSize;

                if(data_size > gpsb[uiCh].dTxDma.dbSize)
                {
                    copy_length = gpsb[uiCh].dTxDma.dbSize;
                }
                else
                {
                    copy_length = data_size;
                }

                if(gpsb[uiCh].dAsyncRxBuf != NULL_PTR)
                {
                    //(void)SAL_MemCopy(gpsb[uiCh].dAsyncRxBuf, (void *)(gpsb[uiCh].dRxDma.dbAddr), copy_length);
                    (void)SAL_MemCopy(gpsb[uiCh].dAsyncRxBuf, buffaddr, copy_length);
                }


                if(gpsb[uiCh].dAsyncTxDataSize >= copy_length)
                {
                    gpsb[uiCh].dAsyncTxDataSize -= copy_length;
                }

                for(i = 0 ; i < copy_length ; i++)
                {
                    gpsb[uiCh].dAsyncTxBuf++;
                    gpsb[uiCh].dAsyncRxBuf++;
                }

                GPSB_D("%s: ch %d xfer in this time %d remainder %d\n",
                        __func__, uiCh, copy_length, gpsb[uiCh].dAsyncTxDataSize);

                if(gpsb[uiCh].dAsyncTxDataSize != 0UL)
                {
                    data_size = gpsb[uiCh].dAsyncTxDataSize;

                    if(data_size > gpsb[uiCh].dTxDma.dbSize)
                    {
                        copy_length = gpsb[uiCh].dTxDma.dbSize;
                    }
                    else
                    {
                        copy_length = data_size;
                    }

                    if(gpsb[uiCh].dAsyncTxBuf  != NULL_PTR)
                    {
                        (void)SAL_MemCopy((void *)(gpsb[uiCh].dTxDma.dbAddr), gpsb[uiCh].dAsyncTxBuf, copy_length);
                    }

                    /* Set Packet size and count */
                    SAL_WriteReg(((copy_length & 0x1FFFUL) << 0UL), (uint32)(gpsb[uiCh].dBase + GPSB_PACKET));

                    /* Re-start DMA for transferring remainder */
                    GPSB_DmaStart(uiCh);

                }
                else
                {
                    gpsb[uiCh].dAsyncTxBuf      = NULL;
                    gpsb[uiCh].dAsyncRxBuf      = NULL;
                    gpsb[uiCh].dAsyncTxDataSize = 0;
                    gpsb[uiCh].dAsyncRxDataSize = 0;

                    GPSB_IsrDone(uiCh);

                    /* Notify the completion of transfer */
                    event = GPSB_EVENT_COMPLETE;

                    if(gpsb[uiCh].dComplete.xcCallback != NULL_PTR)
                    {
                        gpsb[uiCh].dComplete.xcCallback(uiCh, event, gpsb[uiCh].dComplete.xcArg);
                    }
                }
            }
        }
    }
}


/*
***************************************************************************************************
*                                          GPSB_SetLbCfg
*
* Function to set channel to enable loopback.
*
* @param    uiMstCh [in]    : Master Channel index
* @param    uiLoopCh [in]   : Loopback Channel index
* @param    uiEnable [in]   : Loopback enable
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
)
{
    uint32  reg;
    SALRetCode_t ret;

    reg = 0UL;
    ret = SAL_RET_SUCCESS;

    if((uiMstCh >= GPSB_CH_NUM) || (uiLoopCh >= GPSB_CH_NUM))
    {
        GPSB_D("Invaild Channel value. \n");

        ret = SAL_RET_FAILED;
    }
    else
    {
        if(gpsb[0].dCfgAddr < (UINT_MAX_VALUE - GPSB_LB_CFG))
        {
            reg = SAL_ReadReg((uint32)(gpsb[0].dCfgAddr + GPSB_LB_CFG));

            if(uiEnable < 2UL)
            {
                reg |= (((uiEnable + ((uiMstCh & 0x7UL) << 1UL))) << uiLoopCh) * 4UL;
            }

            SAL_WriteReg(reg, (uint32)(gpsb[0].dCfgAddr + (uint32)GPSB_LB_CFG));

            GPSB_D("GPSB LB_CFG : %x\n", reg);
        }
    }

    return ret;
}

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
)
{
    if(gpsb[0].dCfgAddr < (UINT_MAX_VALUE - GPSB_LB_CFG))
    {
        SAL_WriteReg(0x0UL , (uint32)(gpsb[0].dCfgAddr + GPSB_LB_CFG));

        GPSB_D("Done.Clear LB_CFG : %x \n", SAL_ReadReg((uint32)(gpsb[0].dCfgAddr + GPSB_LB_CFG)));
    }

    return SAL_RET_SUCCESS;
}

/*
***************************************************************************************************
*                                          GPSB_Isr0
*
* Function to process interrupt about channel 0.
*
* @param    pArg
* @return
* Notes
*
***************************************************************************************************
*/

static void GPSB_Isr0
(
    void *                              pArg
)
{
    (void)pArg;
    GPSB_D("%s: enter ISR0\n", __func__);
    GPSB_Isr(0);
}

/*
***************************************************************************************************
*                                          GPSB_Isr1
*
* Function to process interrupt about channel 1.
*
* @param    pArg
* @return
* Notes
*
***************************************************************************************************
*/

static void GPSB_Isr1
(
    void *                              pArg
)
{
    (void)pArg;
    GPSB_D("%s: enter ISR1\n", __func__);
    GPSB_Isr(1);
}

/*
***************************************************************************************************
*                                          GPSB_Isr2
*
* Function to process interrupt about channel 2.
*
* @param    pArg
* @return
* Notes
*
***************************************************************************************************
*/

static void GPSB_Isr2
(
    void *                              pArg
)
{
    (void)pArg;
    GPSB_D("%s: enter ISR2\n", __func__);
    GPSB_Isr(2);
}

/*
***************************************************************************************************
*                                          GPSB_Isr3
*
* Function to process interrupt about channel 3.
*
* @param    pArg
* @return
* Notes
*
***************************************************************************************************
*/

static void GPSB_Isr3
(
    void *                              pArg
)
{
    (void)pArg;
    GPSB_D("%s: enter ISR3\n", __func__);
    GPSB_Isr(3);
}

/*
***************************************************************************************************
*                                          GPSB_Isr4
*
* Function to process interrupt about channel 4.
*
* @param    pArg
* @return
* Notes
*
***************************************************************************************************
*/

static void GPSB_Isr4
(
    void *                              pArg
)
{
    (void)pArg;
    GPSB_D("%s: enter ISR4\n", __func__);
    GPSB_Isr(4);
}

/*
***************************************************************************************************
*                                          GPSB_Isr5
*
* Function to process interrupt about channel 5.
*
* @param    pArg
* @return
* Notes
*
***************************************************************************************************
*/

static void GPSB_Isr5
(
    void *                              pArg
)
{
    (void)pArg;
    GPSB_D("%s: enter ISR5\n", __func__);
    GPSB_Isr(5);
}

/*
***************************************************************************************************
*                                          GPSB_DmaIsr0
*
* Function to process dma interrupt about channel 0.
*
* @param    pArg
* @return
* Notes
*
***************************************************************************************************
*/

static void GPSB_DmaIsr0
(
    void *                              pArg
)
{
    (void)pArg;
    GPSB_D("%s: enter DMA-ISR0\n", __func__);
    GPSB_Isr(0);
}

/*
***************************************************************************************************
*                                          GPSB_DmaIsr1
*
* Function to process dma interrupt about channel 1.
*
* @param    pArg
* @return
* Notes
*
***************************************************************************************************
*/

static void GPSB_DmaIsr1
(
    void *                              pArg
)
{
    (void)pArg;
    GPSB_D("%s: enter DMA-ISR1\n", __func__);
    GPSB_Isr(1);
}

/*
***************************************************************************************************
*                                          GPSB_DmaIsr2
*
* Function to process dma interrupt about channel 2.
*
* @param    pArg
* @return
* Notes
*
***************************************************************************************************
*/

static void GPSB_DmaIsr2
(
    void *                              pArg
)
{
    (void)pArg;
    GPSB_D("%s: enter DMA-ISR2\n", __func__);
    GPSB_Isr(2);
}

/*
***************************************************************************************************
*                                          GPSB_DmaIsr3
*
* Function to process dma interrupt about channel 3.
*
* @param    pArg
* @return
* Notes
*
***************************************************************************************************
*/


static void GPSB_DmaIsr3
(
    void *                              pArg
)
{
    (void)pArg;
    GPSB_D("%s: enter DMA-ISR3\n", __func__);
    GPSB_Isr(3);
}

/*
***************************************************************************************************
*                                          GPSB_DmaIsr4
*
* Function to process dma interrupt about channel 4.
*
* @param    pArg
* @return
* Notes
*
***************************************************************************************************
*/

static void GPSB_DmaIsr4
(
    void *                              pArg
)
{
    (void)pArg;
    GPSB_D("%s: enter DMA-ISR4\n", __func__);
    GPSB_Isr(4);
}

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
)
{
    /* Register gpsb irq handler */
    (void)GIC_IntVectSet((uint32)GIC_GPSB, (uint32)GIC_PRIORITY_NO_MEAN,
                         (uint8)GIC_INT_TYPE_LEVEL_HIGH, &GPSB_Isr0, NULL_PTR);
    (void)GIC_IntSrcEn(GIC_GPSB);

    (void)GIC_IntVectSet((uint32)GIC_GPSB1, (uint32)GIC_PRIORITY_NO_MEAN,
                         (uint8)GIC_INT_TYPE_LEVEL_HIGH, &GPSB_Isr1, NULL_PTR);
    (void)GIC_IntSrcEn(GIC_GPSB1);

    (void)GIC_IntVectSet((uint32)GIC_GPSB2, (uint32)GIC_PRIORITY_NO_MEAN,
                         (uint8)GIC_INT_TYPE_LEVEL_HIGH, &GPSB_Isr2, NULL_PTR);
    (void)GIC_IntSrcEn(GIC_GPSB2);

    (void)GIC_IntVectSet((uint32)GIC_GPSB3, (uint32)GIC_PRIORITY_NO_MEAN,
                         (uint8)GIC_INT_TYPE_LEVEL_HIGH, &GPSB_Isr3, NULL_PTR);
    (void)GIC_IntSrcEn(GIC_GPSB3);

    (void)GIC_IntVectSet((uint32)GIC_GPSB4, (uint32)GIC_PRIORITY_NO_MEAN,
                         (uint8)GIC_INT_TYPE_LEVEL_HIGH, &GPSB_Isr4, NULL_PTR);
    (void)GIC_IntSrcEn(GIC_GPSB4);

    // DMA Intr

    (void)GIC_IntVectSet((uint32)GIC_GPSB0_DMA, (uint32)GIC_PRIORITY_NO_MEAN,
                        (uint8)GIC_INT_TYPE_LEVEL_HIGH, &GPSB_DmaIsr0, NULL_PTR);
    (void)GIC_IntSrcEn(GIC_GPSB0_DMA);

    (void)GIC_IntVectSet((uint32)GIC_GPSB1_DMA, (uint32)GIC_PRIORITY_NO_MEAN,
                         (uint8)GIC_INT_TYPE_LEVEL_HIGH, &GPSB_DmaIsr1, NULL_PTR);
    (void)GIC_IntSrcEn(GIC_GPSB1_DMA);

    (void)GIC_IntVectSet((uint32)GIC_GPSB2_DMA, (uint32)GIC_PRIORITY_NO_MEAN,
                         (uint8)GIC_INT_TYPE_LEVEL_HIGH, &GPSB_DmaIsr2, NULL_PTR);
    (void)GIC_IntSrcEn(GIC_GPSB2_DMA);

    (void)GIC_IntVectSet((uint32)GIC_GPSB3_DMA, (uint32)GIC_PRIORITY_NO_MEAN,
                         (uint8)GIC_INT_TYPE_LEVEL_HIGH, &GPSB_DmaIsr3, NULL_PTR);
    (void)GIC_IntSrcEn(GIC_GPSB3_DMA);

    (void)GIC_IntVectSet((uint32)GIC_GPSB4_DMA, (uint32)GIC_PRIORITY_NO_MEAN,
                         (uint8)GIC_INT_TYPE_LEVEL_HIGH, &GPSB_DmaIsr4, NULL_PTR);
    (void)GIC_IntSrcEn(GIC_GPSB4_DMA);

    return;
}

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

void GPSB_Deinit(void)
{
    (void)GIC_IntSrcDis(GIC_GPSB);
    (void)GIC_IntSrcDis(GIC_GPSB1);
    (void)GIC_IntSrcDis(GIC_GPSB2);
    (void)GIC_IntSrcDis(GIC_GPSB3);
    (void)GIC_IntSrcDis(GIC_GPSB4);

    (void)GIC_IntSrcDis(GIC_GPSB0_DMA);
    (void)GIC_IntSrcDis(GIC_GPSB1_DMA);
    (void)GIC_IntSrcDis(GIC_GPSB2_DMA);
    (void)GIC_IntSrcDis(GIC_GPSB3_DMA);
    (void)GIC_IntSrcDis(GIC_GPSB4_DMA);

    return;
}

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
)
{
    SALRetCode_t    ret;
    uint32          tmp;

    ret = SAL_RET_SUCCESS;
    tmp = 0;

    if(gpsb[0].dCfgAddr < (UINT_MAX_VALUE - GPSB_PCFG_WR_PW))
    {
        SAL_WriteReg(GPSB_PORT_PSWD, (uint32)(gpsb[0].dCfgAddr + GPSB_PCFG_WR_PW));
    }

    if(gpsb[0].dCfgAddr < (UINT_MAX_VALUE - GPSB_PCFG_WR_LOCK))
    {
        SAL_WriteReg(GPSB_PCFG_WR_EN_BIT, (uint32)(gpsb[0].dCfgAddr + GPSB_PCFG_WR_LOCK));
        tmp = SAL_ReadReg((uint32)(gpsb[0].dCfgAddr + GPSB_PCFG_WR_LOCK));
    }

    if ((tmp & GPSB_PCFG_WR_EN_BIT) != GPSB_PCFG_WR_EN_BIT)
    {
        GPSB_D("GPSB write lock fail : %d\n", tmp);
        ret = SAL_RET_FAILED;
    }

    return ret;
}

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
)
{
    SALRetCode_t    ret;
    uint32          tmp;

    ret = SAL_RET_SUCCESS;
    tmp = 0;

    if(gpsb[0].dCfgAddr < (UINT_MAX_VALUE - GPSB_PCFG_WR_PW))
    {
        SAL_WriteReg(GPSB_PORT_PSWD, (uint32)(gpsb[0].dCfgAddr + GPSB_PCFG_WR_PW));
    }

    if(gpsb[0].dCfgAddr < (UINT_MAX_VALUE - GPSB_PCFG_WR_LOCK))
    {
        SAL_WriteReg(~(GPSB_PCFG_WR_EN_BIT), (uint32)(gpsb[0].dCfgAddr + GPSB_PCFG_WR_LOCK));
        tmp = SAL_ReadReg((uint32)(gpsb[0].dCfgAddr + GPSB_PCFG_WR_LOCK));
    }

    if ((tmp & GPSB_PCFG_WR_EN_BIT) == GPSB_PCFG_WR_EN_BIT)
    {
        GPSB_D("GPSB write lock fail : 0x%08x\n", tmp);
        ret = SAL_RET_FAILED;
    }

    return ret;
}

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
)
{
    SALRetCode_t    ret;
    uint32          tmp;

    ret = SAL_RET_SUCCESS;
    tmp = 0UL;

    GPSB_D("loopback: ch%d(m) -> ch%d(s)\n", uiMstCh, uiSlvCh);

    if ((uiMstCh >= (GPSB_CH_NUM)) || (uiSlvCh >= (GPSB_CH_NUM)))
    {
        GPSB_D("loopback supports max %d channels.\n", (GPSB_CH_NUM));
        ret = SAL_RET_FAILED;
    }

    if (ret == SAL_RET_SUCCESS)
    {
        tmp = (uiMstCh << ((uiSlvCh * 4UL) + 1UL)) | ( (uint32)0x1UL << (uiSlvCh * 4UL));

        if(gpsb[0].dCfgAddr < (UINT_MAX_VALUE - GPSB_LB_CFG))
        {
            SAL_WriteReg(tmp, (uint32)(gpsb[0].dCfgAddr + GPSB_LB_CFG));
        }
    }

    return ret;
}

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
)
{
    SALRetCode_t    ret;
    uint32          tmp;

    ret = SAL_RET_SUCCESS;
    tmp = 0UL;

    if ((uiMstCh >= (GPSB_CH_NUM)) || (uiSlvCh >= (GPSB_CH_NUM)))
    {
        GPSB_D("loopback supports max %d channels.\n", (GPSB_CH_NUM-1UL));
        ret = SAL_RET_FAILED;
    }

    if (ret == SAL_RET_SUCCESS)
    {
        if(gpsb[0].dCfgAddr < (UINT_MAX_VALUE - GPSB_LB_CFG))
        {
            tmp = SAL_ReadReg((uint32)(gpsb[0].dCfgAddr + GPSB_LB_CFG));
            tmp &= (~((0x3UL << 1UL) << (uiSlvCh * 4UL)));
            SAL_WriteReg(tmp, (uint32)(gpsb[0].dCfgAddr + GPSB_LB_CFG));
        }
    }

    return ret;
}

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
)
{
    SALRetCode_t    ret;
    uint32          tmp;

    ret = SAL_RET_SUCCESS;
    tmp = 0UL;

    if (uiCh >= GPSB_CH_NUM)
    {
        GPSB_D("IO monitor doesn't support ch %d.\n", uiCh);
        ret = SAL_RET_FAILED;
    }
    else
    {
        /* Enable I/O monitor */
        tmp = BSP_BIT_16 | uiMonWidth; // Protocol & monitor enable

        if(uiMonMode == (uint32)GPSB_SM_MON_STUCK_BOTH)
        {
            tmp |= BSP_BIT_20 | BSP_BIT_21;
        }
        else if(uiMonMode == (uint32)GPSB_SM_MON_STUCK_HIGH)
        {
            tmp |= BSP_BIT_20;
        }
        else if(uiMonMode == (uint32)GPSB_SM_MON_STUCK_LOW)
        {
            tmp |= BSP_BIT_21;
        }
        else
        {
            GPSB_D("%s : unknown GPSB_SM_MODE.\n",__func__);
        }

        if((SAL_ReadReg((uint32)(GPSB_CH_BASE(uiCh)+GPSB_MODE)) & BSP_BIT_16) > 0UL)
        {
            tmp |= BSP_BIT_17; // SCK Polarity set
        }
        else
        {
            tmp &= ~(BSP_BIT_17); // Clear SCK_POL
        }

        SAL_WriteReg(tmp, gpsb_sm[uiCh].sbBaseAddr);
        GPSB_D("Set IO Mon value Ch %d : 0x%x \n", uiCh , SAL_ReadReg(gpsb_sm[uiCh].sbBaseAddr));

    }

    return ret;
}

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
)
{
    SALRetCode_t    ret;

    ret = SAL_RET_SUCCESS;

    if (uiCh >= GPSB_CH_NUM)
    {
        GPSB_D("IO monitor doesn't supports ch %d.\n", uiCh);
        ret = SAL_RET_FAILED;
    }
    else
    {
        /* Disable I/O monitor */
        SAL_WriteReg(0x0, gpsb_sm[uiCh].sbBaseAddr);
        SAL_WriteReg((uint32)((uint32)0x1UL << (uint32)uiCh) , (uint32)(GPSB_IO_MONITOR_BASE + GPSB_IOMON_CLR));
    }

    return ret;
}

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
)
{
    SALRetCode_t    ret;
    uint32          tmp;

    ret = SAL_RET_SUCCESS;
    tmp = 0;

    if (uiCh >= GPSB_CH_NUM)
    {
        GPSB_D("GPSB-sm support max %d channels.\n", GPSB_CH_NUM);
        ret = SAL_RET_FAILED;
    }

    if (ret == SAL_RET_SUCCESS)
    {
        /* Read status */
        tmp = SAL_ReadReg(gpsb_sm[uiCh].sbBaseAddr);
        GPSB_D("Get IO Monitor value [ch : %d] : 0x%x \n", uiCh ,tmp);

        if ((tmp & BSP_BIT_24) > 0UL)
        {
            GPSB_D("SM MONITOR : PROTOCOL ERROR \n");
        }

        if ((tmp & BSP_BIT_25) > 0UL)
        {
            GPSB_D("SM MONITOR : STUCK ERROR \n");
        }
    }

    return ret;
}

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
)
{
    uint32  bpw;
    uint32  idx;

    if(uiSize > 256)
    {
        //hsj mcu_printf("Invaild data size for TX FIFO\n");
    }

    bpw = GPSB_GetBpw(uiCh);

    if(bpw == 8)
    {
        const uint8 *   data_8 = NULL_PTR;
        data_8 = (const uint8 *) pBuff;

        for(idx = 0 ; idx < uiSize ; idx++)
        {
            SAL_WriteReg(data_8[idx], gpsb[uiCh].dBase); /* write data */
        }
    }
    else if(bpw == 16)
    {
        const uint16 *  data_16 = NULL_PTR;
        data_16 = (const uint16 *) pBuff;

        for(idx = 0 ; idx < (uiSize/2) ; idx++)
        {
            SAL_WriteReg(data_16[idx], gpsb[uiCh].dBase); /* write data */
        }

    }
    else if(bpw == 32)
    {
        const uint32 *  data_32 = NULL_PTR;
        data_32 = (const uint32 *) pBuff;

        for(idx = 0 ; idx < (uiSize/4) ; idx++)
        {
            SAL_WriteReg(data_32[idx], gpsb[uiCh].dBase); /* write data */
        }
    }
    else
    {
        //hsj mcu_printf("ch %d bpw value [%d] is invaild\n", uiCh, bpw);
    }
}

/*
***************************************************************************************************
*                                          GPSB_SetSlaveDMAMode
* Function to set dma mode in slave mode.
*
* @param    uiCh [in]       : Value of channel to control
* @param    pTx [in]        : Phsical memory address for slave mode Tx DMA
* @param    pRx [in]        : Phsical memory address for slave mode Rx DMA
* @param    uiDataSize [in] : Data size
* @return   none
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
)
{
    uint32  bpw;
    uint32  copy_length;
    uint32  reg;
    uint32  addr;

    bpw  = 0;
    reg  = 0;
    addr = 0;

    /*
     * For channels which is able to use a dedicated dma,
     * set byte endian mode according to bits per word.
     */
    bpw = GPSB_GetBpw(uiCh);

    if(bpw != 32UL)
    {
        addr = gpsb[uiCh].dBase + GPSB_DMACTR;
        reg = SAL_ReadReg(addr);
        reg |= BSP_BIT_28;
        SAL_WriteReg(reg, addr);
    }
    else
    {
        /* When bits per word is equal to 32, disable half-word and byte */
        addr = gpsb[uiCh].dBase + GPSB_INTEN;
        reg = SAL_ReadReg(addr);
        reg &= ~(BSP_BIT_24 | BSP_BIT_25 | BSP_BIT_26 | BSP_BIT_27);
        SAL_WriteReg(reg, addr);
        //GPSB_BitClear(gpsb[uiCh].dBase + GPSB_INTEN, (BSP_BIT_24 | BSP_BIT_25 | BSP_BIT_26 | BSP_BIT_27));

        addr = gpsb[uiCh].dBase + GPSB_DMACTR;
        reg = SAL_ReadReg(addr);
        reg &= ~(BSP_BIT_28);
        SAL_WriteReg(reg, addr);
        //GPSB_BitClear(gpsb[uiCh].dBase + GPSB_DMACTR, BSP_BIT_28);
    }

    /* Clear packet counter (CH 0-2) */
    //GPSB_BitSet(gpsb[uiCh].dBase + GPSB_DMACTR, BSP_BIT_02);

    //GPSB_BitClear(gpsb[uiCh].dBase + GPSB_DMACTR, BSP_BIT_02);

    {
        //copy_length = gpsb[uiCh].dTxDma.dbSize;
        copy_length = 8;
    }

    /* Set DMA TXBASE and RXBASE */
    (void)GPSB_DmaSetAddr(uiCh, gpsb[uiCh].dTxDma.dbAddr, gpsb[uiCh].dRxDma.dbAddr);

    gpsb[uiCh].dAsyncTxBuf      = pTx;
    gpsb[uiCh].dAsyncRxBuf      = pRx;
    gpsb[uiCh].dAsyncTxDataSize = uiDataSize;

    gpsb[uiCh].dAsyncRxDataSize = 0;

    /* Set Packet size and count */
    SAL_WriteReg((uint32)((copy_length & 0x1FFFUL) << 0UL), (uint32)(gpsb[uiCh].dBase + GPSB_PACKET));
    GPSB_Enable(uiCh);
    GPSB_DmaStart(uiCh);
}

/*
***************************************************************************************************
*                                          GPSB_GetSlaveRXData
* Function to get rx data in slave mode.
*
* @param    uiCh [in]       : Value of channel to control
* @return   rx data
* Notes
*
***************************************************************************************************
*/

uint32 GPSB_GetSlaveRXData
(
    uint32                              uiCh
)
{
    uint32 reg;

    reg = SAL_ReadReg(MCU_BSP_GPSB_BASE+(0x10000UL * uiCh));

    return reg;
}

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
)
{
    uint32 reg;

    reg = SAL_ReadReg(MCU_BSP_GPSB_BASE+(0x10000UL * uiCh) + GPSB_STAT);

    return reg;
}

//hsj #endif  // ( MCU_BSP_SUPPORT_DRIVER_GPSB == 1 )

