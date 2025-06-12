/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _WIRING_ANALOG_
#define _WIRING_ANALOG_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(BOARD_VCP_G)
#define ANALOG_PIN_BASE 1000
#define A00 (ANALOG_PIN_BASE + 0)
#define A01 (ANALOG_PIN_BASE + 1)
#define A02 (ANALOG_PIN_BASE + 2)
#define A03 (ANALOG_PIN_BASE + 3)
#define A04 (ANALOG_PIN_BASE + 4)
#define A05 (ANALOG_PIN_BASE + 5)
#define A06 (ANALOG_PIN_BASE + 6)
#define A07 (ANALOG_PIN_BASE + 7)
#define A08 (ANALOG_PIN_BASE + 8)
#define A09 (ANALOG_PIN_BASE + 9)
#define A10 (ANALOG_PIN_BASE + 10)
#define A11 (ANALOG_PIN_BASE + 11)
#define AREF (ANALOG_PIN_BASE + 12)

#define A0  A00
#define A1  A01
#define A2  A02
#define A3  A03
#define A4  A04
#define A5  A05
#define A6  A06
#define A7  A07
#define A8  A08
#define A9  A09
#endif

typedef struct {
  uint32_t pin;
  uint32_t portNum;
  uint32_t portSelCh;
} PDMChannelConfig_t;

#define PDM_TOTAL_CHANNELS 9

#if defined(BOARD_VCP_B)
static PDMChannelConfig_t sPdmChannelConfig[PDM_TOTAL_CHANNELS] = {
  { 11, 0, GPIO_PERICH_CH1 },  // PWM_CH[00] -> arduino pin 11 -> pdm channel 0, channel 1(GPIO-B)
  { 22, 1, GPIO_PERICH_CH3 },  // PWM_CH[01] -> arduino pin 22 -> pdm channel 1, channel 3(GPIO-K)
  { 0, 0, 0 },                 // PWM_CH[02] -> NA
  { 15, 3, GPIO_PERICH_CH3 },  // PWM_CH[03] -> arduino pin 33 -> pdm channel 2, channel 2(GPIO-C)
  { 0, 0, 0 },                 // PWM_CH[04] -> NA
  { 33, 5, GPIO_PERICH_CH2 },  // PWM_CH[05] -> arduino pin 33 -> pdm channel 3, channel 2(GPIO-C)
  { 40, 6, GPIO_PERICH_CH2 },  // PWM_CH[06] -> arduino pin 40 -> pdm channel 4, channel 2(GPIO-C)
  { 26, 7, GPIO_PERICH_CH2 },  // PWM_CH[07] -> arduino pin 26 -> pdm channel 5, channel 2(GPIO-C)
  { 38, 8, GPIO_PERICH_CH2 },  // PWM_CH[08] -> arduino pin 38 -> pdm channel 6, channel 2(GPIO-C)
};
#elif defined(BOARD_VCP_G)
static PDMChannelConfig_t sPdmChannelConfig[PDM_TOTAL_CHANNELS] = {
  { 45, 0, GPIO_PERICH_CH0 },  // PWM_CH[00] -> arduino pin 45 -> pdm channel 0, channel 0(GPIO-A)
  { 44, 1, GPIO_PERICH_CH0 },  // PWM_CH[01] -> arduino pin 44 -> pdm channel 1, channel 0(GPIO-A)
  { 9,  2, GPIO_PERICH_CH0 },  // PWM_CH[02] -> arduino pin 9 -> pdm channel 2, channel 0(GPIO-A)
  { 6,  3, GPIO_PERICH_CH0 },  // PWM_CH[03] -> arduino pin 6 -> pdm channel 3, channel 0(GPIO-A)
  { 5,  4, GPIO_PERICH_CH1 },  // PWM_CH[04] -> arduino pin 5 -> pdm channel 4, channel 1(GPIO-B)
  { 3,  5, GPIO_PERICH_CH1 },  // PWM_CH[05] -> arduino pin 3 -> pdm channel 5, channel 1(GPIO-B)
  { 43, 6, GPIO_PERICH_CH0 },  // PWM_CH[06] -> arduino pin 43 -> pdm channel 6, channel 0(GPIO-A)
  { 42, 7, GPIO_PERICH_CH0 },  // PWM_CH[07] -> arduino pin 42 -> pdm channel 7, channel 0(GPIO-A)
  { 41, 8, GPIO_PERICH_CH0 },  // PWM_CH[08] -> arduino pin 41 -> pdm channel 8, channel 0(GPIO-A)
};
#endif

/*
 * \brief Reads the value from the specified analog pin.
 *
 * \param ulPin
 *
 * \return Read value from selected pin, if no error.
 */
extern uint32_t analogRead(uint32_t ulPin) ;

/*
 * \brief Writes an analog value (PWM wave) to a pin.
 *
 * \param ulPin
 * \param ulValue
 */
extern void analogWrite(uint32_t ulPin, uint32_t ulValue) ;

#ifdef __cplusplus
}
#endif

#endif /* _WIRING_ANALOG_ */
