int encoderPinA = 18;
int encoderPinB = 19;
int encoderValue1 = 0;
int encoderValue2 = 0;
void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
    encoderValue1 = analogRead(encoderPinA);
    encoderValue2 = analogRead(encoderPinB);
    Serial.print(encoderValue1);
    Serial.print(",");
    Serial.println(encoderValue2);
}
