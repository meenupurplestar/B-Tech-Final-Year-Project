/*
 * Segement(1_0).cpp
 *
 *  Created on: 03-Jan-2014
 *      Author: mac
 */
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
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
#include "learn.h"
#include "findcontour.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "GRT.h"
using namespace cv;
using namespace std;
using namespace GRT;


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


	VideoCapture cap("/home/mac/Documents/PROJECT/Training_Sets/Book/book_divya (2).mp4");
	noframes=1;
	Mat3b frame;
	LabelledTimeSeriesClassificationData trainingData,testData;
    trainingData.setNumDimensions( 2 );
	trainingData.setDatasetName("DummyData");
    trainingData.setInfoText("This data contains some dummy timeseries data");
	UINT gestureLabel = 16;
	MatrixDouble trainingSample;
	DTW dtws,dtwd;


	int options,hands_count=0;
	cout<<"Enter the options "<<endl;
	cout<<"1 Training Mode "<<endl;
	cout<<"2 Recognize Gesture "<<endl;
	cin>>options;
	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
	Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));
	//VideoWriter oVideoWriter ("/home/mac/Documents/PROJECT/Output/Friend_t.avi", CV_FOURCC('P','I','M','1'), 20, frameSize, true); //initialize the VideoWriter object
	//VideoWriter oVideoWriter1 ("/home/mac/Documents/PROJECT/Output/HSV2_s.avi", CV_FOURCC('P','I','M','1'), 20, frameSize, true); //initialize the VideoWriter object
	intialize_particlefilter(condensH);
	intialize_particlefilter(condensL);
	intialize_particlefilter(condensR);
	Mat trace = Mat::zeros( Size(dWidth,dHeight), CV_8UC3 );
	learn L;
	if(options==1)
	{
		L.record=true; //Yes I want to record
		//L.setup();
		cout<<"Enter the Gesture [0 to 99] that needs to trained";
		//cin>>L.trainingClassLabel; //I am telling the label to which data belongs
	}
	else
		L.record=false;

	VectorDouble head;bool flag=true;
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
		skin1=draw_contour(src_gray,array,sz);
		if(sz and flag)
		{ flag=false;
		 face=array[0];
		}
		face_map(array,sz);
		if(sz>1)
		{
			MyCircle(trace,array[0],0);
			imshow("draw",trace);
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
			VectorDouble sample(2);

            cout<<array[0].x<<" "<<array[0].y<<" "<<array[1].x<<" "<<array[1].y<<endl;
			sample[0] = array[0].x;
			sample[1] = array[0].y;

		    trainingSample.push_back(sample);
		}
		waitKey(50);

	}

	destroyAllWindows();
	int ch=0,single=1;
	if(hands_count>=5)
			single++;

	cout<<"Hand number "<<single<<endl;
	if(single==1)
	{
		if(options==1 )
		{
		cout<<"You want to save it "<<endl;
		cin>>ch;
		}
		if( trainingData.loadDatasetFromFile("TrainingData_SingleHand.txt") );
		if(ch==1 and options==1)
		{
				//After recording your training data you can then save it to a file
				 //Load the data then save the file into it.
				trainingData.addSample(gestureLabel,trainingSample);
				if( trainingData.saveDatasetToFile( "TrainingData_SingleHand.txt" ) )
							cout << "Training data saved to file\n";
		}

		if(options==2)
		{

		//dtw.enableTrimTrainingData(true,0.1,90);

		//dtw.enableZNormalization( true );
			dtws.train(trainingData);
			if( !dtws.saveModelToFile("DTWModel_SingleHand.txt") ){
		        cout << "Failed to save the classifier model!\n";
		        return EXIT_FAILURE;
		    }
			cout<<"42"<<endl;
		    //Load the DTW model from a file
		    if( !dtws.loadModelFromFile("DTWModel_SingleHand.txt") ){
		        cout << "Failed to load the classifier model!\n";
		        return EXIT_FAILURE;
		    }

		    //MatrixDouble timeseries = trainingSample;
		    if( !dtws.predict( trainingSample ) )
		    {
		               cout << "Failed to perform prediction for test sampel: "  <<"\n";
		               return EXIT_FAILURE;
		     }
		      UINT predictedClassLabel = dtws.getPredictedClassLabel();
		      double maximumLikelihood = dtws.getMaximumLikelihood();
		      VectorDouble classLikelihoods = dtws.getClassLikelihoods();
		      VectorDouble classDistances = dtws.getClassDistances();
		      cout << "TestSample: "  <<  "\tClassLabel: " << 1 << "\tPredictedClassLabel: " << predictedClassLabel << "\tMaximumLikelihood: " << maximumLikelihood << endl;
		      cout<<"Gesture Signature "<<name[predictedClassLabel]<<endl;
	}
	}
	else
	{

				if(options==1)
					{
						cout<<"You want to save it "<<endl;
						cin>>ch;
					}
				if( trainingData.loadDatasetFromFile("TrainingData_DoubleHand.txt") );
				if(ch==1 )
				{
						//After recording your training data you can then save it to a file
						 //Load the data then save the file into it.
						trainingData.addSample(gestureLabel,trainingSample);
						if( trainingData.saveDatasetToFile( "TrainingData_DoubleHand.txt" ) )
									cout << "Training data saved to file\n";
				}

				if(options==2)
				{

				//dtw.enableTrimTrainingData(true,0.1,90);

				//dtwd.enableZNormalization( true );
					//dtwd. enableNullRejection( true );
					dtwd.train(trainingData);
					if( !dtwd.saveModelToFile("DTWModel_DoubleHand.txt") ){
				        cout << "Failed to save the classifier model!\n";
				        return EXIT_FAILURE;
				    }
					cout<<"42"<<endl;
				    //Load the DTW model from a file
				    if( !dtwd.loadModelFromFile("DTWModel_DoubleHand.txt") ){
				        cout << "Failed to load the classifier model!\n";
				        return EXIT_FAILURE;
				    }

				    //MatrixDouble timeseries = trainingSample;
				    if( !dtwd.predict( trainingSample ) )
				    {
				               cout << "Failed to perform prediction for test sampel: "  <<"\n";
				               return EXIT_FAILURE;
				     }
				      UINT predictedClassLabel = dtwd.getPredictedClassLabel();
				      double maximumLikelihood = dtwd.getMaximumLikelihood();
				      VectorDouble classLikelihoods = dtwd.getClassLikelihoods();
				      VectorDouble classDistances = dtwd.getClassDistances();
				      cout << "TestSample: "  <<  "\tClassLabel: " << 1 << "\tPredictedClassLabel: " << predictedClassLabel << "\tMaximumLikelihood: " << maximumLikelihood << endl;
				      cout<<"Gesture Signature "<<name[predictedClassLabel]<<endl;
			}
	}


    return 0;
}

