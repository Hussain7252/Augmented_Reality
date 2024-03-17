/*
Author:- Hussain Kanchwala & Abdul Aziz Surya
Date:- 03/14/2024 End:-
*/

/* Description of File

*/

// Dependencies
#include <stdio.h>
#include "../include/function.h"
using namespace std;
using namespace cv;

// Function to find the corners of chessboard pattern
void findCheckboard(Mat &image, vector<Point2f> &corner_set, bool &patternFound)
{
    Mat gray;
    cvtColor(image, gray, COLOR_BGR2GRAY);
    Size patternSize(9, 6);
    patternFound = cv::findChessboardCorners(image, patternSize, corner_set, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);

    // generating the corners of chessboard
    if (!patternFound)
    {
        return;
    }
    cv::cornerSubPix(gray, corner_set, Size(11, 11), Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.0001));
    // drawing the line between chessboard corners
    cv::drawChessboardCorners(image, patternSize, corner_set, patternFound);
    return;
}

// Generate 3D points
vector<Vec3f> generate3DPoints()
{
    vector<Vec3f> points;
    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 9; ++j)
        {
            points.push_back(Vec3f(j, -i, 0.0f));
        }
    }
    return points;
}
