// SPDX-License-Identifier: Apache-2.0

/*
***************************************************************************************************
*
*   FileName : gpsb_reg.c
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*/

#include <gpsb_reg.h>

/*                                                                                                 */
/***************************************************************************************************/
/*                                             LOCAL VARIABLES                                     */
/***************************************************************************************************/


GPSBDev_t gpsb[GPSB_CH_NUM + 1UL] =
{
    {
        0,                              /* GPSB channel */
        GPSB0_BASE,                     /* GPSB base address */
        GPSB_PCFG_BASE,                 /* GPSB Port Configuration address */
        (uint32)CLOCK_PERI_GPSB0,       /* Peri. clock number */
        (sint32)CLOCK_IOBUS_GPSB0,      /* IOBUS number */
        TRUE,                           /* GPSB mode (Master or Slave) */
        NULL,                           /* SDO */
        NULL,                           /* SDI */
        NULL,                           /* SCLK */
        NULL,                           /* PORT */
        GIC_PRIORITY_NO_MEAN,           /* Interrupt priority */
        FALSE,                          /* support dma */
        {NULL, },
        {NULL, },
        NULL_PTR,                       /* async pio tx buffer address */
        NULL_PTR,                       /* async pio rx buffer address */
        NULL,                           /* async pio tx buffer size */
        NULL,                           /* async pio rx buffer size */
        NULL,
        {NULL_PTR, },
    },
    {
        1,                              /* GPSB channel */
        GPSB1_BASE,                     /* GPSB base address */
        GPSB_PCFG_BASE,                 /* GPSB Port Configuration address */
        (uint32)CLOCK_PERI_GPSB1,       /* Peri.clock nmumber */
        (sint32)CLOCK_IOBUS_GPSB1,      /* IOBUS number */
        TRUE,                           /* GPSB mode (Master or Slave) */
        NULL,                           /* SDO */
        NULL,                           /* SDI */
        NULL,                           /* SCLK */
        NULL,                           /* PORT */
        GIC_PRIORITY_NO_MEAN,           /* Interrupt priority */
        FALSE,
        {NULL, },
        {NULL, },
        NULL_PTR,                       /* async pio tx buffer address */
        NULL_PTR,                       /* async pio rx buffer address */
        NULL,                           /* async pio tx buffer size */
        NULL,                           /* async pio rx buffer size */
        NULL,
        {NULL_PTR, },
    },
    {
        2,
        GPSB2_BASE,                     /* GPSB channel */
        GPSB_PCFG_BASE,                 /* GPSB Port Configuration address */
        (uint32)CLOCK_PERI_GPSB2,       /* GPSB base address */
        (sint32)CLOCK_IOBUS_GPSB2,      /* Peri.clock number */
        TRUE,                           /* GPSB mode (Master or Slave) */
        NULL,                           /* SDO */
        NULL,                           /* SDI */
        NULL,                           /* SCLK */
        NULL,                           /* PORT */
        GIC_PRIORITY_NO_MEAN,           /* Interrupt priority */
        FALSE,
        {NULL, },
        {NULL, },
        NULL_PTR,                       /* async pio tx buffer address */
        NULL_PTR,                       /* async pio rx buffer address */
        NULL,                           /* async pio tx buffer size */
        NULL,                           /* async pio rx buffer size */
        NULL,
        {NULL_PTR, },
    },
    {
        3,
        GPSB3_BASE,                     /* GPSB channel */
        GPSB_PCFG_BASE,                 /* GPSB Port Configuration address */
        (uint32)CLOCK_PERI_GPSB3,       /* GPSB base address */
        (sint32)CLOCK_IOBUS_GPSB3,      /* Peri.clock number */
        TRUE,                           /* GPSB mode (Master or Slave) */
        NULL,                           /* SDO */
        NULL,                           /* SDI */
        NULL,                           /* SCLK */
        NULL,                           /* PORT */
        GIC_PRIORITY_NO_MEAN,           /* Interrupt priority */
        FALSE,
        {NULL, },
        {NULL, },
        NULL_PTR,                       /* async pio tx buffer address */
        NULL_PTR,                       /* async pio rx buffer address */
        NULL,                           /* async pio tx buffer size */
        NULL,                           /* async pio rx buffer size */
        NULL,
        {NULL_PTR, },
    },
    {
        4,
        GPSB4_BASE,                     /* GPSB channel */
        GPSB_PCFG_BASE,                 /* GPSB Port Configuration address */
        (uint32)CLOCK_PERI_GPSB4,       /* GPSB base address */
        (sint32)CLOCK_IOBUS_GPSB4,      /* Peri.clock number */
        TRUE,                           /* mode (Master or Slave) */
        NULL,                           /* SDO */
        NULL,                           /* SDI */
        NULL,                           /* SCLK */
        NULL,                           /* PORT */
        GIC_PRIORITY_NO_MEAN,           /* Interrupt priority */
        FALSE,
        {NULL, },
        {NULL, },
        NULL_PTR,                       /* async pio tx buffer address */
        NULL_PTR,                       /* async pio rx buffer address */
        NULL,                           /* async pio tx buffer size */
        NULL,                           /* async pio rx buffer size */
        NULL,
        {NULL_PTR, },
    },
};

const uint32 gpsbport[GPSB_PORT_NUM][9] =
{
    {GPIO_GPA(0UL)  , GPIO_GPA(1UL)  , GPIO_GPA(2UL)  , GPIO_GPA(3UL)   , GPIO_FUNC(2UL), GPIO_PERICH_SEL_SPISEL_1, GPIO_PERICH_CH0, NULL, NULL},
    {GPIO_GPB(4UL)  , GPIO_GPB(5UL)  , GPIO_GPB(6UL)  , GPIO_GPB(7UL)   , GPIO_FUNC(1UL), GPIO_PERICH_SEL_SPISEL_0, GPIO_PERICH_CH0, NULL, NULL},
    {GPIO_GPC(7UL)  , GPIO_GPC(8UL)  , GPIO_GPC(9UL)  , GPIO_GPC(10UL)  , GPIO_FUNC(1UL), GPIO_PERICH_SEL_SPISEL_0, GPIO_PERICH_CH1, NULL, NULL},
    {GPIO_GPC(12UL) , GPIO_GPC(13UL) , GPIO_GPC(14UL) , GPIO_GPC(15UL)  , GPIO_FUNC(1UL), GPIO_PERICH_SEL_SPISEL_1, GPIO_PERICH_CH1, NULL, NULL},

    // Multi-Function IO
    {GPIO_GPA(19UL) , GPIO_GPA(18UL) , GPIO_GPA(16UL) , GPIO_GPA(17UL)  , GPIO_FUNC(3UL), GPIO_MFIO_CFG_PERI_SEL0, GPIO_MFIO_CFG_CH_SEL0, GPIO_MFIO_SPI2 , GPIO_MFIO_CH0},
    {GPIO_GPA(23UL) , GPIO_GPA(22UL) , GPIO_GPA(20UL) , GPIO_GPA(21UL)  , GPIO_FUNC(3UL), GPIO_MFIO_CFG_PERI_SEL1, GPIO_MFIO_CFG_CH_SEL1, GPIO_MFIO_SPI3 , GPIO_MFIO_CH0},
    {GPIO_GPA(27UL) , GPIO_GPA(26UL) , GPIO_GPA(24UL) , GPIO_GPA(25UL)  , GPIO_FUNC(3UL), GPIO_MFIO_CFG_PERI_SEL2, GPIO_MFIO_CFG_CH_SEL2, GPIO_MFIO_SPI4 , GPIO_MFIO_CH0},
    {GPIO_GPK(14UL) , GPIO_GPK(13UL) , GPIO_GPK(11UL) , GPIO_GPK(12UL)  , GPIO_FUNC(3UL), GPIO_MFIO_CFG_PERI_SEL0, GPIO_MFIO_CFG_CH_SEL0, GPIO_MFIO_SPI2 , GPIO_MFIO_CH3},

    {GPIO_GPB(3UL)  , GPIO_GPB(2UL)  , GPIO_GPB(0UL)  , GPIO_GPB(1UL)   , GPIO_FUNC(3UL), GPIO_MFIO_CFG_PERI_SEL0, GPIO_MFIO_CFG_CH_SEL0, GPIO_MFIO_SPI2 , GPIO_MFIO_CH1},
    {GPIO_GPB(7UL)  , GPIO_GPB(6UL)  , GPIO_GPB(4UL)  , GPIO_GPB(5UL)   , GPIO_FUNC(3UL), GPIO_MFIO_CFG_PERI_SEL1, GPIO_MFIO_CFG_CH_SEL1, GPIO_MFIO_SPI3 , GPIO_MFIO_CH1},
    {GPIO_GPB(11UL) , GPIO_GPB(10UL) , GPIO_GPB(8UL)  , GPIO_GPB(9UL)   , GPIO_FUNC(3UL), GPIO_MFIO_CFG_PERI_SEL2, GPIO_MFIO_CFG_CH_SEL2, GPIO_MFIO_SPI4 , GPIO_MFIO_CH1},

    {GPIO_GPC(7UL)  , GPIO_GPC(6UL)  , GPIO_GPC(4UL)  , GPIO_GPC(5UL)   , GPIO_FUNC(3UL), GPIO_MFIO_CFG_PERI_SEL0, GPIO_MFIO_CFG_CH_SEL0, GPIO_MFIO_SPI2 , GPIO_MFIO_CH2},
    {GPIO_GPC(11UL) , GPIO_GPC(10UL) , GPIO_GPC(8UL)  , GPIO_GPC(9UL)   , GPIO_FUNC(3UL), GPIO_MFIO_CFG_PERI_SEL1, GPIO_MFIO_CFG_CH_SEL1, GPIO_MFIO_SPI3 , GPIO_MFIO_CH2},
    {GPIO_GPC(15UL) , GPIO_GPC(14UL) , GPIO_GPC(12UL) , GPIO_GPC(13UL)  , GPIO_FUNC(3UL), GPIO_MFIO_CFG_PERI_SEL2, GPIO_MFIO_CFG_CH_SEL2, GPIO_MFIO_SPI4 , GPIO_MFIO_CH2},

};

const GPSBSafetyBase_t gpsb_sm[GPSB_CH_NUM + 1UL] =
{
    {
        (GPSB_IO_MONITOR_BASE + GPSB0_IOMON),
    },
    {
        (GPSB_IO_MONITOR_BASE + GPSB1_IOMON),
    },
    {
        (GPSB_IO_MONITOR_BASE + GPSB2_IOMON),
    },
    {
        (GPSB_IO_MONITOR_BASE + GPSB3_IOMON),
    },
    {
        (GPSB_IO_MONITOR_BASE + GPSB4_IOMON),
    },
    {
        (GPSB_IO_MONITOR_BASE + GPSB5_IOMON),
    }
};

