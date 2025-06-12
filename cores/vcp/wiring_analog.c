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

#include "Arduino.h"
#include "debug.h"
#include "adc.h"

#include "pdm.h"

#ifdef __cplusplus
extern "C" {
#endif

static bool isPdmInitialized = false;
static bool isAdcInitialized = false;

#if defined(BOARD_VCP_G)
/* ADC channel and module information */
typedef struct {
  uint32_t ulPin;
  uint32_t ulModule;
  uint32_t ulChannel;
} sAnalogPinInfo_t;

/* table of analog pin information for VCP_G */
sAnalogPinInfo_t sAnalogPinInfo[32] = {
  { A00, ADC_MODULE_0, 3 },
  { A01, ADC_MODULE_0, 4 },
  { A02, ADC_MODULE_1, 2 },
  { A03, ADC_MODULE_1, 3 },
  { A04, ADC_MODULE_1, 5 },
  { A05, ADC_MODULE_1, 4 },
  { A06, ADC_MODULE_0, 5 },
  { A07, ADC_MODULE_0, 1 },
  { A08, ADC_MODULE_1, 8 },
  { A09, ADC_MODULE_1, 9 },
  { A10, ADC_MODULE_1, 10 },
  { A11, ADC_MODULE_0, 2 },
  { AREF, ADC_MODULE_0, 6 }
};
#endif

/* get pdm channel number from pin number
  @param ulPin: The analog pin number to read from
  @return: pdm channel number (0~8)
*/
uint32_t pinToPdmChannel(uint32_t ulPin) {
  /* search pdm channel number from pin number */
  for (uint32_t i = 0; i < PDM_TOTAL_CHANNELS; i++) {
    if (sPdmChannelConfig[i].pin == ulPin) {
      return sPdmChannelConfig[i].portNum;
    }
  }
}

/* 
  get pdm port channel number from pin number
  @param ulPin: The analog pin number to read from
  @return: pdm port channel number (0~3) (GPIO-A, GPIO-B, GPIO-C, GPIO-K)
*/
uint32_t pinToPdmPortCh(uint32_t ulPin) {
  /* search pdm port channel number from pin number */
  for (uint32_t i = 0; i < PDM_TOTAL_CHANNELS; i++) {
    if (sPdmChannelConfig[i].pin == ulPin) {
      return sPdmChannelConfig[i].portSelCh;
    }
  }
}

/*
  Performs an analog read operation on the specified pin.
  This function handles both the initialization and reading of the analog pin.
  
  @param ulPin: The analog pin number to read from
  @return: 12-bit ADC conversion value (0-4095)
*/
uint32_t analogRead(uint32_t ulPin)
{
  uint32_t value = 0;

  if (!isAdcInitialized) {
    ADC_Init(ADC_MODE_NORMAL, ADC_MODULE_0);
    ADC_Init(ADC_MODE_NORMAL, ADC_MODULE_1);
    isAdcInitialized = true;
  }

  #if defined(BOARD_VCP_B)
  switch (ulPin)
  {
    case 16: value = 0xFFF & ADC_Read(4UL, ADC_MODULE_1, 0);
             break;
    case 18: value = 0xFFF & ADC_Read(5UL, ADC_MODULE_1, 0);
             break;
    default:
      break;
  }
  #elif defined(BOARD_VCP_G)
  if (ulPin >= ANALOG_PIN_BASE) {
    uint32_t i = ulPin - ANALOG_PIN_BASE;
    value = 0xFFF & ADC_Read(sAnalogPinInfo[i].ulChannel, sAnalogPinInfo[i].ulModule, 0);
  }
  #endif

  return value;
}

void analogWrite(uint32_t ulPin, uint32_t ulValue)
{
  uint32_t uiDutyRate = ulValue;
  PDMModeConfig_t sModeConfigInfo = {0,};
  SALRetCode_t ret = SAL_RET_FAILED;

  if (!isPdmInitialized) {
    PDM_Init();
    isPdmInitialized = true;
  }

  /* get pdm channel number from pin number */
  uint32_t uiChannel = pinToPdmChannel(ulPin);

  /* get pdm port channel number from pin number */
  uint32_t uiPortCh = pinToPdmPortCh(ulPin);

  sModeConfigInfo.mcOperationMode  = PDM_OUTPUT_MODE_PHASE_1;
  sModeConfigInfo.mcPortNumber     = uiPortCh;
  sModeConfigInfo.mcPeriodNanoSec1 = 1000UL * 1000UL;
  sModeConfigInfo.mcDutyNanoSec1   = uiDutyRate * 1000UL;

  ret = PDM_SetConfig(uiChannel, &sModeConfigInfo);

  if(ret == SAL_RET_SUCCESS)
  {
    PDM_Enable(uiChannel, PMM_ON);
  }
}


#ifdef __cplusplus
}
#endif
