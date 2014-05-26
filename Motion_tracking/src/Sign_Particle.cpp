/*
 * Segement(1_0).cpp
 *
 *  Created on: 03-Jan-2014
 *      Author: mac
 */
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <bits/stdc++.h>
#include <vector>
#include <algorithm>
#include "skincolor.h"
#include "predict.h"
#include "findcontour.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
using namespace cv;
using namespace std;



struct mouse_info_struct { int x,y; };
struct mouse_info_struct mouse_info = {-1,-1}, last_mouse;

vector<Point> mousev,kalmanv;


void on_mouse(int event, int x, int y, int flags, void* param) {
		last_mouse = mouse_info;
		mouse_info.x = x;
		mouse_info.y = y;

}
Point face,CX,CY;
void face_map(Point array[3],int sz)
{
	int s=100000,index;
	for(int i=0;i<sz;i++)
	{
		if(abs(array[i].x-face.x)+abs(array[i].y-face.y)<s)
		{ index=i;s=abs(array[i].x-face.x)+abs(array[i].y-face.y);}
	}
	swap(array[1],array[index]);
	if(sz>2 and array[0].x>array[2].x)
			swap(array[0],array[2]);

}


//This function returns the square of the euclidean distance between 2 points.
Mat src, src_gray,skin1,drawing,skin,skin2;
Point L,R,H;
int dim = 2, nParticles = 100;
RNG rng(12345);
vector<Point> mouseV, particleV;
CvConDensation* condensH = cvCreateConDensation(dim, dim, nParticles);
CvConDensation* condensL = cvCreateConDensation(dim, dim, nParticles);
CvConDensation* condensR = cvCreateConDensation(dim, dim, nParticles);
long long int noframes;

void MyCircle( Mat &img, Point center,int color )
{
 int thickness = -1;
 int lineType = 8;
 circle(img,center,5.50,Scalar(0,255*color,255),-1,8,0);

}
string name[50]={"Zero_Class","Air","Book","Moon","Mother","Father","Brother","Sister","Boy","Girl","Money","I","Food","Friend","Pen","Pencil","Sorry","ThankYou"};

int main(int argc, const char *argv[])
{


	VideoCapture cap("/home/mac/Documents/PROJECT/Training_Sets/Father/father_divya (1).mp4");
	noframes=1;
	Mat3b frame;
	 cv::Mat frame1;
	 cv::Mat back;
	 cv::Mat fore;
	 std::vector<std::vector<cv::Point> > contours;
	 BackgroundSubtractorMOG2 bg;
	// bg.nmixtures = 3;
	 //bg.bShadowDetection = false;

	 int options=1,hands_count=0;
	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
	Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));
	//VideoWriter oVideoWriter ("/home/mac/Documents/PROJECT/Output/Friend_t.avi", CV_FOURCC('P','I','M','1'), 20, frameSize, true); //initialize the VideoWriter object
	//VideoWriter oVideoWriter1 ("/home/mac/Documents/PROJECT/Output/HSV2_s.avi", CV_FOURCC('P','I','M','1'), 20, frameSize, true); //initialize the VideoWriter object
	Mat trace = Mat::zeros( Size(dWidth,dHeight), CV_8UC3 );
	bool flag=true;
	while(cap.read(frame ) and (options==1 or options==2))
	{
		noframes+=1;
		//if(noframes<5) continue;
		skin = GetSkin(frame);
		cvtColor(skin,skin,CV_RGB2GRAY);
		skin1 = skin> 50;
		blur( skin1, skin1, Size(3,3) );
		char* source_window = "Source";
		src_gray=skin1;
		Mat output;
		Point array[3];int sz=0;
		frame1=frame;
		skin1=draw_contour(src_gray,array,sz,frame1);
		bg.operator()(frame1,fore);
		bg.getBackgroundImage(back);
		cv::erode(fore,fore,cv::Mat());
		cv::dilate(fore,fore,cv::Mat());
		cv::findContours(fore,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
		cv::drawContours(frame1,contours,-1,cv::Scalar(0,0,255),2);
		cv::imshow("Frame",frame1);
		cv::imshow("Background",back);


		if(sz and flag)
		{
			flag=false;
			face=array[0];
		}
		//imshow("image",human);

		face_map(array,sz);
		if(sz>1)
		{
			MyCircle(trace,array[0],0);
		//	imshow("draw",trace);
		//	 oVideoWriter.write(trace);
		}

		//Mat aux=thresh_callback(0,0);
		//drawKeypoints(skin1, keypoints, output);
		skin2=frame;
		blur( skin1, skin1, Size( 5, 5 ) );
		imshow(source_window, skin1);
		// oVideoWriter1.write(skin1);
		if(sz>2) hands_count++;
		if(sz>1)
		{

		}
		waitKey(50);

	}

	destroyAllWindows();
	int ch=0,single=1;
	if(hands_count>=5)
			single++;



    return 0;
}

