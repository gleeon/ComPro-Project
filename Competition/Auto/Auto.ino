#include<Servo.h>
/*Define Motors and Pins*/

//Servo
Servo Servo_9;

//Left Motors
const int l1 = 4; //INA1
const int l2 = 2; //INA2
const int len = 5; //ENA

//Right Motors
const int r1 = 8; //INB1
const int r2 = 7; //INB2
const int ren = 6; //ENB

const int IRsensor = A3; //IRSensor

const int trigLeft = 12;//LeftUltrasonicsSensor
const int echoLeft = A2;

const int trigRight = 11;//RightUltrasonicsSensor
const int echoRight = A1;

const int trigFront = 10;//FrontUltrasonicsSensor
const int echoFront = A0;

const int Dis = 4; //the robot should stay away from the wall at least 4 cm

const int MaxIR = 490;
const int MinIR = 390;

const int MaxMoves = 100;
const int del = 200;

struct Moves{
  char direction;
  int duration;
};

Moves path[MaxMoves];
Moves *p = path;

int TargetCount = 0;
int PathIndex = 0;

void setup() {
  /*Sensors*/
  pinMode(IRsensor, INPUT);

  pinMode(trigLeft, OUTPUT);
  pinMode(echoLeft, INPUT);

  pinMode(trigRight, OUTPUT);
  pinMode(echoRight, INPUT);

  pinMode(trigFront, OUTPUT);
  pinMode(echoFront, INPUT);

  /*Motors*/
  pinMode(l1, OUTPUT);
  pinMode(l2, OUTPUT);
  pinMode(len, OUTPUT);

  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  pinMode(ren, OUTPUT);

  /*Servo*/
  Servo_9.attach(9);
  Servo_9.write(0);

}

void loop()
{
  //Before grabbing the object
  while(IRSensor() < MinIR)
  {
    MazeSolver();
  }
  //Found the object IR > min IR
  ServoGrab()

  //After grabbing the object solve the maze to the target
  while(TargetCount < 2){
      MazeSolver();
  }
  //Found the target
  ServoDrop();
  //Return to the exit
  Reverse();
  //solve the maze to the exit
  MazeSolver();
}

/*Functions for Sensors*/
long GetDistance(int Trig, int Echo){
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);

  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(Trig, LOW);

  long dur = pulseIn(Echo, HIGH);
  return 0.017*dur;
}

long FrontSensor()
{
  return GetDistance(trigFront, echoFront);
}

long LeftSensor()
{
  return GetDistance(trigLeft, echoLeft);
}


long RightSensor()
{
  return GetDistance(trigRight, echoRight);
}

long IRSensor()
{
  return(analogRead(IRsensor));
}

/*Functions for Robot movements*/
void ServoGrab(){
  Servo_9.write(180);
  delay(500);
  Forward();
  delay(500);
  Servo_9.write(140);
  //Adjust
  Backward();
  TurnLeft();
  Forward();
}

void ServoDrop(){
  Forward();
  Servo_9.write(180);
  delay(500);
  Backward();
  Servo_9.write(0);
}
void RecordMove(char dire, int dura){
  if (PathIndex < MaxMoves){
    p[PathIndex].direction = dire;
    p[PathIndex].duration = dura;
    PathIndex++;
  }
}

void Forward()
{
  digitalWrite(l1, HIGH);
  digitalWrite(l2, LOW);
  analogWrite(len, 255);

  digitalWrite(r1, HIGH);
  digitalWrite(r2, LOW);
  analogWrite(ren, 255);
  delay(del);
  RecordMove('F', del);
}

void TurnLeft()
{
  digitalWrite(l1, LOW);
  digitalWrite(l2, HIGH);
  analogWrite(len, 255);

  digitalWrite(r1, HIGH);
  digitalWrite(r2, LOW);
  analogWrite(ren, 255);
  delay(del);
  RecordMove('L', del);
}

void TurnRight()
{
  digitalWrite(l1, HIGH);
  digitalWrite(l2, LOW);
  analogWrite(len, 255);

  digitalWrite(r1, LOW);
  digitalWrite(r2, HIGH);
  analogWrite(ren, 255);
  delay(del);
  RecordMove('R', del);
}

void Stop()
{
  digitalWrite(l1, LOW);
  digitalWrite(l2, LOW);
  analogWrite(len, 0);

  digitalWrite(r1, LOW);
  digitalWrite(r2, LOW);
  analogWrite(ren, 0);
}

void Backward()
{
  digitalWrite(l1, LOW);
  digitalWrite(l2, HIGH);
  analogWrite(len, 255);

  digitalWrite(r1, LOW);
  digitalWrite(r2, HIGH);
  analogWrite(ren, 255);
  delay(del);
  RecordMove('B', del);
}

void MazeSolver()
{
  long f = FrontSensor();
  long l = LeftSensor();
  long r = RightSensor();
  if(f < Dis && l < Dis && r < Dis)
  {
    TargetCount++;
    Backward();
    TurnLeft();
    Forward();
  }
  else if(f > Dis && l < Dis && r < Dis)
  {
    Forward();
  }
  else if(f < Dis && l < Dis && r > Dis)
  {
    TurnRight();
    Forward();
  }
  else if(f < Dis && l > Dis && r < Dis)
  {
    TurnLeft();
    Forward();
  }
  else if(f > Dis && l < Dis && r > Dis)
  {
    TurnRight();
    Forward();
  }
  else if(f > Dis && l > Dis && r < Dis)
  {
    TurnLeft();
    Forward();
  }
  else if(f > Dis && l > Dis && r > Dis)
  {
    TurnLeft();
    Forward();
  }
  else if(f < Dis && l > Dis && r > Dis)
  {
    TurnLeft();
    Forward();
  }
}

void Reverse()
{
  for(int i = PathIndex -1; i >= 0; i--){
    Moves *m = &p[i];
    switch(m->direction){
      case 'F':
        Backward();
        break;
      case 'L':
        TurnRight();
        break;
      case 'R':
        TurnLeft();
        break;
      case 'B':
        Forward();
        break;
    }
    delay(m->duration);
  }
}