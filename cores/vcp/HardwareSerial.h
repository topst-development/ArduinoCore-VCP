/*

*/

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <inttypes.h>

#include "Stream.h"
#include "uart.h"

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

// // typedef struct _HwSerialOpen
// // {
// //   uint8        ucCh;
// //   uint32       uiPriority;
// //   uint32       uiBaudrate;
// //   uint8        ucMode;
// //   uint8        ucCtsRts;
// //   uint8        ucPortCfg;
// //   uint8        ucWordLength;
// //   uint8        ucFIFO;
// //   uint8        uc2StopBit;
// //   uint8        ucParity;
// //   GICIsrFunc   fnCallback;
// // } HwSerialOpen;

// // extern HwSerialOpen hwSerialOpen;

class HardwareSerial : public Stream  
{
  private:
    uint8_t ucCh;
    uint32_t uiBaudrate;

    //pseudo code : print.h
    size_t printNumber(unsigned long, uint8_t);
    size_t printULLNumber(unsigned long long, uint8_t);
    template <class T>
    size_t printFloat(T, uint8_t);

  public:
    HardwareSerial();
    //HardwareSerial(uint8_t Ch, uint32_t Baudrate);
    void begin(uint32_t baud);
    void end();
    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    virtual int availableForWrite(void);
    virtual void flush(uint32_t timeout = 0);
    // pseudo code 
    virtual size_t write(uint8_t);
    inline size_t write(unsigned long n)
    {
      return write((uint8_t)n);
    }
    inline size_t write(long n)
    {
      return write((uint8_t)n);
    }
    inline size_t write(unsigned int n)
    {
      return write((uint8_t)n);
    }
    inline size_t write(int n)
    {
      return write((uint8_t)n);
    }
    size_t write(const uint8_t *buffer, size_t size);
    // pseudo code 
    using Print::write; // pull in write(str) from Print
    operator bool()
    {
      return true;
    }

    //size_t write(uint8_t c);
    size_t write(const char *);

    //size_t print(uint8_t);


    size_t print(const __FlashStringHelper *);
    size_t print(const String &);
    size_t print(const char *);
    size_t print(char);
    size_t print(unsigned char, int = DEC);    
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(float, int = 2);

    size_t println(void);
    size_t println(uint8_t);
    size_t println(const char[]);
    size_t println(int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(float, int = 2);
    
    // void print();
};

extern HardwareSerial Serial;

// #if defined(USART1)
//   extern HardwareSerial Serial1;
// #endif
// #if defined(USART2)
//   extern HardwareSerial Serial2;
// #endif
// #if defined(USART3)
//   extern HardwareSerial Serial3;
// #endif


#endif
