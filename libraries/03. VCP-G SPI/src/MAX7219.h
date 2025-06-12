/*
	MAX7219.h - Library for MAX7219-driven 8*8 LED-Matrix.
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

#ifndef MAX7219_h
#define MAX7219_h

#include "Arduino.h"

class MAX7219
{
	private:
		byte _dataPin;
		byte _clockPin;
		byte _loadPin;
		
		byte _maxDevices; 
		
		byte _ledState[9][8];
		byte _decodeModeState[9];
		byte _intensityState[9];
		byte _scanLimitState[9];
		bool _shutdownState[9];
		bool _displayTestState[9];

	private:
		
		/*
		 *	Send data to the MAX7219 Driver
		 *	registerAddress 	= opcode
		 *	registerData		= value
		 */
		
		void _sendData(byte device, byte registerAddress, byte registerData);
	
	public:

		/*
		 *	Constructor with initialy blanked display
		 */
		
		MAX7219(byte devices, byte dataPin, byte clockPin, byte loadPin);
		
		/*
		 *	Lighting the displays
		 */
		
		void clearDisplay(byte device);
		void clearAllDisplays();
		
		void fillDisplay(byte device);
		void fillAllDisplays();
		
		void invertDisplay(byte device);
		void invertAllDisplays();
		
		/*
		 *	LED-Matrix functions
		 */
		
		void setLed(byte device, byte column, byte row, bool state);
		void invertLed(byte device, byte column, byte row);
		bool getLed(byte device, byte column, byte row);
		
		void setRow(byte device, byte row, byte value);
		void invertRow(byte device, byte row);
		byte getRow(byte device, byte row);
		
		void setColumn(byte device, byte column, byte value);
		void invertColumn(byte device, byte column);
		byte getColumn(byte device, byte column);
		
		/*
		 *	Change operational states of the MAX7219 driver
		 */
		
		
		/*
		 *	Specify wether the MAX7219 driver uses its internal BCD-Codes
		 *	or individual segment-control for the given device
		 *
		 *	OPCODE 0x09
		*/
		void setDecodeMode(byte device, byte digits);
		byte getDecodeMode(byte device);
		
		/*
		 *	Change the intensity of the LEDs of the given device
		 *	intensity	0 to 15
		 *
		 *	OPCODE 0x10
		 */
		void setIntensity(byte device, byte intensity);
		byte getIntensity(byte device);
		
		/*
		 *	Specify how many digits are active on the given device
		 *	Be aware of increased intensity on low scan-limit
		 *
		 *	OPCODE 0x11
		 */
		void setScanLimit(byte device, byte limit);
		byte getScanLimit(byte device);
		
		/*
		 *	Specify wether the given device goes to shutdownmode or normal-operation-mode
		 *	in order to keep it logical the boolean value is inverted in the process
		 *
		 *	This is because of the logic of the IC, stated in the datasheet
		 *	0 = shutdown-mode
		 *	1 = normal-operation-mode
		 *
		 *	OPCODE 0x12
		 */
		void setShutdown(byte device, bool status);
		bool getShutdown(byte device);
		
		/*
		 *	Specify wether the given device goes into display-test-mode or not
		 *
		 *	OPCODE 0x15
		 */
		void setDisplayTest(byte device, bool status);
		bool getDisplayTest(byte device);

};

#endif