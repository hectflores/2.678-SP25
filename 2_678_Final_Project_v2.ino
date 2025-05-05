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
float p;
float i;
float d;
float error;

float sp;
float lp;
int correction;

float Kp = 5;
float Ki = 0;
float Kd = 40;

int speed = 200; //completely arbitrary, adjust as needed

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

  //find initial position of robot!!
  sensL = analogRead(sens1);
  sensM = analogRead(sens2);
  sensR = analogRead(sens3);

  Serial.print(sensR);
  Serial.print(", ");
  Serial.print(sensM);
  Serial.print(", ");
  Serial.println(sensL);

  int sensR_mapped = map(sensR, 37, 909, 0, 100); //values determined by readings on paper, adjust as needed
  int sensM_mapped = map(sensM, 33, 903, 0, 100); 
  int sensL_mapped = map(sensL, 29, 675, 0, 100); 

  Serial.print("Mapped values: ");
  Serial.print(sensR_mapped);
  Serial.print(", ");
  Serial.print(sensM_mapped);
  Serial.print(", ");
  Serial.println(sensL_mapped);

  sum = sensR_mapped + sensM_mapped + sensL_mapped;
  avg = sum/3;

  pos = avg/sum;

  sp = pos;
}

void loop()
{
  pid();
  
  //i'm putting the right angle or discontinuous line case in loop() but i think it could also put it in turn()? idk which would be better
  
  if (sum > 50) //visible line, 50 value is random choice, can be updated if needed
  {
    turn();
  }

  //now assuming line isn't visible
  
  unsigned long t1 = millis();
  unsigned long time1 = millis() - t1;
  
  while (time1 < 1000 and sum < 50)
  {
    drive(-200,200); //turning left
    time1 = millis() - t1;
    sum = sensor();
  }
  
  if (sensor() > 50) return; //line visible again
  
  unsigned long t2 = millis();
  unsigned long time2 = millis() - t2;

  while (time2 < 2000 and sum < 50) //adjust time and sum as needed
  {
    drive(200, -200); //turning right
    time2 = millis() - t2;
    sum = sensor();
  }

  if (sensor() > 50) return; //line visible again

  unsigned long t3 = millis();
  unsigned long time3 = millis() - t3;
  
  while (time3 < 1000 and sum < 50) //adjust time based on turning speed
  {
    drive (-200, 200); //reorienting straight (tunring left)
    time3 = millis() - t3;
    sum = sensor();
  }

  unsigned long t4 = millis();
  unsigned long time4 = millis() - t4;

  while (time4 < 2000 and sum < 50) //driving over any gap, adjust time as needed
  {
    drive(200,200);
    time4 = millis() - t4;
    sum = sensor();
  }
}

int sensor()
{
  sensL = analogRead(sens1);
  sensM = analogRead(sens2);
  sensR = analogRead(sens3);

  int sensR_mapped = map(sensR, 37, 909, 0, 100);
  int sensM_mapped = map(sensM, 33, 903, 0, 100); 
  int sensL_mapped = map(sensL, 29, 675, 0, 100); 
  sum = sensR_mapped + sensM_mapped + sensL_mapped;

  return sum;
}

void pid()
{
  int i = 0;
  
  sum = sensor();

  avg = sum/3;

  pos = avg/sum;

  error = pos - sp;

  p = error;
  i += error;
  d = p - lp;
  lp = p;

  correction = int(Kp*p + Ki*i + Kd*d);
}

void turn()
{
  int rspeed = speed + correction;
  int lspeed = speed - correction;

  if (rspeed > 200) //arbitrarily deciding no speed can be over 200 and under -200
  {
    rspeed = 200;
  }

  if (lspeed > 200)
  {
    lspeed = 200;
  }

  if (rspeed < -200)
  {
    rspeed = -200;
  }

  if (lspeed < -200)
  {
    lspeed = -200;
  }

  drive(lspeed, rspeed);
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
