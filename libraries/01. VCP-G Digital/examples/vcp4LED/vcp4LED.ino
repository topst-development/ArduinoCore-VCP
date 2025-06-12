/*
*  TOPST VCP : 4 LED Control
*/

int ledPin[] = {47, 17, 50, 48};

// the setup function runs once when you press reset or power the board
void setup() {
  // LED Mode and Init 
  for(int i=0; i<4; i++)
  {
    pinMode(ledPin[i], OUTPUT);
    digitalWrite(ledPin[i], HIGH);
  }
}
  
// the loop function runs over and over again forever
void loop() {
  for(int i=0; i<4; i++)
  {
    digitalWrite(ledPin[i], LOW);
    delay(500);
  }

  for(int i=3; i>=0; i--)
  {
    digitalWrite(ledPin[i], HIGH);
    delay(500);
  }
}
