#include "SWire.h"

SoftWire::SoftWire() {
}

void SoftWire::i2cDelay() {
  uint32_t i;
  for(i = 0; i < 4000; i++)
  {
      asm("nop");
  }
}

void SoftWire::sclHi() {
  #if defined(BOARD_VCP_B)
  GPIO_Config(GPIO_GPB(2UL), (GPIO_FUNC(0UL) | GPIO_INPUTBUF_EN | GPIO_PULLUP));
  #elif defined(BOARD_VCP_G)
  /* VCP_G use GPIO_GPA(4UL) as SCL and GPIO_GPA(5UL) as SDA */
  GPIO_Config(GPIO_GPA(4UL), (GPIO_FUNC(0UL) | GPIO_INPUTBUF_EN | GPIO_PULLUP));
  GPIO_Config(GPIO_GPA(24UL), (GPIO_FUNC(0UL) | GPIO_INPUTBUF_EN | GPIO_PULLUP));
  GPIO_Config(GPIO_GPB(0UL), (GPIO_FUNC(0UL) | GPIO_INPUTBUF_EN | GPIO_PULLUP));
  GPIO_Config(GPIO_GPB(2UL), (GPIO_FUNC(0UL) | GPIO_INPUTBUF_EN | GPIO_PULLUP));
  GPIO_Config(GPIO_GPC(4UL), (GPIO_FUNC(0UL) | GPIO_INPUTBUF_EN | GPIO_PULLUP));
  GPIO_Config(GPIO_GPC(7UL), (GPIO_FUNC(0UL) | GPIO_INPUTBUF_EN | GPIO_PULLUP));
  #else
  #error BOARD_VCP_B or BOARD_VCP_G is not defined
  #endif
}

void SoftWire::sdaHi() {
  #if defined(BOARD_VCP_B)
  GPIO_Config(GPIO_GPB(3UL), (GPIO_FUNC(0UL) | GPIO_INPUTBUF_EN | GPIO_PULLUP));
  #elif defined(BOARD_VCP_G)
  /* VCP_G use GPIO_GPA(5UL) as SDA */
  GPIO_Config(GPIO_GPA(5UL), (GPIO_FUNC(0UL) | GPIO_INPUTBUF_EN | GPIO_PULLUP));
  GPIO_Config(GPIO_GPA(26UL), (GPIO_FUNC(0UL) | GPIO_INPUTBUF_EN | GPIO_PULLUP));
  GPIO_Config(GPIO_GPB(1UL), (GPIO_FUNC(0UL) | GPIO_INPUTBUF_EN | GPIO_PULLUP));
  GPIO_Config(GPIO_GPB(3UL), (GPIO_FUNC(0UL) | GPIO_INPUTBUF_EN | GPIO_PULLUP));
  GPIO_Config(GPIO_GPC(5UL), (GPIO_FUNC(0UL) | GPIO_INPUTBUF_EN | GPIO_PULLUP));
  GPIO_Config(GPIO_GPC(6UL), (GPIO_FUNC(0UL) | GPIO_INPUTBUF_EN | GPIO_PULLUP));
  #else
  #error BOARD_VCP_B or BOARD_VCP_G is not defined
  #endif
}

void SoftWire::sclLo() {
  #if defined(BOARD_VCP_B)
  GPIO_Set(GPIO_GPB(2UL), 0UL);
  GPIO_Config(GPIO_GPB(2UL), (GPIO_FUNC(0UL) | GPIO_OUTPUT));
  #elif defined(BOARD_VCP_G)
  /* VCP_G use GPIO_GPA(4UL) as SCL */
  GPIO_Set(GPIO_GPA(4UL), 0UL);
  GPIO_Config(GPIO_GPA(4UL), (GPIO_FUNC(0UL) | GPIO_OUTPUT));
  GPIO_Set(GPIO_GPA(24UL), 0UL);
  GPIO_Config(GPIO_GPA(24UL), (GPIO_FUNC(0UL) | GPIO_OUTPUT));
  GPIO_Set(GPIO_GPB(0UL), 0UL);
  GPIO_Config(GPIO_GPB(0UL), (GPIO_FUNC(0UL) | GPIO_OUTPUT));
  GPIO_Set(GPIO_GPB(2UL), 0UL);
  GPIO_Config(GPIO_GPB(2UL), (GPIO_FUNC(0UL) | GPIO_OUTPUT));
  GPIO_Set(GPIO_GPC(4UL), 0UL);
  GPIO_Config(GPIO_GPC(4UL), (GPIO_FUNC(0UL) | GPIO_OUTPUT));
  GPIO_Set(GPIO_GPC(7UL), 0UL);
  GPIO_Config(GPIO_GPC(7UL), (GPIO_FUNC(0UL) | GPIO_OUTPUT));
  #else
  #error BOARD_VCP_B or BOARD_VCP_G is not defined
  #endif
}

void SoftWire::sdaLo() {
  #if defined(BOARD_VCP_B)
  GPIO_Set(GPIO_GPB(3UL), 0UL);
  GPIO_Config(GPIO_GPB(3UL), (GPIO_FUNC(0UL) | GPIO_OUTPUT));
  #elif defined(BOARD_VCP_G)
  /* VCP_G use GPIO_GPA(5UL) as SDA */
  GPIO_Set(GPIO_GPA(5UL), 0UL);
  GPIO_Config(GPIO_GPA(5UL), (GPIO_FUNC(0UL) | GPIO_OUTPUT));
  GPIO_Set(GPIO_GPA(26UL), 0UL);
  GPIO_Config(GPIO_GPA(26UL), (GPIO_FUNC(0UL) | GPIO_OUTPUT));
  GPIO_Set(GPIO_GPB(1UL), 0UL);
  GPIO_Config(GPIO_GPB(1UL), (GPIO_FUNC(0UL) | GPIO_OUTPUT));
  GPIO_Set(GPIO_GPB(3UL), 0UL);
  GPIO_Config(GPIO_GPB(3UL), (GPIO_FUNC(0UL) | GPIO_OUTPUT));
  GPIO_Set(GPIO_GPC(5UL), 0UL);
  GPIO_Config(GPIO_GPC(5UL), (GPIO_FUNC(0UL) | GPIO_OUTPUT));
  GPIO_Set(GPIO_GPC(6UL), 0UL);
  GPIO_Config(GPIO_GPC(6UL), (GPIO_FUNC(0UL) | GPIO_OUTPUT));
  #else
  #error BOARD_VCP_B or BOARD_VCP_G is not defined
  #endif
}

void SoftWire::start() {
  sdaHi();
  sclHi();
  i2cDelay();
  sdaLo();
  i2cDelay();
  sclLo();
  i2cDelay();
}

void SoftWire::stop() {
  sdaLo();
  sclLo();
  i2cDelay();
  sclHi();
  i2cDelay();
  sdaHi();
  i2cDelay();
}

void SoftWire::begin() {
  //SAL_WriteReg(0xC, 0xA0F22044); // hsj I2C1 - GPIOB 2/3
  sdaHi();
  sclHi();
}

void SoftWire::begin(uint8_t sdaPin, uint8_t sclPin) {
  data_pin = sdaPin;
  clock_pin = sclPin;
  sdaHi();
  sclHi();
}

void SoftWire::clockPulse() {
  sclHi();
  i2cDelay();
  sclLo();
}

int SoftWire::writeByte(uint8_t data_byte) {
  for (uint8_t i = 0; i < 8; i++) {
    if (bitRead(data_byte, 7 - i)) {
      sdaHi(); // write 1
    } else {
      sdaLo(); // write 0
    }
    i2cDelay();
    clockPulse();
  }
  return readAck();
}

uint8_t SoftWire::readBit() {
  uint8_t out_bit;

  sclHi();
  i2cDelay();
  #if defined(BOARD_VCP_B)
  out_bit = GPIO_Get(GPIO_GPB(3UL)); 
  #elif defined(BOARD_VCP_G)
  /* VCP_G use GPIO_GPA(5UL) as SDA */
  uint8_t out_bit_a5  = GPIO_Get(GPIO_GPA(5UL)); 
  uint8_t out_bit_a26 = GPIO_Get(GPIO_GPA(26UL)); 
  uint8_t out_bit_b3  = GPIO_Get(GPIO_GPB(3UL)); 
  uint8_t out_bit_b1  = GPIO_Get(GPIO_GPB(1UL)); 
  uint8_t out_bit_c6  = GPIO_Get(GPIO_GPC(6UL)); 
  uint8_t out_bit_c5  = GPIO_Get(GPIO_GPC(5UL)); 
  out_bit = out_bit_a5 & out_bit_a26 & out_bit_b3 & out_bit_b1 & out_bit_c6 & out_bit_c5;
  #else
  #error BOARD_VCP_B or BOARD_VCP_G is not defined
  #endif
  sclLo();
  i2cDelay();
  return out_bit;
}

uint8_t SoftWire::readByte() {
  uint8_t out_byte = 0;

  sdaHi();
  i2cDelay();
  for (uint8_t i = 0; i < 8; i++) {
    bitWrite(out_byte, 7 - i, readBit());
  }

  return out_byte;
}

/**
 * Return 0 if ACK was received, else 1
 */
uint8_t SoftWire::readAck() {
  sdaHi();
  sclHi();
  i2cDelay();
  #if defined(BOARD_VCP_B)
  uint8_t result =GPIO_Get(GPIO_GPB(3UL)); 
  #elif defined(BOARD_VCP_G)
  /* VCP_G use GPIO_GPA(5UL) as SDA */
  uint8_t result_a5  = GPIO_Get(GPIO_GPA(5UL));
  uint8_t result_a26 = GPIO_Get(GPIO_GPA(26UL)); 
  uint8_t result_b3  = GPIO_Get(GPIO_GPB(3UL)); 
  uint8_t result_b1  = GPIO_Get(GPIO_GPB(1UL)); 
  uint8_t result_c6  = GPIO_Get(GPIO_GPC(6UL)); 
  uint8_t result_c5  = GPIO_Get(GPIO_GPC(5UL)); 
  uint8_t result = result_a5 & result_a26 & result_b3 & result_b1 & result_c6 & result_c5;
  #else
  #error BOARD_VCP_B or BOARD_VCP_G is not defined
  #endif
  sclLo();
  sdaLo();
  i2cDelay();
  return result;
}

/**
 * Return 0 if NACK was received, else 1
 */
uint8_t SoftWire::readNack() {
  sdaHi();
  return readBit() == 1 ? 0 : 1;
}

void SoftWire::sendAck() {
  sdaLo();
  i2cDelay();
  clockPulse();
  i2cDelay();
}

void SoftWire::sendNack() {
  sdaHi();
  i2cDelay();
  clockPulse();
  sdaLo();
  i2cDelay();
}

// uint8_t SoftWire::doStartWriteAckStop(uint8_t data_byte) {
//   start();
//   if (writeByte(data_byte)) {
//     return 1;
//   }
//   stop();
//   return 0;
// }

// uint8_t SoftWire::doStartWriteAckStop(uint8_t data_bytes[], uint8_t data_length) {
//   start();
//   for (uint8_t i = 0; i < data_length; i++) {
//     if(writeByte(data_bytes[i])) {
//       stop();
//       return 1;
//     }
//   }
//   stop();
//   return 0;
// }

void SoftWire::beginTransmission(uint8_t address) {
  current_rw_address = address;
}

void SoftWire::beginTransmission(int address) {
  beginTransmission((uint8_t)address);
}

uint8_t SoftWire::endTransmission(void) {
  return endTransmission(true);
}

uint8_t SoftWire::endTransmission(uint8_t shouldStop) {
  start();
  if (writeByte(((current_rw_address << 1) | 0) & 0xFF)) {
    stop();
    return 2;
  }
  for (int i=0; i<txBufferIndex; i++) {
    if (writeByte(txBuffer[i])) {
      txBufferIndex = 0;
      stop();
      return 3;
    }
  }
  txBufferIndex = 0;
  stop();
  return 0;
}

uint8_t SoftWire::requestFrom(uint8_t address, uint8_t numBytes) {
  rxBufferIndex = 0;
  rxBufferReadIndex = 0;
  start();
  current_rw_address = address;
  if (writeByte(((current_rw_address << 1) | 1) & 0xFF)) {
    stop();
    return 0;
  }

  if (numBytes > BufferLength) {
    numBytes = BufferLength;
  }

  for (int i=0; i<numBytes; i++) {
    rxBuffer[i] = readByte();
    rxBufferIndex ++;
    if (i != numBytes - 1) {
      sendAck();
    } else {
      sendNack();
    }
  }

  stop();
  return numBytes;
}

uint8_t SoftWire::requestFrom(int address, int numBytes) {
  return requestFrom((uint8_t)address, (uint8_t)numBytes);
}

int SoftWire::available() {
  return rxBufferIndex;
}

int SoftWire::write(uint8_t value) {
  if (txBufferIndex == BufferLength) {
    return 0;
  }
  txBuffer[txBufferIndex] = value;
  txBufferIndex += 1;
  return 1;
}

int SoftWire::write(int value) {
  return write((uint8_t)value);
}

int SoftWire::read() {
  if (rxBufferReadIndex > BufferLength) {
    return -1;
  } else if (rxBufferReadIndex > rxBufferIndex) {
    return -1;
  }
  int value = rxBuffer[rxBufferReadIndex];
  rxBufferReadIndex ++;
  return value;
}

SoftWire SWire = SoftWire();
