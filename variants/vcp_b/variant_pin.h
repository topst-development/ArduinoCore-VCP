/**
 * @file variant_pin.h
 * @brief Pin definitions for VCP-B variant
 * 
 * This header file contains the pin mapping and configuration definitions
 * specific to the VCP-B variant board. It defines constants and macros
 * for GPIO pins, peripheral connections, and other hardware-specific settings.
 */

#ifndef VARIANT_PIN_H
#define VARIANT_PIN_H

#include "gpio.h"

/* Arduino pin name and GPIO pin name mapping */
#define D0  0
#define D1  0
#define D2  0
#define D3  GPIO_GPB(1UL)
#define D4  0
#define D5  GPIO_GPB(0UL)
#define D6  0
#define D7  GPIO_GPB(4UL)
#define D8  GPIO_GPB(25UL)
#define D9  0
#define D10 GPIO_GPB(26UL)
#define D11 GPIO_GPB(6UL)
#define D12 GPIO_GPA(16UL)
#define D13 GPIO_GPB(23UL)
#define D14 0
#define D15 GPIO_GPK(11UL)
#define D16 GPIO_GPC(4UL)
#define D17 0
#define D18 GPIO_GPC(5UL)
#define D19 GPIO_GPA(24UL)
#define D20 0
#define D21 GPIO_GPA(25UL)
#define D22 GPIO_GPK(9UL)
#define D23 GPIO_GPA(27UL)
#define D24 GPIO_GPA(26UL)
#define D25 0
#define D26 GPIO_GPC(13UL)
#define D27 GPIO_GPB(3UL)
#define D28 GPIO_GPB(2UL)
#define D29 GPIO_GPA(29UL)
#define D30 0
#define D31 GPIO_GPB(19UL)
#define D32 GPIO_GPB(11UL)
#define D33 GPIO_GPC(11UL)
#define D34 0
#define D35 GPIO_GPC(15UL)
#define D36 GPIO_GPB(18UL)
#define D37 GPIO_GPA(18UL)
#define D38 GPIO_GPC(14UL)
#define D39 0
#define D40 GPIO_GPC(12UL)

/* GPIO Pin mapping table for VCP-B */
static const uint32_t PIN_MAP[] = {
    0,              // D0
    0,              // D1
    0,              // D2
    GPIO_GPB(1UL),  // D3
    0,              // D4
    GPIO_GPB(0UL),  // D5
    0,              // D6
    GPIO_GPB(4UL),  // D7
    GPIO_GPB(25UL), // D8
    0,              // D9
    GPIO_GPB(26UL), // D10
    GPIO_GPB(6UL),  // D11
    GPIO_GPC(15UL), // D12
    GPIO_GPC(12UL), // D13
    0,              // D14
    GPIO_GPC(01UL), // D15
    GPIO_GPA(06UL), // D16
    0,              // D17
    GPIO_GPC(5UL),  // D18
    GPIO_GPA(24UL), // D19
    0,              // D20
    GPIO_GPA(25UL), // D21
    GPIO_GPK(9UL),  // D22
    GPIO_GPA(27UL), // D23
    GPIO_GPA(26UL), // D24
    0,              // D25
    GPIO_GPC(13UL), // D26
    GPIO_GPB(3UL),  // D27
    GPIO_GPB(2UL),  // D28
    GPIO_GPA(29UL), // D29
    0,              // D30
    GPIO_GPB(19UL), // D31
    GPIO_GPB(11UL), // D32
    GPIO_GPC(11UL), // D33
    0,              // D34
    GPIO_GPC(15UL), // D35
    GPIO_GPB(18UL), // D36
    GPIO_GPA(18UL), // D37
    GPIO_GPC(14UL), // D38
    0,              // D39
    GPIO_GPC(12UL), // D40
    GPIO_GPK(16UL)  // LED_BUILTIN
};

#define LED_BUILTIN 41

#endif /* VARIANT_PIN_H */
