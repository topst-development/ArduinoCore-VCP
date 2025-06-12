// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : gdma.c
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#include "gdma.h"
#include "clock.h"
#include "clock_dev.h"
#include <gic.h>
#include <debug.h>
#include "gdma_dev.h"
#include <bsp.h> //BSP_NOP_DELAY

#if (DEBUG_ENABLE)
    #define GDMA_LOGD(fmt, args...)     {LOGD(DBG_TAG_GDMA, fmt, ## args)}
    #define GDMA_LOGE(fmt, args...)     {LOGE(DBG_TAG_GDMA, fmt, ## args)}
#else
    #define GDMA_LOGD(fmt, args...)
    #define GDMA_LOGE(fmt, args...)
#endif
/*
***************************************************************************************************
*                                             DEFINITIONS
***************************************************************************************************
*/

// GDMA Interrupt Flag Group
#define GDMA_EVT_FLAG_GRP_TIMEOUT       (1000UL)

#define GDMA_FLOW_TYPE_M2M              (0UL)
#define GDMA_FLOW_TYPE_M2P              (1UL)
#define GDMA_FLOW_TYPE_P2M              (2UL)
#define GDMA_FLOW_TYPE_P2P              (3UL)

/*
***************************************************************************************************
                                             LOCAL VARIABLES
***************************************************************************************************
*/
//static uint32_t uiGDMAEvtFlgGrpID;
//static uint32_t uiGDMAEnabled = 0UL;

/*
***************************************************************************************************
                                         LOCAL FUNCTION PROTOTYPES
***************************************************************************************************
*/
#define GDMA_CON0_BASE                  (MCU_BSP_GDMA_BASE)
#define GDMA_CON1_BASE                  (MCU_BSP_GDMA_BASE + 0x10000UL)
#define GDMA_CON2_BASE                  (MCU_BSP_GDMA_BASE + 0x20000UL)
#define GDMA_CON3_BASE                  (MCU_BSP_GDMA_BASE + 0x30000UL)
#define GDMA_CON4_BASE                  (MCU_BSP_GDMA_BASE + 0x40000UL)
#define GDMA_CON5_BASE                  (MCU_BSP_GDMA_BASE + 0x50000UL)

#define GDMA_CON_BASE(x)                ((uint32_t)MCU_BSP_GDMA_BASE + ((x) * 0x10000UL))

// UART DMA Register offsets
#define GDMA_INTSR                      (0x00UL)  // Interrupt Status Register
#define GDMA_ITCSR                      (0x04UL)  // Interrupt Terminal Count Status Register
#define GDMA_ITCCR                      (0x08UL)  // Interrupt Terminal Count Clear Register
#define GDMA_IESR                       (0x0CUL)  // Interrupt Error Status Register
#define GDMA_IECR                       (0x10UL)  // Interrupt Error Clear Register
#define GDMA_RITCSR                     (0x14UL)  // Raw Interrupt Terminal Count Status Register
#define GDMA_REISR                      (0x18UL)  // Raw Error Interrupt Status Register
#define GDMA_ECR                        (0x1CUL)  // Enabled Channel Register
#define GDMA_SBRR                       (0x20UL)  // Software Burst Request Register
#define GDMA_SSRR                       (0x24UL)  // Software Single Request Register
#define GDMA_SLBRR                      (0x28UL)  // Software Last Burst Request Register
#define GDMA_SLSRR                      (0x2CUL)  // Software Last Single Request Register
#define GDMA_CR                         (0x30UL)  // Configuration Register
#define GDMA_SR                         (0x34UL)  // Reserved

#define GDMA_CH_SRC_ADDR(x)             (((uint32_t)0x100UL + ((x) * 0x20UL)))
#define GDMA_CH_DST_ADDR(x)             (((uint32_t)0x104UL + ((x) * 0x20UL)))
#define GDMA_CH_LLI(x)                  (((uint32_t)0x108UL + ((x) * 0x20UL)))
#define GDMA_CH_CON(x)                  (((uint32_t)0x10CUL + ((x) * 0x20UL)))
#define GDMA_CH_CONFIG(x)               (((uint32_t)0x110UL + ((x) * 0x20UL)))

#define GDMA_CH0_FLAG                   ((uint32_t)1UL << 0UL)
#define GDMA_CH1_FLAG                   ((uint32_t)1UL << 1UL)


typedef struct GDMA_CONTROLLER
{
    uint32_t                              cController;
    GDMAInformation_t *                 cCh[GDMA_CH_MAX];
} GDMAController_t;

static void GDMA_ISR
(
    void *                              pArg
);

/*
***************************************************************************************************
*                                         FUNCTION PROTOTYPES
***************************************************************************************************
*/

void GDMA_InterruptEnable
(
    GDMAInformation_t *                 sDmacon
)
{
    uint32_t uiAddr;
    uint32_t uiValue;

    if (sDmacon != NULL_PTR)
    {
        // Enable terminal count interrupt
        uiAddr  = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CH_CON(sDmacon->iCh);
        uiValue = (uint32_t)SAL_ReadReg(uiAddr) | ((uint32_t)1UL << 31UL);

        SAL_WriteReg(uiValue,uiAddr);

        // Mask terminal count interrupt
        uiAddr  = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CH_CONFIG(sDmacon->iCh);
        uiValue = (uint32_t)SAL_ReadReg(uiAddr) | ((uint32_t)1UL << 15UL);
        SAL_WriteReg(uiValue,uiAddr);
    }
    else
    {
        (void)SAL_DbgReportError(SAL_DRVID_GDMA, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }
}

void GDMA_InterruptDisable
(
    GDMAInformation_t *                 sDmacon
)
{
    uint32_t uiAddr;
    uint32_t uiValue;

    if (sDmacon != NULL_PTR)
    {
        // Disable terminal count interrupt
        uiAddr  = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CH_CON(sDmacon->iCh);
        uiValue = (uint32_t)SAL_ReadReg(uiAddr) & ~((uint32_t)1UL << 31UL);
        SAL_WriteReg(uiValue,uiAddr);

        // Unask terminal count interrupt
        uiAddr  = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CH_CONFIG(sDmacon->iCh);
        uiValue = (uint32_t)SAL_ReadReg(uiAddr) & ~((uint32_t)1UL << 15UL);
        SAL_WriteReg(uiValue,uiAddr);
    }
    else
    {
        (void)SAL_DbgReportError(SAL_DRVID_GDMA, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }
}

void GDMA_SetAddrIncrement
(
    GDMAInformation_t *                 sDmacon,
    uint8_t                               ucDestInc,
    uint32_t                              uiSrcInc
)
{
    uint32_t inc;
    uint32_t uiAddr;
    uint32_t uiValue;

    if (sDmacon != NULL_PTR)
    {
        uiAddr  = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CH_CON(sDmacon->iCh);
        uiValue = SAL_ReadReg(uiAddr);
        //clear desc, src increment
        uiValue &= ~(0x3UL << 26UL);

        inc     = (((uint32_t)ucDestInc & 0x1UL) << 1UL) | (uiSrcInc & 0x1UL);
        uiValue |= (inc << 26UL);

        SAL_WriteReg(uiValue, uiAddr);
    }
    else
    {
        (void)SAL_DbgReportError(SAL_DRVID_GDMA, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }
}

void GDMA_SetTransferWidth
(
    GDMAInformation_t *                 sDmacon,
    uint8_t                               ucDestWidth,
    uint32_t                              uiSrcWidth
)
{
    uint32_t transfer_width;
    uint32_t uiAddr;
    uint32_t uiValue;

    if (sDmacon != NULL_PTR)
    {
        uiAddr  = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CH_CON(sDmacon->iCh);
        uiValue = SAL_ReadReg(uiAddr);
        //clear desc, src transfer width
        uiValue         &= ~(0x3FUL << 18UL);
        transfer_width  = (((uint32_t)ucDestWidth & 0x7UL) << 3UL) | (uiSrcWidth & 0x7UL);
        uiValue         |= (transfer_width << 18UL);

        SAL_WriteReg(uiValue, uiAddr);
    }
    else
    {
        (void)SAL_DbgReportError(SAL_DRVID_GDMA, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }
}

void GDMA_SetBurstSize
(
    GDMAInformation_t *                 sDmacon,
    uint8_t                               ucDestBurst,
    uint32_t                              uiSrcBurst
)
{
    uint32_t burst_size;
    uint32_t uiAddr;
    uint32_t uiValue;

    if (sDmacon != NULL_PTR)
    {
        uiAddr  = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CH_CON(sDmacon->iCh);
        uiValue = SAL_ReadReg(uiAddr);
        //clear desc, src burst size
        uiValue     &= ~(0x3FUL << 12UL);
        burst_size  = (((uint32_t)ucDestBurst & 0x7UL) << 3UL) | (uiSrcBurst & 0x7UL);
        uiValue     |= (burst_size << 12UL);

        SAL_WriteReg(uiValue, uiAddr);
    }
    else
    {
        (void)SAL_DbgReportError(SAL_DRVID_GDMA, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }
}

void GDMA_SetTransferSize
(
    GDMAInformation_t *                 sDmacon,
    uint32_t                              uiTransferSize
)
{
    uint32_t uiAddr;
    uint32_t uiValue;

    if (sDmacon != NULL_PTR)
    {
        uiAddr  = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CH_CON(sDmacon->iCh);
        uiValue = SAL_ReadReg(uiAddr);
        //clear transfer size
        uiValue &= ~(0xFFFUL);
        uiValue |= (uiTransferSize & 0xFFFUL);
        SAL_WriteReg(uiValue, uiAddr);
    }
    else
    {
        (void)SAL_DbgReportError(SAL_DRVID_GDMA, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }
}

void GDMA_SetFlowControl
(
    GDMAInformation_t *                 sDmacon,
    uint32_t                              uiFlow
)
{
    uint32_t uiAddr;
    uint32_t uiValue;

    if (sDmacon != NULL_PTR)
    {
        uiAddr  = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CH_CONFIG(sDmacon->iCh);
        uiValue = SAL_ReadReg(uiAddr);
        uiValue &= ~(0x7UL << 11UL);
        uiValue |= ((uiFlow & 0x7UL) << 11UL);
        SAL_WriteReg(uiValue, uiAddr);
    }
    else
    {
        (void)SAL_DbgReportError(SAL_DRVID_GDMA, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }
}

/*
***************************************************************************************************
*                                          GDMA_Reset
*
* Function to reset UDMA Channel.
*
* @param    ucCon [in]       : Value of control to reset
* @return   SAL_RET_SUCCESS or SAL_ERR
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GDMA_Reset
(
    uint8_t                               ucCon
)
{
    int32_t  ret = (int32_t)SAL_RET_SUCCESS;
    int32_t  iClkBusId;

    if ( ucCon < GDMA_CON_MAX)
    {
        iClkBusId   = (int32_t)CLOCK_IOBUS_DMA_CON0 + (int32_t)ucCon;

        /* SW reset */
        (void)CLOCK_SetSwReset(iClkBusId, TRUE);

    	    /* Bit Clear */
    	 (void)CLOCK_SetSwReset(iClkBusId, FALSE);

    }
    else
    {
            ret = (int32_t)SAL_RET_FAILED;
    }

    return (SALRetCode_t)ret;
}

void GDMA_SetPeri
(
    GDMAInformation_t *                 sDmacon,
    uint8_t                               ucDestPeri,
    uint8_t                               ucSrcPeri
)
{
    uint32_t uiAddr;
    uint32_t uiValue;

    if ( sDmacon != NULL_PTR )
    {
        uiAddr  = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CH_CONFIG(sDmacon->iCh);
        uiValue = SAL_ReadReg(uiAddr);
        uiValue &= ~(0x7FEUL);
        uiValue |= ((uint32_t)ucDestPeri & 0xFUL) << 6UL;
        uiValue |= ((uint32_t)ucSrcPeri  & 0xFUL) << 1UL;
        SAL_WriteReg(uiValue, uiAddr);
    }
}

uint32_t GDMA_GetTransferSize
(
    GDMAInformation_t *                 sDmacon
)
{
    uint32_t size = 0U;
    uint32_t uiAddr;
    uint32_t uiValue;
    if ( sDmacon != NULL_PTR )
    {
        uiAddr  = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CH_CON(sDmacon->iCh);
        uiValue = SAL_ReadReg(uiAddr);
        size    = (uiValue & (0xFFFUL));
    }
    return size;
}

static void GDMA_ISR
(
    void *                              pArg
)
{
    uint32_t              uiIntStatus;
    uint32_t              uiTCIntStatus;
    uint32_t              uiErrIntStatus;
    uint32_t              uiAddr;
    GDMAController_t *  pConTable;
    GDMAInformation_t * dmacon;
    uint32_t              uiIndex;

    pConTable   = (GDMAController_t *)pArg;
    dmacon      = NULL;

    if (pArg != NULL_PTR)
    {
        uiAddr = GDMA_CON_BASE(pConTable->cController) + GDMA_INTSR;
        uiIntStatus = SAL_ReadReg(uiAddr);
        uiAddr = GDMA_CON_BASE(pConTable->cController) + GDMA_ITCSR;
        uiTCIntStatus = SAL_ReadReg(uiAddr);
        uiAddr = GDMA_CON_BASE(pConTable->cController) + GDMA_IESR;
        uiErrIntStatus = SAL_ReadReg(uiAddr);
        for(uiIndex = 0; uiIndex < GDMA_CH_MAX; uiIndex++)
        {
            // check interrupt status
            if ((uiIntStatus & ((uint32_t)1UL << uiIndex)) != 0UL)
            {
                // check terminal count interrupt status.
                if ((uiTCIntStatus & ((uint32_t)1UL << uiIndex)) != 0UL)
                {
                    dmacon = (GDMAInformation_t *)pConTable->cCh[uiIndex];
                    if (dmacon != NULL_PTR) //Codesonar : Null Test After Dereference
                    {
                        // Clear terminal count interrupt
                        uiAddr = GDMA_CON_BASE(dmacon->iCon) + GDMA_ITCCR;
                        SAL_WriteReg(((uint32_t)1UL << uiIndex), uiAddr);
                        if(dmacon->fpIsrCallbackForComplete != NULL_PTR)
                        {
                            dmacon->fpIsrCallbackForComplete((void *)dmacon);
                        }
                    }
                }
                // check error interrupt status.
                if ((uiErrIntStatus & ((uint32_t)1UL << uiIndex)) != 0UL)
                {
                    dmacon = (GDMAInformation_t *)pConTable->cCh[uiIndex];
                    if (dmacon != NULL_PTR) //Codesonar : Null Test After Dereference
                    {
                        // Clear error interrupt
                        uiAddr = GDMA_CON_BASE(dmacon->iCon) + GDMA_IECR;
                        SAL_WriteReg(((uint32_t)1UL << uiIndex), uiAddr);
                    }
                }
            }
        }
    }
    else
    {
        (void)SAL_DbgReportError(SAL_DRVID_GDMA, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }
}

void GDMA_ChannelEnable
(
    GDMAInformation_t *                 sDmacon
)
{
    uint32_t uiAddr;
    uint32_t uiValue;

    if (sDmacon != NULL_PTR)
    {
        uiAddr  = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CH_CONFIG(sDmacon->iCh);
        uiValue = SAL_ReadReg(uiAddr);
        uiValue &= ~(1UL << 18UL);  // enable DMA requests
        uiValue |= (1UL << 0UL);    // channel enabled

        SAL_WriteReg(uiValue, uiAddr);
    }
    else
    {
        (void)SAL_DbgReportError(SAL_DRVID_GDMA, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }
}

void GDMA_WaitInterrupt
(
    GDMAInformation_t *                 sDmacon
)
{
    uint32_t uiMaxWaitLoopCnt = 0UL;
    uint32_t uiAddr;
    uint32_t uiValue;

    if (sDmacon != NULL_PTR)
    {
		uiAddr = GDMA_CON_BASE(sDmacon->iCon) + GDMA_ECR; // Enabled Channel Register
        // break loop if the FIFO is not empty after 2ms. time out.
        // 6250 loops for 1ms can be changed depending on Cortex-R5 Single Core clock speed.
        // 6250 loops for 1ms is based on 600MHz which is max clock speed for Cortex-R5
        // clock speed lower than 600MHz is okay because 1ms is guaranteed for 6250 loop to have enough time that isr is complete after FIFO is empty.
        // if it is higher than 600MHz, isr can be complete too fast before FIFO is empty.
        do
        {
        	uiValue = SAL_ReadReg(uiAddr);
            if((uiValue & ((uint32_t)1UL << sDmacon->iCh)) == 0UL) // Currently the channel is disabled(Transfer Completed)
            {
                break;
            }
            BSP_NOP_DELAY();
            uiMaxWaitLoopCnt += 1UL;
        } while(uiMaxWaitLoopCnt <= 12500UL); //Max Wait 2ms
    }
    else
    {
        (void)SAL_DbgReportError(SAL_DRVID_GDMA, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }
}

void GDMA_ChannelDisable
(
    GDMAInformation_t *                 sDmacon
)
{
    uint32_t uiAddr;
    uint32_t uiValue;
	uint32_t uiMaxCount  = 0U;

    if (sDmacon != NULL_PTR)
    {
		// checking channel enabled status
		uiAddr = GDMA_CON_BASE(sDmacon->iCon) + GDMA_ECR; // Enabled Channel Register
		uiValue = SAL_ReadReg(uiAddr);
		if((uiValue & ((uint32_t)1UL << sDmacon->iCh)) != 0UL) // Currently the channel is enabled
		{
			uiAddr  = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CH_CONFIG(sDmacon->iCh);
	        uiValue = SAL_ReadReg(uiAddr);
	        uiValue |= (1UL << 18UL);   // ignore subsequent source DMA requests
	        uiValue &= ~(1UL << 0UL);   // channel disabled
	        SAL_WriteReg(uiValue, uiAddr);

			// break loop if the FIFO is not empty after 2ms. time out.
			// 6250 loops for 1ms can be changed depending on Cortex-R5 Single Core clock speed.
			// 6250 loops for 1ms is based on 600MHz which is max clock speed for Cortex-R5
			// clock speed lower than 600MHz is okay because 1ms is guaranteed for 6250 loop to have enough time that isr is complete after FIFO is empty.
			// if it is higher than 600MHz, isr can be complete too fast before FIFO is empty.

	        while (uiMaxCount <= 12500UL) // 12500 : 2ms, 6250 : 1ms
            {
                // check the Active flag
                // 0 = there is no data in the FIFO of the channel
                uiAddr = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CH_CONFIG(sDmacon->iCh);

                if ((SAL_ReadReg(uiAddr) & (0x20000UL)) == 0UL)
                {
                    break;
                }
                uiMaxCount+=1UL;
            }
		}
    }
    else
    {
        (void)SAL_DbgReportError(SAL_DRVID_GDMA, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }
}

void GDMA_SetSrcAddr
(
    GDMAInformation_t *                 sDmacon,
    uint32_t                              uiAddr
)
{
    uint32_t uiRegAddr;

    if (sDmacon != NULL_PTR)
    {
        uiRegAddr = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CH_SRC_ADDR(sDmacon->iCh);
        SAL_WriteReg(uiAddr, uiRegAddr);
    }
    else
    {
        (void)SAL_DbgReportError(SAL_DRVID_GDMA, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }
}

void GDMA_SetDestAddr
(
    GDMAInformation_t *                 sDmacon,
    uint32_t                              uiAddr
)
{
    uint32_t uiRegAddr;

    if (sDmacon != NULL_PTR)
    {
        uiRegAddr = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CH_DST_ADDR(sDmacon->iCh);
        SAL_WriteReg(uiAddr, uiRegAddr);
    }
    else
    {
        (void)SAL_DbgReportError(SAL_DRVID_GDMA, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }
}

void GDMA_SetlliAddr
(
    GDMAInformation_t *                 sDmacon,
    uint32_t                              uiLLIAddr
)
{
    uint32_t uiAddr;

    if (sDmacon != NULL_PTR)
    {
        uiAddr = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CH_LLI(sDmacon->iCh);
        SAL_WriteReg(uiLLIAddr, uiAddr);
    }
    else
    {
        (void)SAL_DbgReportError(SAL_DRVID_GDMA, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }
}

void GDMA_TransferMem2Mem
(
    GDMAInformation_t *                 sDmacon,
    uint32_t                              uiSrcAddr,
    uint32_t                              uiDestAddr,
    uint32_t                              uiLen
)
{
    uint32_t word_len;
    uint32_t word_len_rest;
    uint32_t i;
    uint32_t part_num;
    uint32_t part_rest;
    uint32_t src_addr_rest;
    uint32_t dest_addr_rest;
    uint32_t uiAddr;

    if (sDmacon != NULL_PTR)
    {
		uiAddr = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CR;
    	if ((SAL_ReadReg(uiAddr) & 0x1UL) == 1UL) //if (uiGDMAEnabled == 1UL)
   		{
            // disable channel befer using
            GDMA_ChannelDisable(sDmacon);
            word_len        = (uiLen / 4UL);
            word_len_rest   = uiLen % 4UL;
            part_num        = word_len / DMA_MAX_XFER_SIZE;
            part_rest       = word_len % DMA_MAX_XFER_SIZE;
            src_addr_rest   = uiSrcAddr;
            dest_addr_rest  = uiDestAddr;

            GDMA_SetFlowControl(sDmacon, GDMA_FLOW_TYPE_M2M);
            GDMA_SetAddrIncrement(sDmacon, GDMA_INC, GDMA_INC);
            GDMA_SetBurstSize(sDmacon, GDMA_BURST_SIZE_256, GDMA_BURST_SIZE_256);

            /* max transfer */
            for (i = 0UL ; i < part_num ; i++)
            {
                GDMA_SetSrcAddr(sDmacon, src_addr_rest);
                GDMA_SetDestAddr(sDmacon, dest_addr_rest);
                GDMA_InterruptEnable(sDmacon);
                GDMA_SetTransferWidth(sDmacon, GDMA_TRANSFER_SIZE_WORD, GDMA_TRANSFER_SIZE_WORD);
                GDMA_SetTransferSize(sDmacon, DMA_MAX_XFER_SIZE);
                GDMA_ChannelEnable(sDmacon);
                GDMA_WaitInterrupt(sDmacon);
                src_addr_rest   += (DMA_MAX_XFER_SIZE * 4UL);
                dest_addr_rest  += (DMA_MAX_XFER_SIZE * 4UL);
            }

            /* word transfer */
            if (part_rest != 0UL)
            {
                GDMA_SetSrcAddr(sDmacon, src_addr_rest);
                GDMA_SetDestAddr(sDmacon, dest_addr_rest);
                GDMA_InterruptEnable(sDmacon);
                GDMA_SetTransferWidth(sDmacon, GDMA_TRANSFER_SIZE_WORD, GDMA_TRANSFER_SIZE_WORD);
                GDMA_SetTransferSize(sDmacon, part_rest);
                GDMA_ChannelEnable(sDmacon);
                GDMA_WaitInterrupt(sDmacon);
                src_addr_rest   += (part_rest * 4UL);
                dest_addr_rest  += (part_rest * 4UL);
            }

            /* byte transfer */
            if (word_len_rest != 0UL)
            {
                GDMA_SetSrcAddr(sDmacon, src_addr_rest);
                GDMA_SetDestAddr(sDmacon, dest_addr_rest);
                GDMA_InterruptEnable(sDmacon);
                GDMA_SetTransferWidth(sDmacon, GDMA_TRANSFER_SIZE_BYTE, GDMA_TRANSFER_SIZE_BYTE);
                GDMA_SetTransferSize(sDmacon, word_len_rest);
                GDMA_ChannelEnable(sDmacon);
                GDMA_WaitInterrupt(sDmacon);
            }
    	}
    }
    else
    {
        (void)SAL_DbgReportError(SAL_DRVID_GDMA, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }
}

int32_t GDMA_Init
(
    GDMAInformation_t *                 sDmacon,
    uint32_t                              uiIntPrio
)
{
    int32_t ret;
    uint32_t uiAddr;
    uint32_t uiValue;
    int32_t iClkId;
    static GDMAController_t gdma_con_table[GDMA_CON_MAX] =
    {
        {0UL, {NULL, NULL}},
        {1UL, {NULL, NULL}},
        {2UL, {NULL, NULL}},
        {3UL, {NULL, NULL}},
        {4UL, {NULL, NULL}},
        {5UL, {NULL, NULL}},
        {6UL, {NULL, NULL}},
        {7UL, {NULL, NULL}}
    };

    ret     = -1;
    iClkId  = 0;

    if (sDmacon != NULL_PTR)
    {
        iClkId = (int32_t)CLOCK_IOBUS_DMA_CON0 + (int32_t)sDmacon->iCon;
        (void)CLOCK_EnableIobus(iClkId, SALEnabled);

        gdma_con_table[sDmacon->iCon].cController       = sDmacon->iCon;
        gdma_con_table[sDmacon->iCon].cCh[sDmacon->iCh] = sDmacon;
        uiAddr = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CR;

        if ((SAL_ReadReg(uiAddr) & 0x1UL) == 0UL)
        {
            // Disable DMA Contorller
            uiValue = SAL_ReadReg(uiAddr);
            SAL_WriteReg(uiValue & ~(0x1UL), uiAddr);

            // Clear Interrupt
            uiAddr = GDMA_CON_BASE(sDmacon->iCon) + GDMA_ITCCR;
            SAL_WriteReg(0xFFUL, uiAddr);
            uiAddr = GDMA_CON_BASE(sDmacon->iCon) + GDMA_IECR;
            SAL_WriteReg(0xFFUL, uiAddr);

            (void)GIC_IntVectSet((uint32_t)GIC_DMA0 + sDmacon->iCon, uiIntPrio, GIC_INT_TYPE_LEVEL_HIGH, (GICIsrFunc)&GDMA_ISR, (void*)&gdma_con_table[sDmacon->iCon]);
            (void)GIC_IntSrcEn((uint32_t)GIC_DMA0 + sDmacon->iCon);

            // Enable DMA Controller
            uiAddr = GDMA_CON_BASE(sDmacon->iCon) + GDMA_CR;
            SAL_WriteReg(0x1UL, uiAddr);
            //ret = (int32_t)SAL_RET_SUCCESS;
            //uiGDMAEnabled = 1UL; // 2 ch per 8 controller
        }
		ret = (int32_t)SAL_RET_SUCCESS;
    }
    else
    {
        (void)SAL_DbgReportError(SAL_DRVID_GDMA, 0UL, (uint32_t)SAL_ERR_INVALID_PARAMETER, __FUNCTION__);
    }

    return ret;
}
