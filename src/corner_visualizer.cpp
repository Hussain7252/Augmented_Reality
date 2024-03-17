/*
Author:- Hussain Kanchwala and Abdulaziz Suria
*/

// Code to implement the augmented reality
// It will project a virtual object onto a chessboard pattern

#include <stdio.h>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main()
{
    Mat frame;
    Mat gray;
    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        cout << "Unable to open video device" << endl;
        return -1;
    }

    // Creating the windows to display video
    namedWindow("Visualizer", 1);

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
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        vector<Point2f> corner_points;
        goodFeaturesToTrack(gray, corner_points, 100, 0.01, 10, Mat(), 3, true);

        for (int i = 0; i < corner_points.size(); i++)
        {
            circle(frame, corner_points[i], 2, Scalar(0, 0, 255), 2);
        }
        // displaying the frame
        imshow("Visualizer", frame);

        // Terminating the video if 'q' is pressed
        if (key == 27 || key == 'Q' || key == 'q')
        {
            break;
        }
    }
    destroyAllWindows();
    return 0;
}