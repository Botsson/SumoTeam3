#include <NewPing.h>
#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
 
#define LED 13

#define TRIGGER_PIN  2
#define ECHO_PIN     6
#define MAX_DISTANCE 200
 
// this might need to be tuned for different lighting conditions, surfaces, etc.
#define QTR_THRESHOLD  1000 // microseconds
  
// these might need to be tuned for different motor types
#define REVERSE_SPEED     400 // 0 is stopped, 400 is full speed
#define TURN_SPEED        200
#define FORWARD_SPEED     200
#define REVERSE_DURATION  100 // ms
#define TURN_DURATION     200 // ms

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12
 
#define NUM_SENSORS 6
unsigned int sensor_values[NUM_SENSORS];
 
ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);

void waitForButtonAndCountDown()
{
  digitalWrite(LED, HIGH);
  button.waitForButton();
  digitalWrite(LED, LOW);
}
 
void setup()
{
  // uncomment if necessary to correct motor directions
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);
   
  pinMode(LED, HIGH);
  
  Serial.begin(115200);
   
  waitForButtonAndCountDown();
}

void loop()
{
  if (button.isPressed())
  {
    // if button is pressed, stop and wait for another press to go again
    motors.setSpeeds(0, 0);
    button.waitForRelease();
    waitForButtonAndCountDown();
  }
  
  //borderDetect();
  /*if(borderDetect()) {
    
  } else {*/
    sensorDetect(10);
  //}
}

bool borderDetect()
{
  sensors.read(sensor_values);
  
  if (sensor_values[0] < QTR_THRESHOLD)
  {
    // if leftmost sensor detects line, reverse and turn to the right
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    
    return true;
  }
  else if (sensor_values[5] < QTR_THRESHOLD)
  {
    // if rightmost sensor detects line, reverse and turn to the left
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    
    return true;
  }
  else
  {
    // otherwise, go straight
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    return false;
  }
}

bool sensorDetect(int attackDistance)
{
  //Serial.print(uS / US_ROUNDTRIP_CM);
  delay(50);
  int sensorDistance = sonar.ping();
  int sensorDistanceCM = sensorDistance / US_ROUNDTRIP_CM;
  
  Serial.print("Ping: ");
  Serial.print(sensorDistance / US_ROUNDTRIP_CM);
  Serial.println("cm");

  if (sensorDistanceCM > 0 && sensorDistanceCM < attackDistance) { 
    attack();
  } else {
    borderDetect();
  }
}

void attack() 
{
  int attackSpeed = 400;
  Serial.print("Attack!");
  motors.setSpeeds(attackSpeed, attackSpeed);
}


