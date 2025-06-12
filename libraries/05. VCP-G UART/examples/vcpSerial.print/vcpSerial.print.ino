/*
  Analog input, analog output, serial output

  Reads an analog input pin, maps the result to a range from 0 to 1000 and uses
  the result to set the pulse width modulation (PWM) of an output pin.
  Also prints the results to the Serial Monitor.

  The circuit:
  - potentiometer connected to analog pin 16.
    Center pin of the potentiometer goes to the analog pin.
    side pins of the potentiometer go to +3.3V and ground
  - LED connected from digital pin 5 to ground through 220 ohm resistor
*/

#include "HardwareSerial.h"
HardwareSerial Serial;

// These constants won't change. They're used to give names to the pins used:
const int analogInPin = 16;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 5;  // Analog output pin that the LED is attached to

int sensorValue = 0;  // value read from the pot
int outputValue = 0;  // value output to the PWM (analog out)

void setup() {
  // initialize serial communications at 115200 bps:
  Serial.begin(115200);
}

void loop() {
  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  // map it to the range of the analog out:
  outputValue = map(sensorValue, 0, 4095, 0, 1000);
  // change the analog out value:
  analogWrite(analogOutPin, outputValue);

  // print the results to the Serial Monitor:
  Serial.print("sensor = ");
  Serial.print(sensorValue);
  Serial.print("\t output = ");
  Serial.println(outputValue);

  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  delay(2);
}
