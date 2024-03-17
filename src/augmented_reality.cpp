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

    bool showPoints = false;
    bool showShape = false;
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
            if (key == 'a')
            {
                showPoints = !showPoints;
            }
            else if (key == 'v')
            {
                showShape = !showShape;
            }

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

            // Providing the corners for black background
            vector<Point3f> cornerFrame;
            cornerFrame.push_back(Point3f(0, 1, 0));
            cornerFrame.push_back(Point3f(8, 1, 0));
            cornerFrame.push_back(Point3f(8, -6, 0));
            cornerFrame.push_back(Point3f(0, -6, 0));

            vector<Point3f> prismFrame;
            int x = 2, y = -4;
            int base = 4, height = 3;
            // Base Triangle
            prismFrame.push_back(Point3f(x, y, 0));
            prismFrame.push_back(Point3f(x + base, y, 0));
            prismFrame.push_back(Point3f(x + base / 2, y + sqrt(3) / 2 * base, 0));

            // Top Triangle
            prismFrame.push_back(Point3f(x, y, height));
            prismFrame.push_back(Point3f(x + base, y, height));
            prismFrame.push_back(Point3f(x + base / 2, y + sqrt(3) / 2 * base, height));

            // Projecting 3D coordinates into 2D coordinates
            vector<Point2f> cameraFrame;
            vector<Point2f> cameraAxes;
            vector<Point2f> cameraCorners;
            vector<Point2f> cameraPrism;
            projectPoints(worldFrame, rot, trans, camera_matrix, dist_coeff, cameraFrame);
            projectPoints(axesFrame, rot, trans, camera_matrix, dist_coeff, cameraAxes);
            projectPoints(cornerFrame, rot, trans, camera_matrix, dist_coeff, cameraCorners);
            projectPoints(prismFrame, rot, trans, camera_matrix, dist_coeff, cameraPrism);

            vector<Point> poly_corners(cameraCorners.begin(), cameraCorners.end());
            fillConvexPoly(frame, poly_corners, Scalar(0, 0, 0));
            line(frame, cameraAxes[0], cameraAxes[1], Scalar(0, 255, 255), 2); // X-axis in GR
            line(frame, cameraAxes[0], cameraAxes[2], Scalar(255, 0, 255), 2); // Y-axis in BR
            line(frame, cameraAxes[0], cameraAxes[3], Scalar(255, 255, 0), 2); // Z-axis in BG

            if (showPoints)
            {
                // drawing circles of radius 20 on corresponding points
                circle(frame, cameraFrame[0], 10, Scalar(0, 0, 255), FILLED, LINE_8);
                circle(frame, cameraFrame[1], 10, Scalar(0, 0, 255), FILLED, LINE_8);
                circle(frame, cameraFrame[2], 10, Scalar(0, 0, 255), FILLED, LINE_8);
                circle(frame, cameraFrame[3], 10, Scalar(0, 0, 255), FILLED, LINE_8);
            }
            if (showShape)
            {

                // Draw base Triangle
                line(frame, cameraPrism[0], cameraPrism[1], Scalar(255, 255, 0), 2);
                line(frame, cameraPrism[1], cameraPrism[2], Scalar(255, 0, 255), 2);
                line(frame, cameraPrism[2], cameraPrism[0], Scalar(0, 255, 255), 2);

                // Draw Top Triangle
                line(frame, cameraPrism[3], cameraPrism[4], Scalar(0, 255, 0), 2);
                line(frame, cameraPrism[4], cameraPrism[5], Scalar(255, 0, 0), 2);
                line(frame, cameraPrism[5], cameraPrism[3], Scalar(0, 0, 255), 2);

                // Connect lines between bases
                line(frame, cameraPrism[0], cameraPrism[3], Scalar(230, 200, 0), 2);
                line(frame, cameraPrism[1], cameraPrism[4], Scalar(0, 165, 255), 2);
                line(frame, cameraPrism[2], cameraPrism[5], Scalar(127, 255, 0), 2);
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