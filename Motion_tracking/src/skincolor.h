#ifndef SKINCOLOR
#define SKINCOLOR
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <bits/stdc++.h>
using namespace cv;
using namespace std;
bool R1(int R, int G, int B);
bool R2(float Y, float Cr, float Cb);
bool R3(float Y, float Cr, float Cb);
Mat GetSkin(Mat const &src);
Mat Getmotion(Mat src,Mat mask);

#endif
