// AHR AIR HOCKEY ROBOT PROJECT

// Each time a new data packet from camera is reveived this function is called
void newDataStrategy()
{
  // predict_status == 1 => Puck is moving to our field directly
  // predict_status == 2 => Puck is moving to our field with a bounce
  // predict_status == 3 => Puck is in our field moving slowly, attack?
    
    // Default
    robot_status = 0;   // Going to initial position (defense)
    
    if ((predict_status==1)&&(predict_time<350))
      {
      // WE  come from a bounce?
      if (predict_bounce == 1)
        {
        if ((puckSpeedYAverage)>-150)
          // puck is moving slowly...
          robot_status = 2;  // Defense+Attack
        else
          {
          if (predict_x < 200)
            predict_x = 200;
          if (predict_x > 400)
            predict_x = 400;
          robot_status = 4;
          }
        }
      else
        {
        if ((predict_x > (ROBOT_MIN_X+35))&&(predict_x < (ROBOT_MAX_X-35))) 
          robot_status = 2; //2  //Defense+attack mode
        else
          robot_status = 1; //1
        }
      }
    
    // Prediction with side bound
    if ((predict_status==2)&&(predict_time<350))
      {
      robot_status = 1;  //1   // Defense mode
      // We limit the movement in this phase
      if (predict_x < 200)
        predict_x = 200;
      if (predict_x > 400)
        predict_x = 400;
      }
   
    // If the puck is moving slowly in the robot field we could start an attack
    if ((predict_status==0)&&(puckCoordY < ROBOT_CENTER_Y)&&(myAbs(puckSpeedY)<50))
      robot_status = 3; //3
      
}

void robotStrategy()
{
  switch(robot_status) {
      case 0:
        // Go to defense position
        com_pos_y = defense_position;
        com_pos_x = ROBOT_CENTER_X;  //center X axis
        setSpeedS(com_speed_x,com_speed_y);
        setPosition(com_pos_x,com_pos_y);
        attack_time = 0;
        break;
      case 1:
        // Defense mode (only move on X axis on the defense line)
        com_pos_y = defense_position;
        com_pos_x = predict_x;
        setSpeedS(com_speed_x,com_speed_y);
        setPosition(com_pos_x,com_pos_y);
        attack_time = 0;
        break;
      case 2:
        // Defense+attack
        if (predict_time_attack<180)  // If time is less than 180ms we start the attack
          {
          com_pos_y = attack_position + 80;
          com_pos_x = predict_x_attack;
          setSpeedS(com_speed_x,com_speed_y);
          setPosition(com_pos_x,com_pos_y); // We use a straight line path
          }
        else      // Defense position
          {
          com_pos_y = predict_y;
          com_pos_x = predict_x;  // predict_x_attack;
          setSpeedS(com_speed_x,com_speed_y);
          setPosition(com_pos_x,com_pos_y);
          attack_time = 0;
          }
        
        break;
      case 3:
        // ATTACK MODE
        if (attack_time == 0)
          {
          attack_predict_x = predictPuckXPosition(500);
          attack_predict_y = predictPuckYPosition(500);
          if ((attack_predict_x > 120)&&(attack_predict_x < 480)&&(attack_predict_y >180)&&(attack_predict_y<320))
            {
            attack_time = millis() + 500;  // Prepare an attack in 500ms
            attack_pos_x = attack_predict_x;  // predict_x
            attack_pos_y = attack_predict_y;  // predict_y
            Serial.print("AM:");
            //Serial.print(attack_time);
            //Serial.print(",");
            Serial.print(attack_pos_x);
            Serial.print(",");
            Serial.println(attack_pos_y);
            //Serial.print(" ");
            // Go to pre-attack position
            com_pos_x = attack_pos_x;
            com_pos_y = attack_pos_y-120;
            setSpeedS(com_speed_x/2,com_speed_y/2);
            setPosition(com_pos_x,com_pos_y);
            attack_status = 1;
            }
          else
            {
            attack_time = 0;  // Continue...
            attack_status = 0;
            // And go to defense position
            com_pos_y = defense_position;
            com_pos_x = ROBOT_CENTER_X;  //center X axis
            setSpeedS(com_speed_x/2,com_speed_y/2);
            setPosition(com_pos_x,com_pos_y);
            }
          }
        else
          {
          if (attack_status == 1)
            {
            if ((attack_time-millis())<200)
              {
              // Attack movement
              com_pos_x = predictPuckXPosition(200); 
              com_pos_y = predictPuckYPosition(200) + 80;
              setSpeedS(com_speed_x,com_speed_y);
              setPosition(com_pos_x,com_pos_y);
        
              Serial.print("ATTACK:");
              Serial.print(com_pos_x);
              Serial.print(",");
              Serial.println(com_pos_y-80);
                
              attack_status = 2; // Attacking      
              }
            else  // attack_status=1 but it´s no time to attack yet
              {
              // Go to pre-attack position
              com_pos_x = attack_pos_x;
              com_pos_y = attack_pos_y-120;
              setSpeedS(com_speed_x/2,com_speed_y/2);
              setPosition(com_pos_x,com_pos_y);
              }
            }
          if (attack_status==2)
            {
            if (millis()>(attack_time+80))  // Attack move is done? => Reset to defense position
              {
              Serial.print("RESET");
              attack_time = 0;
              robot_status = 0;
              attack_status = 0;
              }
            }
          }
        break;
      case 4:
        // The puck came from a bounce
        // Only defense now (we could improve this in future)
        // Defense mode (only move on X axis on the defense line)
        com_pos_y = defense_position;
        com_pos_x = predict_x;
        setSpeedS(com_speed_x,com_speed_y);
        setPosition(com_pos_x,com_pos_y);
        attack_time = 0;
        break;
      default:
        // Default : go to defense position
        com_pos_y = defense_position;
        com_pos_x = ROBOT_CENTER_X; // center
        setSpeedS(com_speed_x,com_speed_y);
        setPosition(com_pos_x,com_pos_y);
        attack_time = 0;
    }
}
