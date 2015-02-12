#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <iostream>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

using namespace cv;
using namespace std;

#define MAX_COUNT 500
char rawWindow[] = "Raw Video";
char opticalFlowWindow[] = "Optical Flow Window";
char imageFileName[32];
long imageIndex = 0;
char keyPressed;

static void help()
{
    printf("\nThis program demonstrates using features2d detector, descriptor extractor and simple matcher\n"
           "Using the SURF desriptor:\n"
           "\n"
           "Usage:\n matcher_simple <image1> <image2>\n");
}

// CG - Arrowed Line drawing method. (PhilLab - http://stackoverflow.com/questions/10161351) (Built into OpenCV v3.0.0)
static void arrowedLine(Mat& img, Point pt1, Point pt2, const Scalar& color, int thickness=1, int line_type=8, int shift=0, double tipLength=0.1)
{
    const double tipSize = norm(pt1-pt2)*tipLength; // Factor to normalize the size of the tip depending on the length of the arrow
    line(img, pt1, pt2, color, thickness, line_type, shift);
    const double angle = atan2( (double) pt1.y - pt2.y, (double) pt1.x - pt2.x );
    Point p(cvRound(pt2.x + tipSize * cos(angle + CV_PI / 4)),
            cvRound(pt2.y + tipSize * sin(angle + CV_PI / 4)));
    line(img, p, pt2, color, thickness, line_type, shift);
    p.x = cvRound(pt2.x + tipSize * cos(angle - CV_PI / 4));
    p.y = cvRound(pt2.y + tipSize * sin(angle - CV_PI / 4));
    line(img, p, pt2, color, thickness, line_type, shift);
}

int main(int argc, char** argv) {
    
    if(argc != 3)
    {
        help();
        return -1;
    }
    
    Mat img1 = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    Mat img2 = imread(argv[2], CV_LOAD_IMAGE_COLOR);
    
    if(img1.empty() || img2.empty())
    {
        printf("Can't read one of the images\n");
        return -1;
    }
    
    // VideoCapture cap(0);
    
    Mat frame, grayFrames, rgbFrames, prevGrayFrame, resultFrame;
    
    //CG - Get the height/width of the video frame from the VideoCapture object using the '.get' method.
    //Mat opticalFlow = Mat(cap.get(CV_CAP_PROP_FRAME_HEIGHT),cap.get(CV_CAP_PROP_FRAME_HEIGHT), CV_32FC3);
    
    Mat opticalFlow = Mat(img1.rows, img1.cols, CV_32FC3);
    
    vector<Point2f> points1;
    vector<Point2f> points2;
    
    Point2f diff;
    
    vector<uchar> status;
    vector<float> err;
    
    RNG rng(12345);
    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
    bool needToInit = true;
    
    int i, k;
    
    TermCriteria termcrit(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 20, 0.03);
    
    Size subPixWinSize(10, 10), winSize(31, 31);
    
    double angle;
    
    cvtColor(img1, prevGrayFrame, cv::COLOR_BGR2GRAY);
    goodFeaturesToTrack(prevGrayFrame, points1, MAX_COUNT, 0.01, 5, Mat(), 3, 0, 0.04);
    
    img2.copyTo(resultFrame);
    
    cvtColor(img2, grayFrames, cv::COLOR_BGR2GRAY);
    
    calcOpticalFlowPyrLK(prevGrayFrame, grayFrames, points1, points2, status, err, winSize, 3, termcrit, 0, 0.001);
    
    cout << "Optical Flow Difference:\n\n";
    
    for (i = k = 0; i < points2.size(); i++) {
        
        cout << "Vector: " << i << " - X: " << int(points1[i].x - points2[i].x) << ", Y: " << int(points1[i].y - points2[i].y) << ", Norm: " << norm(points1[i]-points2[i]) << "\n";
        
        
        char str[4];
        sprintf(str,"%d",i);
        
        //CG - Draw the vector number above the vector arrow on the image. 
        putText(resultFrame, str, points1[i], FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255,255,255));
        putText(opticalFlow, str, points1[i], FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255,255,255));
        
        //CG - If the motion vector is going in the UP direction, draw red arrow.
        if ((points1[i].y - points2[i].y) > 0) {
            
            // CG - Note: Scalar COLOUR values use the format (B,G,R)
            arrowedLine(resultFrame, points1[i], points2[i], Scalar(0,0,255));
            
            circle(resultFrame, points1[i], 2, Scalar(255, 0, 0), 1, 1, 0);
            
            arrowedLine(opticalFlow, points1[i], points2[i], Scalar(0,0,255));
            
            circle(opticalFlow, points1[i], 1, Scalar(255, 0, 0), 1, 1, 0);
         
        //CG - Otherwise the motion must be going DOWN, so draw a green arrow.
        } else {
            
            arrowedLine(resultFrame, points1[i], points2[i], Scalar(0,255,0));
            
            circle(resultFrame, points1[i], 2, Scalar(0, 0, 0), 1, 1, 0);
            
            arrowedLine(opticalFlow, points1[i], points2[i], Scalar(0,255,0));
            
            circle(opticalFlow, points1[i], 1, Scalar(255, 0, 0), 1, 1, 0);
        }
        
        // CG - Don't think I need this - used for unlimited capturing via camera.
        
        //points1[k++] = points1[i];
        //cout << "i = " << i << " k = " << k << "\n";
        
    }
    
    resize(img1, img1, Size(img1.cols/4, img1.rows/4));
    
    namedWindow( "Image 1", WINDOW_NORMAL );// Create a window for display.
    
    resize(img2, img2, Size(img2.cols/4, img2.rows/4));
    
    namedWindow( "Image 2", WINDOW_NORMAL );// Create a window for display.
    
    resize(resultFrame, resultFrame, Size(resultFrame.cols/2, resultFrame.rows/2));
    
    resize(opticalFlow, opticalFlow, Size(opticalFlow.cols/2, opticalFlow.rows/2));
    
    namedWindow( "Result", WINDOW_NORMAL );// Create a window for display.
    
    namedWindow( "OF", WINDOW_NORMAL );// Create a window for display.
    
    imshow("Image 1", img1);
    
    imshow("Image 2", img2);
    
    imshow("Result", resultFrame);
    
    imshow("OF", opticalFlow);
    
    //CG - Wait for the user to press a key before exiting.
    cvWaitKey(0);
    
}