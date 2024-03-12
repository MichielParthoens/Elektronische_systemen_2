int sensorPin = A5; // select the input pin for the potentiometer

void setup ()
{
 Serial.begin (115200);
}

void loop ()
{
 int sensorValue = analogRead (sensorPin);
 delay (500);
 Serial.println (sensorValue, DEC);
}
