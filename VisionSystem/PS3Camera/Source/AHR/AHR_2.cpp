// Air Hockey Robot project
// Computer Vision for PS3 EYE Camera (OpenCV)
// Author: Jose Julio
// Date: Dec 2013 - Jan 2014

// This code needs the OpenCV libraries

// This code detects the puck and the robot in the hockey game at 60Hz
// Detects two objects PUCK and ROBOT with two different colors
// And sends serial packets to arduino at 115200 bauds
// Serial packet:
//     Sync start: 2 bytes: mm
//     TimeStamp: 2 bytes
//     Pos_X:     2 bytes (0-640)
//     Pos_Y:     2 bytes (0-480)
//     Object size:     2 bytes (0-400)
//     Robot Pos_X:     2 bytes (0-640)
//     Robot Pos_Y:     2 bytes (0-480)

// Steps: 
// Capture a frame, convert to HSV, 
// For both PUCK and ROBOT:
//    threshold based on HSV range, Object extraction, roundness and size filter, Center calculation
// For the PUCK we make a trajectory prediction to draw the lines...
// We record a video in the file output.mpeg and a log file with the received serial messages from robot

// H : 70-94 S: 60-150 V: 10-145   GREEN EVA FOAM
// H : 5-20  S: 110-200 V: 90-200  ORANGE EVA FOAM

// Default parameters are for a GREEN EVA FOAM PUCK and an ORANGE EVA FOAM ROBOT MARK

// PS3 Camera windows driver from : http://codelaboratories.com/products/eye/driver/

#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <time.h>
#include <Windows.h>
#include <stdio.h>
#include <math.h>

// All units in milimeters
#define robot_center_x 300   // Center of robot
#define robot_center_y 500

// PS3 Camera pixels
// NOTE: We are using the camera at 320x240 but we are outputing a 640x480 pixel position
#define CAM_PIX_WIDTH 640
#define CAM_PIX_HEIGHT 480
#define CAM_PIX_TO_MM 1.4

time_t start,end;

// Parameters (with default values)
char comPort[20] = "COM19";
int minH=70;
int maxH=94;
int minS=60;
int maxS=150;
int minV=10;
int maxV=145;
int RminH=5;
int RmaxH=20;
int RminS=110;
int RmaxS=200;
int RminV=90;
int RmaxV=200;
int fps=60;
int viewWindow=1;

// OpenCV variables
CvFont font;
CvVideoWriter* record;
IplImage* imgTracking;
IplImage* imgThresh;
IplImage* imgThresh2;
IplImage* frameGrabbed=0;
IplImage* frame=0;
int lastX = -1;
int lastY = -1;
int posX;
int posY;
int objectSize;
int RposX;
int RposY;
int RobjectSize;
int status;
// time variables
DWORD frameTimestamp=0;
DWORD firstTimestamp=0;
DWORD oldFrameTimestamp;

HANDLE serialPort;  // Serial port
BYTE message[20];   // BYTES buffer

FILE* logFile;
char tempStr[80];
char logStr[4096];

// Trajectory prediction
// Puck variables
int puckCoordX;
int puckCoordY;
int puckOldCoordX;
int puckOldCoordY;
int puckSpeedX;
int puckSpeedY;
float puckSpeed;         // mm/sec
float puckDirection;     // radians

int defense_position;
int predict_x;    // X position at impact (mm)
int predict_y;
int predict_x_old;
int predict_y_old;
int predict_time;   // time to impact in ms
char tempStr2[80];

// Camera variables
int cam_center_x;
int cam_center_y;
float cam_pix_to_mm;
float cam_rotation;  //Camera rotation in radians 


// Camera process, convert puck position to coordinates and generate trajectory prediction and visualization
// Simple lens distortion correction (one parameter) NOT USED NOW
// Xu = (Xd)/(1+param*dist2)  dist2 = distancia al cuadrado del pixel al centro
void cameraProcess(int PosX, int PosY, int time)
{
  int coordX;
  int coordY;
  int vectorX;
  int vectorY;
  double slope;

  int bounce_x;
  int bounce_y;
  int predict_pixX;
  int predict_pixY;
  int bounce_pixX;
  int bounce_pixY;

  // Convert from Camera reference system to Robot reference system
  // We suppose very small angle rotatios (less than 5 degrees) so we use the 
  // aproximation that sin cam_rotation = cam_rotation (in radians)
  // Camera X axis correspond to robot Y axis
  coordY = (posX - cam_center_x);   // First we convert image coordinates to center of image
  coordX = (posY - cam_center_y);
  
  coordY = robot_center_y - coordY*cam_pix_to_mm;
  coordX = robot_center_x - coordX*cam_pix_to_mm*(1-cam_rotation);
  
  // Calculate speed and angle
  vectorX = (coordX-puckCoordX);
  vectorY = (coordY-puckCoordY);
 
  puckSpeedX = vectorX*100/time;  // speed in dm/ms (
  puckSpeedY = vectorY*100/time;
  //puckSpeed = sqrt(vectorX*vectorX + vectorY*vectorY)*1000.0/time;
  //puckDirection = atan2(vectorY,vectorX);
  puckOldCoordX = puckCoordX;
  puckOldCoordY = puckCoordY;
  puckCoordX = coordX;
  puckCoordY = coordY;

  // Noise detection, big vector...
  if ((vectorY<-100)||(vectorY>100)||(vectorX>100)||(vectorX<-100))
  {
	  sprintf(tempStr2,"NOISE %d",vectorY);
	  return;
  }
  
  // Its time to predict...
  // Based on actual position, speed and angle we need to know the future...
  // Posible impact?
  if (puckSpeedY<-50)
    {
      // Puck is comming...
      // We need to predict the puck position when it reaches our goal Y=0
      // slope formula: m = (y2-y1)/(x2-x1)
	  if (vectorX == 0)  // To avoid division by 0
		slope = 9999999;
	  else
		slope = (float)vectorY/(float)vectorX;
      // x = (y2-y1)/m + x1
	  predict_y = defense_position;
	  predict_x = (predict_y-coordY)/slope + coordX;
	 
	  // puck has a bounce with side wall?
      if ((predict_x<38)||(predict_x>562))
        {
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
		bounce_pixX = cam_center_x - (bounce_y-robot_center_y)/cam_pix_to_mm;
		bounce_pixY = cam_center_y - (bounce_x-robot_center_x)/(cam_pix_to_mm*(1-cam_rotation));
		predict_time = (bounce_y-puckCoordY)*100/puckSpeedY;  // time until bouce
		// bounce prediction
		// Slope change
		slope = -slope;
		predict_y = defense_position;
		predict_x = (predict_y-bounce_y)/slope + bounce_x;

		if ((predict_x<38)||(predict_x>562))
			{
			// New bounce?? 
			// We do nothing then...
			sprintf(tempStr2,"2B %d %d",bounce_x,bounce_y);
			predict_x_old = -1;
			}
		else
			{
			// draw line
			cvLine(frameGrabbed, cvPoint(posX/2, posY/2), cvPoint(bounce_pixX/2, bounce_pixY/2), cvScalar(255,0,0), 2);
			// result average
			if (predict_x_old != -1)
				predict_x = (predict_x_old + predict_x)>>1;
			predict_x_old = predict_x;
			predict_time = predict_time + (predict_y-puckCoordY)*100/puckSpeedY;  // in ms
			sprintf(tempStr2,"%d;%d %d %d t%d",coordX,coordY,predict_x,puckSpeedY,predict_time);
			predict_pixX = cam_center_x - (predict_y-robot_center_y)/cam_pix_to_mm;
			predict_pixY = cam_center_y - (predict_x-robot_center_x)/(cam_pix_to_mm*(1-cam_rotation));
			// draw line
			cvLine(frameGrabbed, cvPoint(bounce_pixX/2, bounce_pixY/2), cvPoint(predict_pixX/2, predict_pixY/2), cvScalar(0,255,0), 2);
			}
		 }
      else  // No bounce, direct impact
        {
		// result average
		if (predict_x_old != -1)
			predict_x = (predict_x_old + predict_x)>>1;
		predict_x_old = predict_x;
		
		predict_time = (predict_y-puckCoordY)*100/puckSpeedY;  // in ms
		sprintf(tempStr2,"%d;%d %d %d t%d",coordX,coordY,predict_x,puckSpeedY,predict_time);
		// Convert impact prediction position to pixels (to show on image)
		predict_pixX = cam_center_x - (predict_y-robot_center_y)/cam_pix_to_mm;
		predict_pixY = cam_center_y - (predict_x-robot_center_x)/(cam_pix_to_mm*(1-cam_rotation));
		// draw line
		cvLine(frameGrabbed, cvPoint(posX/2, posY/2), cvPoint(predict_pixX/2, predict_pixY/2), cvScalar(0,255,0), 2);
        }
    }
  else // Puck is moving slowly or to the other side
	{	
	sprintf(tempStr2,"NO %d,%d %d",coordX,coordY,puckSpeedY);
	predict_x_old = -1;
	}

}

void cameraProcessInit()
{
	  // Default values
  cam_center_x = CAM_PIX_WIDTH/2;
  cam_center_y = CAM_PIX_HEIGHT/2;
  cam_pix_to_mm = CAM_PIX_TO_MM;
  cam_rotation = 0; //radians  1º = 0.01745 2º = 0.035 4º = 0.07 5º = 0.087
  predict_x_old = -1;
  defense_position = 100;  // Pusher defense position
}


//This function threshold the HSV image and create a binary image
IplImage* GetThresholdedImage(IplImage* imgHSV, int minH, int maxH, int minS, int maxS, int minV, int maxV){       
    IplImage* imgThresh=cvCreateImage(cvGetSize(imgHSV),IPL_DEPTH_8U, 1);
    cvInRangeS(imgHSV, cvScalar(minH,minS,minV), cvScalar(maxH,maxS,maxV), imgThresh); 
    return imgThresh;
}


// Image segmentation, object extraction based on size and form (roundness)
void trackObjectPuck(IplImage* imgThresh){

 CvSeq* contours;  //hold the pointer to a contour in the memory block
 CvSeq* result;   //hold sequence of points of a contour
 CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours

 // Position initialization
 posX = 0;
 posY = 0;
 status = 0;
 
 //finding all contours in the image (segmentation)
 cvFindContours(imgThresh, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
 if (contours)
	status=1;
 while (contours)
 {
	 double area = cvContourArea(contours);

	 if ((area>20)&&(area<400))  // Min and Max size of object
	 {
		status = 2;
        //Detecting roundness   roundness = perimeter2/(2*pi*area)
		double perimeter = cvArcLength(contours);
		double roundness = (perimeter*perimeter)/(6.28*area);
		//printf("%lf",roundness);
		if (roundness < 8)
		{
			status = 3;
			CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
			cvMoments(contours, moments, 1);
			double moment10 = cvGetSpatialMoment(moments, 1, 0);
			double moment01 = cvGetSpatialMoment(moments, 0, 1);
			area = cvGetCentralMoment(moments, 0, 0);
		
			// Calculate object center
			// We are using 320x240 pix but we are going to output the 640x480 equivalent (*2)
			posX = floor(moment10*2/(double)area+0.5); // round
			posY = floor(moment01*2/(double)area+0.5);
			objectSize = area;
			if(lastX>=0 && lastY>=0 && posX>=0 && posY>=0)
			{
				status = 4;
				// Draw a line from the previous point to the current point
				cvLine(frameGrabbed, cvPoint(posX/2, posY/2), cvPoint(lastX/2, lastY/2), cvScalar(255,255,0), 4);
				// Draw contour
				cvDrawContours(frameGrabbed,contours,cvScalar(255,0,0),cvScalar(0,0,255),0,1,8,cvPoint(0,0));
			}
			lastX = posX;
			lastY = posY;
		}
	 }

	 //obtain the next contour
     contours = contours->h_next; 
 }
 //printf("\n");
 cvReleaseMemStorage(&storage);
}

// Image segmentation, object extraction based on size and form (roundness)
void trackObjectRobot(IplImage* imgThresh){

 CvSeq* contours;  //hold the pointer to a contour in the memory block
 CvSeq* result;   //hold sequence of points of a contour
 CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours

 // Position initialization
 RposX = 0;
 RposY = 0;
 RobjectSize = 0;
 
 //finding all contours in the image (segmentation)
 cvFindContours(imgThresh, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
 while (contours)
 {
	 double area = cvContourArea(contours);

	 if ((area>18)&&(area<100))  // Min and Max size of object
	 {
        //Detecting roundness   roundness = perimeter2/(2*pi*area)
		double perimeter = cvArcLength(contours);
		double roundness = (perimeter*perimeter)/(6.28*area);
		//printf("%lf",roundness);
		if (roundness < 4)
		{
			//printf("area:%.2lf R:%.2lf\n",area,roundness);
			CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
			cvMoments(contours, moments, 1);
			double moment10 = cvGetSpatialMoment(moments, 1, 0);
			double moment01 = cvGetSpatialMoment(moments, 0, 1);
			area = cvGetCentralMoment(moments, 0, 0);
		
			// Calculate object center
			// We are using 320x240 pix but we are going to output the 640x480 equivalent (*2)
			RposX = floor(moment10*2/(double)area+0.5); // round
			RposY = floor(moment01*2/(double)area+0.5);
			RobjectSize = area;
			if(RposX>=0 && RposY>=0)
			{
				cvLine(frameGrabbed, cvPoint(RposX/2, RposY/2), cvPoint(RposX/2, RposY/2), cvScalar(100,255,50), 4);
				cvDrawContours(frameGrabbed,contours,cvScalar(0,0,100),cvScalar(100,0,0),0,1,8,cvPoint(0,0));
			}
		}
	 }

	 //obtain the next contour
     contours = contours->h_next; 
 }
 cvReleaseMemStorage(&storage);
}


bool openComPort(wchar_t* portSpecifier)
{
	DCB dcb;

	serialPort = CreateFile(portSpecifier,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);

	if (!GetCommState(serialPort,&dcb))
		return(false);

	// Serial port configuration
	dcb.BaudRate = CBR_115200;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	if (!SetCommState(serialPort,&dcb))
		return(false);
}

bool writeComPort(BYTE *message,int length)
{
	DWORD byteswritten;

	bool retVal = WriteFile(serialPort,message,length,&byteswritten,NULL);
	return retVal;
}

// Read from COM PORT and output to console
bool readComPort()
{
	DWORD dwRead;
	char m_pDataBuf[4096];
	BYTE unbyte;
	DWORD temp; 
    COMSTAT comstat;
	BOOL readResult;

	// Get Serial stats
	ClearCommError(serialPort,&temp,&comstat);

	// New bytes pending read?
	if (comstat.cbInQue>0)
		{
		ReadFile(serialPort,m_pDataBuf, comstat.cbInQue, &dwRead, NULL);
		m_pDataBuf[dwRead] = 0;
		sprintf(tempStr,"[%ld]",frameTimestamp-firstTimestamp);
		fwrite(tempStr,strlen(tempStr),1,logFile);   
		fwrite(m_pDataBuf,dwRead,1,logFile);
		printf("%s\n",m_pDataBuf);
		sprintf(logStr,"%s",m_pDataBuf);
		}
	return true;
}


// Send message to Serial port with the object information 14 bytes message
bool sendMessage()
{
	DWORD timestamp;
	// Initial sync 0x6D 0x6D
	message[0] = 0x6D;
	message[1] = 0x6D;
	// TimeStamp
	timestamp = frameTimestamp - firstTimestamp;
	message[2] = (timestamp>>8)&0xFF;
	message[3] = timestamp&0xFF;
	// Pos_X (high byte, low byte)
	message[4] = (posX>>8)&0xFF;
	message[5] = posX&0xFF;
	// Pos_Y (high byte, low byte)
	message[6] = (posY>>8)&0xFF;
	message[7] = posY&0xFF;
	// Object Size
	message[8] = (objectSize>>8)&0xFF;
	message[9] = objectSize&0xFF;
	// Robot Pos_X (high byte, low byte)
	message[10] = (RposX>>8)&0xFF;
	message[11] = RposX&0xFF;
	// Robot Pos_Y (high byte, low byte)
	message[12] = (RposY>>8)&0xFF;
	message[13] = RposY&0xFF;

	return writeComPort(message,14); // Send message (14 bytes)
}


int main(int argc, char* argv[]){
	int counter;
	wchar_t auxstr[20];

	cameraProcessInit();
	// Parse arguments
	if (argc < 2)
	{
		printf("USAGE: AHR.exe COMPORT PUCK_HMIN PUCK_HMAX PUCK_SMIN PUCK_SMAX PUCK_VMIN PUCK_VMAX ROBOT_HMIN ROBOT_HMAX ROBOT_SMIN ROBOT_SMAX ROBOT_VMIN ROBOT_VMAX FPS\n");
		printf("Example with default paremeters: AHR.exe COM18\n");
		// H : 70-94 S: 60-150 V: 10-145   GREEN EVA FOAM (PUCK)
		// H : 5-20  S: 110-200 V: 90-200  ORANGE EVA FOAM (ROBOT)
		printf("Example full parameters: AHR.exe COM19 70 94 60 150 10 145 5 20 110 200 90 200 60\n");
		return -1;
	}
	
	// COM port 
	swprintf_s(auxstr,L"\\\\.\\%S",argv[1]);

	// Other parameters
	if (argc==15)  // Full parameters
	{
		minH = atoi(argv[2]);
		maxH = atoi(argv[3]);
		minS = atoi(argv[4]);
		maxS = atoi(argv[5]);
		minV = atoi(argv[6]);
		maxV = atoi(argv[7]);
		RminH = atoi(argv[8]);
		RmaxH = atoi(argv[9]);
		RminS = atoi(argv[10]);
		RmaxS = atoi(argv[11]);
		RminV = atoi(argv[12]);
		RmaxV = atoi(argv[13]);
		fps  = atoi(argv[14]);
	}

	// SHOW parameters:
	printf("PUCK MINH: %d\n",minH);
	printf("PUCK MAXH: %d\n",maxH);
	printf("PUCK MINS: %d\n",minS);
	printf("PUCK MAXS: %d\n",maxS);
	printf("PUCK MINV: %d\n",minV);
	printf("PUCK MAXV: %d\n",maxV);
	printf("ROBOT MINH: %d\n",RminH);
	printf("ROBOT MAXH: %d\n",RmaxH);
	printf("ROBOT MINS: %d\n",RminS);
	printf("ROBOT MAXS: %d\n",RmaxS);
	printf("ROBOT MINV: %d\n",RminV);
	printf("ROBOT MAXV: %d\n",RmaxV);
	printf("FPS: %d\n",fps);

	// Open Serial Port
	wprintf(L"Opening COM PORT: %s",auxstr);
	printf("\n");
	
	// LogFile
	logFile = fopen("log.txt","wt");
	if (!logFile)
		printf("Error opening Log File!\n");

	CvCapture* capture =0;       
    capture = cvCaptureFromCAM(CV_CAP_ANY);

	// TAKE A VIDEO FROM FILE
	//capture = cvCaptureFromFile("pru.mpeg");
	//cvSetCaptureProperty(capture,CV_CAP_PROP_POS_MSEC,9000);//9700 //17500 //20200 //23500 //31200 //36000 //42700 //47000
	
    if(!capture){
       printf("Capture failure\n");
       return -1;
    }
    cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,320);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,240);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FPS,fps);
    frameGrabbed = cvQueryFrame(capture);           
    if(!frameGrabbed) return -1;
	
	// Get some properties
	// This properties dont work...
	//printf("CAMERA GAIN: %ld\n",cvGetCaptureProperty(capture,CV_CAP_PROP_GAIN));
	//printf("CAMERA EXPOSURE: %ld\n",cvGetCaptureProperty(capture,CV_CAP_PROP_EXPOSURE));
	frameGrabbed = cvQueryFrame(capture);
    firstTimestamp = GetTickCount();
	frameTimestamp = firstTimestamp;
    //create a blank image and assgned to 'imgTracking' which has the same size of original video
	frame=cvCreateImage(cvGetSize(frameGrabbed),IPL_DEPTH_8U, 3);
    imgTracking=cvCreateImage(cvGetSize(frameGrabbed),IPL_DEPTH_8U, 3);
    cvZero(imgTracking); //covert the image, 'imgTracking' to black
	
	cvNamedWindow("Video");     
    //cvNamedWindow("Processed");
	cvWaitKey(1200);
	openComPort(auxstr);  // L"\\\\.\\COM19");
	cvWaitKey(1000);
	
	//iterate through each frames of the video    

	// Video writer MPG1
	record = cvCreateVideoWriter("output.mpeg", CV_FOURCC('P','I','M','1'), 60, cvGetSize(frameGrabbed), 1);

	// Init font
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, 0.4,0.4,0,1);

    while(true){
			
		oldFrameTimestamp = frameTimestamp;
		frameGrabbed = cvQueryFrame(capture);  // Query a new frame       
		if(!frameGrabbed)
		{
			printf("No frames!\n");
			break;
		}

		frameTimestamp = GetTickCount(); // Get timestamp (not too much resolution)
		
		//printf("%ld\n",(frameTimestamp-oldFrameTimestamp));

		frameGrabbed=cvCloneImage(frameGrabbed); 
            
		cvSmooth(frameGrabbed, frame, CV_GAUSSIAN,3,3); //smooth the original image using Gaussian kernel

		IplImage* imgHSV = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3); 
		cvCvtColor(frame, imgHSV, CV_BGR2HSV); //Change the color format from BGR to HSV
		imgThresh = GetThresholdedImage(imgHSV,minH,maxH,minS,maxS,minV,maxV);
		imgThresh2 = GetThresholdedImage(imgHSV,RminH,RmaxH,RminS,RmaxS,RminV,RmaxV);

          	
		/// Apply the erosion-dilatation operation for filtering
		cvErode( imgThresh, imgThresh, NULL,1 );
		cvDilate( imgThresh, imgThresh, NULL,1 );
            
		//track the possition of the puck and the robot
		trackObjectPuck(imgThresh);
		trackObjectRobot(imgThresh2);

		// Send Message to Serial Port
		sendMessage();

		// Put text over image
		sprintf(tempStr,"%ld;%d;%d;%d",frameTimestamp-firstTimestamp,posX,posY,status);
		cvPutText (frameGrabbed, tempStr, cvPoint(10,20), &font, cvScalar(255,255,0));

		// TEST CAMERA PROCESS
		cameraProcess(posX,posY,16.66);
		cvPutText (frameGrabbed, tempStr2, cvPoint(150,20), &font, cvScalar(255,255,0));

		// LOG TEXT
		cvPutText (frameGrabbed, logStr, cvPoint(20,220), &font, cvScalar(50,220,220));

		//cvShowImage("Processed", imgThresh);           
		cvShowImage("Video", frameGrabbed);

		//Write image to output video
		cvWriteFrame(record,frameGrabbed);

		// Write to logFile
		//sprintf(tempStr,"%ld;%d;%d;%d\n",(frameTimestamp-oldFrameTimestamp),posX,posY,status);
		//fwrite(tempStr,strlen(tempStr),1,logFile);
           
		//Clean up used images
		cvReleaseImage(&imgHSV);
		cvReleaseImage(&imgThresh);  
		cvReleaseImage(&imgThresh2); 
		cvReleaseImage(&frameGrabbed);
		
		// Sometnig to read on SerialPort?
		readComPort();

		//Wait 1mS necesary???
		int c = cvWaitKey(1);
		//If 'ESC' is pressed, break the loop
		if((char)c==27 ) break;

		counter++;
    }

    cvDestroyAllWindows() ;
    cvReleaseImage(&imgTracking);
    cvReleaseCapture(&capture);
	cvReleaseVideoWriter(&record);

	fclose(logFile);

    return 0;
}