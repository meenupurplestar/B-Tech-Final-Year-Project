#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <bits/stdc++.h>
#include "skincolor.h"
#include "findcontour.h"
using namespace cv;
using namespace std;
vector<Rect> inside;

bool cmp(pair<double,Rect > A, pair<double,Rect > B)
{
	 return (int)A.first<(int)B.first;

}
bool cmp2(pair<double,Rect> A,pair<double,Rect> B)
{
	return (A.second.br().y<B.second.br().y);
}
bool checkinside(Rect A)
{
	for(int i=0;i<(int)inside.size();i++)
	{
		Point P,P1,P2,P3;
		P=inside[i].tl();P1=inside[i].br();P2=A.tl();P3=A.br();
		if(P.x<=P2.x and P.y<=P2.y and P1.x>=P3.x and P1.y>=P3.y)
					return false;
	}
	inside.push_back(A);
	return true;
}
void MyFilledCircle( Mat img, Point center,int color )
{
 int thickness = -1;
 int lineType = 8;
 circle(img,center,5.50,Scalar(0,255*color,255),-1,8,0);

}

Mat draw_contour(Mat src_gray,Point array[3],int& sz,Mat &colorface)
{

	Mat human;
	int thresh = 100,max_thresh = 255;
	  Mat threshold_output;
	  vector<vector<Point> > contours;
	  vector<Vec4i> hierarchy;/// Detect edges using Threshold
	  threshold( src_gray, threshold_output, thresh, 255, THRESH_BINARY );/// Find contourss
	  findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );/// Approximate contours to polygons + get bounding rects and circles
	  vector<vector<Point> > contours_poly( contours.size() );
	  vector<Rect> boundRect( contours.size() );
	  vector<pair<double,Rect > > boundrectangle;
	  vector<vector<Point> > contours_polygone;
	  vector<Point2f>center( contours.size() );
	  vector<float>radius( contours.size() );
	  vector<pair<double,Rect > > Bound(contours.size());
	  for( int i = 0; i < (int) contours.size(); i++ )
	     {
		  	  approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
		  	  boundRect[i] = boundingRect( Mat(contours_poly[i]) );
		  	  Bound[i].second=boundRect[i];
		  	  Bound[i].first=contourArea(contours[i]);
		  	  minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );

	     }
	  sort(Bound.begin(),Bound.end(),cmp);
	  int size=(int) Bound.size();/// Draw polygonal contour + bonding rects + circles
	  Mat drawing = Mat::zeros( src_gray.size(), CV_8UC3 );
	  Point headpoint;int maxi=3;
	  sort(Bound.begin(),Bound.end(),cmp);
	  for(int j=size-1;j>=0 and maxi;j--)
	  	  for( int i = 0; i< (int)contours.size(); i++ )
	  		  	  if(Bound[j].second==boundRect[i] and checkinside(boundRect[i]))
	  		  	  {
	  		  		  	  	  	  if(Bound[j].first>800.0)
	  		  		  	  	  	  {
	  		  		  	  	  		  boundrectangle.push_back(Bound[j]);
	  		  		  	  	  	  	  contours_polygone.push_back(contours_poly[i]);
	  		  		  	  	  	  }
	  		  		  	  	  	  maxi--;
	  		  		  	  	  	  if(Bound[j].first>800.0)

	  		  		  	  	  	  break;
	  		  	  }
	  	sort(boundrectangle.begin(),boundrectangle.end(),cmp2);
	  	Scalar colorB = Scalar(255,0,0);
	  	Scalar colorR = Scalar(0,0,255);
	  	Scalar color = Scalar(255,0,0);
	  	vector<vector<Point> >hull( boundrectangle.size() );
	  	vector<vector<cv::Vec4i> > convexityDefect(boundrectangle.size());
	  	for( int i = 0; i< (int)boundrectangle.size(); i++ )
	  	{
	  	drawContours( drawing, contours_polygone, i, colorB, -1, 8, vector<Vec4i>(), 0, Point() );
	  	rectangle( drawing, boundrectangle[i].second.tl(), boundrectangle[i].second.br(), color, 2, 8, 0 );
	  	array[sz].x=(boundrectangle[i].second.tl().x+boundrectangle[i].second.br().x)/2;
	  	array[sz].y=(boundrectangle[i].second.tl().y+boundrectangle[i].second.br().y)/2;
	  	sz++;
	  	convexHull( Mat(contours_polygone[i]), hull[i], false );
	  	//convexityDefects(contours[i], hull[i], convexityDefect[i]);
	  	}


	  	for(int i=0;i<sz;i++)
	  		for(int j=i;j<sz;j++)
	  				if(array[i].x>array[j].x)
	  					swap(array[i],array[j]);
	  	//if(sz>2 and array[1].x> array[2].x)
	  		//swap(array[1],array[2]);
	  	for(int i=0;i< hull.size();i++)
	  			for(int j=0;j<hull[i].size();j++)
	  					MyFilledCircle(drawing,hull[i][j],0);

	  	Mat humangray;
	  	cvtColor(drawing, humangray, CV_RGB2GRAY);
	  	colorface=Getmotion(colorface,humangray);
	  	imshow("Face",colorface);
	  	inside.clear();
	  	//colorface=human;
	  /// Show in a window



	  return drawing;
}
