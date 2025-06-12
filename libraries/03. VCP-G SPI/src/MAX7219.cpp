/*
	MAX7219.cpp - Library for MAX7219-driven 8*8 LED-Matrix.
	Copyright (C) by Michael Kubina, February 14, 2019.

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Arduino.h"
#include "MAX7219.h"

#include "wiring_private.h"

uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
	uint8_t value = 0;
	uint8_t i;

	for (i = 0; i < 8; ++i) {
		digitalWrite(clockPin, HIGH);
		if (bitOrder == LSBFIRST)
			value |= digitalRead(dataPin) << i;
		else
			value |= digitalRead(dataPin) << (7 - i);
		digitalWrite(clockPin, LOW);
	}
	return value;
}

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{
	uint8_t i;

	for (i = 0; i < 8; i++)  {
		if (bitOrder == LSBFIRST) {
			digitalWrite(dataPin, val & 1);
			//GPIO_Set(GPIO_GPC(dataPin), val & 1);
			val >>= 1;
		} else {	
			digitalWrite(dataPin, (val & 128) != 0);
			//GPIO_Set(GPIO_GPC(dataPin), (val & 128) != 0);
			val <<= 1;
		}
			
		digitalWrite(clockPin, HIGH);
		//GPIO_Set(GPIO_GPC(clockPin), HIGH);
		digitalWrite(clockPin, LOW);		
		//GPIO_Set(GPIO_GPC(clockPin), LOW);
	}
}


// OPCODES

#define NO_OP			0
#define DIGIT_0			1
#define DIGIT_1			2
#define DIGIT_2			3
#define DIGIT_3			4
#define DIGIT_4			5
#define DIGIT_5			6
#define DIGIT_6			7
#define DIGIT_7			8
#define DECODE_MODE		9
#define INTENSITY		10
#define SCAN_LIMIT		11
#define SHUTDOWN		12
#define DISPLAY_TEST	15

/*
 *	Constructor
 */

MAX7219::MAX7219(byte devices, byte dataPin, byte clockPin, byte loadPin)
{
	if(devices > 0)
	{
		_maxDevices = devices;
		
		_dataPin = dataPin;
		_clockPin = clockPin;
		_loadPin = loadPin;
		
		pinMode(_dataPin, OUTPUT);
		pinMode(_clockPin, OUTPUT);
		pinMode(_loadPin, OUTPUT);
		//GPIO_Config(GPIO_GPC(_dataPin), (GPIO_FUNC(0UL) | GPIO_OUTPUT));
        //GPIO_Config(GPIO_GPC(_clockPin), (GPIO_FUNC(0UL) | GPIO_OUTPUT));
		//GPIO_Config(GPIO_GPC(_loadPin), (GPIO_FUNC(0UL) | GPIO_OUTPUT));
		
		/* 	On initial power-up, all control registers are reset, the
			display is blanked, and the MAX7219/MAX7221 enter
			shutdown mode. Program the display driver prior to
			display use. Otherwise, it will initially be set to scan one
			digit, it will not decode data in the data registers, and
			the intensity register will be set to its minimum value.
		*/
		
		for(int device = 1; device <= _maxDevices; device++)
		{
			// force all SEGMENTS of all DIGITS to be turned off
			clearDisplay(device);
			
			// deactivate DECODE_MODE for all digits
			setDecodeMode(device, 0b00000000);
			
			// set INTENSITY to lowest value
			setIntensity(device, 1);
			
			// set SCAN_LIMIT to scan through all DIGITS
			setScanLimit(device, 7);
			
			// leave SHUTDOWN
			setShutdown(device, false);
			
			// deactivate DISPLAY_TEST if any
			setDisplayTest(device, false);
		}
	}
};

/*
 *	Send data to the MAX7219 driver
 */

void MAX7219::_sendData(byte device, byte registerAddress, byte registerData)
{
	// check if within limit
	if(device <= _maxDevices)
	{
		// enable data-transfer
		digitalWrite(_loadPin, LOW);
		//GPIO_Set(GPIO_GPC(_loadPin), LOW);
		
		//for(int i = 1; i <= _maxDevices; i++)
		for(int i = _maxDevices; i >= 1; i--)
		{
			if(i == device)
			{
				// shift out data - 8Bit Opcode, 8Bit Value
				shiftOut(_dataPin, _clockPin, MSBFIRST, (byte) registerAddress);
				shiftOut(_dataPin, _clockPin, MSBFIRST, (byte) registerData);
			}
			else
			{
				// shift out no-op-code
				shiftOut(_dataPin, _clockPin, MSBFIRST, (byte) NO_OP);
				shiftOut(_dataPin, _clockPin, MSBFIRST, (byte) 0);
			}
		}
		
		// disable data-transfer
		digitalWrite(_loadPin, HIGH);
		//GPIO_Set(GPIO_GPC(_loadPin), HIGH);
	}
};

/*
 *	Set all LEDs to OFF for the given Device
 */

void MAX7219::clearDisplay(byte device)
{
	// check if within limit
	if(device <= _maxDevices)
	{
		for(int row = 0; row <= 7; row++)
		{
			// keep track of clearing the display
			_ledState[device][row] = 0;
			
			// push data to display
			_sendData(device, row+1, _ledState[device][row]);
		}
	}
};

void MAX7219::clearAllDisplays()
{
	for(int i = 1; i <= _maxDevices; i++)
	{
		clearDisplay(i);
	}
};

/*
 *	Set all LEDs to ON for the given Device
 */
 
void MAX7219::fillDisplay(byte device)
{
	// check if within limit
	if(device <= _maxDevices)
	{
		for(int row = 0; row <= 7; row++)
		{
			// keep track of filling the display
			_ledState[device][row] = 1;
			
			// push data to display
			_sendData(device, row+1, _ledState[device][row]);
		}
	}
};

void MAX7219::fillAllDisplays()
{
	for(int i = 1; i <= _maxDevices; i++)
	{
		fillDisplay(i);
	}
};

/*
 *	Toggle the state of all LEDs
 */
 
void MAX7219::invertDisplay(byte device)
{
	// check if within limit
	if(device <= _maxDevices)
	{
		for(int row = 0; row <= 7; row++)
		{
			// keep track of inverting each LED
			_ledState[device][row] = ~_ledState[device][row];
			
			// push data to display
			_sendData(device, row+1, _ledState[device][row]);
		}
	}
};

void MAX7219::invertAllDisplays()
{
	for(int i = 1; i <= _maxDevices; i++)
	{
		invertDisplay(i);
	}
};

/*
 *	Set the state of a specific LED for the given Device
 */
 
void MAX7219::setLed(byte device, byte column, byte row, bool state)
{
	// check if within limit
	if(device <= _maxDevices)
	{
		if((column >= 0 && column <= 7) && (row >= 0 && row <= 7))
		{
			byte value = 0b10000000 >> column;
			if(state)
			{
				_ledState[device][row] = _ledState[device][row]|value;
			}
			else
			{
				value = ~value;
				_ledState[device][row] = _ledState[device][row]&value;
			}
			_sendData(device, row+1, _ledState[device][row]);
		}
	}
};

void MAX7219::invertLed(byte device, byte column, byte row)
{
	bool currentLedState = getLed(device, column, row);
	setLed(device, column, row, ~currentLedState);
};

/*
 *	Get the state of a specific LED for the given Device
 */

bool MAX7219::getLed(byte device, byte column, byte row)
{
	// check if within limit
	if(device <= _maxDevices)
	{
		if((column >= 0 && column <= 7) && (row >= 0 && row <= 7))
		{
			// get the state of the specified row
			byte currentRowState = getRow(device, row);
			
			// mask for single bit value
			byte mask = 0b00000001 << 7-column;
			byte state = currentRowState & mask;
			state = state >> 7-column;
			
			return (bool)state;
		}
	}
};

/*
 *	Set the state of all LEDs on a specific row for the given Device
 */

void MAX7219::setRow(byte device, byte row, byte value)
{
	// check if within limit
	if(device <= _maxDevices)
	{
		if(row >= 0 && row <= 7)
		{
			// keep track of LED-State
			_ledState[device][row] = value;
			_sendData(device, row+1, _ledState[device][row]);
		}
	}
};

void MAX7219::invertRow(byte device, byte row)
{
	byte currentRowState = getRow(device, row);
	setRow(device, row, ~currentRowState);
};

/*
 *	Get the state of all LEDs on a specific row for the given Device
 */

byte MAX7219::getRow(byte device, byte row)
{
	return _ledState[device][row];
};

/*
 *	Set the state of all LEDs on a specific column for the given Device
 */

void MAX7219::setColumn(byte device, byte column, byte value)
{
	// check if within limit
	if(device <= _maxDevices)
	{
		if(column >= 0 && column <= 7)
		{
			for(int row = 0; row <= 7; row++)
			{
				// mask the value
				byte mask = 0b00000001 << 7-row;
				byte masked_value = value & mask;
				byte bit = masked_value >> 7-row;
				
				setLed(device, column, row, (bool)bit);
			}
		}
	}
};

void MAX7219::invertColumn(byte device, byte column)
{
	byte currentColumnState = getColumn(device, column);
	setColumn(device, column, ~currentColumnState);
};

/*
 *	Get the state of all LEDs on a specific column for the given Device
 */

byte MAX7219::getColumn(byte device, byte column)
{
	// check if within limit
	if(device <= _maxDevices)
	{
		if(column >= 0 && column <= 7)
		{
			byte currentColumnState = 0b00000000;
			
			for(int row = 0; row <= 7; row++)
			{
				// get the current row state
				byte currentRowState = getRow(device, row);
				
				// mask for bit value in column
				byte mask = 0b00000001 << 7-column;
				byte masked_value = currentRowState & mask;
				masked_value = masked_value >> 7-column;
				
				// shift left to match bit position of columnstate
				masked_value = masked_value << 7-row;
				
				// XOR for position in column
				currentColumnState = currentColumnState ^ masked_value;
			}
			return currentColumnState;
		}
	}
};

/*
 *	Specify wether the MAX7219 driver uses its internal BCD-Codes
 *	or individual segment-control for all digits of the given device
 *
 *	OPCODE 0x09
 */

void MAX7219::setDecodeMode(byte device, byte digits)
{
	// check if within limit
	if(device <= _maxDevices)
	{
		// keep track of DECODE_MODE
		_decodeModeState[device] = digits;
		
		// push data
		_sendData(device, DECODE_MODE, digits);
	}
};

byte MAX7219::getDecodeMode(byte device)
{
	return _decodeModeState[device];
};

/*
 *	Change the intensity of the LEDs of the given device
 *	intensity	0 to 15
 *
 *	OPCODE 0x10
 */

void MAX7219::setIntensity(byte device, byte intensity)
{
	// check if within limit
	if(device <= _maxDevices)
	{
		if(intensity > 15)
		{
			intensity = 15;
		}
		
		// keep track of INTENSITY
		_intensityState[device] = intensity;
		
		// push data
		_sendData(device, INTENSITY, intensity);
	}
};

byte MAX7219::getIntensity(byte device)
{
	return _intensityState[device];
};

/*
 *	Specify how many digits are active on the given device
 *	Be aware of increased intensity on low scan-limit
 *
 *	OPCODE 0x11
 */
void MAX7219::setScanLimit(byte device, byte limit)
{
	// check if within limit
	if(device <= _maxDevices)
	{
		if(limit > 7)
		{
			limit = 7;
		}
		if(limit < 0)
		{
			limit = 0;
		}
		
		// keep track of SCAN_LIMIT for each device
		_scanLimitState[device] = limit;
		
		// push data
		_sendData(device, SCAN_LIMIT, limit);
	}
};

byte MAX7219::getScanLimit(byte device)
{
	return _scanLimitState[device];
};

/*
 *	Specify wether the given device goes to shutdown-mode or normal-operation-mode
 *
 *	OPCODE 0x12
 */
void MAX7219::setShutdown(byte device, bool status)
{
	if(device <= _maxDevices)
	{
		// keep track of SHUTDOWN
		_shutdownState[device] = status;
		
		// push data
		if(status)
		{			
			// 0 = enter shutdown-mode
			_sendData(device, SHUTDOWN, 0);
		}
		else
		{
			// 1 = enter normal-operation-mode
			_sendData(device, SHUTDOWN, 1);
		}
	}
};

bool MAX7219::getShutdown(byte device)
{
	return _shutdownState[device];
};

/*
 *	Specify wether the given device goes into display-test-mode or not
 *
 *	OPCODE 0x15
 */

void MAX7219::setDisplayTest(byte device, bool status){
	// check if within limit
	if(device <= _maxDevices)
	{
		// keep track of DISPLAY_TEST
		_displayTestState[device] = status;
		
		// push data
		if(status)
		{
			_sendData(device, DISPLAY_TEST, 1);
		}
		else
		{
			_sendData(device, DISPLAY_TEST, 0);
		}
	}
};

bool MAX7219::getDisplayTest(byte device)
{
	return _displayTestState[device];
};