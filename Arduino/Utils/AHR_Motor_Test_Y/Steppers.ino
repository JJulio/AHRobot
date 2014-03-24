// AHR AIR HOCKEY ROBOT PROJECT

// STEPPERS MOTOR CONTROL
// Updated: Now it supports DRV8825 drivers
// SPEED, ACCELERATION AND POSITION CONTROL using Arduino 16 bit Timer interrupts

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
// Y and Z motors controls the Y axis of the robot (same output)

// We control the speed of the motors with interrupts (Timer1 and Timer3) tested up to 25Khz. 
// The position of the motor is controlled at 1Khz (in the main loop)


// TIMER 1 : STEPPER MOTOR SPEED CONTROL X-AXIS
ISR(TIMER1_COMPA_vect)
{
  if (dir_x==0)
    return;

  SET(PORTF,0); // STEP X-AXIS
  position_x += dir_x;
  __asm__ __volatile__ (
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop");  // Wait 2 cycles. With the other instruction and this we ensure a more than 1 microsenconds step pulse
  CLR(PORTF,0);
}

// TIMER 3 : STEPPER MOTOR SPEED CONTROL Y-AXIS  (2 Motors, left and right)
ISR(TIMER3_COMPA_vect)
{
  if (dir_y==0)
    return;

  SET(PORTF,6); // STEP Y-AXIS (Y-left)
  SET(PORTL,3); // STEP Z-AXIS (Y-right)
  position_y += dir_y;
  __asm__ __volatile__ (
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop");  // Wait 2 cycles. With the other instruction and this we ensure a more than 1 microsenconds step pulse
  CLR(PORTF,6);
  CLR(PORTL,3);
}

// We use a ramp for acceleration and deceleration
// To calculate the point we should start to decelerate we use this formula:
// stop_position = actual_posicion + (actual_speed*actual_speed)/(2*max_deceleration)
// Beacuase we are using a S-profile we apply a correction factor to the above formula (0.85)
// Input parameters:
//    target_position_x
//    target_speed_x
//    max_acceleration_x

void positionControl()
{
  //int16_t pos_stop;
  int32_t temp;
  uint32_t timer;
  int16_t dt;

  SET(PORTF,3); // for external timing debug
  timer = micros();
  // dt = delta time in microseconds...
  dt = constrain(timer-micros_old,0,2000);   // Limit dt (it should be around 1000 most times)
  //Serial.println(dt);
  micros_old = timer;

  // We use an acceleration ramp to imitate an S-curve profile at the begining and end (depend on speed)
  acceleration_x = map(abs(speed_x),0,accel_ramp,MIN_ACCEL_X,max_acceleration_x);
  acceleration_x = constrain(acceleration_x,MIN_ACCEL_X,max_acceleration_x);

  acceleration_y = map(abs(speed_y),0,accel_ramp,MIN_ACCEL_Y,max_acceleration_y);
  acceleration_y = constrain(acceleration_y,MIN_ACCEL_Y,max_acceleration_y);

  // X AXIS
  temp = (long)speed_x*speed_x;
  temp = temp/(1800*(long)acceleration_x);  // 2000*0.85 = 1700 0.85 is a compensation for deceleration ramp (S-curve)
  pos_stop_x = position_x + sign(speed_x)*temp;
  if (target_position_x>position_x)  // Positive move
  {
    if (pos_stop_x >= target_position_x)  // Start decelerating?
      setMotorXSpeed(0,dt);          // The deceleration ramp is done inside the setSpeed function
    else
      setMotorXSpeed(target_speed_x,dt);    // The aceleration ramp is done inside the setSpeed function
  }
  else   // Negative move
  {
    if (pos_stop_x <= target_position_x)  // Start decelerating?
      setMotorXSpeed(0,dt);
    else 
      setMotorXSpeed(-target_speed_x,dt);
  }

  // Y AXIS
  temp = (long)speed_y*speed_y;
  temp = temp/(1800*(long)acceleration_y);
  pos_stop_y = position_y + sign(speed_y)*temp;
  if (target_position_y>position_y)  // Positive move
  {
    if (pos_stop_y >= target_position_y)  // Start decelerating?
      setMotorYSpeed(0,dt);          // The deceleration ramp is done inside the setSpeed function
    else 
      setMotorYSpeed(target_speed_y,dt);    // The aceleration ramp is done inside the setSpeed function
  }
  else   // Negative move
  {
    if (pos_stop_y <= target_position_y)  // Start decelerating?
      setMotorYSpeed(0,dt);
    else 
      setMotorYSpeed(-target_speed_y,dt);
  }
  CLR(PORTF,3); // for external timing debug
}

// Speed could be positive or negative
void setMotorXSpeed(int16_t tspeed, int16_t dt)
{ 
  long timer_period;
  int16_t accel;

  // Limit max speed
  if (tspeed > MAX_SPEED_X)
    tspeed = MAX_SPEED_X;
  else if (tspeed < -MAX_SPEED_X)
    tspeed = -MAX_SPEED_X;
  //Serial.println(tspeed);

  // We limit acceleration => speed ramp
  accel = ((long)acceleration_x*dt)/1000;   // We divide by 1000 because dt are in microseconds
  if (((long)tspeed-speed_x)>accel)  // We use long here to avoid overflow on the operation
    speed_x += accel;
  else if (((long)speed_x-tspeed)>accel)
    speed_x -= accel;
  else
    speed_x = tspeed;

  // Check if we need to change the direction pins
  if ((speed_x==0)&&(dir_x!=0))    
    dir_x = 0;	
  else if ((speed_x>0)&&(dir_x!=1))
  {
#ifdef INVERT_X_AXIS
    CLR(PORTF,1);   // X-DIR
#else
    SET(PORTF,1);   
#endif
    dir_x = 1;	
  }
  else if ((speed_x<0)&&(dir_x!=-1))
  {
#ifdef INVERT_X_AXIS
    SET(PORTF,1);
#else
    CLR(PORTF,1);
#endif
    dir_x = -1;	
  }

  if (speed_x==0)
    timer_period = ZERO_SPEED;
  else if (speed_x>0)
    timer_period = 2000000/speed_x;   // 2Mhz timer
  else
    timer_period = 2000000/-speed_x;

  if (timer_period > 65535)   // Check for minimun speed (maximun period without overflow)
    timer_period = ZERO_SPEED;

  OCR1A = timer_period;  
  // Check  if we need to reset the timer...
  if (TCNT1 > OCR1A)
    TCNT1 = 0;
}

// Speed could be positive or negative
void setMotorYSpeed(int16_t tspeed,int16_t dt)
{ 
  long timer_period;
  int16_t accel;

  // Limit max speed
  if (tspeed > MAX_SPEED_Y)
    tspeed = MAX_SPEED_Y;
  else if (tspeed < -MAX_SPEED_Y)
    tspeed = -MAX_SPEED_Y;
  //Serial.println(tspeed);

  // We limit acceleration => speed ramp
  accel = ((long)acceleration_y*dt)/1000;
  if (((long)tspeed-speed_y)>accel)
    speed_y += accel;
  else if (((long)speed_y-tspeed)>accel)
    speed_y -= accel;
  else
    speed_y = tspeed;

  // Check if we need to change the direction pins
  if ((speed_y==0)&&(dir_y!=0))    
    dir_y = 0;	
  else if ((speed_y>0)&&(dir_y!=1))
  {
#ifdef INVERT_Y_AXIS // Y-DIR (Y-left)
    CLR(PORTF,7);
#else
    SET(PORTF,7);   
#endif

#ifdef INVERT_Z_AXIS  // Z-DIR (Y-right)
    CLR(PORTL,1);
#else
    SET(PORTL,1);
#endif

    dir_y = 1;	
  }
  else if ((speed_y<0)&&(dir_y!=-1))
  {
#ifdef INVERT_Y_AXIS  // Y-DIR (Y-left)
    SET(PORTF,7);
#else
    CLR(PORTF,7);
#endif

#ifdef INVERT_Z_AXIS  // Z-DIR (Y-right)
    SET(PORTL,1);
#else
    CLR(PORTL,1);
#endif

    dir_y = -1;	
  }

  if (speed_y==0)
    timer_period = ZERO_SPEED;
  else if (speed_y>0)
    timer_period = 2000000/speed_y;   // 2Mhz timer
  else
    timer_period = 2000000/-speed_y;

  if (timer_period > 65535)   // Check for minimun speed (maximun period without overflow)
    timer_period = ZERO_SPEED;

  OCR3A = timer_period;  
  // Check  if we need to reset the timer...
  if (TCNT3 > OCR3A)
    TCNT3 = 0;
}

// Set speed on each axis in steps/sec
void setSpeedS(int target_sx, int target_sy)
{
  target_sx = constrain(target_sx,0,MAX_SPEED_X);
  target_sy = constrain(target_sy,0,MAX_SPEED_Y);
  target_speed_x = target_sx;
  target_speed_y = target_sy;
}

// set Robot position in mm. 
// This function check for valid robot positions values
// Convert from mm units to steps
void setPosition(int target_x_mm_new, int target_y_mm_new)
{ 
  target_x_mm = constrain(target_x_mm_new,ROBOT_MIN_X,ROBOT_MAX_X);
  target_y_mm = constrain(target_y_mm_new,ROBOT_MIN_Y,ROBOT_MAX_Y);
  target_position_x = target_x_mm*X_AXIS_STEPS_PER_UNIT;
  target_position_y = target_y_mm*Y_AXIS_STEPS_PER_UNIT;
}

// set Robot position in mm. 
// This function check for valid robot positions values
// Convert from mm units to steps
void setPosition_straight(int target_x_mm_new, int target_y_mm_new)
{ 
  int old_target_position_x;
  int old_target_position_y;
  int diff_x;
  int diff_y;
  
  target_x_mm = constrain(target_x_mm_new,ROBOT_MIN_X,ROBOT_MAX_X);
  target_y_mm = constrain(target_y_mm_new,ROBOT_MIN_Y,ROBOT_MAX_Y);
  old_target_position_x = target_position_x;
  old_target_position_y = target_position_y;
  target_position_x = target_x_mm*X_AXIS_STEPS_PER_UNIT;
  target_position_y = target_y_mm*Y_AXIS_STEPS_PER_UNIT;
  // Speed adjust to draw straight lines
  diff_x = myAbs(target_position_x - old_target_position_x);
  diff_y = myAbs(target_position_y - old_target_position_y);
  if (diff_x > diff_y)  // Wich axis should be slower?
    {
    com_speed_x = target_speed_x;
    com_speed_y = (float)target_speed_y*(float)diff_y/(float)diff_x;
    setSpeedS(com_speed_x,com_speed_y);
    }
  else
    {
    com_speed_x = (float)target_speed_x*(float)diff_x/(float)diff_y;
    com_speed_y = target_speed_y;
    setSpeedS(com_speed_x,com_speed_y);
    }
}

// set Robot position in 1/10 mm. 
// This function check for valid robot positions values
// Convert from 1/10 mm units to steps
// This function moves the robot in a straight line
void setPosition_mm10_straight(int target_x_mm_new, int target_y_mm_new)
{ 
  int old_target_position_x;
  int old_target_position_y;
  int diff_x;
  int diff_y;
  
  target_x_mm = constrain(target_x_mm_new,ROBOT_MIN_X*10,ROBOT_MAX_X*10);
  target_y_mm = constrain(target_y_mm_new,ROBOT_MIN_Y*10,ROBOT_MAX_Y*10);
  old_target_position_x = target_position_x;
  old_target_position_y = target_position_y;
  target_position_x = (float)target_x_mm*X_AXIS_STEPS_PER_UNIT/10.0;
  target_position_y = (float)target_y_mm*Y_AXIS_STEPS_PER_UNIT/10.0;
  // Speed adjust to draw straight lines
  diff_x = myAbs(target_position_x - old_target_position_x);
  diff_y = myAbs(target_position_y - old_target_position_y);
  if (diff_x > diff_y)  // Wich axis should be slower?
    {
    com_speed_x = target_speed_x;
    com_speed_y = (float)target_speed_y*(float)diff_y/(float)diff_x;
    setSpeedS(com_speed_x,com_speed_y);
    }
  else
    {
    com_speed_x = (float)target_speed_x*(float)diff_x/(float)diff_y;
    com_speed_y = target_speed_y;
    setSpeedS(com_speed_x,com_speed_y);
    }
}


