
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <time.h>

int lowerH=70;
int lowerS=80;
int lowerV=10;

int upperH=94;
int upperS=150;
int upperV=125;

//This function threshold the HSV image and create a binary image
IplImage* GetThresholdedImage(IplImage* imgHSV){
 
 IplImage* imgThresh=cvCreateImage(cvGetSize(imgHSV),IPL_DEPTH_8U, 1);
 cvInRangeS(imgHSV, cvScalar(lowerH,lowerS,lowerV), cvScalar(upperH,upperS,upperV), imgThresh); 
 
 return imgThresh;

}

//This function create two windows and 6 trackbars for the "Ball" window
void setwindowSettings(){
 cvNamedWindow("Video");
 cvNamedWindow("Ball");
 cvNamedWindow("Controls");

 cvCreateTrackbar("LowerH", "Controls", &lowerH, 180, NULL);
        cvCreateTrackbar("UpperH", "Controls", &upperH, 180, NULL);

  cvCreateTrackbar("LowerS", "Controls", &lowerS, 256, NULL);
        cvCreateTrackbar("UpperS", "Controls", &upperS, 256, NULL);

  cvCreateTrackbar("LowerV", "Controls", &lowerV, 256, NULL);
        cvCreateTrackbar("UpperV", "Controls", &upperV, 256, NULL); 
}

int main(){
 CvCapture* capture =0; 
 
 capture = cvCaptureFromCAM(0);

 if(!capture){
   printf("Capture failure\n");
   return -1;
 }

 cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,320);
 cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,240);
 cvSetCaptureProperty(capture,CV_CAP_PROP_FPS,50);

 IplImage* frame=0;
  
 setwindowSettings();

  //iterate through each frames of the video
 while(true){
 
  frame = cvQueryFrame(capture);
  if(!frame)  break;
  frame=cvCloneImage(frame); 

  // optional load an image
  //frame = cvLoadImage("imagen3.png",CV_LOAD_IMAGE_COLOR );

  cvSmooth(frame, frame, CV_GAUSSIAN,3,3);

  IplImage* imgHSV = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3); 
  cvCvtColor(frame, imgHSV, CV_BGR2HSV); //Change the color format from BGR to HSV
   
  IplImage* imgThresh = GetThresholdedImage(imgHSV);
  
  cvShowImage("Ball", imgThresh);
  cvShowImage("Video", frame);

   //Clean up used images
  cvReleaseImage(&imgHSV);
  cvReleaseImage(&imgThresh);
  cvReleaseImage(&frame);

   //Wait 80mS
  int c = cvWaitKey(80);
  //If 'ESC' is pressed, break the loop
  if((char)c==27 ) break;

 }

  cvDestroyAllWindows();
 cvReleaseCapture(&capture);

       return 0;
}
