// AHR AIR HOCKEY ROBOT PROJECT

// USER CONFIGURATIONS HERE
// ROBOT DIMENSIONS, MAX SPEED, MAX ACCELERATION, CALIBRATION

// THIS VALUES DEPENDS ON THE VOLTAGE, MOTORS, PULLEYS AND ROBOT CONSTRUCTION
// RECOMMENDED VALUES FOR 12V POWER SUPPLY
#define MIN_ACCEL_X 60
#define MAX_ACCEL_X 300   //360 //300//320      // Maximun motor acceleration in (steps/seg2)/1000
#define MIN_ACCEL_Y 60
#define MAX_ACCEL_Y 145    //140//220 
#define MAX_SPEED_X 25000     //max 25000 for 12V   // Maximun speed in steps/seg
#define MAX_SPEED_Y 25000

// RECOMMENDED VALUES FOR 15V POWER SUPPLY
//#define MIN_ACCEL_X 40
//#define MAX_ACCEL_X 380   //360 //300//320      // Maximun motor acceleration in (steps/seg2)/1000
//#define MIN_ACCEL_Y 70
//#define MAX_ACCEL_Y 200    //140//220 
//#define MAX_SPEED_X 28000     //max 25000 for 12V   // Maximun speed in steps/seg
//#define MAX_SPEED_Y 28000


// This is for the Accel ramp implementation (to smooth the intial acceleration), simplified S-profile
#define ACCEL_RAMP_MIN 2500  // The S profile is generated up to this speed
#define ACCEL_RAMP_MAX 10000

// UNCOMMENT THIS LINES TO INVERT MOTORS
#define INVERT_X_AXIS 1
//#define INVERT_Y_AXIS 1  //Y-LEFT
//#define INVERT_Z_AXIS 1  //Y_RIGHT

// Geometric calibration.
// This depends on the pulley teeth. For 42 teeth GT2 => 19, for 40 teeth GT2 => 20, for 16 teeth T5 => 20
#define X_AXIS_STEPS_PER_UNIT 19    // With 42 teeth GT2 pulley and 1/8 microstepping on drivers
#define Y_AXIS_STEPS_PER_UNIT 19    // 200*8 = 1600 steps/rev = 1600/42teeth*2mm = 19.047, using 19 is an error of 1mm every 40cm not too much! and we use int operations...

// Absolute Min and Max robot positions in mm (measured from center of robot pusher)
#define ROBOT_MIN_X 100
#define ROBOT_MIN_Y 80
#define ROBOT_MAX_X 500
#define ROBOT_MAX_Y 400

// This is the center of the table. All units in milimeters
#define ROBOT_CENTER_X 300   // Center of robot. The table is 600x1000mm, so center is 300,500
#define ROBOT_CENTER_Y 500

// Initial robot position in mm
// The robot must be at this position at start time
// Default: Centered in X and minimun position in Y
#define ROBOT_INITIAL_POSITION_X 300
#define ROBOT_INITIAL_POSITION_Y 45   // Measured from center of the robot pusher to the table border

// Robot defense and attack lines
#define ROBOT_DEFENSE_POSITION 95
#define ROBOT_DEFENSE_ATTACK_POSITION 220

#define POSITION_TOLERANCE 5 // 5 steps

// PS3 Camera pixels
// NOTE: We are using the camera at 320x240 but we are outputing a 640x480 pixel equivalent position
#define CAM_PIX_WIDTH 640
#define CAM_PIX_HEIGHT 480
#define CAM_PIX_CENTER_X 320
#define CAM_PIX_CENTER_Y 240

// IMPORTANT: THIS VALUE IS FOR A 87.5 cm CARBON TUBE!!
// Camera geometric calibration
// Measure two known position (in mm) calculare the pixels on camera and generate divide: mm/pix
#define CAM_PIX_TO_MM 1.48    // Camera scale To calculate, you need to measure two points with known coordinates

// CORRECTION OF MISSING STEPS ON MOTORS
// Coment this lines if you don´t want to make the corrections
#define CORRECT_MISSING_STEPS_X 1
#define CORRECT_MISSING_STEPS_Y 1

#define MISSING_STEPS_MAX_ERROR_X 8
#define MISSING_STEPS_MAX_ERROR_Y 10
#define ROBOT_POSITION_CAMERA_CORRECTION_Y -20 // Correction of the position of the camera because the camera point of view and mark position

// AIR HOCKEY TABLE FANS SPEED
// USE 255 for FULL SPEED (if you use 15V power supply 180 is ok)
#define FAN1_SPEED 255  //180
#define FAN2_SPEED 255  //180

// Utils (don´t modify)
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))

#define RAD2GRAD 57.2957795
#define GRAD2RAD 0.01745329251994329576923690768489

#define ZERO_SPEED 65535

