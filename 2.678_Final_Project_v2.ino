const int PWMA=11; // Pololu drive A
const int AIN2=10;
const int AIN1 =9;
const int STDBY=8;
const int BIN1 =7; // Pololu drive B
const int BIN2 =6;
const int PWMB =5;

const int sens1 = 1;
const int sens2 = 2;
const int sens3 = 3;

int sensR;
int sensM;
int sensL;

int avg;
int sum;
float pos;

void setup()
{
  pinMode(PWMA , OUTPUT);
  pinMode(AIN1 , OUTPUT);
  pinMode(AIN2 , OUTPUT);
  pinMode(BIN1 , OUTPUT);
  pinMode(BIN2 , OUTPUT);
  pinMode(PWMB , OUTPUT);
  pinMode(STDBY , OUTPUT);
  digitalWrite(STDBY , HIGH);
  
  pinMode(sens1, INPUT);
  pinMode(sens2, INPUT);
  pinMode(sens3, INPUT);
  Serial.begin(9600);
}


void loop()
{
  sensR = analogRead(sens1);
  sensM = analogRead(sens2);
  sensL = analogRead(sens3);

  Serial.print(sensR);
  Serial.print(", ");
  Serial.print(sensM);
  Serial.print(", ");
  Serial.println(sensL);

  int sensR_mapped = map(sensR, 250, 958, 0, 100); //put in actual values
  int sensM_mapped = map(sensM, 250, 958, 0, 100); //put in actual values
  int sensL_mapped = map(sensL, 250, 958, 0, 100); //put in actual values

  Serial.print("Mapped values: ");
  Serial.print(sensR_mapped);
  Serial.print(", ");
  Serial.print(sensM_mapped);
  Serial.print(", ");
  Serial.println(sensL_mapped);

  delay(500);

  sum = (sensR_mapped, sensM_mapped, sensL_mapped);
  avg = sum/3;

  while (sensR_mapped = sensM_mapped = sensL_mapped = 0) //for right angles and breaks
  {
      
  }

  int dual = sensR_mapped-200;

  if (sensL_mapped - sensR_mapped > 40)
  {
    drive (200, dual);
  }
  else if (sensR_mapped - sensL_mapped > 40)
  {
    drive (dual, 200);
  }
  else
  {
    drive(200,200);
  }
}

void motorWrite(int spd, int pin_IN1 , int pin_IN2 , int pin_PWM)
{
  if (spd < 0)
  {
    digitalWrite(pin_IN1 , HIGH); // go one way
    digitalWrite(pin_IN2 , LOW);
  }
  else
  {
    digitalWrite(pin_IN1 , LOW); // go the other way
    digitalWrite(pin_IN2 , HIGH);
  }
  analogWrite(pin_PWM , abs(spd));
}

void drive(int speedL, int speedR)
{
  motorWrite(speedL, AIN1, AIN2, PWMA); //assuming Pololu drive A is left motor
  motorWrite(speedR, BIN1, BIN2, PWMB); //assuming Pololu drive B is right motor
}