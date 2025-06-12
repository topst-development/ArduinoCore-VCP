// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : sal_com.c
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
#include "sal_api.h"
#include <string.h>

/*
***************************************************************************************************
*                                             [DEFINITIONS]
***************************************************************************************************
*/

/*
***************************************************************************************************
*                                             [LOCAL VARIABLES]
***************************************************************************************************
*/

static SALCoreFunc_t                    gCoreTable;
static SALOSRunFunc_t                   gOSRunTable;
static SALMemFunc_t                     gMemFuncTable;
static SALFileFunc_t                    gFileTable;
static SALDataStructFunc_t              gDataStructTable;
static SALSyncObjectFunc_t              gSyncObjectTable;
static SALTaskFunc_t                    gTaskTable;
static SALTraceFunc_t                   gTraceTable;
static SALEtcFunc_t                     gEtcTable;

SALCoreFunc_t *                         gSALCoreTable = &gCoreTable;
SALOSRunFunc_t *                        gSALOSRunTable = &gOSRunTable;
SALMemFunc_t *                          gSALMemoryTable = &gMemFuncTable;
SALFileFunc_t *                         gSALFileTable = &gFileTable;
SALDataStructFunc_t *                   gSALDataStructTable = &gDataStructTable;
SALSyncObjectFunc_t *                   gSALSyncObjectTable = &gSyncObjectTable;
SALTaskFunc_t *                         gSALTaskTable = &gTaskTable;
SALTraceFunc_t *                        gSALTraceTable = &gTraceTable;
SALEtcFunc_t *                          gSALEtcTable = &gEtcTable;

/*
***************************************************************************************************
*                                         [FUNCTIONS]
***************************************************************************************************
*/
 /*
***************************************************************************************************
*                                          [SAL_RegisterFunction]
*
* Registers  functions needed by specific application.
*
* @param    uiFunctionIndex [in] Index of function to register.
* @param    pFunc [in] Pointer to function that corresponds with function index.
* @param    pPreFunc [in] Pointer to function that
* @return
*
* Notes
*
***************************************************************************************************
*/
// Deviation Record - HIS metric violation (HIS_CCM)
SALRetCode_t SAL_RegisterFunction
(
    uint32_t                              uiFunctionIndex,
    void *                              pFunc,
    const void * const *                ppPreFunc
)
{
    SALRetCode_t    ret = SAL_RET_SUCCESS;

    (void)ppPreFunc;

    switch (uiFunctionIndex)
    {
        case SAL_API_CORE_MB:
        {
            (void)memcpy(&gCoreTable.fnCoreMB, &pFunc, 4);
            break;
        }

        case SAL_API_CORE_RMB:
        {
            (void)memcpy(&gCoreTable.fnCoreRMB, &pFunc, 4);
            break;
        }

        case SAL_API_CORE_WMB:
        {
            (void)memcpy(&gCoreTable.fnCoreWMB, &pFunc, 4);
            break;
        }

        case SAL_API_CORE_CRITICAL_ENTER:
        {
            (void)memcpy(&gCoreTable.fnCoreCriticalEnter, &pFunc, 4);
            break;
        }

        case SAL_API_CORE_CRITICAL_EXIT:
        {
            (void)memcpy(&gCoreTable.fnCoreCriticalExit, &pFunc, 4);
            break;
        }

        case SAL_API_CORE_DIV_64_32:
        {
            (void)memcpy(&gCoreTable.fnCoreDiv64To32, &pFunc, 4);
            break;
        }

        case SAL_API_OS_INIT_FUNCS:
        {
            (void)memcpy(&gOSRunTable.fnOsInitFuncs, &pFunc, 4);
            break;
        }

        case SAL_API_OS_START:
        {
            (void)memcpy(&gOSRunTable.fnOsStart, &pFunc, 4);
            break;
        }

        case SAL_API_MEM_ALLOC:
        {
            (void)memcpy(&gMemFuncTable.fnMemAlloc, &pFunc, 4);
            break;
        }

        case SAL_API_MEM_CALLOC:
        {
            (void)memcpy(&gMemFuncTable.fnMemCalloc, &pFunc, 4);
            break;
        }

        case SAL_API_MEM_FREE:
        {
            (void)memcpy(&gMemFuncTable.fnMemFree, &pFunc, 4);
            break;
        }

        case SAL_API_MEM_SET:
        {
            (void)memcpy(&gMemFuncTable.fnMemSet, &pFunc, 4);
            break;
        }

        case SAL_API_MEM_COPY:
        {
            (void)memcpy(&gMemFuncTable.fnMemCopy, &pFunc, 4);
            break;
        }

        case SAL_API_MEM_CMP:
        {
            (void)memcpy(&gMemFuncTable.fnMemCmp, &pFunc, 4);
            break;
        }

        case SAL_API_STR_COPY:
        {
            (void)memcpy(&gMemFuncTable.fnStrCopy, &pFunc, 4);
            break;
        }

        case SAL_API_STR_CMP:
        {
            (void)memcpy(&gMemFuncTable.fnStrCmp, &pFunc, 4);
            break;
        }

        case SAL_API_STR_NCMP:
        {
            (void)memcpy(&gMemFuncTable.fnStrNCmp, &pFunc, 4);
            break;
        }

        case SAL_API_STR_LENGTH:
        {
            (void)memcpy(&gMemFuncTable.fnStrLength, &pFunc, 4);
            break;
        }

        case SAL_API_DATA_QUEUE_CREATE:
        {
            (void)memcpy(&gDataStructTable.fnQueueCreate, &pFunc, 4);
            break;
        }

        case SAL_API_DATA_QUEUE_DELETE:
        {
            (void)memcpy(&gDataStructTable.fnQueueDelete, &pFunc, 4);
            break;
        }

        case SAL_API_DATA_QUEUE_GET:
        {
            (void)memcpy(&gDataStructTable.fnQueueGet, &pFunc, 4);
            break;
        }

        case SAL_API_DATA_QUEUE_PUT:
        {
            (void)memcpy(&gDataStructTable.fnQueuePut, &pFunc, 4);
            break;
        }

        case SAL_API_EVENT_CREATE:
        {
            (void)memcpy(&gSyncObjectTable.fnEventCreate, &pFunc, 4);
            break;
        }

        case SAL_API_EVENT_DELETE:
        {
            (void)memcpy(&gSyncObjectTable.fnEventDelete, &pFunc, 4);
            break;
        }

        case SAL_API_EVENT_SET:
        {
            (void)memcpy(&gSyncObjectTable.fnEventSet, &pFunc, 4);
            break;
        }

        case SAL_API_EVENT_GET:
        {
            (void)memcpy(&gSyncObjectTable.fnEventGet, &pFunc, 4);
            break;
        }

        case SAL_API_MUTEX_CREATE:
        {
            (void)memcpy(&gSyncObjectTable.fnMutexCreate, &pFunc, 4);
            break;
        }

        case SAL_API_MUTEX_DELETE:
        {
            (void)memcpy(&gSyncObjectTable.fnMutexDelete, &pFunc, 4);
            break;
        }

        case SAL_API_MUTEX_LOCK:
        {
            (void)memcpy(&gSyncObjectTable.fnMutexLock, &pFunc, 4);
            break;
        }

        case SAL_API_MUTEX_UNLOCK:
        {
            (void)memcpy(&gSyncObjectTable.fnMutexUnlock, &pFunc, 4);
            break;
        }

        case SAL_API_SEMAPHORE_CREATE:
        {
            (void)memcpy(&gSyncObjectTable.fnSemaphoreCreate, &pFunc, 4);
            break;
        }

        case SAL_API_SEMAPHORE_DELETE:
        {
            (void)memcpy(&gSyncObjectTable.fnSemaphoreDelete, &pFunc, 4);
            break;
        }

        case SAL_API_SEMAPHORE_RELEASE:
        {
            (void)memcpy(&gSyncObjectTable.fnSemaphoreRelease, &pFunc, 4);
            break;
        }

        case SAL_API_SEMAPHORE_WAIT:
        {
            (void)memcpy(&gSyncObjectTable.fnSemaphoreWait, &pFunc, 4);
            break;
        }

        case SAL_API_TASK_CREATE:
        {
            (void)memcpy(&gTaskTable.fnTaskCreate, &pFunc, 4);
            break;
        }

        case SAL_API_TASK_CURRENT:
        {
            (void)memcpy(&gTaskTable.fnTaskCurrent, &pFunc, 4);
            break;
        }

        case SAL_API_TASK_DELETE:
        {
            (void)memcpy(&gTaskTable.fnTaskDelete, &pFunc, 4);
            break;
        }

        case SAL_API_TASK_SLEEP:
        {
            (void)memcpy(&gTaskTable.fnTaskSleep, &pFunc, 4);
            break;
        }

        case SAL_API_DBG_REPORT_ERR:
        {
            (void)memcpy(&gTraceTable.fnReportError, &pFunc, 4);
            break;
        }

        case SAL_API_GET_CURRENT_ERR:
        {
            (void)memcpy(&gTraceTable.fnGetCurrentError, &pFunc, 4);
            break;
        }

        case SAL_API_GET_TICKCOUNT:
        {
            (void)memcpy(&gEtcTable.fnGetTickCount, &pFunc, 4);
            break;
        }

        case SAL_API_HANDLE_TICK:
        {
            (void)memcpy(&gEtcTable.fnHandleTick, &pFunc, 4);
            break;
        }

        case SAL_API_GET_VERSION:
        {
            (void)memcpy(&gEtcTable.fnGetVersion, &pFunc, 4);
            break;
        }

        case SAL_API_GET_SYSTEM_TICK_CNT:
        {
            (void)memcpy(&gEtcTable.fnGetSystemTickCount, &pFunc, 4);
            break;
        }

        case SAL_API_INCREASE_SYSTEM_TICK:
        {
            (void)memcpy(&gEtcTable.fnIncreaseSystemTick, &pFunc, 4);
            break;
        }

        default :
        {
            ret = SAL_RET_FAILED;
            break;
        }
    }

    /* Useless because of the following issues
       MISRA C-2012 Rule 18.4/18.1 - Pointers and Arrays
       MISRA C-2012 Rule 11.6      - Pointer Type Conversions
       CERT ARR37-C                - CERT-C Array
       ARRAY_VS_SINGLETON          - Out-of-bounds access
    
    SALAddr *     tableAddr;

    uint32_t        funcCategory = uiFunctionIndex & 0xFF00UL;
    uint32_t        funcIndex = uiFunctionIndex & 0x00FFUL;
    
    switch (funcCategory)
    {
        case SAL_CORE_FUNCTIONS_INDEX :
        {
            (void)memcpy((&(gCoreTable.fnCoreMB) + funcIndex), &pFunc, 4);
            break;
        }

        case SAL_OS_RUN_FUNCTIONS_INDEX :
        {
            (void)memcpy((&(gOSRunTable.fnOsInitFuncs) + funcIndex), &pFunc, 4);
            break;
        }

        case SAL_MEM_FUNCTIONS_INDEX:
        {
            (void)memcpy((&(gMemFuncTable.fnMemAlloc) + funcIndex), &pFunc, 4);
            break;
        }

        case SAL_FILE_FUNCTIONS_INDEX:
        {
            (void)memcpy((&(gFileTable.fnFileOpenA) + funcIndex), &pFunc, 4);
            break;
        }

        case SAL_DATAS_FUNCTIONS_INDEX:
        {
            (void)memcpy((&(gDataStructTable.fnQueueCreate) + funcIndex), &pFunc, 4);
            break;
        }

        case SAL_SYNCOBJ_FUNCTIONS_INDEX:
        {
            (void)memcpy((&(gSyncObjectTable.fnEventCreate) + funcIndex), &pFunc, 4);
            break;
        }

        case SAL_TASK_FUNCTIONS_INDEX:
        {
            (void)memcpy((&(gTaskTable.fnTaskCreate) + funcIndex), &pFunc, 4);
            break;
        }

        case SAL_SOCKET_FUNCTIONS_INDEX:
        {
            (void)memcpy((&(gSocketTable.fnNetSocket) + funcIndex), &pFunc, 4);
            break;
        }

        case SAL_TRACE_FUNCTIONS_INDEX:
        {
            (void)memcpy((&(gTraceTable.fnReportError) + funcIndex), &pFunc, 4);
            break;
        }

        case SAL_ETC_FUNCTIONS_INDEX:
        {
            (void)memcpy((&(gEtcTable.fnGetTickCount) + funcIndex), &pFunc, 4);
            break;
        }

        default :
        {
            ret = SAL_RET_FAILED;
            break;
        }
    }*/

    return ret;
}

