
#include <LiquidCrystal_I2C.h>
  
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  LiquidCrystal_I2C lcd(0x27,16,2);

  lcd.init(); 
  lcd.backlight(); // initialize the lcd 

  lcd.setCursor(2, 0);
  delay(10);
  lcd.print("* TOPST VCP *");
  
  lcd.setCursor(4, 1);
  delay(10);
  lcd.print("I2C Test!");
}

void loop()
{  

}




