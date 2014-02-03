// AHR AIR HOCKEY ROBOT PROJECT

// THIS VALUES DEPENDS ON THE MOTORS, PULLEYS AND ROBOT CONSTRUCTION
#define MIN_ACCEL_X 50
#define MAX_ACCEL_X 50    //320      // Maximun motor acceleration in (steps/seg2)/1000
#define MIN_ACCEL_Y 50
#define MAX_ACCEL_Y 50    //220 
#define MAX_SPEED_X 4000     //25000   // Maximun speed in steps/seg
#define MAX_SPEED_Y 4000
#define SPEED_PAINT 500

// This is for the Accel ramp implementation (to smooth the intial acceleration), simplified S-profile
#define ACCEL_RAMP_MIN 10
#define ACCEL_RAMP_MAX 10000

// UNCOMMENT THIS LINES TO INVERT MOTORS
#define INVERT_X_AXIS 1
//#define INVERT_Y_AXIS 1  //Y-LEFT
//#define INVERT_Z_AXIS 1  //Y_RIGHT

// This depends on the pulley teeth. For 42 teeth GT2 => 19, for 40 teeth GT2 => 20
#define X_AXIS_STEPS_PER_UNIT 19.04    // With 42 teeth GT2 pulley and 1/8 microstepping on drivers
#define Y_AXIS_STEPS_PER_UNIT 19.04    // 200*8 = 1600 steps/rev = 1600/42*2 = 19.047, using 19 is an error of 1mm every 40cm not too much! and we use int operations...

// Min and Max robot positions in mm
#define ROBOT_MIN_X 60
#define ROBOT_MIN_Y 100
#define ROBOT_MAX_X 540
#define ROBOT_MAX_Y 440

// All units in milimeters
#define ROBOT_CENTER_X 300   // Center of robot. The table is 600x1000mm, so center is 300,500
#define ROBOT_CENTER_Y 500

// Initial robot position in mm
// The robot must be at this position at start time
#define ROBOT_INITIAL_POSITION_X 300
#define ROBOT_INITIAL_POSITION_Y 100

#define POSITION_TOLERANCE 4 // 5 steps

// PS3 Camera pixels
// NOTE: We are using the camera at 320x240 but we are outputing a 640x480 pixel equivalent position
#define CAM_PIX_WIDTH 640
#define CAM_PIX_HEIGHT 480
#define CAM_PIX_TO_MM 1.4    // Camera scale To calculate, you need to measure two points with known coordinates

// Utils
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))

#define RAD2GRAD 57.2957795
#define GRAD2RAD 0.01745329251994329576923690768489

#define ZERO_SPEED 65535

