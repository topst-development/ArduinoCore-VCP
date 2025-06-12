// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : adc.c
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#define MCU_BSP_SUPPORT_DRIVER_ADC 1

#if ( MCU_BSP_SUPPORT_DRIVER_ADC == 1 )

/*
***************************************************************************************************
*                                             INCLUDE FILES
***************************************************************************************************
*/

#include <bsp.h>
#include "reg_phys.h"
#include "clock.h"
#include "clock_dev.h"
#include "adc.h"
#include "gic.h"

#include "sal_com.h"

/*
***************************************************************************************************
*                                             LOCAL DEFINES
***************************************************************************************************
*/

/* Flags */
//#define ADC_DBG_ENABLED
//#define ADC_DMA_ENABLED
//#define ADC_TEST_ENABLED
#define ADC_AUTO_SCAN_ENABLED

/* Defines */
#define ADC_BASE(N)                     ((0xA0080000UL) + ((uint32)(N) * (0x10000UL)))

#define ADC_DMA_BAS_ADDR                (MCU_BSP_GDMA_BASE)
#define ADC_DMA_SREQ_REG                (ADC_DMA_BAS_ADDR + 0x24UL)
#define ADC_DMA_CONFIG_REG              (ADC_DMA_BAS_ADDR + 0x30UL)
#define ADC_DMA_SYNC_REG                (ADC_DMA_BAS_ADDR + 0x34UL)

#define ADC_DMA0_SRC_ADDR_REG           (ADC_DMA_BAS_ADDR + 0x100UL)
#define ADC_DMA0_DEST_ADDR_REG          (ADC_DMA_BAS_ADDR + 0x104UL)
#define ADC_DMA0_LLI_REG                (ADC_DMA_BAS_ADDR + 0x108UL)
#define ADC_DMA0_CONTROL_REG            (ADC_DMA_BAS_ADDR + 0x10CUL)
#define ADC_DMA0_CONFIG_REG             (ADC_DMA_BAS_ADDR + 0x110UL)

#define ADC_PERI_CLOCK                  (120000000UL)
//#define ADC_PERI_CLOCK                  (200000000UL)
#define ADC_CONV_CYCLE                  (30UL)

#define ADC_SMP_CMD_STS_REG             (0x00U)  //0x00
#define ADC_IRQ_DREQ_STS_REG            (0x04U)  //0x04
#define ADC_CTRL_CFG_REG                (0x08U)  //0x08
#define ADC_TIMEOUT_CFG_REG             (0x0CU)  //0x0C
#define ADC_ICTRL_REG                   (0x10U)  //0x10
#define ADC_TIMING_REG                  (0x40U)  //0x40
#define ADC_SDOUT0_REG                  (0x80U)  //0x80
#define ADC_SDOUT1_REG                  (0x84U)  //0x84
#define ADC_SDOUT2_REG                  (0x88U)  //0x88
#define ADC_SDOUT3_REG                  (0x8CU)  //0x8C
#define ADC_SDOUT4_REG                  (0x90U)  //0x90
#define ADC_SDOUT5_REG                  (0x94U)  //0x94
#define ADC_SDOUT6_REG                  (0x98U)  //0x98
#define ADC_SDOUT7_REG                  (0x9CU)  //0x9C
#define ADC_SDOUT8_REG                  (0xA0U)  //0xA0
#define ADC_SDOUT9_REG                  (0xA4U)  //0xA4
#define ADC_SDOUT10_REG                 (0xA8U)  //0xA8
#define ADC_SDOUT11_REG                 (0xACU)  //0xAC

#define ADC_IRQ_CLR                     (1 << 0)
#define ADC_IRQ_EN                      (1 << 8)
#define ADC_DMA_EN                      (1 << 9)

#define ADC_SDOUT_INIT                  (0xFFFFUL)
#define ADC_STDBIT                      (0x01UL)
#define ADC_CHNNEL_MASK                 (0x1FFUL)

#ifdef ADC_TEST_ENABLED
    /* Key definition*/
#define ADC_KEY_NULL                    (0x00)
#define ADC_KEY_1                       (0x01)
#define ADC_KEY_2                       (0x02)
#define ADC_KEY_3                       (0x03)
#define ADC_KEY_4                       (0x04)
#define ADC_KEY_5                       (0x05)
#define ADC_KEY_6                       (0x06)

#define ADC_KEY_ENTER                   (0x11)
#define ADC_KEY_SCAN                    (0x12)
#define ADC_KEY_SETUP                   (0x13)
#define ADC_KEY_MENU                    (0x14)
#define ADC_KEY_DISP                    (0x15)
#define ADC_KEY_TRACKDOWN               (0x16)
#define ADC_KEY_TRACKUP                 (0x17)
#define ADC_KEY_FOLDERDOWN              (0x18)
#define ADC_KEY_FOLDERUP                (0x19)
#define ADC_KEY_POWER                   (0x1a)
#define ADC_KEY_RADIO                   (0x1b)
#define ADC_KEY_MEDIA                   (0x1c)
#define ADC_KEY_PHONE                   (0x1d) //KEY_HANDSFREE
#define ADC_KEY_VOLUMEUP                (0x1e)
#define ADC_KEY_VOLUMEDOWN              (0x1f)

typedef struct ADCButton
{
    uint32                              btLower;
    uint32                              btUpper;
    uint16                              btVkcode;
    uint8                               btName[15];
} ADCButton_t;

typedef struct ADCButtonMap
{
    sint32                              bmSize;
    ADCButton_t *                       bmButton;
} ADCButtonMap_t;

/* Variables */
//uint32 ADCCompltedFlag = 0;   // removed for codesonar warning
#ifdef ADC_DMA_ENABLED
    uint32                              uiDMAData[100];
#endif

static struct ADCButton_t adc_btn_map_00[5] =
{
    /* ADC_CHANNEL_0 */
    {
        0x00,
        0x83,
        ADC_KEY_ENTER,
        "ENTER      "
    },
    {
        0x266,
        0x2EE,
        ADC_KEY_SCAN,
        "SCAN       "
    },
    {
        0x732,
        0x8CC,
        ADC_KEY_SETUP,
        "SETUP      "
    },
    {
        0x9E5,
        0xC18,
        ADC_KEY_MENU,
        "MENU       "
    },
    {
        0xD4A,
        0xFA0,
        ADC_KEY_DISP,
        "DISP       "
    },
};

static struct ADCButton_t adc_btn_map_01[4] =
{
    /* ADC_CHANNEL_1 */
    {
        0x6B,
        0x83,
        ADC_KEY_TRACKDOWN,
        "TRACK DOWN "
    },
    {
        0x732,
        0x8CC,
        ADC_KEY_TRACKUP,
        "TRACK UP   "
    },
    {
        0x9E5,
        0xC18,
        ADC_KEY_FOLDERDOWN,
        "FOLDER DOWN"
    },
    {
        0xD4A,
        0xFA0,
        ADC_KEY_FOLDERUP,
        "FOLDER UP  "
    },
};

static struct ADCButton_t adc_btn_map_02[4] =
{
    /* ADC_CHANNEL_2 */
    {
        0x00,
        0x83,
        ADC_KEY_POWER,
        "POWER      "
    },
    {
        0x732,
        0x8CC,
        ADC_KEY_RADIO,
        "RADIO      "
    },
    {
        0x9E5,
        0xC18,
        ADC_KEY_MEDIA,
        "MEDIA      "
    },
    {
        0xD4A,
        0xFA0,
        ADC_KEY_PHONE,
        "PHONE      "
    },
};

static struct ADCButton_t adc_btn_map_10[5] =
{
    /* ADC_CHANNEL_10 */
    {
        0x6B,
        0x83,
        ADC_KEY_1,
        "KEY1       "
    },
    {
        0x266,
        0x2EE,
        ADC_KEY_2,
        "KEY2       "
    },
    {
        0x52B,
        0x651,
        ADC_KEY_3,
        "KEY3       "
    },
    {
        0x732,
        0x8CC,
        ADC_KEY_4,
        "KEY4       "
    },
    {
        0x9E5,
        0xC18,
        ADC_KEY_5,
        "KEY5       "
    },
    {
        0xD4A,
        0xFA0,
        ADC_KEY_6,
        "KEY6       "
    },
};


struct ADCButtonMap_t adc_btn_map_ptr[12] =
{
    {
        SAL_ArraySize(adc_btn_map_00),
        adc_btn_map_00
    },
    {
        SAL_ArraySize(adc_btn_map_01),
        adc_btn_map_01
    },
    {
        SAL_ArraySize(adc_btn_map_02),
        adc_btn_map_02
    },
    {
        0,
        NULL
    },  // 03
    {
        0,
        NULL
    },  // 04
    {
        0,
        NULL
    },  // 05
    {
        0,
        NULL
    },  // 06
    {
        0,
        NULL
    },  // 07
    {
        0,
        NULL
    },  // 08
    {
        0,
        NULL
    },  // 09
    {
        SAL_ArraySize(adc_btn_map_10),
        adc_btn_map_10
    },
    {
        0,
        NULL
    }   // 11
};
#endif /* ADC_TEST_ENABLED */

/*
***************************************************************************************************
*                                         FUNCTION PROTOTYPES
***************************************************************************************************
*/
static void ADC_RegWrite
(
    uint8                               ucModule,
    uint32                              uiAddr,
    uint32                              uiSetValue
);

static uint32 ADC_RegRead
(
    uint8                               ucModule,
    uint32                              uiAddr
);

static void ADC_Delay
(
    sint32                              iDelay
);

static void ADC_0_Isr
(
    void *                              pArg
);

static void ADC_1_Isr
(
    void *                              pArg
);

static void ADC_SwresetSet
(
    void
);

static void ADC_SwresetClear
(
    void
);

static sint32 ADC_ClockEnable
(
    uint8                               ucModule
);

static sint32 ADC_ClockDisable
(
    void
);

static void ADC_StartMultiChannel
(
    uint32                              iMultiChannel,
    ADCMode_t                           iType
);

static uint32 ADC_CheckDoneBit
(
    uint8                               ucModule
);

static uint32 ADC_GetCVTCount
(
    uint8                               ucModule
);

#ifdef ADC_DMA_ENABLED
    static void ADC_SetDma
    (
        void
    );
#endif

static uint32 ADC_RegRead
(
    uint8                               ucModule,
    uint32                              uiAddr
)
{
    uint32 uiRet;
    uint32 uiBaseAddr;
    uint32 uiRegAddr;

    uiRet       = 0;

    uiBaseAddr = ADC_BASE(ucModule);
    uiAddr      &= 0xFFFFU;
    uiRegAddr   = uiBaseAddr + uiAddr;
    uiRet       = SAL_ReadReg(uiRegAddr);

    return uiRet;
}

static void ADC_RegWrite
(
    uint8                               ucModule,
    uint32                              uiAddr,
    uint32                              uiSetValue
)
{
    uint32 uiBaseAddr;
    uint32 uiRegAddr;

    uiBaseAddr = ADC_BASE(ucModule);
    uiAddr      &= 0xFFFFU;
    uiRegAddr   = uiBaseAddr + uiAddr;
    SAL_WriteReg(uiSetValue, uiRegAddr);

    return;
}

/*
***************************************************************************************************
*                                          ADC_Delay
*
* @param    iDelay [in]
*
* Notes
*
***************************************************************************************************
*/

static void ADC_Delay
(
    sint32                              iDelay
)
{
    sint32 i;
    sint32 modi_delay;

    /* wait 1 cycle */
    modi_delay = iDelay * 50;   // 600/12 (CPU_CLOCK/ADC_PERI_CLOCK)

    for (i = 0 ; i < modi_delay ; i++)
    {
        BSP_NOP_DELAY();
    }
}

/*
***************************************************************************************************
*                                          ADC_0_Isr
*
* @param    pArg [in]
*
* Notes
*
***************************************************************************************************
*/

static void ADC_0_Isr
(
    void *                              pArg
)
{
    uint32 IRQStatus;
    (void) pArg;

    IRQStatus = (ADC_RegRead(0U, ADC_IRQ_DREQ_STS_REG) | 0x01UL);
    ADC_RegWrite(0U, ADC_IRQ_DREQ_STS_REG, IRQStatus);
}
/*
***************************************************************************************************
*                                          ADC_1_Isr
*
* @param    pArg [in]
*
* Notes
*
***************************************************************************************************
*/

static void ADC_1_Isr
(
    void *                              pArg
)
{
    uint32 IRQStatus;
    (void)pArg;

    IRQStatus = (ADC_RegRead(1U, ADC_IRQ_DREQ_STS_REG) | 0x01UL);
    ADC_RegWrite(0U, ADC_IRQ_DREQ_STS_REG, IRQStatus);
}

/*
***************************************************************************************************
*                                          ADC_ClockEnable
*
*
*
* @return
*
* Notes
*
***************************************************************************************************
*/

static sint32 ADC_ClockEnable
(
    uint8                                   ucModule
)
{
    sint32 ret;
    int32 clksrc;

    if ((ucModule != ADC_MODULE_0) && (ucModule != ADC_MODULE_1))
    {
        ret = ADC_ERR_INVALID_MODULE;
        ADC_E("%s Invalid module\n", __func__);
    }
    else
    {
        if(ucModule == ADC_MODULE_0)
        {
            clksrc = (int32)CLOCK_PERI_ADC0;
        }
        else if (ucModule == ADC_MODULE_1)
        {
            clksrc = (int32)CLOCK_PERI_ADC1;
        }
        else
        {
            ;   /* Nothing */
        }

        ADC_RegWrite(ucModule, ADC_CTRL_CFG_REG, (uint32)0x00000002);
        ret = CLOCK_SetPeriRate((sint32)clksrc, ADC_PERI_CLOCK); //set clock

        if (ret == 0)
        {
            (void)CLOCK_EnablePeri((sint32)clksrc);
        }
        else
        {
            ADC_E("%s failed to enable adc0 clock\n", __func__);
        }
    }

    return ret;
}

/*
***************************************************************************************************
*                                          ADC_ClockDisable
*
*
*
* @return
*
* Notes
*
***************************************************************************************************
*/

static sint32 ADC_ClockDisable
(
    void
)
{
    sint32 ret;

    ret = CLOCK_DisablePeri((sint32)CLOCK_PERI_ADC0);

    return ret;
}


/*
***************************************************************************************************
*                                          ADC_CheckDoneBit
*
*
*
* Notes
*
***************************************************************************************************
*/

static uint32 ADC_CheckDoneBit
(
    uint8                               ucModule
)
{
    return (ADC_RegRead(ucModule, (ADC_SMP_CMD_STS_REG) & (uint32)(0x80000000U)) >> (uint32)31);
}



#ifdef ADC_DMA_ENABLED

/*
***************************************************************************************************
*                                          ADC_GetCVTCount
*
* @return
*
* Notes
*
***************************************************************************************************
*/

static uint32 ADC_GetCVTCount
(
    uint8                               ucModule
)
{
    return (ADC_RegRead(ucModule, (ADC_SMP_CMD_STS_REG) & (uint32)(0x1f00000)));
}


/*
***************************************************************************************************
*                                          ADC_StartMultiChannel
*
*
* @param    iMultiChannel [in]
* @param    iType [in]
*
* Notes
*
***************************************************************************************************
*/

static void ADC_StartMultiChannel
(
    uint32                              iMultiChannel,
    ADCMode_t                           iType
)
{
    if(iMultiChannel != (uint32)0)
    {
        SAL_WriteReg(((iMultiChannel) & (uint32)0x1FF), ADC_REG + ADC_SMP_CMD_STS_REG);

        if(iType == ADC_MODE_IRQ)
        {
            //HwMC_ADC->uADC_CFG.bReg.IRQ_EN = 1;
            SAL_WriteReg((SAL_ReadReg(ADC_REG + ADC_CTRL_CFG_REG) | (uint32)(0x100)), (ADC_REG + ADC_CTRL_CFG_REG));//IRQ_EN
        }

#ifdef ADC_DMA_ENABLED
        if(iType == ADC_MODE_DMA)
        {
            ADC_SetDma();
            //HwMC_ADC->uADC_CFG.bReg.DREQ_EN= 1;
            SAL_WriteReg((SAL_ReadReg(ADC_REG + ADC_CTRL_CFG_REG) | (uint32)(0x200)), (ADC_REG + ADC_CTRL_CFG_REG));//DREQ_EN
        }
#endif
    }
}


/*
***************************************************************************************************
*                                          ADC_SetDma
*
* @return
*
* Notes
*
***************************************************************************************************
*/

void ADC_SetDma
(
    void
)
{
    SAL_WriteReg((SAL_ReadReg(DMA_CONFIG_REG) |= (1 << 0)), DMA_CONFIG_REG);  //DMA Enable
    //  DMA_SREQ_REG |= (1 << 12);
    //  DMA_SYNC_REG |= (1 << 12);
    SAL_WriteReg(0xFFFFFFFF, DMA_HIDDEN_REG);

    //DMA 0 channel
    SAL_WriteReg(0x1B700080, DMA0_SRC_ADDR_REG);    //ADC OUTPUT(0 Channl)
    SAL_WriteReg((uint32)&uiDMAData[0], DMA0_DEST_ADDR_REG);
    SAL_WriteReg((uint32)&uiDMAData[0], DMA0_LLI_REG );
    SAL_WriteReg(((2 << 21) | (2 << 18) | (0 << 15) | (0 << 12) | (100 << 0)), DMA0_CONTROL_REG);           //
    SAL_WriteReg((SAL_ReadReg(DMA0_CONFIG_REG) |= (2 << 11) | (12 << 1) | (1 << 0)), DMA0_CONFIG_REG);      //0 channel of DMA Enable
}
#endif

/*
***************************************************************************************************
*                                          ADC_Init
*
*
* @param    Type [in]
* @param    ucModule [in]
*
* Notes
*
***************************************************************************************************
*/

void ADC_Init
(
    uint8                               Type,
    uint8                               ucModule
)
{
    sint32 ret;

    ret = ADC_ClockEnable(ucModule);

    if (ret != 0)
    {
        ADC_E("ADC_Init failed\n");
    }
    else
    {
        ADC_D("ADC_Init\n");
    }

    if (Type == (uint8)ADC_MODE_IRQ)
    {
        if(ucModule == ADC_MODULE_0)
        {
            (void)GIC_IntVectSet(GIC_ADC0, GIC_PRIORITY_NO_MEAN, GIC_INT_TYPE_LEVEL_HIGH, (GICIsrFunc)&ADC_0_Isr, (void *) 0);//BSP_NULL);
            (void)GIC_IntSrcEn(GIC_ADC0);
        }
        else if(ucModule == ADC_MODULE_1)
        {
            (void)GIC_IntVectSet(GIC_ADC1, GIC_PRIORITY_NO_MEAN, GIC_INT_TYPE_LEVEL_HIGH, (GICIsrFunc)&ADC_1_Isr, (void *) 0);//BSP_NULL);
            (void)GIC_IntSrcEn(GIC_ADC1);
        }
        else
        {
            ADC_E("Invalid Module Number\n");
        }
    }
#ifdef ADC_DMA_ENABLED
    else if (Type == (uint8)ADC_MODE_DMA)
    {
        ADC_SetDma();
    }
#endif
}


/*
***************************************************************************************************
*                                          ADC_Read
*
*
* @param    Channel [in]
* @param    ucModule [in]
* @return
*
* Notes
*
***************************************************************************************************
*/

uint32 ADC_Read
(
    ADCChannel_t                        Channel,
    uint8                               ucModule,
    int32                               siWatCh
)
{
    uint32 data;
    uint32 i;
    uint32 timeout;

    data    = ADC_SDOUT_INIT;
    timeout = ADC_CONV_CYCLE * ADC_CONV_TIMEOUT;

    if(Channel < ADC_CHANNEL_MAX)
    {
        ADC_RegWrite(ucModule, ADC_SMP_CMD_STS_REG, ADC_STDBIT << ((uint32) Channel & ADC_CHNNEL_MASK));

        if (ADC_CheckDoneBit(ucModule) == 0UL)
        {
            for (i = 0 ; i < timeout ; i++)
            {
                if (ADC_CheckDoneBit(ucModule) == 1UL)
                {
                    break;
                }

                //ADC_Delay(1);   // wait 1 cycle for ADC peri
                (void)SAL_TaskSleep(1);
            }

            if (i >= timeout)
            {
                ADC_E("(mod %d|ch %d) adc timeout\n", ucModule, Channel);
            }
        }

        data = ADC_RegRead(ucModule, + ADC_SDOUT0_REG);
        if ((int32)Channel == siWatCh)
            ADC_D("(mod %d|ch %d) cmd 0x%08X data 0x%08X\n", ucModule, Channel, ADC_RegRead(ucModule, ADC_SMP_CMD_STS_REG), data);
    }
    else
    {
        ADC_E("(mod %d|ch %d) Wrong param\n",  ucModule, Channel);
    }

    return data;
}


/*
***************************************************************************************************
*                                          ADC_AutoScan
*
*
* @param    ucModule [in]
*
* @return
*
* Notes
*
***************************************************************************************************
*/

uint32 * ADC_AutoScan
(
    uint8                               ucModule,
    int32                               siWatCh
)
{
#ifdef ADC_AUTO_SCAN_ENABLED
    uint32 ch;
    uint32 read_data;
    static uint32 adc_scan_val[ADC_CHANNEL_MAX];

    read_data   = 0;

    for (ch = 0 ; ch < (uint32)ADC_CHANNEL_MAX ; ch++)
    {
        read_data           = ADC_Read((ADCChannel_t)ch, ucModule, siWatCh);
        adc_scan_val[ch]    = read_data;

        if ((siWatCh >= (int32)ADC_CHANNEL_0) && (siWatCh < (int32)ADC_CHANNEL_MAX))
        {
            SAL_TaskSleep(100);
        }
    }

    return adc_scan_val;
#else
    ADC_E("Not supported ADC auto scan\n");
    return NULL;
#endif
}

/*
***************************************************************************************************
*                                          ADC_DeInit
*
*
* @return
*
* Notes
*
***************************************************************************************************
*/
void ADC_DeInit
(
    uint8                               ucModule
)
{
    (void)ADC_ClockDisable();
    if(ucModule ==0U)
    {
        (void)GIC_IntSrcDis(GIC_ADC0);
    }
    else
    {
        (void)GIC_IntSrcDis(GIC_ADC1);
    }

}

#endif  // ( MCU_BSP_SUPPORT_DRIVER_ADC == 1 )

