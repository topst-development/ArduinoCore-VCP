#ifndef ARDUINO_GENERIC_VARIANT_H
#define ARDUINO_GENERIC_VARIANT_H

#include <stdint.h>
#include "variant_pin.h"
#include "wiring_constants.h"
#include "gpio.h"

uint8_t pinCFG(uint32_t Pin, uint32_t PinMode)
{
    uint8_t ret;
    uint32_t GPIO_PinMode;
    /* INPUT: 0, OUTPUT: 1 INPUT_PULLUP: 2 */
    if (PinMode == INPUT_PULLUP)
    {
        GPIO_PinMode = GPIO_INPUT | GPIO_INPUTBUF_EN | GPIO_PULLUP;
    } else if (PinMode == INPUT)
    {
        GPIO_PinMode = GPIO_INPUT | GPIO_INPUTBUF_EN | GPIO_NOPULL;
    } else if (PinMode == OUTPUT)
    {
        GPIO_PinMode = GPIO_OUTPUT;
    }

    ret = (uint8_t)GPIO_Config(PIN_MAP[Pin], GPIO_PinMode);
    if (ret != 0) // 0 means success, so return 0 if failed
    {
        return 0;
    }

    return ret;
}

uint8_t pinSET(uint32_t Pin, uint32_t Value)
{
    uint8_t ret;

    ret = (uint8_t)GPIO_Set(PIN_MAP[Pin], Value);
    if (ret != 0) // not 0 means failed, so return FALSE
    {
        return FALSE;
    } else {
        return TRUE;
    }
}

int pinREAD(uint32_t Pin)
{
    uint8_t level = 0;

    level = GPIO_Get(PIN_MAP[Pin]);

    return (level) ? HIGH : LOW;
}

#endif /* ARDUINO_GENERIC_VARIANT_H */
