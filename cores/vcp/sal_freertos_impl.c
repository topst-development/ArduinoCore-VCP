// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : sal_freertos_impl.c
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

/*
***************************************************************************************************
*                                             [INCLUDE FILES]
***************************************************************************************************
*/
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <event_groups.h>
#include <stdio.h> //QAC-<stdio.h> shall not be used in production code, but freertos recommends using <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <sal_freertos_impl.h>
#include <version.h>

/*
***************************************************************************************************
*                                             [DEFINITIONS]
***************************************************************************************************
*/
/*
 * Debugger
 */
#if (DEBUG_ENABLE)
// Deviation Record - MISRA C-2012 Rule 20.10, Preprocessing Directives
#   include <debug.h>
#   define SAL_D(fmt, args...)          {LOGD(DBG_TAG_SAL, fmt, ## args)}
#   define SAL_E(fmt, args...)          {LOGE(DBG_TAG_SAL, fmt, ## args)}
#else
#   define SAL_D(fmt, args...)
#   define SAL_E(fmt, args...)
#endif

typedef int                             FRInt32;/* For stdio api                        */

/* Task */
typedef struct SALTaskiInfoTable
{
    bool                             tiUsed; /* if already used or not               */
    StaticTask_t                        tiCb;   /* pointer to Task Control Block        */

} SALTask_t;

/* queue */
typedef struct SALQueueInfoTable
{
    bool                             qiUsed; /* if already used or not               */
    StaticQueue_t                       qiCb;   /* pointer to Queue control block       */

} SALQueue_t;

/* semaphore */
typedef struct SALSemaphoreInfoTable
{
    bool                             siUsed; /* if already used or not               */
    StaticSemaphore_t                   siCb;   /* pointer to Semaphore control block   */

} SALSemaphore_t;

/* event */
typedef struct SALEventInfoTable
{
    bool                             eiUsed; /* if already used or not               */
    StaticEventGroup_t                  eiCb;   /* pointer to Event control block       */

} SALEvent_t;

/*
***************************************************************************************************
*                                             [LOCAL VARIABLES]
***************************************************************************************************
*/
static uint32_t                           gCPU_SR = 0;

static StaticSemaphore_t                gSemObj;
static SALErrorInfo_t                   gErrorInfo;

/* OS object */
static SALTask_t                        gTaskObj[SAL_MAX_TASK];
static SALQueue_t                       gQueueObj[SAL_MAX_QUEUE];
static SALSemaphore_t                   gSemaObj[SAL_MAX_SEMA];
static SALEvent_t                       gEventObj[SAL_MAX_EVENT];

/*
***************************************************************************************************
*                                        [FUNCTION PROTOTYPES]
***************************************************************************************************
*/
static SALRetCode_t FR_CoreMB
(
    void
);

static SALRetCode_t FR_CoreCriticalEnter
(
    void
);

static SALRetCode_t FR_CoreCriticalExit
(
    void
);

static SALRetCode_t FR_CoreWaitForEvent
(
    void
);

static SALRetCode_t FR_CoreDiv64To32
(
    uint64_t *                            pullDividend,
    uint32_t                              uiDivisor,
    uint32_t *                            puiRem
);

static SALRetCode_t FR_OSStart
(
    void
);

static SALRetCode_t FR_MemSet
(
    void *                              pMem,
    uint8_t                               ucValue,
    SALSize                             uiSize
);

static SALRetCode_t FR_MemCopy
(
    void *                              pDest,
    const void *                        pSrc,
    SALSize                             uiSize
);

static SALRetCode_t FR_MemCmp
(
    const void *                        pMem1,
    const void *                        pMem2,
    SALSize                             uiSize,
    int32_t *                            piRetCmp
);

static SALRetCode_t FR_StrCopy
(
    uint8_t *                             pucDestString,
    const uint8_t *                       pucSrcString
);

static SALRetCode_t FR_StrCmp
(
    const uint8_t *                       puc1String,
    const uint8_t *                       puc2String,
    int32_t *                            piRetCmp
);

static SALRetCode_t FR_StrNCmp
(
    const uint8_t *                       puc1String,
    const uint8_t *                       puc2String,
    SALSize                             uiLength,
    int32_t *                            piRetCmp
);

static SALRetCode_t FR_StrLength
(
    const int8_t *                       pucString,
    SALSize *                           puiLength
);

static SALRetCode_t FR_QueueCreate
(
    uint32_t *                            puiId,
    const uint8_t *                       pucName,
    uint32_t                              uiDepth,
    SALSize                             uiDataSize
);

static SALRetCode_t FR_QueueGet
(
    uint32_t                              uiId,
    void *                              pData,
    uint32_t *                            puiSizeCopied,
    uint32_t                             iTimeout,
    SALBlockingOption_t                 ulOptions
);

static SALRetCode_t FR_QueuePut
(
    uint32_t                              uiId,
    void *                              pData,
    SALSize                             uiSize,
    int32_t                              iTimeout,
    SALBlockingOption_t                 ulOptions
);

static void FR_SemaphoreGive(
    SemaphoreHandle_t                   psHandle
);

static BaseType_t FR_SemaphoreTake(
    SemaphoreHandle_t                   psHandle
);

static SALRetCode_t FR_SemaphoreCreate
(
    uint32_t *                            puiId,
    const uint8_t *                       pucName,
    uint32_t                              uiInitialValue,
    SALBlockingOption_t                 ulOptions
);

static SALRetCode_t FR_SemaphoreRelease
(
    uint32_t                              uiId
);

static SALRetCode_t FR_SemaphoreWait
(
    uint32_t                              uiId,
    int32_t                              iTimeout,
    SALBlockingOption_t                 ulOptions
);

static SALRetCode_t FR_EventCreate
(
    uint32_t *                            puiId,
    const uint8_t *                       pucName,
    uint32_t                              uiInitialValue
);

static SALRetCode_t FR_EventSet
(
    uint32_t                              uiId,
    uint32_t                              uiEvent,
    SALEventOption_t                    ulOptions
);

static SALRetCode_t FR_EventGet
(
    uint32_t                              uiId,
    uint32_t                              uiEvent,
    uint32_t                              iTimeout,
    uint32_t                              uiOptions,
    uint32_t *                            puiFlags
);

static SALRetCode_t FR_TaskCreate
(
    uint32_t *                            puiTaskId,
    const uint8_t *                       pucTaskName,
    SALTaskFunc                         fnTask,
    uint32_t *                            puiStackPtr,
    uint32_t                              uiStackSize,
    SALTaskPriority_t                   uiPriority,
    void *                              pTaskParam
);

static SALRetCode_t FR_TaskSleep
(
    uint32_t                              uiMilliSec
);

static SALRetCode_t FR_ReportError
(
    SALDriverId_t                       uiDriverId,
    uint32_t                              uiApiId, 
    SALErrorCode_t                      uiErrorCode,
    const int8_t *                        pucEtc
);

static SALRetCode_t FR_GetCurrentError
(
    SALErrorInfo_t *                    psInfo
);

static SALRetCode_t FR_GetTickCount
(
    uint32_t *                            puiTickCount
);

static SALRetCode_t FR_HandleTick
(
    void
);

static SALRetCode_t FR_GetMcuVersionInfo
(
    SALMcuVersionInfo_t *             psVersion
);

/*
***************************************************************************************************
*                                        [FUNCTIONS]
***************************************************************************************************
*/
/*
***************************************************************************************************
*                                          [FR_CoreMB]
*
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_CoreMB (void)
{
    CPU_DSB();

    return SAL_RET_SUCCESS;
}

/*
***************************************************************************************************
*                                          [FR_CoreCriticalEnter]
*
* Save CPU status word & Disable interrupts.
*
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_CoreCriticalEnter (void)
{
    gCPU_SR = 0;
    gCPU_SR = CPU_SR_Save();

    return SAL_RET_SUCCESS;
}

/*
***************************************************************************************************
*                                          [FR_CoreCriticalExit]
*
* Restore CPU status word & Re-enable interrupts.
*
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_CoreCriticalExit (void)
{
    CPU_SR_Restore(gCPU_SR);

    return SAL_RET_SUCCESS;
}

/*
***************************************************************************************************
*                                          [FR_CoreWaitForEvent]
*
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_CoreWaitForEvent (void)
{
    CPU_WaitForEvent();

    return SAL_RET_SUCCESS;
}

/*
***************************************************************************************************
*                                          [FR_CoreDiv64To32]
*
*
* @param    pullDividend [in]
* @param    uiDivisor [in]
* @param    puiRem [out]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_CoreDiv64To32
(
    uint64_t * pullDividend,
    uint32_t uiDivisor,
    uint32_t * puiRem
) {
    SALRetCode_t retVal     = SAL_RET_SUCCESS;
    uint64_t rem              = 0;
    uint64_t b                = uiDivisor;
    uint64_t d                = 1;
    uint64_t res              = 0;
    uint32_t high             = 0;

    if (pullDividend != NULL_PTR)
    {
        rem = *pullDividend;
        high = (uint32_t)(rem >> 32ULL);
        
        /* Reduce the thing a bit first */
        if (high >= uiDivisor)
        {
            high /= uiDivisor;
            res = ((uint64_t)high) << 32ULL;

            // CERT-C Integers (CERT INT30-C) : Ensure that unsigned integer operations do not wrap
            if ((uiDivisor > 0UL) && ((rem / (uint64_t)uiDivisor) >= (uint64_t)high))
            {
                retVal = FR_ReportError(SAL_DRVID_SAL, 
                                        SAL_API_CORE_DIV_64_32,
                                        SAL_ERR_INVALID_PARAMETER, 
                                        __FUNCTION__);
            }
            else
            {
                rem -= (((uint64_t)high * (uint64_t)uiDivisor) << 32ULL);
            }
        }

        if (retVal == SAL_RET_SUCCESS)
        {
            while (((b > 0ULL) && (b < rem)))
            {
                b = b+b;
                d = d+d;
            }

            do
            {
                if (rem >= b)
                {
                    rem -= b;

                    if ((0xFFFFFFFFFFFFFFFFULL - d) < res)
                    {
                        retVal = FR_ReportError(SAL_DRVID_SAL, 
                                                SAL_API_CORE_DIV_64_32,
                                                SAL_ERR_INVALID_PARAMETER, 
                                                __FUNCTION__);
                        break;
                    }
                    else
                    {
                        res += d;
                    }
                }

                b >>= 1UL;
                d >>= 1UL;
            }
            while (d != 0ULL);

            if (retVal == SAL_RET_SUCCESS)
            {
                *pullDividend = res;
            }
        }
     }
    else
    {
        retVal = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_CORE_DIV_64_32,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }

    if (puiRem != NULL_PTR)
    {
        *puiRem = (uint32_t)rem;
    }
    
    return retVal;
}

/*
***************************************************************************************************
*                                          [FR_OSStart]
*
*
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_OSStart (void)
{
    SAL_D("~ Done to initialize Free RT Operating System ~\n");

    vTaskStartScheduler();

    return SAL_RET_SUCCESS;
}

/*
***************************************************************************************************
*                                          [FR_MemSet]
*
*
* @param    pMem [out]
* @param    ucValue [in]
* @param    uiSize [in]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_MemSet
(
    void * pMem,
    uint8_t ucValue,
    SALSize uiSize
) {
    SALRetCode_t retVal = SAL_RET_SUCCESS;

    if (pMem != NULL_PTR)
    {
        (void)memset(pMem, (FRInt32)ucValue, (size_t)uiSize); //QAC-Not use return value
    }
    else
    {
        retVal = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_MEM_SET,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }

    return retVal;
}

/*
***************************************************************************************************
*                                          [FR_MemCopy]
*
*
* @param    pDest [out]
* @param    pSrc [in]
* @param    uiSize [in]
* @return
*
* Notes
*
***************************************************************************************************
*/
// Deviation Record - HIS metric violation (HIS_CALLING)
static SALRetCode_t FR_MemCopy
(
    void * pDest,
    const void * pSrc,
    SALSize uiSize
) {
    SALRetCode_t retVal = SAL_RET_SUCCESS;

    if ((pDest != NULL_PTR) && (pSrc != NULL_PTR))
    {
        (void)memcpy(pDest, pSrc, (size_t)uiSize); //QAC-Not use return value
    }
    else
    {
        retVal = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_MEM_COPY,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }

    return retVal;
}

/*
***************************************************************************************************
*                                          [FR_MemCmp]
*
*
* @param    pMem1 [in]
* @param    pMem2 [in]
* @param    uiSize [in]
* @param    piRetCmp [out]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_MemCmp
(
    const void * pMem1,
    const void * pMem2,
    SALSize uiSize,
    int32_t * piRetCmp
) {
    SALRetCode_t retVal = SAL_RET_SUCCESS;

    if ((pMem1 != NULL_PTR) && (pMem2 != NULL_PTR) && (uiSize > 0UL) && (piRetCmp != NULL_PTR))
    {
        *piRetCmp = (int32_t)memcmp(pMem1, pMem2, (size_t)uiSize);
    }
    else
    {
        retVal = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_MEM_CMP,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }

    return retVal;
}

/*
***************************************************************************************************
*                                          [FR_StrCopy]
*
*
* @param    pucDestString [out]
* @param    pucSrcString [in]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_StrCopy
(
    uint8_t * pucDestString,
    const uint8_t * pucSrcString
) {
    SALRetCode_t retVal = SAL_RET_SUCCESS;

    if ((pucDestString != NULL_PTR) && (pucSrcString != NULL_PTR))
    {
        uint32_t str_len = 0;
        str_len = strnlen((const int8_t *)pucSrcString, (size_t)(SAL_MAX_INT_VAL - 1UL));

        if (str_len != 0UL)
        {
            (void)strncpy((int8_t *)pucDestString, (const int8_t *)pucSrcString, (size_t)str_len); //Codesonar Security Warning
        }
    }
    else
    {
        retVal = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_STR_COPY,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }

    return retVal;
}

/*
***************************************************************************************************
*                                          [FR_StrCmp]
*
*
* @param    puc1String [in]
* @param    puc2String [in]
* @param    piRetCmp [out]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_StrCmp
(
    const uint8_t * puc1String,
    const uint8_t * puc2String,
    int32_t * piRetCmp
) {
    SALRetCode_t retVal = SAL_RET_SUCCESS;

    if ((puc1String != NULL_PTR) && (puc2String != NULL_PTR) && (piRetCmp != NULL_PTR))
    {
        *piRetCmp = strcmp((const int8_t *)puc1String, (const int8_t *)puc2String);
    }
    else
    {
        retVal = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_STR_CMP,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }

    return retVal;
}

/*
***************************************************************************************************
*                                          [FR_StrNCmp]
*
*
* @param    puc1String [in]
* @param    puc2String [in]
* @param    uiLength [in]
* @param    piRetCmp [out]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_StrNCmp
(
    const uint8_t * puc1String,
    const uint8_t * puc2String,
    SALSize uiLength,
    int32_t * piRetCmp
) {
    SALRetCode_t retVal = SAL_RET_SUCCESS;

    if ((puc1String != NULL_PTR) && (puc2String!= NULL_PTR) && (uiLength > 0UL) && (piRetCmp != NULL_PTR))
    {
        *piRetCmp = strncmp((const int8_t *)puc1String, (const int8_t *)puc2String, (size_t)uiLength);
    }
    else
    {
        retVal = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_STR_NCMP,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }

    return retVal;
}

/*
***************************************************************************************************
*                                          [FR_StrLength]
*
*
* @param    pucString [in]
* @param    puiLength [out]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_StrLength
(
    const int8_t * pucString,
    SALSize * puiLength
) {
    SALRetCode_t retVal = SAL_RET_SUCCESS;

    if ((pucString != NULL_PTR) && (puiLength != NULL_PTR))
    {
        *puiLength = strnlen(pucString, (size_t)(SAL_MAX_INT_VAL - 1UL)); //Codesonar Security Warning
    }
    else
    {
        retVal = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_STR_LENGTH,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }

    return retVal;
}

/*
***************************************************************************************************
*                                          [FR_QueueCreate]
*
*
* @param    puiId [out]
* @param    pucName [in]
* @param    uiDepth [in]
* @param    uiDataSize [in]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_QueueCreate
(
    uint32_t * puiId,
    const uint8_t * pucName,
    uint32_t uiDepth,
    SALSize uiDataSize
) {
    SALRetCode_t                retval = SAL_RET_SUCCESS;
    BaseType_t                  status = 0;
    uint32_t                      search_id = 999999;
    uint32_t                      i;
    SemaphoreHandle_t           semParam;

    if (puiId == NULL_PTR)
    {
        retval = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_DATA_QUEUE_CREATE,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }
    else if ((pucName == NULL_PTR) 
        || (uiDepth < 1UL)
        || ((SAL_MAX_INT_VAL / uiDepth) < uiDataSize))
    {
        retval = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_DATA_QUEUE_CREATE,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
        *puiId = search_id;
    }
    else
    {
        (void)FR_MemCopy(&semParam, &gSemObj, sizeof(SemaphoreHandle_t));
        
        /* obtain sema. wait forever*/
        status = FR_SemaphoreTake(semParam);

        *puiId = search_id;

        if (status == pdTRUE)
        {
            /*search free table*/
            for (i = 0UL; i < (uint32_t)SAL_MAX_QUEUE; i++)
            {
                if (gQueueObj[i].qiUsed == FALSE)
                {
                    search_id = i;
                    break;
                }
            }

            if (search_id < (uint32_t)SAL_MAX_QUEUE)// CS : Redundant Condition
            {
                uint8_t * pValidBuffer;
                void * pQueueBuffer = pvPortMalloc((size_t)(uiDepth * uiDataSize));

                if (pQueueBuffer != NULL_PTR)
                {
                    (void)FR_MemCopy(&pValidBuffer, (const void *)&pQueueBuffer, sizeof(uint8_t *));
                
                    if(xQueueCreateStatic(uiDepth,
                                          uiDataSize,
                                          pValidBuffer,
                                          (StaticQueue_t *)&gQueueObj[search_id].qiCb) != NULL_PTR)
                    {
                        gQueueObj[search_id].qiUsed = TRUE;
                        *puiId = search_id;
                    }
                    else
                    {
                        retval = FR_ReportError(SAL_DRVID_SAL, 
                                                SAL_API_DATA_QUEUE_CREATE,
                                                SAL_ERR_FAIL_CREATE, 
                                                __FUNCTION__);
                    }
                }
            }
            else
            {
                retval = FR_ReportError(SAL_DRVID_SAL, 
                                        SAL_API_DATA_QUEUE_CREATE,
                                        SAL_ERR_NO_SPACE, 
                                        __FUNCTION__);
            }

            /*release sema*/
            FR_SemaphoreGive(semParam);
        }
        else
        {
            retval = FR_ReportError(SAL_DRVID_SAL, 
                                    SAL_API_DATA_QUEUE_CREATE,
                                    SAL_ERR_FAIL_CREATE, 
                                    __FUNCTION__);
        }
    }

    return retval;
}

/*
***************************************************************************************************
*                                          [FR_QueueGet]
*
*
* @param    uiId [in]
* @param    pData [out]
* @param    puiSizeCopied [out]
* @param    iTimeout [in]
* @param    ulOptions [in]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_QueueGet
(
    uint32_t uiId,
    void * pData,
    uint32_t * puiSizeCopied,
    uint32_t iTimeout,
    SALBlockingOption_t  ulOptions
) {
    SALRetCode_t                    retval = SAL_RET_SUCCESS;
    SALReg32                        uiNestingCnt = 0;
    BaseType_t                      status = 0;
    StaticQueue_t *                 address;
    uint32_t                          timeDly; //CS : useless assignment
    QueueHandle_t                   queueHandle;

    if (puiSizeCopied != NULL_PTR)
    {
        *puiSizeCopied = 0;
    }

    if ((uiId < (uint32_t)SAL_MAX_QUEUE) 
        && (gQueueObj[uiId].qiUsed == TRUE) 
        && (pData != NULL_PTR) 
        && (puiSizeCopied != NULL_PTR))
    {
        if (((uint32_t)ulOptions & 0x00008000UL) == (uint32_t)SAL_OPT_NON_BLOCKING)  //CS : Redundant Condition
        {
            timeDly = 0UL; // no block time
        }
        else
        {
            if (iTimeout == 0UL)
            {
                timeDly = portMAX_DELAY;
            }
            else
            {
                timeDly = (uint32_t)iTimeout;
            }
        }

        uiNestingCnt = xPortGetInterruptNestingCount();

        address = &(gQueueObj[uiId].qiCb);

        (void)FR_MemCopy(&queueHandle, (const void *)&address, sizeof(QueueHandle_t));

        if (uiNestingCnt == 0UL)
        {
            status = xQueueReceive(queueHandle,
                                   (void *)pData,
                                   (TickType_t)timeDly);
        }
        else
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            status = xQueueReceiveFromISR(queueHandle,
                                          (void *)pData,
                                          &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken )//;QAC
        }

        if ((status == pdPASS))
        {
            *puiSizeCopied = (uint32_t)gQueueObj[uiId].qiCb.uxDummy4[2];// referenced by uxItemSize on uxItemSize ;
        }
        else
        {
            retval = FR_ReportError(SAL_DRVID_SAL, 
                                    SAL_API_DATA_QUEUE_GET,
                                    SAL_ERR_FAIL_GET_DATA, 
                                    __FUNCTION__);
        }
    }
    else
    {
        retval = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_DATA_QUEUE_GET,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }

    return retval;
}

/*
***************************************************************************************************
*                                          [FR_QueuePut]
*
*
* @param    uiId [in]
* @param    pData [in]
* @param    uiSize [in]
* @param    iTimeout [in]
* @param    ulOptions [in]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_QueuePut
(
    uint32_t uiId,
    void * pData,
    SALSize uiSize,
    int32_t iTimeout,
    SALBlockingOption_t  ulOptions
) {
    SALRetCode_t                    retval = SAL_RET_SUCCESS;
    SALReg32                        uiNestingCnt = 0;
    BaseType_t                      status = 0;
    StaticQueue_t *                 address;
    uint32_t                          timeDly; //CS : useless assignment
    QueueHandle_t                   queueHandle;
    
    if ((uiId < (uint32_t)SAL_MAX_QUEUE) 
        && (gQueueObj[uiId].qiUsed == TRUE) 
        && (pData != NULL_PTR) 
        && (uiSize > 0UL))
    {
        const uint8_t * queueDataPtr;
        uint32_t uxItemSize;
        uint32_t i;
        uint32_t pushCnt;

        if (((uint32_t)ulOptions & 0x00008000UL) == (uint32_t)SAL_OPT_NON_BLOCKING)  //CS : Redundant Condition
        {
            timeDly = 0UL; // no block time
        }
        else
        {
            if (iTimeout == 0)
            {
                timeDly = portMAX_DELAY;
            }
            else
            {
                timeDly = (uint32_t)iTimeout;
            }
        }

        uxItemSize = (uint32_t)gQueueObj[uiId].qiCb.uxDummy4[2];// referenced by uxItemSize on uxItemSize ;

        if (uxItemSize != 0UL) //CS : Uninitialized Variable
        {
            pushCnt = (uiSize / uxItemSize);

            if (pushCnt != 0UL)
            {
                for (i =0; i < pushCnt; i++)
                {
                    uiNestingCnt = xPortGetInterruptNestingCount();
                    address = &(gQueueObj[uiId].qiCb);

                    (void)FR_MemCopy(&queueHandle, (const void *)&address, sizeof(QueueHandle_t));
                    (void)FR_MemCopy(&queueDataPtr, &pData, sizeof(uint8_t *));

                    if (uiNestingCnt == 0UL)
                    {
                        status = xQueueSend(queueHandle,
                                            (const void * const)&queueDataPtr[(i * uxItemSize)],
                                            (TickType_t)timeDly);
                    }
                    else
                    {
                        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                        status = xQueueSendFromISR(queueHandle,
                                                   (const void * const)&queueDataPtr[(i * uxItemSize)],
                                                   &xHigherPriorityTaskWoken);

                        portYIELD_FROM_ISR( xHigherPriorityTaskWoken )//;QAC
                    }

                    if (status != pdTRUE)
                    {
                        retval = FR_ReportError(SAL_DRVID_SAL, 
                                                SAL_API_DATA_QUEUE_PUT,
                                                SAL_ERR_FAIL_SEND_DATA, 
                                                __FUNCTION__);

                        break;
                    }
                    else
                    {
                        retval = SAL_RET_SUCCESS;
                    }
                }
            }
            else
            {
                retval = FR_ReportError(SAL_DRVID_SAL, 
                                        SAL_API_DATA_QUEUE_PUT,
                                        SAL_ERR_OUTOF_SIZE, 
                                        __FUNCTION__);
            }
        }
        else
        {
            retval = FR_ReportError(SAL_DRVID_SAL, 
                                    SAL_API_DATA_QUEUE_PUT,
                                    SAL_ERR_UNINIT_ITEM, 
                                    __FUNCTION__);
        }
    }
    else
    {
        retval = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_DATA_QUEUE_PUT,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }

    return retval;
}

/*
***************************************************************************************************
*                                          [FR_SemaphoreCreate]
*
*
* @param    puiId [out]
* @param    pucName [in]
* @param    uiInitialValue [in]
* @param    ulOptions [in]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_SemaphoreCreate
(
    uint32_t * puiId,
    const uint8_t * pucName,
    uint32_t uiInitialValue,
    SALBlockingOption_t ulOptions
) {
    SALRetCode_t                    retval = SAL_RET_SUCCESS;
    SALReg32                        uiNestingCnt = 0;
    BaseType_t                      status = 0;
    StaticSemaphore_t *             address;
    uint32_t                          search_id = 99999;
    uint32_t                          i;
    SemaphoreHandle_t               semParam;
    SemaphoreHandle_t               semHandle;

    (void)ulOptions;

    if (puiId == NULL_PTR)
    {
        retval = FR_ReportError(SAL_DRVID_SAL,
                                SAL_API_SEMAPHORE_CREATE,
                                SAL_ERR_INVALID_PARAMETER,
                                __FUNCTION__);
    }
    else if ((pucName == NULL_PTR) || (uiInitialValue == 0UL))
    {
        retval = FR_ReportError(SAL_DRVID_SAL,
                                SAL_API_SEMAPHORE_CREATE,
                                SAL_ERR_INVALID_PARAMETER,
                                __FUNCTION__);
        *puiId = search_id;
    }
    else
    {
        /* obtain sema. wait forever*/
        uiNestingCnt = xPortGetInterruptNestingCount();

        (void)FR_MemCopy(&semParam, &gSemObj, sizeof(SemaphoreHandle_t));

        if (uiNestingCnt == 0UL)
        {
            status = xSemaphoreTake(semParam, (TickType_t )portMAX_DELAY);
        }
        else
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            status = xSemaphoreTakeFromISR(semParam, &xHigherPriorityTaskWoken);
        }

        *puiId = search_id;
        
        if (status == pdTRUE)
        {
            /*search free table*/
            for (i = 0UL; i < (uint32_t)SAL_MAX_SEMA; i++)
            {
                if (gSemaObj[i].siUsed == FALSE)
                {
                    search_id = i;
                    break;
                }
            }

            if ( search_id < (uint32_t)SAL_MAX_SEMA)
            {
                (void)xSemaphoreCreateBinaryStatic((StaticSemaphore_t *)&(gSemaObj[search_id].siCb));

                gSemaObj[search_id].siUsed = TRUE;
                *puiId = search_id;

                address = &(gSemaObj[search_id].siCb);

                (void)FR_MemCopy(&semHandle, (const void *)&address, sizeof(SemaphoreHandle_t));

                FR_SemaphoreGive(semHandle);
            }
            else
            {
                retval = FR_ReportError(SAL_DRVID_SAL, 
                                        SAL_API_SEMAPHORE_CREATE,
                                        SAL_ERR_NO_SPACE, 
                                        __FUNCTION__);
            }

            /*release sema*/
            FR_SemaphoreGive(semParam);
        }
        else
        {
            retval = FR_ReportError(SAL_DRVID_SAL, 
                                    SAL_API_SEMAPHORE_CREATE,
                                    SAL_ERR_FAIL_GET_KEY, 
                                    __FUNCTION__);
        }
    }

    return retval;
}

static BaseType_t FR_SemaphoreTake(SemaphoreHandle_t psHandle)
{
    BaseType_t status       = 0;
    SALReg32 uiNestingCnt   = xPortGetInterruptNestingCount();
    
    if (uiNestingCnt == 0UL)
    {
        status = xSemaphoreTake(psHandle, (TickType_t )portMAX_DELAY);
    }
    else
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        status = xSemaphoreTakeFromISR(psHandle, &xHigherPriorityTaskWoken);
    }

    return status;
}

static void FR_SemaphoreGive(SemaphoreHandle_t psHandle)
{
    SALReg32 uiNestingCnt = xPortGetInterruptNestingCount();

    if (uiNestingCnt == 0UL)
    {
        (void)xSemaphoreGive(psHandle);
    }
    else
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        (void)xSemaphoreGiveFromISR(psHandle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken ) /* QAC */
    }
}

/*
***************************************************************************************************
*                                          [FR_SemaphoreRelease]
*
*
* @param    uiId [in]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_SemaphoreRelease (uint32_t uiId)
{
    SALRetCode_t                    retval;
    SALReg32                        uiNestingCnt = 0;
    BaseType_t                      status = 0;
    StaticSemaphore_t *             address;
    SemaphoreHandle_t               semHandle;

    if ((uiId < (uint32_t)SAL_MAX_SEMA) && (gSemaObj[uiId].siUsed == TRUE))
    {
        uiNestingCnt = xPortGetInterruptNestingCount();

        address = &(gSemaObj[uiId].siCb);

        (void)FR_MemCopy(&semHandle, (const void *)&address, sizeof(SemaphoreHandle_t));

        if (uiNestingCnt == 0UL)
        {
            status = xSemaphoreGive(semHandle);
        }
        else
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            status = xSemaphoreGiveFromISR(semHandle, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken )//; QAC
        }

        if (status == pdTRUE)
        {
            retval = SAL_RET_SUCCESS;
        }
        else
        {
            retval = FR_ReportError(SAL_DRVID_SAL, 
                                    SAL_API_SEMAPHORE_RELEASE,
                                    SAL_ERR_FAIL_RELEASE, 
                                    __FUNCTION__);
        }
    }
    else
    {
        retval = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_SEMAPHORE_RELEASE,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }

    return retval;
}

/*
***************************************************************************************************
*                                          [FR_SemaphoreWait]
*
*
* @param    uiId [in]
* @param    iTimeout [in]
* @param    ulOptions [in]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_SemaphoreWait
(
    uint32_t uiId,
    int32_t iTimeout,
    SALBlockingOption_t ulOptions
) {
    SALRetCode_t                    retval;
    SALReg32                        uiNestingCnt = 0;
    BaseType_t                      status = 0;
    uint32_t                          timeDly; //CS : useless assignment
    StaticSemaphore_t *             address;
    SemaphoreHandle_t               semHandle;

    if ((uiId < (uint32_t)SAL_MAX_SEMA) && (gSemaObj[uiId].siUsed == TRUE))
    {
        if (((uint32_t)ulOptions & 0x00008000UL) == (uint32_t)SAL_OPT_NON_BLOCKING)  //CS : Redundant Condition
        {
            timeDly = 0UL; // no block time
        }
        else
        {
            if (iTimeout == 0L)
            {
                timeDly = portMAX_DELAY;
            }
            else
            {
                timeDly = (uint32_t)iTimeout;
            }
        }

        /* obtain sema. wait forever*/
        uiNestingCnt = xPortGetInterruptNestingCount();

        address = &(gSemaObj[uiId].siCb);

        (void)FR_MemCopy(&semHandle, (const void *)&address, sizeof(SemaphoreHandle_t));

        if (uiNestingCnt == 0UL)
        {
            status = xSemaphoreTake(semHandle, (TickType_t)timeDly);
        }
        else
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            status = xSemaphoreTakeFromISR(semHandle, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken )//; QAC
        }

        if (status == pdTRUE)
        {
            retval = SAL_RET_SUCCESS;
        }
        else
        {
            retval = FR_ReportError(SAL_DRVID_SAL, 
                                    SAL_API_SEMAPHORE_WAIT,
                                    SAL_ERR_FAIL_GET_DATA, 
                                    __FUNCTION__);
        }
    }
    else
    {
        retval = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_SEMAPHORE_WAIT,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }

    return retval;
}

/*
***************************************************************************************************
*                                          [FR_EventCreate]
*
*
* @param    puiId [out]
* @param    pucName [in]
* @param    uiInitialValue [in]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_EventCreate
(
    uint32_t * puiId,
    const uint8_t * pucName,
    uint32_t uiInitialValue
) {
    SALRetCode_t retval                 = SAL_RET_SUCCESS;
    SALReg32 uiNestingCnt               = 0;
    BaseType_t status                   = 0;
    uint32_t search_id                    = 99999;
    uint32_t i;
    SemaphoreHandle_t semParam;

    (void)uiInitialValue;

    if (puiId == NULL_PTR)
    {
        retval = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_EVENT_CREATE,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }
    else if (pucName == NULL_PTR)
    {
        retval = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_EVENT_CREATE,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
        *puiId = search_id;
    }
    else
    {
        /* obtain sema. wait forever*/
        uiNestingCnt = xPortGetInterruptNestingCount();

        (void)FR_MemCopy(&semParam, &gSemObj, sizeof(SemaphoreHandle_t));

        if (uiNestingCnt == 0UL)
        {
            status = xSemaphoreTake(semParam, (TickType_t )portMAX_DELAY);
        }
        else
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            status = xSemaphoreTakeFromISR(semParam, &xHigherPriorityTaskWoken);
        }

        *puiId = search_id;

        if (status == pdTRUE)
        {
            /*search free table*/
            for (i = 0; i < (uint32_t)SAL_MAX_EVENT; i++)
            {
                if (gEventObj[i].eiUsed == FALSE)
                {
                    search_id = i;
                    break;
                }
            }

            if (search_id < (uint32_t)SAL_MAX_EVENT)// CS : Redundant Condition
            {
                (void)xEventGroupCreateStatic((StaticEventGroup_t *)&(gEventObj[search_id].eiCb));

                gEventObj[search_id].eiUsed = TRUE;
                *puiId = search_id;
                
            }
            else
            {
                retval = FR_ReportError(SAL_DRVID_SAL, 
                                        SAL_API_EVENT_CREATE,
                                        SAL_ERR_NO_SPACE, 
                                        __FUNCTION__);
            }

            /*release sema*/
            FR_SemaphoreGive(semParam);
        }
        else
        {
            retval = FR_ReportError(SAL_DRVID_SAL, 
                                    SAL_API_EVENT_CREATE,
                                    SAL_ERR_FAIL_GET_KEY, 
                                    __FUNCTION__);
        }
    }

    return retval;
}

/*
***************************************************************************************************
*                                          [FR_EventSet]
*
*
* @param    uiId [in]
* @param    uiEvent [in]
* @param    ulOptions [in]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_EventSet
(
    uint32_t uiId,
    uint32_t uiEvent,
    SALEventOption_t ulOptions
) {
    SALRetCode_t                    retval = SAL_RET_SUCCESS;
    SALReg32                        uiNestingCnt = 0;
    BaseType_t                      status = 0;
    StaticEventGroup_t *            address;
    void *                          eventHandleForISR;
    EventGroupHandle_t              eventHandle;

    if (uiId < (uint32_t)SAL_MAX_EVENT)
    {
        if (gEventObj[uiId].eiUsed == TRUE)
        {
            uiNestingCnt = xPortGetInterruptNestingCount();

            address = &(gEventObj[uiId].eiCb);
            (void)FR_MemCopy(&eventHandle, (const void *)&address, sizeof(EventGroupHandle_t));
            (void)FR_MemCopy(&eventHandleForISR, (const void *)&address, 4);

            if (((uint32_t)ulOptions & 0x00000001UL) == (uint32_t)SAL_EVENT_OPT_FLAG_SET)
            {
                if(uiNestingCnt == 0UL)
                {
                    (void)xEventGroupSetBits(eventHandle, (EventBits_t)uiEvent); //QAC-Not use return value
                }
                else
                {
                    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                    status = xEventGroupSetBitsFromISR(eventHandleForISR, uiEvent, &xHigherPriorityTaskWoken);
        
                    if (status == pdTRUE)
                    {
                        portYIELD_FROM_ISR(xHigherPriorityTaskWoken)
                    }
                    else
                    {
                        retval = FR_ReportError(SAL_DRVID_SAL, 
                                                SAL_API_EVENT_SET,
                                                SAL_ERR_FAIL_SET_CONFIG, 
                                                __FUNCTION__);
                    }
                }
            }
            else if (((uint32_t)ulOptions & 0x00000001UL) == (uint32_t)SAL_EVENT_OPT_CLR_ALL)
            {
                if(uiNestingCnt == 0UL)
                {
                    (void)xEventGroupClearBits(eventHandle, (EventBits_t)uiEvent); //QAC-Not use return value
                }
                else
                {
                    status = xEventGroupClearBitsFromISR(eventHandleForISR, uiEvent);
        
                    if (status == pdFALSE)
                    {
                        retval = FR_ReportError(SAL_DRVID_SAL, 
                                                SAL_API_EVENT_SET,
                                                SAL_ERR_FAIL_SET_CONFIG, 
                                                __FUNCTION__);
                    }
                }
            }
            else
            {
                /* MISRA C:2012-14.3, All if... else if constructs shall be terminated with an else statement  */
                retval = FR_ReportError(SAL_DRVID_SAL, 
                                        SAL_API_EVENT_SET,
                                        SAL_ERR_INVALID_PARAMETER, 
                                        __FUNCTION__);
            }
        }
        else
        {
            retval = FR_ReportError(SAL_DRVID_SAL, 
                                    SAL_API_EVENT_SET,
                                    SAL_ERR_NOT_USEFUL, 
                                    __FUNCTION__);
        }
    }
    else
    {
        /* MISRA C:2012-14.3, All if... else if constructs shall be terminated with an else statement  */
        retval = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_EVENT_SET,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }

    return retval;
}

/*
***************************************************************************************************
*                                          [FR_EventGet]
*
*
* @param    uiId [in]
* @param    uiEvent [in]
* @param    iTimeout [in]
* @param    options [in]
* @param    puiFlags [out]
* @return
*
* Notes
*
***************************************************************************************************
*/

static SALRetCode_t FR_EventGet
(
    uint32_t uiId,
    uint32_t uiEvent,
    uint32_t iTimeout,
    uint32_t uiOptions,
    uint32_t * puiFlags
) {
    SALRetCode_t retval = SAL_RET_SUCCESS;

    if (puiFlags == NULL_PTR)
    {
        retval = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_EVENT_GET,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }
    else if (uiId < (uint32_t)SAL_MAX_EVENT)
    {
        *puiFlags = 0;
        
        if (gEventObj[uiId].eiUsed == TRUE)
        {
            EventBits_t                     uxBits;
            BaseType_t                      is_Consume = pdFALSE;
            BaseType_t                      is_All = pdFALSE;
            uint32_t                          timeDly; // CS : Redundant Condition
            EventGroupHandle_t              eventHandle;
            StaticEventGroup_t *            address;

            if ((uiOptions & (uint32_t)SAL_EVENT_OPT_SET_ALL) != 0UL)
            {
                is_All = pdTRUE;
            }
            else if ((uiOptions & (uint32_t)SAL_EVENT_OPT_CONSUME) != 0UL)
            {
                is_Consume = pdTRUE;
            }
            else
            {
                /* MISRA C:2012-14.3, All if... else if constructs shall be terminated with an else statement  */
            } 

            if ((uiOptions & 0x00008000UL) == (uint32_t)SAL_OPT_NON_BLOCKING)  //CS : Redundant Condition
            {
                timeDly = 0; // no block time
            }
            else
            {
                if (iTimeout == 0UL)
                {
                    timeDly = portMAX_DELAY;
                }
                else
                {
                    timeDly = iTimeout;
                }
            }

            address = &(gEventObj[uiId].eiCb);

            (void)FR_MemCopy(&eventHandle, (const void *)&address, sizeof(EventGroupHandle_t));

            uxBits = xEventGroupWaitBits(eventHandle, uiEvent, is_Consume, is_All, (TickType_t)timeDly);

            if ((uxBits & uiEvent) == 0UL)
            {   // the event bits being waited for became set, or the block time expired
                SAL_D("EventGet : Waited for became set, or the block time expired\n");
            }
            
            *puiFlags = (uxBits & uiEvent);
        }
        else
        {
            retval = FR_ReportError(SAL_DRVID_SAL, 
                                    SAL_API_EVENT_GET,
                                    SAL_ERR_NOT_USEFUL, 
                                    __FUNCTION__);
        }
    }
    else
    {
        /* MISRA C:2012-14.3, All if... else if constructs shall be terminated with an else statement  */
        retval = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_EVENT_GET,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
        *puiFlags = 0;
    }

    return retval;
}

/*
***************************************************************************************************
*                                          [FR_TaskCreate]
*
*
* @param    puiTaskId [out]
* @param    pucTaskName [in]
* @param    fnTask [in]
* @param    puiStackPtr [out]
* @param    uiStackSize [in]
* @param    uiPriority [in]
* @param    pTaskParam [in]
* @return
*
* Notes
*
***************************************************************************************************
*/
// Deviation Record - HIS metric violation (HIS_PARAM)
static SALRetCode_t FR_TaskCreate
(
    uint32_t * puiTaskId,
    const uint8_t * pucTaskName,
    SALTaskFunc fnTask,
    uint32_t * puiStackPtr,
    uint32_t uiStackSize,
    SALTaskPriority_t uiPriority,
    void * pTaskParam
) {
    SALRetCode_t retval                 = SAL_RET_SUCCESS;
    SALReg32 uiNestingCnt               = 0;
    BaseType_t status                   = 0;
    uint32_t search_id                    = 99999;
    uint32_t i;
    SemaphoreHandle_t semParam;

    if (puiTaskId == NULL_PTR)
    {
        retval = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_TASK_CREATE, 
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }
    else if ((pucTaskName == NULL_PTR) || (fnTask == NULL_PTR) ||(puiStackPtr == NULL_PTR))
    {
        retval = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_TASK_CREATE, 
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
        *puiTaskId = search_id;
    }
    else
    {
        /* obtain sema. wait forever*/
        uiNestingCnt = xPortGetInterruptNestingCount();

        (void)FR_MemCopy(&semParam, &gSemObj, sizeof(SemaphoreHandle_t));

        if (uiNestingCnt == 0UL)
        {
            status = xSemaphoreTake(semParam, (TickType_t )portMAX_DELAY);
        }
        else
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            status = xSemaphoreTakeFromISR(semParam, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken )//;QAC
        }

        *puiTaskId = search_id;

        if (status == pdTRUE)
        {
            /*search free table*/
            for (i = 0; i < (uint32_t)SAL_MAX_TASK; i++)
            {
                if (gTaskObj[i].tiUsed == FALSE)
                {
                    search_id = i;
                    break;
                }
            }

            if (search_id < (uint32_t)SAL_MAX_TASK)
            {
                if (xTaskCreateStatic((TaskFunction_t)fnTask,
                                      (const int8_t * const)pucTaskName,
                                      uiStackSize,
                                      pTaskParam,
                                      (UBaseType_t)((16UL - (uint32_t)uiPriority) | portPRIVILEGE_BIT),  /* FREERTOS_MAX_PRIORITY = 16 */
                                      puiStackPtr,  /* QAC-use const */
                                      (StaticTask_t * const)&gTaskObj[search_id].tiCb)  /* QAC-use const */ != NULL)
                {
                    gTaskObj[search_id].tiUsed = TRUE;
                    *puiTaskId = search_id;
                }
                else
                {
                    retval = FR_ReportError(SAL_DRVID_SAL, 
                                            SAL_API_TASK_CREATE,
                                            SAL_ERR_FAIL_CREATE, 
                                            __FUNCTION__);
                }
            }
            else
            {
                retval = FR_ReportError(SAL_DRVID_SAL, 
                                        SAL_API_TASK_CREATE,
                                        SAL_ERR_NO_SPACE, 
                                        __FUNCTION__);
            }

            /*release sema*/
            FR_SemaphoreGive(semParam);
        }

    }

    return retval;
}

/*
***************************************************************************************************
*                                          [FR_TaskSleep]
*
*
* @param    uiMilliSec [in]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_TaskSleep (uint32_t uiMilliSec)
{
    SALRetCode_t            retval = SAL_RET_SUCCESS;
    uint32_t                  ticks = 0;
    

    if ((SAL_MAX_INT_VAL / configTICK_RATE_HZ) < uiMilliSec)
    {
        retval = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_TASK_SLEEP,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }
    else
    {
        ticks = (uint32_t)((uiMilliSec * configTICK_RATE_HZ) / 1000UL);
        vTaskDelay(ticks);
    }
    
    return retval;
}

/*
***************************************************************************************************
*                                          [FR_ReportError]
*
*
* @param    uiDrvierId [in]
* @param    uiApiId [in]
* @param    uiErrorCode [in]
* @param    pucEtc [in]
* @return
*
* Notes
*
***************************************************************************************************
*/
// Deviation Record - HIS metric violation (HIS_CALLING)
static SALRetCode_t FR_ReportError
(
    SALDriverId_t                       uiDriverId,
    uint32_t                              uiApiId, 
    SALErrorCode_t                      uiErrorCode,
    const int8_t *                        pucEtc
) {
    SAL_E("[%d][API : %d][ERROR Code: %d][From: %s]\n", uiDriverId, uiApiId, uiErrorCode, pucEtc);

    (void)FR_CoreCriticalEnter();
    gErrorInfo.eiDrvId = uiDriverId;
    gErrorInfo.eiApiId = uiApiId;
    gErrorInfo.eiErrorId = uiErrorCode;
    (void)FR_CoreCriticalExit();

    return SAL_RET_FAILED;
}

/*
***************************************************************************************************
*                                          [FR_GetCurrentError]
*
*
* @param    psInfo [out]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_GetCurrentError(SALErrorInfo_t * psInfo) 
{
    SALRetCode_t retval = SAL_RET_SUCCESS;
    
    if (psInfo != NULL_PTR)
    {
        (void)FR_CoreCriticalEnter();
        (void)FR_MemCopy(psInfo, &gErrorInfo, sizeof(SALErrorInfo_t));

        // Right after returing the information of error, info table is initialized.
        (void)FR_MemSet(&gErrorInfo, 0, sizeof(SALErrorInfo_t));
        (void)FR_CoreCriticalExit();
    }
    else
    {
        retval = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_GET_CURRENT_ERR, 
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }

    return retval;
}


/*
***************************************************************************************************
*                                          [FR_GetTickCount]
*
* @param    puiTickCount [out]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_GetTickCount (uint32_t * puiTickCount)
{
    SALRetCode_t retval         = SAL_RET_SUCCESS;
    SALReg32 uiNestingCnt       = 0;

    if (puiTickCount != NULL_PTR)
    {
        /* obtain sema. wait forever*/
        uiNestingCnt = xPortGetInterruptNestingCount();

        if (uiNestingCnt == 0UL)
        {
            *puiTickCount = xTaskGetTickCount();
        }
        else
        {
            *puiTickCount = xTaskGetTickCountFromISR();
        }
    }
    else
    {
        retval = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_GET_TICKCOUNT,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }

    return retval;
}

/*
***************************************************************************************************
*                                          [FR_HandleTick]
*
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_HandleTick (void)
{
    FreeRTOS_Tick_Handler();

    return SAL_RET_SUCCESS;
}

/*
***************************************************************************************************
*                                          [FR_GetMcuVersionInfo]
*
* @param    psVersion [out]
* @return
*
* Notes
*
***************************************************************************************************
*/
static SALRetCode_t FR_GetMcuVersionInfo (SALMcuVersionInfo_t * psVersion)
{
    SALRetCode_t retval = SAL_RET_SUCCESS;

    static const SALMcuVersionInfo_t MCU_Version =
    {
        SDK_MAJOR_VERSION,
        SDK_MINOR_VERSION,
        SDK_PATCH_VERSION,
        0U
    };

    if (psVersion != NULL_PTR)
    {
        (void)FR_MemCopy(psVersion, &MCU_Version, sizeof(SALMcuVersionInfo_t));
    }
    else
    {
        retval = FR_ReportError(SAL_DRVID_SAL, 
                                SAL_API_GET_VERSION,
                                SAL_ERR_INVALID_PARAMETER, 
                                __FUNCTION__);
    }

    return retval;
}


/*
***************************************************************************************************
*                                          [S4AL_Init]
*
* Initializes every elements and functions
*
* @return
*
* Notes
*
***************************************************************************************************
*/
// Deviation Record - HIS metric violation (HIS_CALLS)
SALRetCode_t SAL_Init (void)
{
    uint32_t initIndex;
    BaseType_t status                   = 0;
    SALReg32 uiNestingCnt               = 0;
    SemaphoreHandle_t semParam;

    SALRetCode_t ret                    = SAL_RET_SUCCESS;

    heap_init();

    (void)SAL_RegisterFunction(SAL_API_CORE_MB, &FR_CoreMB, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_CORE_CRITICAL_ENTER, &FR_CoreCriticalEnter, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_CORE_CRITICAL_EXIT, &FR_CoreCriticalExit, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_CORE_WAITFOREVENT, &FR_CoreWaitForEvent, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_CORE_DIV_64_32, &FR_CoreDiv64To32, NULL_PTR);

    (void)SAL_RegisterFunction(SAL_API_OS_START, &FR_OSStart, NULL_PTR);

    (void)SAL_RegisterFunction(SAL_API_MEM_SET, &FR_MemSet, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_MEM_COPY, &FR_MemCopy, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_MEM_CMP, &FR_MemCmp, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_STR_COPY, &FR_StrCopy, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_STR_CMP, &FR_StrCmp, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_STR_NCMP, &FR_StrNCmp, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_STR_LENGTH, &FR_StrLength, NULL_PTR);

    (void)SAL_RegisterFunction(SAL_API_DATA_QUEUE_CREATE, &FR_QueueCreate, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_DATA_QUEUE_GET, &FR_QueueGet, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_DATA_QUEUE_PUT, &FR_QueuePut, NULL_PTR);

    (void)SAL_RegisterFunction(SAL_API_EVENT_CREATE, &FR_EventCreate, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_EVENT_SET, &FR_EventSet, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_EVENT_GET, &FR_EventGet, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_SEMAPHORE_CREATE, &FR_SemaphoreCreate, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_SEMAPHORE_RELEASE, &FR_SemaphoreRelease, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_SEMAPHORE_WAIT, &FR_SemaphoreWait, NULL_PTR);

    (void)SAL_RegisterFunction(SAL_API_TASK_CREATE, &FR_TaskCreate, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_TASK_SLEEP, &FR_TaskSleep, NULL_PTR);

    (void)SAL_RegisterFunction(SAL_API_GET_TICKCOUNT, &FR_GetTickCount, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_HANDLE_TICK, &FR_HandleTick, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_GET_VERSION, &FR_GetMcuVersionInfo, NULL_PTR);

    (void)SAL_RegisterFunction(SAL_API_DBG_REPORT_ERR, &FR_ReportError, NULL_PTR);
    (void)SAL_RegisterFunction(SAL_API_GET_CURRENT_ERR, &FR_GetCurrentError, NULL_PTR);

    /* SAL default manage object*/
    (void)xSemaphoreCreateBinaryStatic(&gSemObj); //CS : redundant condition

    /* semaphore must first be given using the xSemaphoreGive() API function before it can subsequently be taken (obtained) using the xSemaphoreTake() function. */
    uiNestingCnt = xPortGetInterruptNestingCount();

    (void)FR_MemCopy(&semParam, &gSemObj, sizeof(SemaphoreHandle_t));

    if (uiNestingCnt == 0UL)
    {
        status = xSemaphoreGive(semParam);
    }
    else
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        status = xSemaphoreGiveFromISR(semParam, &xHigherPriorityTaskWoken);
    }

    if (status == pdTRUE)
    {
        /* Init Object */
        for (initIndex = 0; initIndex < (uint32_t)SAL_MAX_TASK; initIndex++)
        {
            //SAL_MemSet(&gTaskObj[index], 0x00, sizeof(SALTask_t));
            //modify for time save
            gTaskObj[initIndex].tiUsed = FALSE;
        }

        for (initIndex = 0; initIndex < (uint32_t)SAL_MAX_QUEUE; initIndex++)
        {
            //SAL_MemSet(&gQueueObj[index], 0x00, sizeof(SALQueue_t));
            //modify for time save
            gQueueObj[initIndex].qiUsed = FALSE;
        }

        for (initIndex = 0; initIndex < (uint32_t)SAL_MAX_SEMA; initIndex++)
        {
            //SAL_MemSet(&gSemaObj[index], 0x00, sizeof(SALSemaphore_t));
            //modify for time save
            gSemaObj[initIndex].siUsed = FALSE;
        }

        for (initIndex = 0; initIndex < (uint32_t)SAL_MAX_EVENT; initIndex++)
        {
            //SAL_MemSet(&gEventObj[index], 0x00, sizeof(SALEvent_t));
            //modify for time save
            gEventObj[initIndex].eiUsed = FALSE;
        }
    }
    else
    {
        ret = FR_ReportError(SAL_DRVID_SAL, 
                             SAL_API_COMMON_INIT,
                             SAL_ERR_INIT, 
                             __FUNCTION__);
    }

    return ret;
}

