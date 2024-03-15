/*
Author:- Hussain Kanchwala and Abdul Aziz Surya
*/
#include <stdio.h>
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;
//Function to find the corners of chessboard pattern

void findCorner(Mat &image, vector<cv::Point2f> &corner_set, bool &patternFound);

// Generate 3D points
vector<Vec3f> generate3DPoints(); 