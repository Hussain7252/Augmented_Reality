/*
Author:- Hussain Kanchwala, Abdulaziz Suria
*/

// Code to implement the augmented reality
// It will project a virtual object onto a chessboard pattern

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "../include/function.h"
using namespace std;
using namespace cv;

int main()
{

    cv::Mat frame;
    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        cout << "Unable to open video device" << endl;
        return -1;
    }

    // Creating the windows to display video
    namedWindow("Augmented Reality", 1);

    // loading the intrinsic camera parameters
    cv::Mat camera_matrix, dist_coeff;
    string filename = "../Calibration_file/calibration.yml";
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    fs["camera_matrix"] >> camera_matrix;
    fs["distortion_coefficient"] >> dist_coeff;
    fs.release();

    // chessboard corners and world position of corners per frame
    vector<Point2f> corner_set;
    // Take here is that we need Intrinsic parameters don't really want to know the real world distance correspondencies
    vector<Vec3f> point_set = generate3DPoints();
    bool patternFound;
    int debug = 1;
    while (true)
    {

        int key = waitKey(1);
        // Capture Frame
        cap >> frame;
        if (frame.empty())
        {
            std::cout << "frame is empty" << endl;
            break;
        }

        // generating the corners of chessboard
        findCheckboard(frame, corner_set, patternFound);
        if (patternFound)
        {
            // Calculating the extrinsic parameters of camera
            Mat rot, trans;
            solvePnP(point_set, corner_set, camera_matrix, dist_coeff, rot, trans);
            Rodrigues(rot, rot);

            // Providing world coordinates for creating axis in the image plane
            vector<Point3f> worldFrame;
            worldFrame.push_back(Point3f(0, 0, 0));
            worldFrame.push_back(Point3f(8, 0, 0));
            worldFrame.push_back(Point3f(6, 0, 0));
            worldFrame.push_back(Point3f(0, -2, 0));

            // Providing the axes in the image frame
            vector<Point3f> axesFrame;
            axesFrame.push_back(Point3f(0, 0, 0));
            axesFrame.push_back(Point3f(5, 0, 0));
            axesFrame.push_back(Point3f(0, -5, 0));
            axesFrame.push_back(Point3f(0, 0, -5));

            // Projecting 3D coordinates into 2D coordinates
            vector<Point2f> cameraFrame;
            vector<Point2f> cameraAxes;
            projectPoints(worldFrame, rot, trans, camera_matrix, dist_coeff, cameraFrame);
            projectPoints(axesFrame, rot, trans, camera_matrix, dist_coeff, cameraAxes);

            // drawing circles of radius 20 on corresponding points
            // circle(frame, cameraFrame[0], 10, Scalar(0, 0, 255), FILLED, LINE_8);
            // circle(frame, cameraFrame[1], 10, Scalar(0, 0, 255), FILLED, LINE_8);
            // circle(frame, cameraFrame[2], 10, Scalar(0, 0, 255), FILLED, LINE_8);
            // circle(frame, cameraFrame[3], 10, Scalar(0, 0, 255), FILLED, LINE_8);

            line(frame, cameraAxes[0], cameraAxes[1], Scalar(0, 255, 255), 2); // X-axis in GR
            line(frame, cameraAxes[0], cameraAxes[2], Scalar(255, 0, 255), 2); // Y-axis in BR
            line(frame, cameraAxes[0], cameraAxes[3], Scalar(255, 255, 0), 2); // Z-axis in BG

            if (debug != 0)
            {
                if (key == 'd')
                {
                    cout << rot << endl;
                    cout << trans << endl;
                }
            }
        }
        // displaying the frame
        imshow("Augmented Reality", frame);

        // Terminating the video if 'q' is pressed
        if (key == 27 || key == 'Q' || key == 'q')
        {
            break;
        }
    }
    destroyAllWindows();
    return 0;
}