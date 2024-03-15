#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "../include/function.h"
using namespace std;
using namespace cv;

// Main Implementation
int main(){

    Mat frame;
    Mat gray;
    string ipstr, calibimg;
    cout<<"Enter the IP address of the camera straming device"<<endl;
    getline(cin,ipstr);
    cout<<"Enter the calibration Image storage path"<<endl;
    getline(cin,calibimg);
    // Enter the IP address on which your camera is streaming and check if frames are available
    //VideoCapture cap("http://"+ipstr+"/video");
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

    // Set of corners and point_set 
    vector<vector<Point2f>> corner_list;
    vector<vector<Vec3f>> point_list;

    // Display window
    namedWindow("Video Frame", WINDOW_NORMAL);
    
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
        findCorner(frame, corner_set, patternFound);
        imshow("Video Frame",frame);
        if(patternFound){
            if (key == 's' || key == 'S') {
                string cimgname;
                cout<<"Enter new calibration image name followed with .png/.jpg"<<endl;
                getline(cin,cimgname);
                imwrite(calibimg+"/"+cimgname,frame);
                corner_list.push_back(corner_set);
                point_list.push_back(point_set);
            }
        }

        if(key == 27){
            break;
        }
    }
    
    return 0;
}