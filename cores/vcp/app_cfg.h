// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : app_cfg.h
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#ifndef APP_CFG_HEADER
#define APP_CFG_HEADER

/*
***************************************************************************************************
*                                             DEFINITIONS
***************************************************************************************************
*/
/* TASK STACK SIZES : Size of the task stacks (# of WARD entries)                   */
#define ACFG_TASK_USER_STK_SIZE         (128U)
/* normal measn that task has no deep fucnction call or large local variable/array  */
#define ACFG_TASK_NORMAL_STK_SIZE       (128U)
/* medium measn that task has some fucnction call or small local variables/arrays   */
#define ACFG_TASK_MEDIUM_STK_SIZE       (256U)

#define ARDUINO_APP_TASK_STK_SIZE 1024U

/* Sample Application                                                               */
/* Key(ADC+Rotary) manager applicaiton                                              */
#define ACFG_APP_KEY_EN                 (0)

/* LED Control via SPI Demo Applicaiton                                             */
#define ACFG_APP_SPI_LED_EN             (0)

/* Console(uart) demo applicaiton                                                   */
#define ACFG_APP_CONSOLE_EN             (0)

/* Idle State                                                                       */
#define ACFG_APP_IDLE_EN                (0)

/* TOST application                                                          */
#define ACFG_APP_TOST_EN                (1)

/* CAN demo applicaiton                                                             */
#define ACFG_APP_CAN_DEMO_EN            (0)

/* Audio demo applicaiton                                                           */
#define ACFG_APP_AUDIO_DEMO_EN          (0)

/* Ethernet Tx/Rx Test                                                              */
#define ACFG_APP_ETH_EN          		(0)

/* External AP Power Control Demo*/
#define ACFG_APP_POWER_EXT_AP_CTL_EN    (0)

/* Power Communication Demo*/
#define ACFG_APP_POWER_COMMUNICATION_EN (0)

#if (ACFG_APP_POWER_EXT_AP_CTL_EN > 0)
#undef ACFG_APP_CAN_DEMO_EN
#define ACFG_APP_CAN_DEMO_EN            (0)
#endif

/* Drvier Application                                                               */
/* Inter processor Communication service                                            */

#define ACFG_DRV_HSMMANAGER_EN          (0)

#endif  //n _APP_CFG_HEADER_

