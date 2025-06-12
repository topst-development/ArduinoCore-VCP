/*
  Arduino.h - Main include file for the Arduino SDK
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef Arduino_h
#define Arduino_h

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// #include "variant.h"

// Standard Arduino data types
typedef uint8_t byte;
typedef bool boolean;

// Analog reference types
#define DEFAULT 1
#define EXTERNAL 0

#include "WMath.h"

//#include "HardwareSerial.h"

#include "pins_arduino.h"
#include "wiring.h"



void yield( void ) ;

/* system functions */
//int main( void );
void cmain(void); 

/* sketch */
void setup( void ) ;
void loop( void ) ;

// Prototypes for timing functions
unsigned long millis(void);
unsigned long micros(void);
void delay(unsigned long);
void delayMicroseconds(unsigned int us);

// Serial event run handler
extern void serialEventRun(void);

#ifdef __cplusplus
}
#endif

// Include other core headers as needed

#endif // Arduino_h
