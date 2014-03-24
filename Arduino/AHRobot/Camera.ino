// AHR AIR HOCKEY ROBOT PROJECT

// Camera serial packet read and Puck trayectory prediction
// Robot position detection and missing steps check control

uint16_t extractParamInt(uint8_t pos){
  union{
    unsigned char Buff[2];
    uint16_t d;
  }
  u;

  u.Buff[0] = (unsigned char)SBuffer[pos];
  u.Buff[1] = (unsigned char)SBuffer[pos+1];
  return(u.d); 
}

// Read Vision System Packets over Serial Port
void packetRead()
{
  unsigned char i;
  if (Serial.available() > 0) {
    //Serial.println("P");
    // We rotate the Buffer (we could implement a ring buffer in future)
    for (i=11;i>0;i--){
      SBuffer[i] = SBuffer[i-1];
    }
    SBuffer[0] = Serial.read();
    //Serial.print(S1Buffer[0]);
    // We look for a  message start like "mm" to sync packets
    if ((SBuffer[0] == 'm')&&(SBuffer[1] == 'm'))
    {
      if (readStatus == 0)
      {
        readStatus=1;
        readCounter=12;
      }
      else
      {
        Serial.println("S ERR");
        readStatus=1;
        readCounter=12;
      }
      return;
    }
    else if (readStatus==1)
    {
      readCounter--;   // Until we complete the packet
      if (readCounter<=0)   // packet complete!!
      {
        // Extract parameters
        cam_timestamp = extractParamInt(10);
        puckPixX = extractParamInt(8);
        puckPixY = extractParamInt(6);
        puckSize = extractParamInt(4);
        robotPixX = extractParamInt(2);
        robotPixY = extractParamInt(0);  
        readStatus = 0;
        newPacket = 1;
        //Serial.println("P");
      }
    }
  }
}


// This process takes the puck position from camera and calculate puck position in robot reference system 
// and trajectory prediction. time in ms
void cameraProcess(int posX, int posY, int time)
{
  int coordX;
  int coordY;
  int vectorX;
  int vectorY;
  double slope;

  int bounce_x;
  int bounce_y;

  // Convert from Camera reference system to Robot reference system
  // We suppose very small camera angle rotatios (less than 5 degrees) so we use the 
  // aproximation that sin(cam_rotation) = cam_rotation (in radians)
  // Camera X axis correspond to robot Y axis
  coordY = (posX - cam_center_x);   // First we convert image coordinates to center of image
  coordX = (posY - cam_center_y);

  coordY = ROBOT_CENTER_Y - coordY*cam_pix_to_mm;
  coordX = ROBOT_CENTER_X - coordX*cam_pix_to_mm*(1-cam_rotation);

  // Speed calculation on each axis
  vectorX = (coordX-puckCoordX);
  vectorY = (coordY-puckCoordY);
  
  puckOldCoordX = puckCoordX;
  puckOldCoordY = puckCoordY;
  puckCoordX = coordX;
  puckCoordY = coordY;
  
  // Noise detection, if there are a big vector this should be noise
  if ((vectorY<-160)||(vectorY>160)||(vectorX>160)||(vectorX<-160))
  {
    Serial.println("NOISE");
    predict_status = -1;
    predict_x_old = -1;
    return;
  }

  puckOldSpeedX = puckSpeedX;
  puckOldSpeedY = puckSpeedY;
  puckSpeedX = vectorX*100/time;  // speed in dm/ms (we use this units to not overflow the variable)
  puckSpeedY = vectorY*100/time;
  if (predict_status == -1)  // Noise on last reading?
      {
      puckSpeedXAverage = puckSpeedX;
      puckSpeedYAverage = puckSpeedY;
      }
    else
      {
      if (myAbs(puckSpeedX - puckOldSpeedX)<50)
        puckSpeedXAverage = (puckSpeedX + puckOldSpeedX)>>1;
      else
        puckSpeedXAverage = puckSpeedX;
      if (myAbs(puckSpeedY - puckOldSpeedY)<50)
        puckSpeedYAverage = (puckSpeedY + puckOldSpeedY)>>1;
      else
        puckSpeedYAverage = puckSpeedY;
      } 

  //puckSpeed = sqrt(vectorX*vectorX + vectorY*vectorY)*1000.0/time;
  //puckDirection = atan2(vectorY,vectorX);
  
  predict_x_attack = -1;

  // It´s time to predict...
  // Based on actual position and move vector we need to know the future...
  // Posible impact? speed Y is negative when the puck is moving to the robot
  if (puckSpeedYAverage<-40)
  {
    predict_status = 1;
    // Puck is comming...
    // We need to predict the puck position when it reaches our goal Y position = defense_position
    // slope formula: m = (y2-y1)/(x2-x1)
    if (vectorX == 0)  // To avoid division by 0
        slope = 9999999;
    else
      slope = (float)vectorY/(float)vectorX;

    // x = (y2-y1)/m + x1
    predict_y = defense_position;
    predict_x = (predict_y-coordY)/slope;
    predict_x += coordX;

    predict_x_attack = (attack_position-coordY)/slope;
    predict_x_attack += coordX;

    // puck has a bounce with side wall?
    if ((predict_x<38)||(predict_x>562))
    {
      predict_status = 2;
      predict_bounce = 1;
      predict_bounce_status = 1;
      // We start a new prediction
      // Wich side?
      if (predict_x<38)
      {
        //Left side. We calculare the impact point
        bounce_x = 38;
      }
      else
      {
        //Right side. We calculare the impact point
        bounce_x = 562;
      }
      bounce_y = (bounce_x - coordX)*slope + coordY;
      predict_time = (bounce_y-puckCoordY)*100L/puckSpeedY;  // time until bouce
      // bounce prediction => slope change  with the bounce, we only need to change the sign, easy!!
      slope = -slope;
      predict_y = defense_position;
      predict_x = (predict_y-bounce_y)/slope;
      predict_x += bounce_x;

      if ((predict_x<38)||(predict_x>562))
      {
        // New bounce?? 
        // We do nothing then... with two bounces there are small risk of goal...
        predict_x_old = -1;
        predict_status = 0;
      }
      else
      {
        // one side bounce
        // If the puckSpeedY has changed a lot this mean that the puck has touch one side
        if (myAbs(puckSpeedY - puckOldSpeedY) > 50)
          {
          // We dont make a new prediction...
          }
        else
          {
          // average of the results (some noise filtering)
          if (predict_x_old != -1)
            predict_x = (predict_x_old + predict_x)>>1;
          predict_x_old = predict_x;
          // We introduce a factor (130 instead of 100) to model the bounce (30% loss in speed)(to improcve...)
          predict_time = predict_time + (predict_y-puckCoordY)*130L/puckSpeedY;  // in ms
          }
      }
    }
    else  // No bounce, direct impact
    {
      if (predict_bounce_status == 1)  // This is the first direct impact trajectory after a bounce
        {
        // We dont predict nothing new...
        predict_bounce_status = 0;
        }
      else
        {
        // average of the results (some noise filtering)
        if (predict_x_old != -1)
          predict_x = (predict_x_old + predict_x)>>1;
        predict_x_old = predict_x;

        predict_time = (predict_y-puckCoordY)*100L/puckSpeedY;  // in ms
        predict_time_attack = (attack_position-puckCoordY)*100L/puckSpeedY; // in ms
        }
    }
  }
  else // Puck is moving slowly or to the other side
  {	
    predict_x_old = -1;
    predict_status = 0;
    predict_bounce = 0;
    predict_bounce_status = 0;
  }
}

// Return the predicted position of the puck in predict_time miliseconds
int predictPuckXPosition(int predict_time)
{
  return (puckCoordX + (long)puckSpeedXAverage*predict_time/100L);
}

// Return the predicted position of the puck in predict_time miliseconds
int predictPuckYPosition(int predict_time)
{
  return (puckCoordY + (long)puckSpeedYAverage*predict_time/100L);
}

// Initialization routine
void cameraProcessInit()
{
  // Default values
  cam_center_x = CAM_PIX_CENTER_X;
  cam_center_y = CAM_PIX_CENTER_Y;
  cam_pix_to_mm = CAM_PIX_TO_MM;
  cam_rotation = 0; //radians  1Âº = 0.01745 2Âº = 0.035 4Âº = 0.07 5Âº = 0.087

  predict_x_old = -1;
}

// Robot position detection. Transformation from camera reference system (in pixels) to robot reference system
void robotDetection(int posX, int posY)
{
  int coordX;
  int coordY;

  // Convert from Camera reference system to Robot reference system
  // We suppose very small angle rotatios (less than 5 degrees) so we use the 
  // aproximation that sin cam_rotation = cam_rotation (in radians)
  // Camera X axis correspond to robot Y axis
  coordY = (posX - cam_center_x);   // First we convert image coordinates to center of image
  coordX = (posY - cam_center_y);

  coordY = ROBOT_CENTER_Y - coordY*cam_pix_to_mm;
  coordX = ROBOT_CENTER_X - coordX*cam_pix_to_mm*(1-cam_rotation);

  // Valid coordinates?
  if (coordX>40&&coordX<560&&coordY>40&&coordY<500)
  {
    robotCoordX = coordX;
    robotCoordY = coordY;
  }
  else
  {
    robotCoordX = 0;
    robotCoordY = 0;
  }
}

// Function to detect missing steps in steppers
// When the robot is stopped in a known position (defense position) we compare the estimated position from steppers with the position of the robot seen in the camera.
void missingStepsDetection()
{
  int robot_position_x_mm;
  int robot_position_y_mm;


  // If we are stopped and we have a good robot detection (camera)
  if ((speed_x == 0)&&(speed_y == 0)&&(robotCoordX != 0))
  {
    robot_position_x_mm = position_x/X_AXIS_STEPS_PER_UNIT;
    robot_position_y_mm = position_y/Y_AXIS_STEPS_PER_UNIT;
    // Are we near center and near defense position?
    if ((robot_position_x_mm  > (ROBOT_CENTER_X-40))&&(robot_position_x_mm  < (ROBOT_CENTER_X+40))&&(robot_position_y_mm  >= ROBOT_MIN_Y)&&(robot_position_y_mm  < (ROBOT_DEFENSE_POSITION+30)))
    {
      robotCoordSamples++;
      robotCoordXAverage += robotCoordX;
      robotCoordYAverage += robotCoordY;
      // When we collect 10 samples we make the correction
      if (robotCoordSamples == 10)
        {
        // X axis
        robotCoordXAverage = robotCoordXAverage/robotCoordSamples;
        robotMissingStepsErrorX = myAbs(robot_position_x_mm - robotCoordXAverage);  // in milimeters)
        if (robotMissingStepsErrorX > MISSING_STEPS_MAX_ERROR_X) 
        {
          // Missing steps detected on X axis!! We need to correct this...
          #ifdef CORRECT_MISSING_STEPS_X
          position_x = robotCoordXAverage*X_AXIS_STEPS_PER_UNIT;
          Serial.print("MSX ");
          Serial.println(robotMissingStepsErrorX);
          #endif
        }
        // Y AXIS
        robotCoordYAverage = robotCoordYAverage/robotCoordSamples;
        robot_position_y_mm += ROBOT_POSITION_CAMERA_CORRECTION_Y;   // correction because camera point of view and robot mark
        robotMissingStepsErrorY = myAbs(robot_position_y_mm - robotCoordYAverage);
        if (robotMissingStepsErrorY > MISSING_STEPS_MAX_ERROR_Y) 
        {
          // Missing steps detected on Y axis!! We need to correct this...
          #ifdef CORRECT_MISSING_STEPS_Y
          position_y = robotCoordYAverage*Y_AXIS_STEPS_PER_UNIT;
          Serial.print("MSY ");
          Serial.println(robotMissingStepsErrorY);
          #endif 
        }
      }    
    }
    else
    {
      robotCoordSamples = 0;
      robotCoordXAverage = 0;
      robotCoordYAverage = 0;
      robotMissingStepsErrorX = 0;
      robotMissingStepsErrorY = 0;
    }
  }
  else
  {
    robotCoordSamples = 0;
    robotCoordXAverage = 0;
    robotCoordYAverage = 0;
    robotMissingStepsErrorX = 0;
    robotMissingStepsErrorY = 0;
  }
}


