#include <MAX7219.h>

// 8x8 Dot-Matrix
#define LOADPIN 10 //GPIO_C13(SPI1_CS)
#define CLOCKPIN 13 //GPIO_C12(SPI1_CLK)
#define DATAPIN 11 //GPIO_C14(SPI1_DO)

// binary represention of a heart shape
const static byte HEART[8] = {
  0b01100110,
  0b10011001,
  0b10000001,
  0b10000001,
  0b01000010,
  0b00100100,
  0b00011000,
  0b00000000
};

// The Letter "R"
// human-readable binary representation from left-to-right and top-to-bottom
const static byte R[8] = {
  0b00000000,
  0b01111100,
  0b01000110,
  0b01111100,
  0b01001000,
  0b01000100,
  0b01000010,
  0b00000000
};

void setup() {

}

void loop() {
  MAX7219 Matrix(1, DATAPIN, CLOCKPIN, LOADPIN);
  // First LED in upper left Corner
  Matrix.setLed(1, 0, 0, true);
  delay(500);
  Matrix.setLed(1, 0, 0, false);
  delay(500);

  // Make Hearshape from array
  for(int row = 0; row <= 7; row++)
  {
    Matrix.setRow(1, row, HEART[row]);
    delay(100);
  }
  delay(500);

  // Change Intensity to make it pulse
  for(int repeats = 0; repeats < 3; repeats++)
  {
    // increase intensity
    for(int i = 1; i <= 15; i++)
    {
      Matrix.setIntensity(1, i);
      delay(20);
    }

    // decrease intensity
    for(int j = 15; j >= 1; j--)
    {
      Matrix.setIntensity(1, j);
      delay(20);
    }
  }
  delay(500);
  
  // Make Letter "R" from array
  for(int row = 0; row <= 7; row++)
  {
    Matrix.setRow(1, row, R[row]);
    delay(100);
  }
  delay(1000);

  // invert display
  Matrix.invertDisplay(1);
  delay(1000);
  
  // clear display
  Matrix.clearDisplay(1);
  delay(1000);
}
