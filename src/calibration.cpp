/*
Author:- Hussain Kanchwala, Abdulaziz Suria

Code which performs camera callibration based on user saved checkboard images from livestream.
*/

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "../include/function.h"
using namespace std;
using namespace cv;

// Main Implementation
int main()
{

    Mat frame;
    Mat gray;
    int file_id = 1;
    int debug = 1;
    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cout << "Unable to open video device" << std::endl;
        return -1;
    }
    // chessboard corners and world position of corners per frame
    vector<Point2f> corner_set;
    // Take here is that we need Intrinsic parameters don't really want to know the real world distance correspondencies
    vector<Vec3f> point_set = generate3DPoints();
    bool patternFound;

    // list of corners and point_set
    vector<vector<Point2f>> corner_list;
    vector<vector<Vec3f>> point_list;

    // Display window
    namedWindow("Video Frame", WINDOW_NORMAL);

    bool messageDisplay = false;
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
        // get the corner information for the current frame
        findCheckboard(frame, corner_set, patternFound);
        imshow("Video Frame", frame);
        if (patternFound)
        {
            if (key == 's' || key == 'S')
            {
                string cimgname;
                imwrite("callibration_image_" + to_string(file_id) + ".png", frame);
                corner_list.push_back(corner_set);
                point_list.push_back(point_set);
                messageDisplay = false;
                file_id++;
                if (corner_list.size() > 4 && !messageDisplay)
                {
                    cout << "Press 'C' to calibrate" << endl;
                    messageDisplay = true;
                }
            }
        }
        if (corner_list.size() > 4 && (key == 'C' || key == 'c'))
        {
            Mat camera_matrix = Mat::eye(3, 3, CV_64FC1);
            camera_matrix.at<double>(0, 2) = frame.cols / 2.0;
            camera_matrix.at<double>(1, 2) = frame.rows / 2.0;
            // initializing the distortion coefficient
            Mat dist_coeff = Mat::zeros(8, 1, CV_64FC1);
            Mat rot, trans;
            cout << "Camera matrix before Calibration: " << endl
                 << camera_matrix << endl;
            cout << "Distortion coefficients before Calibration: " << dist_coeff << endl;

            double rms_error = calibrateCamera(point_list, corner_list, frame.size(), camera_matrix, dist_coeff, rot, trans, CALIB_FIX_ASPECT_RATIO);

            cout << "Camera matrix: " << endl
                 << camera_matrix << endl;
            cout << "Distortion coefficients: " << dist_coeff << endl;
            cout << "Reprojection error: " << rms_error << endl;

            // Saving calibration
            FileStorage fs("../Calibration_file/calibration.yml", FileStorage::WRITE);
            fs << "camera_matrix" << camera_matrix;
            fs << "distortion_coefficients" << dist_coeff;
            fs.release();
            cout << "Calibration saved to 'calibration.yml'" << endl;
            cout << "If you want to calibrate with more calibration images press 'S' and follow the procedure" << endl;
            cout << "Or else press 'ESC' to exit" << endl;
        }
        else if (corner_list.size() < 5 && !messageDisplay)
        {
            cout << "Please Take at least 5 images to calibrate by pressing 'S'. Images taken: " << corner_list.size() << endl;
            messageDisplay = true;
        }

        if (key == 27)
        {
            break;
        }
    }
    if (debug != 0)
    {
        for (int i = 0; i < corner_list.size(); i++)
        {
            for (int j = 0; j < corner_list[i].size(); j++)
            {
                cout << corner_list[i][j].x << "," << corner_list[i][j].y << " ";
            }
            cout << endl;
        }
    }
    destroyAllWindows();
    return 0;
}