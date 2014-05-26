#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/video/background_segm.hpp>
#include <bits/stdc++.h>
#include <vector>
#include <algorithm>
#include "skincolor.h"
#include "findcontour.h"
#include "flandmark_detector.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "label.h"
#define PI 3.14159265
using namespace cv;
using namespace std;
Mat src, src_gray,skin1,drawing,skin,skin2;
IplImage* frame2;
long long int noframes;
Point aa,bb,cc;
double collideframe=0,anglesum=0;

Mat fgMaskMOG; //fg mask generated by MOG method
Mat fgMaskMOG2; //fg mask fg mask generated by MOG2 method
Ptr<BackgroundSubtractor> pMOG; //MOG Background subtractor
Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
int keyboard=2;

int main(int argc, const char *argv[])
{


	//cout<<"Hi all "<<endl;
    VideoCapture cap("//home/mac/Documents/PROJECT/Training_Sets/Air/AIR_1_NAVIN (2).mp4");
	cout<<"Hi "<<endl;
    //VideoCapture cap("/home/mac/Documents/PROJECT/Training_Sets/Mother/mother_divya (2).mp4");
	//VideoCapture cap("/home/mac/Documents/PROJECT/Training_Sets/Father/father_kiruthika (1).mp4");
	//VideoCapture cap(0);
	noframes=1;
	Mat3b frame;
	vector<Point> coordinate;
	FLANDMARK_Model * model = flandmark_init("/home/mac/workspace/flandmark-master/data/flandmark_model.dat");
	int bbox[10];
	cout<<"Hi "<<endl;
	bool trackstart,trackstop;
	trackstart=trackstop=false;

	mapskeleton human;

	int options=1,hands_count=0;
	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
	Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));
	VideoWriter oVideoWriter ("/home/mac/Documents/PROJECT/Output/edge_1.avi", CV_FOURCC('P','I','M','1'), 20, frameSize, true); //initialize the VideoWriter object
	VideoWriter oVideoWriter1 ("/home/mac/Documents/PROJECT/Output/edge_2.avi", CV_FOURCC('P','I','M','1'), 20, frameSize, true); //initialize the VideoWriter object
	Mat trace = Mat::zeros( Size(dWidth,dHeight), CV_8UC3 );
	Mat prev,current,next,d1,d2,results;
	current=next=prev=Mat::zeros( Size(dWidth,dHeight), CV_8UC3 );
	Point location[3][2];
	cout<<"Hi "<<endl;

	while(cap.read(frame ) and (options==1 or options==2))
	{
		noframes+=1;

		Mat framegray;cvtColor(frame,framegray,CV_RGB2GRAY);
		skin = GetSkin(frame);

		cvtColor(skin,skin,CV_RGB2GRAY);
		skin1 = skin> 50;
		blur( skin1, skin1, Size(3,3) );
		char* source_window = "Source";
		src_gray=skin1;
		Mat output;
		Point array[3],dime[3];int sz=0;

		skin1=draw_contour(src_gray,array,sz,dime,frame,location);
		if(sz and !human.facefixed)
		{
			human.facefixed=true;
			human.face=array[0];
			human.facel=dime[0];
			bbox[0]=location[0][0].x;
			bbox[1]=location[0][0].y;
			bbox[2]=location[0][1].x;
			bbox[3]=location[0][1].y;
			human.facetl=location[0][0];
			human.facebr=location[0][1];
		}
		human.face_map(array,dime,sz,skin1,location);


		double *landmarks=(double*)malloc(2*model->data.options.M*sizeof(double));;
		IplImage *dst_img;
		dst_img = cvCreateImage(cvSize(framegray.cols,framegray.rows), IPL_DEPTH_8U, 1);
		memcpy(dst_img->imageData, framegray.data, framegray.cols*framegray.rows);

		flandmark_detect(dst_img, bbox, model, landmarks);

		Mat myMat(dst_img);
		imshow("Fland",skin1);

		Mat skin3,skin1_gray,cdst,dst,dstg;
		cvtColor(skin1,skin1_gray,CV_BGR2GRAY);
		skin2=frame;
		skin1_gray=CannyThreshold(0, 0,skin1,skin1_gray);
		cvtColor(skin1_gray, dst, CV_BGR2GRAY);
		cvtColor(skin1, dstg, CV_BGR2GRAY);
		cvtColor(dst,cdst,CV_GRAY2BGR);


		Mat canny_output;
		vector<vector<Point> > contours;

		vector<Vec4i> hierarchy;
		findContours( dst, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
		vector<vector<Point> > contours_poly( contours.size() );
		int thresh = 100;
		int max_thresh = 255;
		RNG rng(12345);
			  /// Draw contours
		Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
		vector<vector<Point> >hull(contours_poly.size());
		for( int i = 0; i< contours.size(); i++ )
		{
			       Scalar color = Scalar( 255, 0, 255 );
			       approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
			       drawContours( dst, contours_poly, i, color, 2, 8, hierarchy, 0, Point() );
			       convexHull( Mat(contours_poly[i]), hull[i], false );

		}
		cvtColor(dst,cdst,CV_GRAY2BGR);
		Point Downmost;
		for (int i = 2; i < 2*model->data.options.M; i += 2)
			  			 MyFilledCircle(cdst,Point (landmarks[i],landmarks[i+1]),1);
		bb=Point(landmarks[14],landmarks[15]);
	    int facearea,face;
	    facearea=abs(human.facetl.x-human.facebr.x)*abs(human.facetl.y-human.facebr.y);
		face=abs(location[1][0].x-location[1][1].x)*abs(location[1][0].y-abs(location[1][1].y));
		Downmost=human.face;
		if(face-facearea>3500)
		{
			cout<<face<<" "<<facearea<<endl;
	    MyFilledCircle( cdst, location[1][0],0 );
	    MyFilledCircle( cdst, location[1][1],0 );
	    for(int i=0;i< hull.size();i++)
	    {
	    			  	  					int n=hull[i].size();
	    			  	  		  			for(int j=0;j<hull[i].size();j++)
	    			  	  		  				if(hull[i][j].x<Downmost.x and (hull[i][j].y>human.facebr.y+5 or hull[i][j].x<(human.facetl.x) ) )
	    			  	  		  						Downmost=hull[i][j];
	    }
	    if(trackstart==false)
	    		{cc=aa=Downmost;trackstart=true;}

	    else{
	    cc=Downmost;
	    anglesum+=angle(cc,bb,aa);
	    collideframe+=1;
	    cout<<angle(cc,bb,aa)<<endl;
	    }
	    }
		imshow("Contours",cdst);
		blur( skin1, skin1, Size( 5, 5 ) );
		oVideoWriter1.write(cdst);
		waitKey(50);
	}
	cout<<anglesum/collideframe<<"Average angle "<<endl;
	//processVideo("/home/mac/Documents/PROJECT/Output/edge.avi");
	return 0;
}