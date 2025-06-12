#include <HardwareSerial.h>
HardwareSerial Serial;

int sensorPin = A5;
int ledPin = 5;
int sensorValue = 0;  

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

}

void loop()
{

  for(;;)
  {
    sensorValue = analogRead(sensorPin);
	Serial.println(sensorValue);
    if(sensorValue<3000)
    {
      digitalWrite(ledPin, LOW);
    }
    else 
    {
      digitalWrite(ledPin, HIGH);  
    }
    
    delay(10);
  }

}
