// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : gpio.c
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#include <gpio.h>
#include <reg_phys.h>
#include <bsp.h>
#include "debug.h"
#include <gic.h>

/*
***************************************************************************************************
*                                             LOCAL DEFINES
***************************************************************************************************
*/

#ifndef ENABLE
#   define GPIO_ENABLE                  (1)
#endif

#ifndef DISABLE
#   define GPIO_DISABLE                 (0)
#endif

#define GPIO_PMGPIO_BASE                (MCU_BSP_PMIO_BASE)

#define GPIO_REG_BASE(x)                (MCU_BSP_GPIO_BASE + ((((x) & GPIO_PORT_MASK) >> (uint32_t)GPIO_PORT_SHIFT) * 0x40UL))

#define GPIO_IS_GPIOK(x)                ((((x) & GPIO_PORT_MASK) == GPIO_PORT_K) ? 1 : 0)

#define GPIO_REG_DATA(x)                (GPIO_REG_BASE(x) + 0x00UL)
#define GPIO_REG_OUTEN(x)               (GPIO_REG_BASE(x) + 0x04UL)
#define GPIO_REG_DATA_OR(x)             (GPIO_REG_BASE(x) + 0x08UL)
#define GPIO_REG_DATA_BIC(x)            (GPIO_REG_BASE(x) + 0x0CUL)
#define GPIO_REG_PULLEN(x)              (GPIO_IS_GPIOK(x) ? (GPIO_PMGPIO_BASE + 0x10UL) : (GPIO_REG_BASE(x) + 0x1CUL))
#define GPIO_REG_PULLSEL(x)             (GPIO_IS_GPIOK(x) ? (GPIO_PMGPIO_BASE + 0x14UL) : (GPIO_REG_BASE(x) + 0x20UL))
#define GPIO_REG_CD(x,pin)              ((GPIO_IS_GPIOK(x) ? (GPIO_PMGPIO_BASE + 0x18UL) : (GPIO_REG_BASE(x) + 0x14UL)) + (0x4UL * ((pin) / (uint32_t)16)))
#define GPIO_REG_IEN(x)                 (GPIO_IS_GPIOK(x) ? (GPIO_PMGPIO_BASE + 0x0CUL) :( GPIO_REG_BASE(x) + 0x24UL))
//#define GPIO_REG_IS(x)                  (GPIO_REG_BASE(x) + 0x28UL)
//#define GPIO_REG_SR(x)                  (GPIO_REG_BASE(x) + 0x2CUL)
#define GPIO_REG_FN(x,pin)              ((GPIO_REG_BASE(x) + 0x30UL) + (0x4UL * ((pin)/(uint32_t)8)))
#define GPIO_MFIO_CFG					(MCU_BSP_GPIO_BASE + (0x2B4UL))
#define GPIO_PERICH_SEL					(MCU_BSP_GPIO_BASE + (0x2B8UL))

#define GPIO_PMGPIO_SEL                 (GPIO_PMGPIO_BASE + 0x8UL)

#define GPIO_LIST_NUM                   (6)
#define GPIO_MAX_STRING                 (20)
#define GPIO_SCAN_DATA                  (0)
#define GPIO_SCAN_OUTPUT_ENABLE         (1)
#define GPIO_SCAN_INPUT_ENABLE          (2)
#define GPIO_SCAN_PULL_UP_DOWN          (3)
#define GPIO_SCAN_DRIVE_STRENGTH        (4)
#define GPIO_SCAN_SCHMITT               (5)
#define GPIO_SCAN_ALL                   (6)


//#define GPIO_SCAN_DEBUG

/*
***************************************************************************************************
*                                             VARIABLES
***************************************************************************************************
*/

int mfio_ch_cfg_flag[3] = { 0, };

#ifdef GPIO_SCAN_DEBUG
static const int8_t scan_list[GPIO_LIST_NUM][GPIO_MAX_STRING] =
{
    "DATA",
    "OUTPUT ENABLE",
    "INPUT ENABLE",
    "PULL UP/DOWN",
    "DRIVE STRENGH",
    "SCHMITT"
};

static int32_t scan_offset[] = { 0x0, 0x04, 0x24, 0x1C, 0x14, 0x28 };
#endif


/*
***************************************************************************************************
*                                         FUNCTION PROTOTYPES
***************************************************************************************************
*/


/*
***************************************************************************************************
*                                          GPIO_Config
*
* @param    uiPort [in]
* @param    uiConfig [in]
* @return
*
* Notes
*
***************************************************************************************************
*/

/*
 * GPIO_Config() function descripiton.
 * Parameters
 *  - port  : gpio index
 *  - config: default setting values are below when config is 0.
 *            * slew rate          : fast (gpio_k is not support)
 *            * input type         : cmos (gpio_k is not support)
 *            * input buffer       : no change
 *            * direction          : input
 *            * driver strength    : no change
 *            * pull up/down       : no pull (pull disable)
 *            * function selection : normal gpio
 */
SALRetCode_t GPIO_Config
(
    uint32_t                              uiPort,
    uint32_t                              uiConfig
)
{
    uint32_t pin;
    uint32_t bit;
    uint32_t func;
    uint32_t pull;
    uint32_t ds;
    uint32_t ien;
    uint32_t base_val;
    uint32_t comp_val;
    uint32_t set_val;
    uint32_t reg_fn;
    uint32_t pullen_addr;
    uint32_t pullsel_addr;
    uint32_t cd_addr;
    uint32_t outen_addr;
    uint32_t ien_addr;


    pin     = uiPort & (uint32_t)GPIO_PIN_MASK;
    bit     = (uint32_t)1 << pin;
    func    = uiConfig & (uint32_t)GPIO_FUNC_MASK;
    pull    = uiConfig & ((uint32_t)GPIO_PULL_MASK << (uint32_t)GPIO_PULL_SHIFT);
    ds      = uiConfig & ((uint32_t)GPIO_DS_MASK << (uint32_t)GPIO_DS_SHIFT);
    ien     = uiConfig & ((uint32_t)GPIO_INPUTBUF_MASK << (uint32_t)GPIO_INPUTBUF_SHIFT);


    /* function */
    reg_fn      = GPIO_REG_FN(uiPort,pin);
    base_val    = SAL_ReadReg(reg_fn) & (~((uint32_t)0xF<<((pin%(uint32_t)8)*(uint32_t)4)));
    set_val     = base_val | (func<<((pin%(uint32_t)8)*(uint32_t)4));
    SAL_WriteReg(set_val, reg_fn);
    /* configuration check */
    comp_val    = SAL_ReadReg(reg_fn);

    if(comp_val != set_val)
    {
        return SAL_RET_FAILED;
    }

    /* pull-up/down */
    if (pull == GPIO_PULLUP)
    {
        if(GPIO_IS_GPIOK(uiPort))
        {
            pullen_addr = (GPIO_PMGPIO_BASE + 0x10UL);
        }
        else
        {
            pullen_addr = (GPIO_REG_BASE(uiPort) + 0x1CUL);
        }

        base_val    = SAL_ReadReg(pullen_addr);
        set_val     = (base_val | bit);
        SAL_WriteReg(set_val, pullen_addr);
        comp_val    = SAL_ReadReg(pullen_addr);

        if(comp_val != set_val)
        {
            return SAL_RET_FAILED;
        }

        if(GPIO_IS_GPIOK(uiPort))
        {
            pullsel_addr = (GPIO_PMGPIO_BASE + 0x14UL);
        }
        else
        {
            pullsel_addr = (GPIO_REG_BASE(uiPort) + 0x20UL);
        }

        base_val    = SAL_ReadReg(pullsel_addr);
        set_val     = base_val | bit;
        SAL_WriteReg(set_val, pullsel_addr);
        comp_val    = SAL_ReadReg(pullsel_addr);

        if(comp_val != set_val)
        {
            return SAL_RET_FAILED;
        }
    }
    else if (pull == GPIO_PULLDN)
    {
        if(GPIO_IS_GPIOK(uiPort))
        {
            pullen_addr = (GPIO_PMGPIO_BASE + 0x10UL);
        }
        else
        {
            pullen_addr = (GPIO_REG_BASE(uiPort) + 0x1CUL);
        }

        base_val    = SAL_ReadReg(pullen_addr);
        set_val     = base_val | bit;
        SAL_WriteReg(set_val, pullen_addr);
        comp_val    = SAL_ReadReg(pullen_addr);

        if(comp_val != set_val)
        {
            return SAL_RET_FAILED;
        }

        if(GPIO_IS_GPIOK(uiPort))
        {
            pullsel_addr = (GPIO_PMGPIO_BASE + 0x14UL);
        }
        else
        {
            pullsel_addr = (GPIO_REG_BASE(uiPort) + 0x20UL);
        }

        base_val    = SAL_ReadReg(pullsel_addr);
        set_val     = base_val & ~bit;
        SAL_WriteReg(set_val, pullsel_addr);
        comp_val    = SAL_ReadReg(pullsel_addr);

        if(comp_val != set_val)
        {
            return SAL_RET_FAILED;
        }
    }
    else
    {
        if(GPIO_IS_GPIOK(uiPort))
        {
            pullen_addr = (GPIO_PMGPIO_BASE + 0x10UL);
        }
        else
        {
            pullen_addr = (GPIO_REG_BASE(uiPort) + 0x1CUL);
        }

        base_val    = SAL_ReadReg(pullen_addr);
        set_val     = base_val & ~bit;
        /* set pull disable. */
        SAL_WriteReg(set_val, pullen_addr);
        comp_val    = SAL_ReadReg(pullen_addr);

        if(comp_val != set_val)
        {
            return SAL_RET_FAILED;
        }
    }

    /* drive strength */
    if (ds != 0UL)
    {
        if(GPIO_IS_GPIOK(uiPort))
        {
            cd_addr = (GPIO_PMGPIO_BASE + 0x18UL) + (0x4UL * ((pin) / (uint32_t)16));
        }
        else
        {
            cd_addr = (GPIO_REG_BASE(uiPort) + 0x14UL) + (0x4UL * ((pin) / (uint32_t)16));
        }

        ds          = ds >> (uint32_t)GPIO_DS_SHIFT;
        base_val    = SAL_ReadReg(cd_addr) & ~((uint32_t)3 << ((pin % (uint32_t)16) * (uint32_t)2));
        set_val     = base_val | ((ds & (uint32_t)0x3) << ((pin % (uint32_t)16) * (uint32_t)2));
        SAL_WriteReg(set_val, cd_addr);
        comp_val    = SAL_ReadReg(cd_addr);

        if(comp_val != set_val)
        {
            return SAL_RET_FAILED;
        }
    }

    /* direction */
    if ((uiConfig&GPIO_OUTPUT) != 0UL)
    {
        outen_addr  = GPIO_REG_OUTEN(uiPort);
        base_val    = SAL_ReadReg(outen_addr);
        set_val     = base_val | bit;
        SAL_WriteReg(set_val, outen_addr);
        comp_val    = SAL_ReadReg(outen_addr);

        if(comp_val != set_val)
        {
            return SAL_RET_FAILED;
        }
    }
    else
    {
        outen_addr  = GPIO_REG_OUTEN(uiPort);
        base_val    = SAL_ReadReg(outen_addr);
        set_val     = base_val & ~bit;
        SAL_WriteReg(set_val, outen_addr);
        comp_val    = SAL_ReadReg(outen_addr);

        if(comp_val != set_val)
        {
            return SAL_RET_FAILED;
        }
    }

    /* input buffer enable */
    if (ien == GPIO_INPUTBUF_EN)
    {
        if(GPIO_IS_GPIOK(uiPort))
        {
            ien_addr = (GPIO_PMGPIO_BASE + 0x0CUL);
        }
        else
        {
            ien_addr = (GPIO_REG_BASE(uiPort) + 0x24UL);
        }

        base_val    = SAL_ReadReg(ien_addr);
        set_val     = base_val | bit;
        SAL_WriteReg(set_val, ien_addr);
        comp_val    = SAL_ReadReg(ien_addr);

        if(comp_val != set_val)
        {
            return SAL_RET_FAILED;
        }
    }
    else if (ien == GPIO_INPUTBUF_DIS)
    {
        if(GPIO_IS_GPIOK(uiPort))
        {
            ien_addr = (GPIO_PMGPIO_BASE + 0x0CUL);
        }
        else
        {
            ien_addr = (GPIO_REG_BASE(uiPort) + 0x24UL);
        }

        base_val    = SAL_ReadReg(ien_addr);
        set_val     = base_val & ~bit;
        SAL_WriteReg(set_val, ien_addr);
        comp_val    = SAL_ReadReg(ien_addr);

        if(comp_val != set_val)
        {
            return SAL_RET_FAILED;
        }
    }
    else //QAC
    {
        ; // no statement
    }

    return SAL_RET_SUCCESS;
}

/*
***************************************************************************************************
*                                          GPIO_Get
*
* @param    uiPort [in]
* @return
*
* Notes
*
***************************************************************************************************
*/

uint8_t GPIO_Get
(
    uint32_t                              uiPort
)
{
    uint32_t reg_data;
    uint32_t value;

    reg_data    = GPIO_REG_DATA(uiPort);
    value       = (SAL_ReadReg(reg_data) & ((uint32_t)1 << (uiPort & GPIO_PIN_MASK)));

    if (value != 0UL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/*
***************************************************************************************************
*                                          GPIO_Set
*
* @param    uiPort [in]
* @param    uiData [in]
* @return
*
* Notes
*
***************************************************************************************************
*/

SALRetCode_t GPIO_Set
(
    uint32_t                              uiPort,
    uint32_t                              uiData
)
{
    uint32_t bit;
    uint32_t data_or;
    uint32_t data_bic;

    bit = (uint32_t)1 << (uiPort & GPIO_PIN_MASK);

    if (uiData > 1UL)
    {
        return SAL_RET_FAILED;
    }

    /* set data */
    if (uiData!=0UL)
    {
        data_or = GPIO_REG_DATA_OR(uiPort);
        SAL_WriteReg(bit, data_or);
    }
    else
    {
        data_bic = GPIO_REG_DATA_BIC(uiPort);
        SAL_WriteReg(bit, data_bic);
    }

    //gpio_select(port, ENABLE);

    return SAL_RET_SUCCESS;
}


/*
***************************************************************************************************
*                                          GPIO_ToNum
*
* @param    uiPort [in]
* @return
*
* Notes
*
***************************************************************************************************
*/
uint32_t GPIO_ToNum
(
    uint32_t                              uiPort
)
{
    uint32_t pin;

    pin = uiPort&GPIO_PIN_NUM_MASK;

    switch (uiPort & GPIO_PORT_MASK)
    {
        case GPIO_PORT_A:
        {
            if (pin <= (uint32_t)31)
            {  //removed codesonar warning
                return pin;
            }

            break;
        }

        case GPIO_PORT_B:
        {
            if (pin <= (uint32_t)28)
            {
                return (0x20UL + pin);
            }

            break;
        }
        case GPIO_PORT_C:
        {
            if (pin <= (uint32_t)29)
            {
                return (0x3dUL + pin);
            }

            break;
        }

        case GPIO_PORT_K:
        {
            if (pin <= (uint32_t)18)
            {
                return (0x86UL + pin);
            }

            break;
        }

        default:
        {
            GPIO_E("\n Can't find GPIO Port \n");
            break;
        }
    }

    return (uint32_t)SAL_RET_FAILED;
}

/*
***************************************************************************************************
*										   GPIO_PerichSel
*
* @param	uiPerichSel [in]
* @param	uiCh [in]
*
*
* Notes
*
***************************************************************************************************
*/
SALRetCode_t GPIO_PerichSel
(
	uint32_t								uiPerichSel,
	uint32_t								uiCh
)
{
	uint32_t peri_sel_addr;
	uint32_t clear_bit;
	uint32_t set_bit;
	uint32_t base_val;
	uint32_t comp_val;

	peri_sel_addr = GPIO_PERICH_SEL;
	base_val = SAL_ReadReg(peri_sel_addr);

	if(uiPerichSel < GPIO_PERICH_SEL_I2SSEL_0)
	{
		if(uiCh < 2)
		{
			//clear bit
			clear_bit = base_val & ~((0x1UL) << uiPerichSel);
			SAL_WriteReg(clear_bit, peri_sel_addr);
			//set bit
			base_val = SAL_ReadReg(peri_sel_addr);
			set_bit = base_val | ((uiCh & 0x1UL) << uiPerichSel);
			SAL_WriteReg(set_bit,peri_sel_addr);
			comp_val	= SAL_ReadReg(peri_sel_addr);

			if(comp_val != set_bit)
			{
				GPIO_E("GPIO PerichSel 1bit error \n");
				return SAL_RET_FAILED;
			}
		}
		else
		{
			GPIO_E("GPIO PerichSel ch 1bit error \n");
			return SAL_RET_FAILED;
		}
	}
	else
	{
		if(uiCh < 4)
		{
			//clear bit
			clear_bit = base_val & ~((0x3UL) << uiPerichSel);
			SAL_WriteReg(clear_bit, peri_sel_addr);
			//set bit
			base_val = SAL_ReadReg(peri_sel_addr);
			set_bit = base_val | ((uiCh & 0x3UL) << uiPerichSel);
			SAL_WriteReg(set_bit,peri_sel_addr);
			comp_val	= SAL_ReadReg(peri_sel_addr);

			if(comp_val != set_bit)
			{
				GPIO_E("GPIO PerichSel 2bit error \n");
				return SAL_RET_FAILED;
			}
		}
		else
		{
			GPIO_E("GPIO PerichSel ch 2bit error \n");
			return SAL_RET_FAILED;
		}
	}

	return SAL_RET_SUCCESS;
}

/*
***************************************************************************************************
*										   GPIO_MfioCfg
*
* @param	uiPeriSel [in]
* @param	uiPeriType [in]
* @param	uiChSel [in]
* @param	uiChNum [in]
*
*
* Notes
*
***************************************************************************************************
*/
SALRetCode_t GPIO_MfioCfg
(
	uint32_t								uiPeriSel,
	uint32_t								uiPeriType,
	uint32_t								uiChSel,
	uint32_t								uiChNum
)
{
	uint32_t base_val;
	uint32_t set_val;
	uint32_t clear_bit;
	uint32_t comp_val;

	if(uiPeriSel == GPIO_MFIO_CFG_PERI_SEL0)
	{
		if(uiChSel == GPIO_MFIO_CFG_CH_SEL0)
		{
			if(mfio_ch_cfg_flag[0] == 0)
			{
				//clear bit
				base_val = SAL_ReadReg(GPIO_MFIO_CFG);
				clear_bit = base_val & ~((0x3UL) << (uint32_t)GPIO_MFIO_CFG_CH_SEL0)
									 & ~((0x3UL) << (uint32_t)GPIO_MFIO_CFG_PERI_SEL0);
				SAL_WriteReg(clear_bit, GPIO_MFIO_CFG);

				base_val = SAL_ReadReg(GPIO_MFIO_CFG);
				set_val = base_val	| ((uiChNum & 0x3UL) << (uint32_t)GPIO_MFIO_CFG_CH_SEL0)
									| ((uiPeriType & 0x3UL) << (uint32_t)GPIO_MFIO_CFG_PERI_SEL0);
				SAL_WriteReg(set_val, GPIO_MFIO_CFG);
				comp_val	= SAL_ReadReg(GPIO_MFIO_CFG);

				if(comp_val != set_val)
				{
					return SAL_RET_FAILED;
				}
				mfio_ch_cfg_flag[0] = 1;
			}
			else
			{
				GPIO_E("MFID 0 ch%d already set!!\n",uiChNum);
				return SAL_RET_FAILED;
			}

		}
		else
		{
			GPIO_E("match perisel0, chsel0 \n");
			return SAL_RET_FAILED;
		}
	}
	else if(uiPeriSel == GPIO_MFIO_CFG_PERI_SEL1)
	{
		if(uiChSel == GPIO_MFIO_CFG_CH_SEL1)
		{
			if(mfio_ch_cfg_flag[1] == 0)
			{
				//clear bit
				base_val = SAL_ReadReg(GPIO_MFIO_CFG);
				clear_bit = base_val & ~((0x3UL) << (uint32_t)GPIO_MFIO_CFG_CH_SEL1)
									 & ~((0x3UL) << (uint32_t)GPIO_MFIO_CFG_PERI_SEL1);
				SAL_WriteReg(clear_bit, GPIO_MFIO_CFG);

				base_val = SAL_ReadReg(GPIO_MFIO_CFG);
				set_val = base_val	| ((uiChNum & 0x3UL) << (uint32_t)GPIO_MFIO_CFG_CH_SEL1)
									| ((uiPeriType & 0x3UL) << (uint32_t)GPIO_MFIO_CFG_PERI_SEL1);
				SAL_WriteReg(set_val, GPIO_MFIO_CFG);
				comp_val	= SAL_ReadReg(GPIO_MFIO_CFG);

				if(comp_val != set_val)
				{
					return SAL_RET_FAILED;
				}
				mfio_ch_cfg_flag[1] = 1;
			}
			else
			{
				GPIO_E("MFID 01 ch%d already set!!\n",uiChNum);
				return SAL_RET_FAILED;
			}
		}
		else
		{
			GPIO_E("match perisel1, chsel1 \n");
			return SAL_RET_FAILED;
		}

	}
	else if(uiPeriSel == GPIO_MFIO_CFG_PERI_SEL2)
	{
		if(uiChSel == GPIO_MFIO_CFG_CH_SEL2)
		{
			if(mfio_ch_cfg_flag[2] == 0)
			{
				//clear bit
				base_val = SAL_ReadReg(GPIO_MFIO_CFG);
				clear_bit = base_val & ~((0x3UL) << (uint32_t)GPIO_MFIO_CFG_CH_SEL2)
									 & ~((0x3UL) << (uint32_t)GPIO_MFIO_CFG_PERI_SEL2);
				SAL_WriteReg(clear_bit, GPIO_MFIO_CFG);

				base_val = SAL_ReadReg(GPIO_MFIO_CFG);
				set_val = base_val	| ((uiChNum & 0x3UL) << (uint32_t)GPIO_MFIO_CFG_CH_SEL2)
									| ((uiPeriType & 0x3UL) << (uint32_t)GPIO_MFIO_CFG_PERI_SEL2);
				SAL_WriteReg(set_val, GPIO_MFIO_CFG);
				comp_val	= SAL_ReadReg(GPIO_MFIO_CFG);

				if(comp_val != set_val)
				{
					return SAL_RET_FAILED;
				}
				mfio_ch_cfg_flag[2] = 1;
			}
			else
			{
				GPIO_E("MFID 02 ch%d already set!!\n",uiChNum);
				return SAL_RET_FAILED;
			}
		}
		else
		{
			GPIO_E("match perisel2, chsel2 \n");
			return SAL_RET_FAILED;
		}
	}
	else
	{
		GPIO_E("check perisel \n");
		return SAL_RET_FAILED;
	}

	return SAL_RET_SUCCESS;
}

/**************************************************************************************************
*                                       GPIO_IntExtSet
*
* Configure external interrupt source select.
*
* @param    uiIntId   [in] Index of Interrupt Source id.
* @param    uiGpio    [in] register offset of gpio, refer to TCC_GPx(n) macro
*
* @return   Configuration result.
*           SAL_RET_SUCCESS  Externel Interrupt successfully set.
*           SAL_RET_FAILED   Error setting interrupt.
*
* Notes
*           ExtIntr table .. refer to External Interrupt and External Input Clock Selection at the datasheet.
*           A : 0 ~  30  GPIO_A[30:00]
*           B :	31 ~ 59  GPIO_B[28:00]
*           C :	60 ~ 87  GPIO_C[27:00]
*           K :	88 ~ 105 GPIO_K[17:00]

**************************************************************************************************/
SALRetCode_t GPIO_IntExtSet
(
    uint32_t                              uiIntId,
    uint32_t                              uiGpio
)
{
    SALRetCode_t  ucRet;
    uint32_t        uiEintSel;
    uint32_t        uiBitField;
    uint32_t        uiExtIntGpioIdx;
    uint32_t        uiEIntSel;
    uint32_t        uiEIntSelMask;

    static const uint32_t ExtIntr[] =
    {
        GPIO_GPA(0UL),   GPIO_GPA(1UL),   GPIO_GPA(2UL),   GPIO_GPA(3UL),   GPIO_GPA(4UL),   GPIO_GPA(5UL),   GPIO_GPA(6UL),   GPIO_GPA(7UL),
        GPIO_GPA(8UL),   GPIO_GPA(9UL),   GPIO_GPA(10UL),  GPIO_GPA(11UL),  GPIO_GPA(12UL),  GPIO_GPA(13UL),  GPIO_GPA(14UL),  GPIO_GPA(15UL),
        GPIO_GPA(16UL),  GPIO_GPA(17UL),  GPIO_GPA(18UL),  GPIO_GPA(18UL),  GPIO_GPA(20UL),  GPIO_GPA(21UL),  GPIO_GPA(22UL),  GPIO_GPA(23UL),
        GPIO_GPA(24UL),  GPIO_GPA(25UL),  GPIO_GPA(26UL),  GPIO_GPA(27UL),  GPIO_GPA(28UL),  GPIO_GPA(29UL),  GPIO_GPA(30UL),  

        GPIO_GPB(0UL),   GPIO_GPB(1UL),   GPIO_GPB(2UL),   GPIO_GPB(3UL),   GPIO_GPB(4UL),   GPIO_GPB(5UL),   GPIO_GPB(6UL),   GPIO_GPB(7UL),
        GPIO_GPB(8UL),   GPIO_GPB(9UL),   GPIO_GPB(10UL),  GPIO_GPB(11UL),  GPIO_GPB(12UL),  GPIO_GPB(13UL),  GPIO_GPB(14UL),  GPIO_GPB(15UL),
        GPIO_GPB(16UL),  GPIO_GPB(17UL),  GPIO_GPB(18UL),  GPIO_GPB(19UL),  GPIO_GPB(20UL),  GPIO_GPB(21UL),  GPIO_GPB(22UL),  GPIO_GPB(23UL),
        GPIO_GPB(24UL),  GPIO_GPB(25UL),  GPIO_GPB(26UL),  GPIO_GPB(27UL),  GPIO_GPB(28UL),

        GPIO_GPC(0UL),   GPIO_GPC(1UL),   GPIO_GPC(2UL),   GPIO_GPC(3UL) ,  GPIO_GPC(4UL),   GPIO_GPC(5UL),   GPIO_GPC(6UL),   GPIO_GPC(7UL),
        GPIO_GPC(8UL),   GPIO_GPC(9UL),   GPIO_GPC(10UL),  GPIO_GPC(11UL),  GPIO_GPC(12UL),  GPIO_GPC(13UL),  GPIO_GPC(14UL),  GPIO_GPC(15UL),
        GPIO_GPC(16UL),  GPIO_GPC(17UL),  GPIO_GPC(18UL),  GPIO_GPC(19UL),  GPIO_GPC(20UL),  GPIO_GPC(21UL),  GPIO_GPC(22UL),  GPIO_GPC(23UL),
        GPIO_GPC(24UL),  GPIO_GPC(25UL),  GPIO_GPC(26UL),  GPIO_GPC(27UL),

        GPIO_GPK(0UL),   GPIO_GPK(1UL),   GPIO_GPK(2UL),   GPIO_GPK(3UL) ,  GPIO_GPK(4UL),   GPIO_GPK(5UL),   GPIO_GPK(6UL),   GPIO_GPK(7UL),
        GPIO_GPK(8UL),   GPIO_GPK(9UL),   GPIO_GPK(10UL),  GPIO_GPK(11UL),  GPIO_GPK(12UL),  GPIO_GPK(13UL),  GPIO_GPK(14UL),  GPIO_GPK(15UL),
        GPIO_GPK(16UL),  GPIO_GPK(17UL),
    };

    ucRet			= (SALRetCode_t)SAL_RET_SUCCESS;
    uiEintSel		= ((MCU_BSP_GPIO_BASE + 0x280UL) + (4UL*((uiIntId-(uint32_t)GIC_EXT0)/4UL))); /* EINT_SEL0,2,3 */
    uiBitField		= 0;
    uiExtIntGpioIdx     = 0;
    uiEIntSel		= 0;
    uiEIntSelMask	= 0;

    if ((uiIntId < (uint32_t)GIC_EINT_START_INT) || (uiIntId > (uint32_t)GIC_EINT_END_INT))
    {
        ucRet = (SALRetCode_t)SAL_RET_FAILED;
    }
    else
    {

        for ( ; uiExtIntGpioIdx < SAL_ArraySize(ExtIntr) ; uiExtIntGpioIdx++)
        {
            if (ExtIntr[uiExtIntGpioIdx] == uiGpio)
            {
                break;
            }
        }

        if (uiExtIntGpioIdx >= SAL_ArraySize(ExtIntr))
        {
            ucRet = (SALRetCode_t)SAL_RET_FAILED;
        }
        else
        {
            uiBitField      = (uint32_t)(8UL * ((uiIntId - (uint32_t)GIC_EXT0) % 4UL));
            uiEIntSelMask   = ((uint32_t)0x7FUL << uiBitField);

            uiEIntSel = (uint32_t)SAL_ReadReg(uiEintSel);
            uiEIntSel = (uint32_t)((uiEIntSel & ~uiEIntSelMask) | (uiExtIntGpioIdx << uiBitField));
            SAL_WriteReg(uiEIntSel, uiEintSel);
        }
    }

    return ucRet;
}


#ifdef GPIO_SCAN_DEBUG
/*
***************************************************************************************************
*                                          GPIO_PrintOut
*
* @param    uiReg [in]
* @param    uiFlag [in]
*
*
* Notes
*
***************************************************************************************************
*/
void GPIO_PrintOut
(
    uint32_t                              uiReg,
    uint32_t                              uiFlag
)
{

    uint32_t  gpio_reg_data;
    int8_t   gpio_reg_buf[32];
    int32_t  i;

    gpio_reg_buf[32]    = {0,};
    gpio_reg_data       = SAL_ReadReg(uiReg + scan_offset[uiFlag]);
    GPIO_D("%s : ", scan_list[uiFlag]);

    for(i = 0 ; i < 32 ; i++)
    {
        gpio_reg_buf[31 - i]    = (gpio_reg_data & 0x1) + '0';
        gpio_reg_data           = (gpio_reg_data >> 1);
    }

    GPIO_D("%s\n", gpio_reg_buf);
}

/*
***************************************************************************************************
*                                          GPIO_Scan
*
* @param    uiPort [in]
* @param    uiFlag [in]
*
*
* Notes
*
***************************************************************************************************
*/
void GPIO_Scan
(
    uint32_t                              uiPort,
    uint32_t                              uiFlag
)
{

    uint32_t  gpio_reg;
    int32_t  i;

    gpio_reg    = GPIO_REG_BASE(uiPort);

    switch(uiPort & GPIO_PORT_MASK)
    {
        case GPIO_PORT_A :
        {
            GPIO_D("\nGPIO A REG DATA \n");
            break;
        }

        case GPIO_PORT_B :
        {
            GPIO_D("\nGPIO B REG DATA \n");
            break;
        }

        case GPIO_PORT_C :
        {
            GPIO_D("\nGPIO C REG DATA \n");
            break;
        }

    }

    if(uiFlag != GPIO_SCAN_ALL)
    {
        GPIO_PrintOut(gpio_reg, uiFlag);
    }
    else
    {
        for(i = 0 ; i < GPIO_LIST_NUM ; i++)
        {
            GPIO_PrintOut(gpio_reg, i);
        }
    }
}
#endif

