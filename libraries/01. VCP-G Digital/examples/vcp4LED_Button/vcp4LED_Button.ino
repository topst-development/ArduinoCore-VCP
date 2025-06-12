/*
*  TOPST VCP : 4 LED and Button Control
*/

int ledPin[] = {47, 17, 50, 48};
int buttonPin[] = {45};

// the setup function runs once when you press reset or power the board
void setup() {
  // LED Mode and Init 
  for(int i=0; i<4; i++)
  {
    pinMode(ledPin[i], OUTPUT);
    digitalWrite(ledPin[i], HIGH);
  }

  pinMode(buttonPin[0], INPUT);

}
  
// the loop function runs over and over again forever
void loop() {

  int button = digitalRead(buttonPin[0]);
  if(button==HIGH)
  {
    for(int i=0; i<2; i++)
    {
      digitalWrite(ledPin[i], LOW);
      digitalWrite(ledPin[i+2], HIGH);
    }
  }
  else
  {
    for(int i=3; i>=2; i--)
    {
      digitalWrite(ledPin[i-2], HIGH);
      digitalWrite(ledPin[i], LOW);
    }
  }

}
