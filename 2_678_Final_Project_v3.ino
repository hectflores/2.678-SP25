const int PWMA=11; // Pololu drive A
const int AIN2=10;
const int AIN1 =9;
const int STDBY=8;
const int BIN1 =7; // Pololu drive B
const int BIN2 =6;
const int PWMB =5;



const int sens_1 = 1;
const int sens_2 = 2;
const int sens_3 = 3;

int sens1;
int sens2;
int sens3;

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

  //find initial position of robot!!
  sens1 = analogRead(sens_1);
  sens2 = analogRead(sens_2);
  sens3 = analogRead(sens_3);

  Serial.print(sens1);
  Serial.print(", ");
  Serial.print(sens2);
  Serial.print(", ");
  Serial.println(sens3);

  int sens1_mapped = map(sens1, 250, 857, 0, 100); //values determined by readings on paper, adjust as needed
  int sens2_mapped = map(sens2, 55, 828, 0, 100); 
  int sens3_mapped = map(sens3, 45, 790, 0, 100); 

  Serial.print("Mapped values: ");
  Serial.print(sens1_mapped);
  Serial.print(", ");
  Serial.print(sens2_mapped);
  Serial.print(", ");
  Serial.println(sens3_mapped);
}

void loop()
{
  int avg = sensor()/3;
  if (avg < 10)
  {
    unsigned long t1 = millis();
    unsigned long time1 = millis() - t1;
    
    while (time1 < 500 and avg < 10)
    {
      drive(-200,200); //turning left
      time1 = millis() - t1;
      avg = sensor()/3;
    }
    
    if (sensor()/3 > 10) return; //line visible again
    
    unsigned long t2 = millis();
    unsigned long time2 = millis() - t2;

    while (time2 < 1000 and avg < 10) //adjust time and sum as needed
    {
      drive(200, -200); //turning right
      time2 = millis() - t2;
      avg = sensor()/3;
    }

    if (sensor()/3 > 10) return; //line visible again

    unsigned long t3 = millis();
    unsigned long time3 = millis() - t3;
    
    while (time3 < 500 and avg < 10) //adjust time based on turning speed
    {
      drive (-200, 200); //reorienting straight (tunring left)
      time3 = millis() - t3;
      avg = sensor()/3;
    }

    unsigned long t4 = millis();
    unsigned long time4 = millis() - t4;

    while (time4 < 2000 and avg < 10) //driving over any gap, adjust time as needed
    {
      drive(100,100);
      time4 = millis() - t4;
      avg = sensor()/3;
    }
  }
  
  sens1 = analogRead(sens_1);
  sens2 = analogRead(sens_2);
  sens3 = analogRead(sens_3);

  int sens1_mapped = map(sens1, 250, 857, 0, 100); //values determined by readings on paper, adjust as needed
  int sens2_mapped = map(sens2, 55, 828, 0, 100); 
  int sens3_mapped = map(sens3, 45, 790, 0, 100);

  int dual = sens1_mapped-200;

  if (sens3_mapped - sens1_mapped > 40)
  {
    drive(200, dual);
  }
  else if (sens1_mapped - sens3_mapped > 40) 
  {
    drive(dual, 200);
  }
  else
  {
    drive(200,200);
  }
}

int sensor()
{
  sens1 = analogRead(sens_1);
  sens2 = analogRead(sens_2);
  sens3 = analogRead(sens_3);

  int sens1_mapped = map(sens1, 250, 857, 0, 100); //values determined by readings on paper, adjust as needed
  int sens2_mapped = map(sens2, 55, 828, 0, 100); 
  int sens3_mapped = map(sens3, 45, 790, 0, 100); 
  sum = sens1_mapped + sens2_mapped + sens3_mapped;

  return sum;
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