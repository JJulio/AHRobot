// AHR: AIR HOCKEY ROBOT PROJECT
// Author: Jose Julio (@jjdrones)
// Hardware: Arduino MEGA + Ramps 1.4
// Date: 18/11/2013
// Last updated: 24/03/2014
// Version: 1.05
// Project page : 
//    Spanish: http://cienciaycacharreo.blogspot.com.es/2014/02/nuevo-proyecto-air-hockey-robot-3d.html 
//    English: http://cienciaycacharreo.blogspot.com.es/2014/02/new-project-air-hockey-robot-3d-printer.html
// GIT repository: https://github.com/JJulio/AHRobot
// License: Open Software GPL License

// ROBOT and USER configuration parameters
#include "Configuration.h"
#include "Definitions.h"   // Variable definitions

void setup() 
{ 
  // STEPPER MOTOR PINS (SAME AS RAMPS 1.4)
  // X MOTOR
  //     X-STEP: A0    (PF0)
  //     X-DIR:  A1    (PF1)
  //     X-ENABLE: D38 (PD7)
  // Y MOTOR (Y-LEFT)
  //     Y-STEP: A6    (PF6)
  //     Y-DIR:  A7    (PF7)
  //     Y-ENABLE: A2  (PF2)
  // Z MOTOR (Y-RIGHT)
  //     Z-STEP: D46   (PL3)
  //     Z-DIR:  D48   (PL1)
  //     Z-ENABLE: A8  (PK0)

  // STEPPER PINS 
  // X_AXIS
  pinMode(38,OUTPUT);  // ENABLE MOTOR
  pinMode(A0,OUTPUT);  // STEP MOTOR
  pinMode(A1,OUTPUT);  // DIR MOTOR
  // Y_AXIS (Y-LEFT)
  pinMode(A2,OUTPUT);  // ENABLE MOTOR
  pinMode(A6,OUTPUT);  // STEP MOTOR
  pinMode(A7,OUTPUT);  // DIR MOTOR
  // Z_AXIS (Y-RIGHT)
  pinMode(A8,OUTPUT);  // ENABLE MOTOR
  pinMode(46,OUTPUT);  // STEP MOTOR
  pinMode(48,OUTPUT);  // DIR MOTOR 

  pinMode(A3,OUTPUT);  // DEBUG PIN FOR OSCILLOSCOPE TIME MEASURES

  pinMode(19,INPUT);  // RX1 Serial Port 1
  pinMode(18,OUTPUT); // TX1

  //FANS and LEDS
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(13,OUTPUT);

  // Disable Motors
  digitalWrite(38,HIGH);
  digitalWrite(A2,HIGH);
  digitalWrite(A8,HIGH);

  Serial.begin(115200);
  Serial.println("AHR Robot version 1.05");
  delay(500);
  Serial.println("Initializing robot...");
  delay(2000);

  cameraProcessInit();  // Camera variable initializations
  
  // Robot positions initialization
  defense_position = ROBOT_DEFENSE_POSITION;   // Robot y axis defense position
  attack_position = ROBOT_DEFENSE_ATTACK_POSITION;   // Robot y axis position for defense+attack

  //LED blink
  for (uint8_t k=0;k<4;k++)
  {
    digitalWrite(13,HIGH);
    delay(300);
    digitalWrite(13,LOW);
    delay(300);
  } 

  // We use TIMER 1 for stepper motor X AXIS and Timer 3 for Y AXIS
  // STEPPER MOTORS INITIALIZATION
  // TIMER1 CTC MODE
  TCCR1B &= ~(1<<WGM13);
  TCCR1B |=  (1<<WGM12);
  TCCR1A &= ~(1<<WGM11); 
  TCCR1A &= ~(1<<WGM10);

  // output mode = 00 (disconnected)
  TCCR1A &= ~(3<<COM1A0); 
  TCCR1A &= ~(3<<COM1B0); 

  // Set the timer pre-scaler
  // Generally we use a divider of 8, resulting in a 2MHz timer on 16MHz CPU
  TCCR1B = (TCCR1B & ~(0x07<<CS10)) | (2<<CS10);

  OCR1A = ZERO_SPEED;   // Motor stopped
  dir_x = 0;
  TCNT1 = 0;

  // We use TIMER 3 for stepper motor Y AXIS 
  // STEPPER MOTORS INITIALIZATION
  // TIMER3 CTC MODE
  TCCR3B &= ~(1<<WGM13);
  TCCR3B |=  (1<<WGM12);
  TCCR3A &= ~(1<<WGM11); 
  TCCR3A &= ~(1<<WGM10);

  // output mode = 00 (disconnected)
  TCCR3A &= ~(3<<COM1A0); 
  TCCR3A &= ~(3<<COM1B0); 

  // Set the timer pre-scaler
  // Generally we use a divider of 8, resulting in a 2MHz timer on 16MHz CPU
  TCCR3B = (TCCR3B & ~(0x07<<CS10)) | (2<<CS10);

  OCR3A = ZERO_SPEED;   // Motor stopped
  dir_y = 0;
  TCNT3 = 0;

  //Initializing init position
  position_x = ROBOT_INITIAL_POSITION_X*X_AXIS_STEPS_PER_UNIT;
  position_y = ROBOT_INITIAL_POSITION_Y*Y_AXIS_STEPS_PER_UNIT;

  delay(4000);

  Serial.println("Initializing Stepper motors...");
  delay(1000);
  TIMSK1 |= (1<<OCIE1A);  // Enable Timer1 interrupt
  TIMSK3 |= (1<<OCIE1A);  // Enable Timer1 interrupt

  // Enable steppers
  digitalWrite(38,LOW);   // X-axis
  digitalWrite(A2,LOW);   // Y-axis left
  digitalWrite(A8,LOW);   // Z-axis (Y-axis right)

  // Output parameters
  Serial.print("Max_acceleration_x: ");
  Serial.println(max_acceleration_x);
  Serial.print("Max_acceleration_y: ");
  Serial.println(max_acceleration_y);
  Serial.print("Max speed X: ");
  Serial.println(MAX_SPEED_X);
  Serial.print("Max speed Y: ");
  Serial.println(MAX_SPEED_Y);
  Serial.println("Moving to initial position...");
  Serial.println("Ready!!");
  delay(1000);

  // Initializing Robot command variables
  com_pos_x = ROBOT_CENTER_X;
  com_pos_y = defense_position;
  com_speed_x = MAX_SPEED_X;
  com_speed_y = MAX_SPEED_Y;

  setSpeedS(com_speed_x,com_speed_y);
  setPosition(com_pos_x,com_pos_y);

  timer_old = micros();
  timer_packet_old = timer_old;
  micros_old = timer_old;


  // Enable Air Hockey FANS
  //digitalWrite(9,HIGH);
  //digitalWrite(10,HIGH);
  analogWrite(9,FAN1_SPEED);
  analogWrite(10,FAN2_SPEED);
}

// Main loop
void loop() 
{ 
  int dt;
  uint8_t logOutput=0;

  debug_counter++;
  timer_value = micros();
  if ((timer_value-timer_old)>=1000)  // 1Khz loop
  {
    //dt = (timer_value-timer_old)/1000;
    timer_old = timer_value;

    loop_counter++;

    packetRead();
    if (newPacket)
    {
      //dt = (timer_value-timer_packet_old)/1000.0;
      dt = 16;  //60 Hz = 16.66ms
      timer_packet_old = timer_value;
      newPacket=0;
      logOutput = 1;
      //Serial.print("P");

      // Puck detection and trayectory prediction
      cameraProcess(puckPixX,puckPixY,dt);

      // Robot position detection for missing steps detection in steppers.
      robotDetection(robotPixX,robotPixY);
      missingStepsDetection();

      // Robot strategy based on puck prediction
      newDataStrategy();
      
     
    }  // End packet received
    
    
    
    // Strategy : Robot behaviour
    robotStrategy();
    
    // Console output
    if (logOutput)
      {
      logOutput = 0;
      Serial.print(robot_status);
      Serial.print(" ");
      Serial.print(com_pos_x);
      Serial.print(" ");
      Serial.print(com_pos_y);
      Serial.print(" ");
      Serial.println(speed_y);
      }
    // DEBUG : PUCK POSITION
    if (loop_counter == 4000)
      {
        Serial.print("PUCK POSITION: ");
        Serial.print(puckCoordX);
        Serial.print(",");
        Serial.println(puckCoordY);
      }
      
    // DEBUG: We inform of the position error of the robot as seen in the camera (util for calibrations)
    if (loop_counter == 4002)
      {
        Serial.print("ROBOT ERROR: ");
        Serial.print(robotMissingStepsErrorX);
        Serial.print(",");
        Serial.println(robotMissingStepsErrorY);
      }

    
    positionControl();
  } // 1Khz loop
}





