/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@arduino.cc>
 * Copyright (c) 2014 by Paul Stoffregen <paul@pjrc.com> (Transaction API)
 * Copyright (c) 2014 by Matthijs Kooijman <matthijs@stdin.nl> (SPISettings AVR)
 * Copyright (c) 2014 by Andrew J. Kroll <xxxajk@gmail.com> (atomicity fixes)
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "SPI.h"

//hsj
// #include "gpsb_reg.h"
// #include "gpsb.h"


SPIClass SPI;

uint8_t SPIClass::initialized = 0;
uint8_t SPIClass::interruptMode = 0;
uint8_t SPIClass::interruptMask = 0;
uint8_t SPIClass::interruptSave = 0;
#ifdef SPI_TRANSACTION_MISMATCH_LED
uint8_t SPIClass::inTransactionFlag = 0;
#endif



void SPIClass::begin()
{
    uint8                               ucMstCh = 1;
    uint32                              cs_le;
    uint32                              led_oe;
    uint32                              led_pctl;
    uint8                               bpw;
    uint32                              speed;

    GPSBOpenParam_t                     OpenParam;

    OpenParam.uiSdo         = GPIO_GPC(14UL) | GPIO_GPA(02UL) | GPIO_GPB(06UL);
    OpenParam.uiSdi         = GPIO_GPC(15UL) | GPIO_GPA(03UL) | GPIO_GPB(07UL);
    OpenParam.uiSclk        = GPIO_GPC(12UL) | GPIO_GPA(00UL) | GPIO_GPB(04UL);
    OpenParam.pDmaAddrTx    = NULL_PTR;
    OpenParam.pDmaAddrRx    = NULL_PTR;
    OpenParam.uiDmaBufSize  = 0;
    OpenParam.fbCallback    = NULL_PTR;
    OpenParam.pArg          = NULL_PTR;
    OpenParam.uiIsSlave     = GPSB_MASTER_MODE;

    cs_le       = GPIO_GPC(13UL) | GPIO_GPA(01UL) | GPIO_GPB(05UL);
    led_oe      = GPIO_GPA(24UL);
    led_pctl    = GPIO_GPC(11UL);

    /* Configure GPSB master */
    speed     = 30UL*100UL*1000UL; //hsj 30UL*1000UL*1000UL;
    bpw       = (uint8)16UL;


   (void)GPSB_Open((uint8)ucMstCh, OpenParam);
   (void)GPSB_SetSpeed(ucMstCh, speed);
   (void)GPSB_SetBpw(ucMstCh, bpw);
   (void)GPSB_CsAlloc(ucMstCh, cs_le);

   // LED_PWR_CTL
   (void)GPIO_Config(led_pctl, (GPIO_FUNC(0UL) | GPIO_OUTPUT));
   GPIO_Set(led_pctl, 1U);

   // OE
   (void)GPIO_Config(led_oe, (GPIO_FUNC(0UL) | GPIO_OUTPUT));
   GPIO_Set(led_oe, 1U);


}

void SPIClass::end() {
  // uint8_t sreg = SREG;
  // noInterrupts(); // Protect from a scheduler and prevent transactionBegin
  // // Decrease the reference counter
  // if (initialized)
  //   initialized--;
  // // If there are no more references disable SPI
  // if (!initialized) {
  //   SPCR &= ~_BV(SPE);
  //   interruptMode = 0;
  //   #ifdef SPI_TRANSACTION_MISMATCH_LED
  //   inTransactionFlag = 0;
  //   #endif
  // }
  // SREG = sreg;
}

// // mapping of interrupt numbers to bits within SPI_AVR_EIMSK
// #if defined(__AVR_ATmega32U4__)
//   #define SPI_INT0_MASK  (1<<INT0)
//   #define SPI_INT1_MASK  (1<<INT1)
//   #define SPI_INT2_MASK  (1<<INT2)
//   #define SPI_INT3_MASK  (1<<INT3)
//   #define SPI_INT4_MASK  (1<<INT6)
// #elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
//   #define SPI_INT0_MASK  (1<<INT0)
//   #define SPI_INT1_MASK  (1<<INT1)
//   #define SPI_INT2_MASK  (1<<INT2)
//   #define SPI_INT3_MASK  (1<<INT3)
//   #define SPI_INT4_MASK  (1<<INT4)
//   #define SPI_INT5_MASK  (1<<INT5)
//   #define SPI_INT6_MASK  (1<<INT6)
//   #define SPI_INT7_MASK  (1<<INT7)
// #elif defined(EICRA) && defined(EICRB) && defined(EIMSK)
//   #define SPI_INT0_MASK  (1<<INT4)
//   #define SPI_INT1_MASK  (1<<INT5)
//   #define SPI_INT2_MASK  (1<<INT0)
//   #define SPI_INT3_MASK  (1<<INT1)
//   #define SPI_INT4_MASK  (1<<INT2)
//   #define SPI_INT5_MASK  (1<<INT3)
//   #define SPI_INT6_MASK  (1<<INT6)
//   #define SPI_INT7_MASK  (1<<INT7)
// #else
//   #ifdef INT0
//   #define SPI_INT0_MASK  (1<<INT0)
//   #endif
//   #ifdef INT1
//   #define SPI_INT1_MASK  (1<<INT1)
//   #endif
//   #ifdef INT2
//   #define SPI_INT2_MASK  (1<<INT2)
//   #endif
// #endif

void SPIClass::usingInterrupt(uint8_t interruptNumber)
{
  // uint8_t mask = 0;
  // uint8_t sreg = SREG;
  // noInterrupts(); // Protect from a scheduler and prevent transactionBegin
  // switch (interruptNumber) {
  // #ifdef SPI_INT0_MASK
  // case 0: mask = SPI_INT0_MASK; break;
  // #endif
  // #ifdef SPI_INT1_MASK
  // case 1: mask = SPI_INT1_MASK; break;
  // #endif
  // #ifdef SPI_INT2_MASK
  // case 2: mask = SPI_INT2_MASK; break;
  // #endif
  // #ifdef SPI_INT3_MASK
  // case 3: mask = SPI_INT3_MASK; break;
  // #endif
  // #ifdef SPI_INT4_MASK
  // case 4: mask = SPI_INT4_MASK; break;
  // #endif
  // #ifdef SPI_INT5_MASK
  // case 5: mask = SPI_INT5_MASK; break;
  // #endif
  // #ifdef SPI_INT6_MASK
  // case 6: mask = SPI_INT6_MASK; break;
  // #endif
  // #ifdef SPI_INT7_MASK
  // case 7: mask = SPI_INT7_MASK; break;
  // #endif
  // default:
  //   interruptMode = 2;
  //   break;
  // }
  // interruptMask |= mask;
  // if (!interruptMode)
  //   interruptMode = 1;
  // SREG = sreg;
}

void SPIClass::notUsingInterrupt(uint8_t interruptNumber)
{
  // // Once in mode 2 we can't go back to 0 without a proper reference count
  // if (interruptMode == 2)
  //   return;
  // uint8_t mask = 0;
  // uint8_t sreg = SREG;
  // noInterrupts(); // Protect from a scheduler and prevent transactionBegin
  // switch (interruptNumber) {
  // #ifdef SPI_INT0_MASK
  // case 0: mask = SPI_INT0_MASK; break;
  // #endif
  // #ifdef SPI_INT1_MASK
  // case 1: mask = SPI_INT1_MASK; break;
  // #endif
  // #ifdef SPI_INT2_MASK
  // case 2: mask = SPI_INT2_MASK; break;
  // #endif
  // #ifdef SPI_INT3_MASK
  // case 3: mask = SPI_INT3_MASK; break;
  // #endif
  // #ifdef SPI_INT4_MASK
  // case 4: mask = SPI_INT4_MASK; break;
  // #endif
  // #ifdef SPI_INT5_MASK
  // case 5: mask = SPI_INT5_MASK; break;
  // #endif
  // #ifdef SPI_INT6_MASK
  // case 6: mask = SPI_INT6_MASK; break;
  // #endif
  // #ifdef SPI_INT7_MASK
  // case 7: mask = SPI_INT7_MASK; break;
  // #endif
  // default:
  //   break;
  //   // this case can't be reached
  // }
  // interruptMask &= ~mask;
  // if (!interruptMask)
  //   interruptMode = 0;
  // SREG = sreg;
}
