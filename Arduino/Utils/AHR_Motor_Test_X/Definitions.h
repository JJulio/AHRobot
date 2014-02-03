// AHR AIR HOCKEY ROBOT PROJECT

#include <stdint.h>

// Variable definitions

// Log and Timer variables
long loop_counter;
long timer_old;
long timer_packet_old;
long timer_value;
int debug_counter;

uint32_t micros_old;

// We have 2 axis => 2 motor controls 0=X axis   1=Y axis  (Y AXIS HAS 2 MOTORS Left and Right)
int16_t speed_m[2];           // Actual speed of motors
uint8_t dir_m[2];             // Actual direction of steppers motors

uint16_t counter_m[2];        // counters for periods
uint16_t period_m[2][8];      // Eight subperiods 
uint8_t period_m_index[2];    // index for subperiods

// kinematic variables
// position, speed and acceleration are in step units
volatile int16_t position_x;  // This variables are modified inside the Timer interrupts
volatile int16_t position_y;

int16_t speed_x;
int16_t speed_y;
int16_t max_speed_x;
int16_t max_speed_y;

int8_t dir_x;     //(dir=1 positive, dir=-1 negative)
int8_t dir_y;  
int16_t target_position_x;
int16_t target_position_y;
int16_t target_speed_x;
int16_t target_speed_y;
int16_t max_acceleration_x = MAX_ACCEL_X;  // default maximun acceleration
int16_t max_acceleration_y = MAX_ACCEL_Y;
int16_t acceleration_x = MAX_ACCEL_X;
int16_t acceleration_y = MAX_ACCEL_Y;
int16_t accel_ramp = ACCEL_RAMP_MIN;

int16_t pos_stop_x;
int16_t pos_stop_y;

uint16_t com_pos_x;
uint16_t com_pos_y;
uint16_t com_speed_x;
uint16_t com_speed_y;
uint16_t target_x_mm;
uint16_t target_y_mm;
int16_t user_speed_x;
int16_t user_speed_y;
int16_t filt_user_speed_x;
int16_t filt_user_speed_y;


// Some util functions...
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

int16_t myAbs(int16_t param)
{
  if (param<0)
    return -param;
  else
    return param;
}

int sign(int val)
{
  if (val<0)
    return(-1);
  else
    return(1);
}

