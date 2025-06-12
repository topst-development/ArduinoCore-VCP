/* ***************************************************************************************************
 * FileName: main.c
 * Description: Main source file for the application. Includes initializations and main loop.
 * 
 * Copyright Notice:
 * This source code contains confidential information of Telechips.
 * Any unauthorized use without written permission, including redistribution in source or binary form,
 * is strictly prohibited. This source code is provided "AS IS" without any warranty.
 * **************************************************************************************************/

#include "main.h"
#include "sal_api.h"
#include "app_cfg.h"
#include "debug.h"
#include "bsp.h"
#include "clock.h"
#include "clock_dev.h"
#include "gpio.h"
#include "timer.h"
#include "uart.h"
#include "pmu_reg.h"
#include "Arduino.h"

// #include "conditional_includes.h" // This will include conditionally required headers like power_app.h, key.h etc.

/* Global Variables */
uint32_t gALiveMsgOnOff;
static uint32_t gALiveCount;

/* Function Prototypes */
static void Main_StartTask(void *pArg);
static void AppTaskCreate(void);
static void DisplayAliveLog(void);
static void DisplayOTPInfo(void);

static TIMERConfig_t sTimerCfg;

#define GPIO_WriteReg(a,v)              (SAL_WriteReg(v,a))
#define GPIO_ReadReg(a)                 (SAL_ReadReg(a))

static int32_t Timer_MainHandler(TIMERChannel_t iChannel, void * args);
static void NOP_delay(uint32_t count)
{
    uint32_t i;
    for(i = 0; i < count; i++)
    {
        BSP_NOP_DELAY();
    }
}

static int32_t Timer_MainHandler(TIMERChannel_t iChannel, void * args)
{
    (void)iChannel;
    (void)args;
    static uint32_t uiCnt = 0;

    if (uiCnt++ % 2 == 0) {
        // LED Off
        //(void)GPIO_Set(GPIO_GPK(16UL), 1UL);
    }
    else
    {
        // LED On
        //(void)GPIO_Set(GPIO_GPK(16UL), 0UL);
    }

    return 0;
}

static void ArduinoAppTask(void *pArg)
{
    (void)pArg;
    #if defined(ARDUINO)
    setup(); 
    #endif
    // uint8_t ucAccSwitchStatus;
    // uint8_t ucILLISwitchStatus;
    // uint8_t ucStrSwitchStatus;
    // uint8_t ucKey0Status;

    GPIO_WriteReg(0xA0F28808, 0xFFFFFFFFUL);
    // set GPIOK_16 as output for LED
    GPIO_Config(GPIO_GPK(16UL), (GPIO_FUNC(0UL) | GPIO_OUTPUT));
    // LED Off
    GPIO_Set(GPIO_GPK(16UL), 1UL);

    // // set GPIOK_7 as input for KEY0 button
    // GPIO_Config(GPIO_GPK(7UL), (GPIO_FUNC(0UL) | GPIO_INPUTBUF_EN | GPIO_PULLUP));

    // // set GPIOK_5 as input for ACC Switch
    // GPIO_Config(GPIO_GPK(5UL), (GPIO_FUNC(0UL) | GPIO_INPUTBUF_EN | GPIO_PULLUP));
    // // set GPIOC_3 as input for ILLI Switch
    // GPIO_Config(GPIO_GPC(3UL), (GPIO_FUNC(0UL) | GPIO_INPUTBUF_EN));
    // // set GPIOK_2 as input for STR Switch
    // GPIO_Config(GPIO_GPK(2UL), (GPIO_FUNC(0UL) | GPIO_INPUTBUF_EN | GPIO_PULLUP));

    // ucAccSwitchStatus = GPIO_Get(GPIO_GPK(5UL));
    // ucILLISwitchStatus = GPIO_Get(GPIO_GPC(3UL));
    // ucStrSwitchStatus = GPIO_Get(GPIO_GPK(2UL));
    // ucKey0Status = GPIO_Get(GPIO_GPK(7UL));

    // mcu_printf("%s, ACC Switch Status : %d\n", __func__, ucAccSwitchStatus);
    // mcu_printf("%s, ILLI Switch Status : %d\n", __func__, ucILLISwitchStatus);
    // mcu_printf("%s, STR Switch Status : %d\n", __func__, ucStrSwitchStatus);
    // mcu_printf("%s, KEY0 Button Status : %d\n", __func__, ucKey0Status);

    // sTimerCfg.ctChannel = TIMER_CH_1;
    // sTimerCfg.ctStartMode = TIMER_START_ZERO;
    // sTimerCfg.ctOpMode = TIMER_OP_FREERUN;
    // sTimerCfg.ctCounterMode = TIMER_COUNTER_MAIN;
    // sTimerCfg.ctMainValueUsec = 1 * 1000 * 1000; // 1 sec
    // sTimerCfg.ctCmp0ValueUsec = 0;
    // sTimerCfg.ctCmp1ValueUsec = 0;
    // sTimerCfg.fnHandler = Timer_MainHandler;
    // sTimerCfg.pArgs = NULL;

    // (void)TIMER_Init();
    // (void)TIMER_EnableWithCfg(&sTimerCfg);
    // GIC_IntSrcEn((uint32_t)GIC_TIMER_0 + sTimerCfg.ctChannel);

    while(1)
    {   
        #if defined(ARDUINO)
        loop(); 
        #endif
        SAL_TaskSleep(0);
    }

}

/* Main Function */
void cmain(void) {
    static uint32_t AppTaskStartID = 0;
    static uint32_t AppTaskStartStk[ACFG_TASK_MEDIUM_STK_SIZE];
    static uint32_t ArduinoAppTaskID = 1UL;
    static uint32_t ArduinoAppTaskStk[ARDUINO_APP_TASK_STK_SIZE];

    SALRetCode_t err;
    SALMcuVersionInfo_t versionInfo = {0};

    NOP_delay(100000);
    SAL_Init();
    BSP_PreInit(); // Initialize basic BSP functions
    
    BSP_Init(); // Initialize advanced BSP functions
    // SAL_GetVersion(&versionInfo);

    // mcu_printf("\n===============================\n");
    // mcu_printf("    MCU BSP Version: V%d.%d.%d\n", versionInfo.viMajorVersion, versionInfo.viMinorVersion, versionInfo.viPatchVersion);
    // mcu_printf("-------------------------------\n");
    // DisplayOTPInfo();
    // mcu_printf("===============================\n\n");

    err = SAL_TaskCreate(&AppTaskStartID, "App Task Start", Main_StartTask, AppTaskStartStk, ACFG_TASK_MEDIUM_STK_SIZE, SAL_PRIO_APP_CFG, NULL);
    err |= SAL_TaskCreate(&ArduinoAppTaskID,"Arduino App Task", ArduinoAppTask, &ArduinoAppTaskStk[0], ARDUINO_APP_TASK_STK_SIZE, SAL_PRIO_ARDUINO_APP, NULL);

    if (err == SAL_RET_SUCCESS) {
        SAL_OsStart(); // Start the OS, this function should not return
    }
}

/* Main Start Task */
static void Main_StartTask(void *pArg) {
    (void)pArg; // Unused parameter
    SAL_OsInitFuncs();
    // AppTaskCreate(); // Create application tasks

    while (1) {
        DisplayAliveLog();
        SAL_TaskSleep(5000); // Sleep for 5 seconds
    }
}

/* Create Application Tasks */
static void AppTaskCreate(void) {
    #if (ACFG_APP_TOST_EN == 1)
    CreateTopstAppTask();
    #endif
}

/* Display Alive Log */
static void DisplayAliveLog(void) {
    if (gALiveMsgOnOff != 0U) {
        mcu_printf("\n %d", gALiveCount);
        gALiveCount = (gALiveCount >= MAIN_UINT_MAX_NUM) ? 0 : gALiveCount + 1;
    } else {
        gALiveCount = 0;
    }
}

#define LDT1_AREA_ADDR  0xA1011800U
#define PMU_REG_ADDR    0xA0F28000U

static void DisplayOTPInfo(void)
{
    volatile uint32_t *ldt1Addr;
    volatile uint32_t *chipNameAddr;
    volatile uint32_t *remapAddr;
    volatile uint32_t *hsmStatusAddr;
    uint32_t          chipName = 0;
    uint32_t          dualBankVal = 0;
    uint32_t          dual_bank = 0;
    uint32_t          expandFlashVal = 0;
    uint32_t          expand_flash = 0;
    uint32_t          remap_mode = 0;
    uint32_t          hsm_ready = 0;

    //----------------------------------------------------------------
    // OTP LDT1 Read
    // [11:0]Dual_Bank_Selection, [59:48]EXPAND_FLASH
    // Dual_Bank_Sel: [0xC0][11: 0] & [0xD0][11: 0] & [0xE0][11: 0] & [0xF0][11: 0]
    // EXPAND_FLASH : [0xC4][27:16] & [0xD4][27:16] & [0xE4][27:16] & [0xF4][27:16]
    // HwMC_PRG_FLS_LDT1: 0xA1011800

    ldt1Addr = (volatile uint32_t *)(LDT1_AREA_ADDR + 0x00C0);
    chipNameAddr = (volatile uint32_t *)(LDT1_AREA_ADDR + 0x0300);
    remapAddr = (volatile uint32_t *)(PMU_REG_ADDR);
    hsmStatusAddr = (volatile uint32_t *)(PMU_REG_ADDR + 0x0020);

    chipName = *chipNameAddr;
    chipName &= 0x000FFFFF;

    dualBankVal = ldt1Addr[ 0];
    expandFlashVal = ldt1Addr[ 1];

    dualBankVal &= ldt1Addr[ 4];
    expandFlashVal &= ldt1Addr[ 5];

    dualBankVal &= ldt1Addr[ 8];
    expandFlashVal &= ldt1Addr[ 9];

    dualBankVal &= ldt1Addr[12];
    expandFlashVal &= ldt1Addr[13];

    dualBankVal = (dualBankVal >> 0) & 0x0FFF;
    expandFlashVal  = (expandFlashVal >> 16) & 0x0FFF;

    dual_bank = (dualBankVal == 0x0FFF) ? 0 : 1;            // (single_bank : dual_bank)
    expand_flash  = (expandFlashVal  == 0x0000) ? 0 : 1;    // (only_eFlash : use_extSNOR)

    remap_mode = remapAddr[ 0];

    mcu_printf("    CHIP   NAME  : %x\n",    chipName);
    mcu_printf("    DUAL   BANK  : %d\n",    dual_bank);
    mcu_printf("    EXPAND FLASH : %d\n",    expand_flash);
    mcu_printf("    REMAP  MODE  : %d\n",    (remap_mode >> 16));
    
    hsm_ready = hsmStatusAddr[ 0];
    hsm_ready = (hsm_ready >> 2) & 0x0001;
#if 0
    if(hsm_ready)
    {
        mcu_printf("    HSM    READY : %d\n",    hsm_ready);
}
else
    {
        while(hsm_ready != 1)
        {
            mcu_printf("    HSM    READY : %d\n",    hsm_ready);
            mcu_printf("    wait...\n");
            hsm_ready = (hsm_ready >> 2) & 0x0001;
        }
    }
#else
    mcu_printf("    HSM    READY : %d\n",    hsm_ready);
#endif
}

