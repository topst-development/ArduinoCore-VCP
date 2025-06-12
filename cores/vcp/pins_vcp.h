/*
* hsj 20240603 test 
 */
#ifndef _PINS_VCP_H_
#define _PINS_VCP_H_

#define TCC7045

#ifdef TCC7045

/* VCP digital pins alias */
typedef enum{
  GPIO_A00,  
  GPIO_A01,  
  GPIO_A02,  
  GPIO_A03,  
  GPIO_A04,  
  GPIO_A05,  
  GPIO_A06,  
  GPIO_A07,  
  GPIO_A08,  
  GPIO_A09,
  GPIO_A10, 
  GPIO_A11, 
  GPIO_A12, 
  GPIO_A13, 
  GPIO_A14, 
  GPIO_A15, 
  GPIO_A16, 
  GPIO_A17, 
  GPIO_A18, 
  GPIO_A19,
  GPIO_A20, 
  GPIO_A21, 
  GPIO_A22, 
  GPIO_A23, 
  SPI3_SDO, 
  SPI3_SDI, 
  SPI3_CS, 
  SPI3_CLK,
  GPIO_A28,
  GPIO_A29, 
  GPIO_A30
} vcpDigitalPinA;

typedef enum{
  I2C0_SCL,  
  I2C0_SDA,  
  I2C1_SCL,  
  I2C1_SDA,  
  GPIO_B04,  
  GPIO_B05,  
  GPIO_B06,  
  GPIO_B07,  
  GPIO_B08,  
  GPIO_B09,
  GPIO_B10, 
  GPIO_B11, 
  GPIO_B12, 
  GPIO_B13, 
  GPIO_B14, 
  GPIO_B15, 
  GPIO_B16, 
  GPIO_B17, 
  GPIO_B18, 
  GPIO_B19,
  GPIO_B20, 
  GPIO_B21, 
  GPIO_B22, 
  GPIO_B23, 
  GPIO_B24, 
  GPIO_B25, 
  GPIO_B26, 
  GPIO_B27,
  GPIO_B28
} vcpDigitalPinB;

typedef enum{
  GPIO_C00,  
  GPIO_C01,  
  GPIO_C02,  
  GPIO_C03,  
  GPIO_AC04,  
  GPIO_AC05,  
  GPIO_C06,  
  GPIO_C07,  
  GPIO_C08,  
  GPIO_C09,
  GPIO_C10, 
  GPIO_C11, 
  SPI1_CLK, 
  SPI1_CS, 
  SPI1_DO, 
  SPI1_DI, 
  GPIO_C16, 
  GPIO_C17, 
  GPIO_C18, 
  GPIO_C19,
  GPIO_C20, 
  GPIO_C21
} vcpDigitalPinC;

typedef enum{
  GPIO_K00,  
  GPIO_K01,  
  GPIO_K02,  
  GPIO_K03,  
  GPIO_K04,  
  GPIO_K05,  
  GPIO_K06,  
  GPIO_K07,  
  GPIO_K08,  
  GPIO_K09,
  GPIO_K10, 
  GPIO_K11, 
  GPIO_K12, 
  GPIO_K13, 
  GPIO_K14, 
  GPIO_K15, 
  GPIO_K16, 
  GPIO_K17
} vcpDigitalPinK;

// typdef enum{
//   SPI3_SDO = 24, // GPIO_A24
//   SPI3_SDI,      // GPIO_A25
//   SPI3_CS,       // GPIO_A26
//   SPI3_CLK       // GPIO_A27
// } vcpCommPin;

#endif

uint8_t pinCFG(uint32_t vcpPin, uint32_t vcpPinM);
uint8_t pinSET(uint32_t vcpPin, uint32_t vcpValue);
int pinREAD(uint32_t vcpPin);

#endif /*_PINS_VCP_H_*/
