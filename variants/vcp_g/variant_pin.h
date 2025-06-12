/**
 * @file variant_pin.h
 * @brief Pin definitions for VCP-G variant
 * 
 * This header file contains the pin mapping and configuration definitions
 * specific to the VCP-G variant board. It defines constants and macros
 * for GPIO pins, peripheral connections, and other hardware-specific settings.
 */

#ifndef VARIANT_PIN_H
#define VARIANT_PIN_H

#include "gpio.h"

/* Arduino pin name and GPIO pin name mapping */
#define D0 GPIO_GPB(26UL)
#define D1 GPIO_GPB(25UL)
#define D2 GPIO_GPB(28UL)
#define D3 GPIO_GPB(11UL)
#define D4 GPIO_GPB(27UL)
#define D5 GPIO_GPB(10UL)
#define D6 GPIO_GPA(13UL)
#define D7 GPIO_GPB(01UL)
#define D8 GPIO_GPB(00UL)
#define D9 GPIO_GPA(12UL)
#define D10 GPIO_GPC(13UL)
#define D11 GPIO_GPC(14UL)
#define D12 GPIO_GPC(15UL)
#define D13 GPIO_GPC(12UL)
#define D14 GPIO_GPC(00UL)
#define D15 GPIO_GPC(01UL)
#define D16 GPIO_GPA(06UL)
#define D17 GPIO_GPA(07UL)
#define D18 GPIO_GPA(28UL)
#define D19 GPIO_GPA(29UL)
#define D20 GPIO_GPB(03UL)
#define D21 GPIO_GPB(02UL)
#define D22 GPIO_GPB(24UL)
#define D23 GPIO_GPB(23UL)
#define D24 GPIO_GPB(22UL)
#define D25 GPIO_GPB(21UL)
#define D26 GPIO_GPB(20UL)
#define D27 GPIO_GPB(19UL)
#define D28 GPIO_GPA(30UL)
#define D29 GPIO_GPA(27UL)
#define D30 GPIO_GPA(26UL)
#define D31 GPIO_GPA(24UL)
#define D32 GPIO_GPA(25UL)
#define D33 GPIO_GPA(23UL)
#define D34 GPIO_GPA(22UL)
#define D35 GPIO_GPA(21UL)
#define D36 GPIO_GPA(20UL)
#define D37 GPIO_GPA(19UL)
#define D38 GPIO_GPK(13UL)
#define D39 GPIO_GPK(12UL)
#define D40 GPIO_GPK(11UL)
#define D41 GPIO_GPA(18UL)
#define D42 GPIO_GPA(17UL)
#define D43 GPIO_GPA(16UL)
#define D44 GPIO_GPA(11UL)
#define D45 GPIO_GPA(10UL)
#define D46 GPIO_GPA(09UL)
#define D47 GPIO_GPA(08UL)
#define D48 GPIO_GPA(05UL)
#define D49 GPIO_GPA(04UL)
#define D50 GPIO_GPA(03UL)
#define D51 GPIO_GPA(02UL)
#define D52 GPIO_GPA(01UL)
#define D53 GPIO_GPA(00UL)
#define D54 GPIO_GPK(14UL)
#define D55 GPIO_GPK(15UL)
#define D56 GPIO_GPK(01UL)
#define D57 GPIO_GPK(08UL)
#define D58 GPIO_GPB(07UL)
#define D59 GPIO_GPB(04UL)
#define D60 GPIO_GPB(06UL)
#defind D61 GPIO_GPB(05UL)

/* GPIO Pin mapping table for VCP-G */
static const uint32_t PIN_MAP[] = {
    GPIO_GPB(26UL), // D0
    GPIO_GPB(25UL), // D1
    GPIO_GPB(28UL), // D2
    GPIO_GPB(11UL), // D3
    GPIO_GPB(27UL), // D4
    GPIO_GPB(10UL), // D5
    GPIO_GPA(13UL), // D6
    GPIO_GPB(01UL), // D7
    GPIO_GPB(00UL), // D8
    GPIO_GPA(12UL), // D9
    GPIO_GPC(13UL), // D10
    GPIO_GPC(14UL), // D11
    GPIO_GPC(15UL), // D12
    GPIO_GPC(12UL), // D13
    GPIO_GPC(00UL), // D14
    GPIO_GPC(01UL), // D15
    GPIO_GPA(06UL), // D16
    GPIO_GPA(07UL), // D17
    GPIO_GPA(28UL), // D18
    GPIO_GPA(29UL), // D19
    GPIO_GPB(03UL), // D20
    GPIO_GPB(02UL), // D21
    GPIO_GPB(24UL), // D22
    GPIO_GPB(23UL), // D23
    GPIO_GPB(22UL), // D24
    GPIO_GPB(21UL), // D25
    GPIO_GPB(20UL), // D26
    GPIO_GPB(19UL), // D27
    GPIO_GPA(30UL), // D28
    GPIO_GPA(27UL), // D29
    GPIO_GPA(26UL), // D30
    GPIO_GPA(24UL), // D31
    GPIO_GPA(25UL), // D32
    GPIO_GPA(23UL), // D33
    GPIO_GPA(22UL), // D34
    GPIO_GPA(21UL), // D35
    GPIO_GPA(20UL), // D36
    GPIO_GPA(19UL), // D37
    GPIO_GPK(13UL), // D38
    GPIO_GPK(12UL), // D39
    GPIO_GPK(11UL), // D40
    GPIO_GPA(18UL), // D41
    GPIO_GPA(17UL), // D42
    GPIO_GPA(16UL), // D43
    GPIO_GPA(11UL), // D44
    GPIO_GPA(10UL), // D45
    GPIO_GPA(9UL), // D46
    GPIO_GPA(8UL), // D47
    GPIO_GPA(5UL), // D48
    GPIO_GPA(4UL), // D49
    GPIO_GPA(3UL), // D50
    GPIO_GPA(2UL), // D51
    GPIO_GPA(1UL), // D52
    GPIO_GPA(0UL), // D53
    GPIO_GPK(14UL), // D54
    GPIO_GPK(15UL), // D55
    GPIO_GPK(1UL), // D56
    GPIO_GPK(8UL), // D57
    GPIO_GPB(07UL), // D58
    GPIO_GPB(04UL), // D59
    GPIO_GPB(06UL), // D60
    GPIO_GPB(05UL), // D61
    GPIO_GPK(16UL) // LED_BUILTIN
};

#define LED_BUILTIN 62

#endif /* VARIANT_PIN_H */
