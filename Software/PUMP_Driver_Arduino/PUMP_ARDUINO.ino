const int Valve_1_Pin =  4;
const int motorPin =  5;
const int Valve_2_Pin =  3;
const int Injector =  6;
void setup() {
  // set the digital pin as output:
  Serial.begin(9600);
  pinMode(Valve_1_Pin, OUTPUT);
   pinMode(motorPin, OUTPUT);
   pinMode(Valve_2_Pin, OUTPUT);
   pinMode(Injector, OUTPUT);
}
int ledState = LOW; 
void loop()
{
digitalWrite(Injector, HIGH);
if(Serial.available())
{
  int data=Serial.read();
  if (data=='A') //pump works at low speed
   {
        analogWrite( motorPin,155);////155
      digitalWrite(Valve_2_Pin, LOW); 
      digitalWrite(Valve_1_Pin, HIGH);
      delay(1000);
      // analogWrite( motorPin, 255);
      //digitalWrite(Valve_2_Pin, LOW); 
      //digitalWrite(Valve_1_Pin, HIGH);
      //delay(3000);
      analogWrite( motorPin,0 );
      digitalWrite(Valve_2_Pin, LOW); 
      digitalWrite(Valve_1_Pin, LOW);
    }
     else if((data=='B') ) //holidng negative pressure
  {
    analogWrite( motorPin,0 );
    digitalWrite(Valve_2_Pin, LOW); 
    digitalWrite(Valve_1_Pin, LOW);
  }
  else if ((data=='C') ) //releasing negative pressure
  {
    analogWrite( motorPin,0 );
    digitalWrite(Valve_2_Pin, HIGH); 
    digitalWrite(Valve_1_Pin, HIGH);/////high changed to low- high open valve 
  }
  else if ((data=='D') ) //injector works
  {
    digitalWrite(Injector, LOW);
    delay(100);
    //digitalWrite(Injector, LOW);
    //delay(4000);
    digitalWrite(Injector, HIGH);
    delay(200);//1200
  }
  else if  (data=='E') //pump works at high speed
   {
      analogWrite( motorPin, 255);
      digitalWrite(Valve_2_Pin, LOW); 
      digitalWrite(Valve_1_Pin, HIGH);
      delay(3000);
      analogWrite( motorPin,0 );
      digitalWrite(Valve_2_Pin, LOW); 
      digitalWrite(Valve_1_Pin, LOW);
    }
}
}
