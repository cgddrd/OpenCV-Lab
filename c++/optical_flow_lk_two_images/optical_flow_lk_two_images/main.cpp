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
    
    Mat frame, grayFrames, rgbFrames, prevGrayFrame, resultFrame;
    
    Mat opticalFlow = Mat(img1.rows, img1.cols, CV_32FC3);
    
    vector<Point2f> points1;
    vector<Point2f> points2;
    vector<Point2f> displacement_points;
    
    Point2f diff;
    
    vector<uchar> status;
    vector<float> err;
    
    int i, k;
    
    TermCriteria termcrit(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 20, 0.03);
    
    //CG - Sizde of the search window when conduction tyhe Lucas-Kanade optical flow anaysis.
    Size winSize(31, 31);

    //CG - Calculate a central column through the two images that has a width of 10% of the original images.
    double centre_point = img1.cols / 2;
    double width_ten_percent = img1.cols * 0.10;
    double half_width_ten_percent = width_ten_percent / 2;
    
    //CG - Extract the central column ROI from the two images ready to perform feature detection and optical flow analysis on them.
    Mat roi = img1( Rect(centre_point - half_width_ten_percent,0,width_ten_percent,img1.rows) );
    Mat roi2 = img2( Rect(centre_point - half_width_ten_percent,0,width_ten_percent,img1.rows) );
    
    //CG - Convert the first ROI to gray scale so we can perform Shi-Tomasi feature detection.
    cvtColor(roi, prevGrayFrame, cv::COLOR_BGR2GRAY);
    
    //CG - Perform Shi-Tomasi feature detection.
    goodFeaturesToTrack(prevGrayFrame, points1, MAX_COUNT, 0.01, 5, Mat(), 3, 0, 0.04);
    
    img2.copyTo(resultFrame);
    cvtColor(roi2, grayFrames, cv::COLOR_BGR2GRAY);
    
    //CG - Perform the actual sparse optical flow within the ROI extracted from the two images.
    calcOpticalFlowPyrLK(prevGrayFrame, grayFrames, points1, points2, status, err, winSize, 3, termcrit, 0, 0.001);
    
    cout << "Optical Flow Difference:\n\n";
    
    for (i = k = 0; i < points2.size(); i++) {
        
        //CG - Get a string ready to display the vector number in the image.
        char str[4];
        sprintf(str,"%d",i);
        
        //CG - We need to move the X position of both the start and end points for each vector over so that it is displayed within the bounds of thr ROI extracted from the main large image.
        points1[i].x += (centre_point - half_width_ten_percent);
        points2[i].x += (centre_point - half_width_ten_percent);
        
        //CG - Draw the vector number above the vector arrow on the image.
        putText(resultFrame, str, points1[i], FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255,255,255));
        putText(opticalFlow, str, points1[i], FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255,255,255));
        
        //CG - Push the 'X' coord from the starting position, and the 'Y' coord from the second position, so we can draw a stright line vector showing the displacement (BLUE LINE)
        displacement_points.push_back(Point2f(points1[i].x, points2[i].y));
        
        //CG - If the motion vector is going in the UP direction, draw red arrow.
        if ((points1[i].y - points2[i].y) > 0) {
            
            // CG - Note: Scalar COLOUR values use the format (B,G,R)
            arrowedLine(resultFrame, points1[i], points2[i], Scalar(0,0,255));
            
            // CG - Draw blue arrow to indicate displacement.
            arrowedLine(resultFrame, points1[i], displacement_points[i], Scalar(255,0,0));
            
            circle(resultFrame, points1[i], 2, Scalar(255, 0, 0), 1, 1, 0);
            
            arrowedLine(opticalFlow, points1[i], points2[i], Scalar(0,0,255));
            
            circle(opticalFlow, points1[i], 1, Scalar(255, 0, 0), 1, 1, 0);
         
        //CG - Otherwise the motion must be going DOWN, so draw a green arrow.
        } else {
            
            arrowedLine(resultFrame, points1[i], points2[i], Scalar(0,255,0));
            
            arrowedLine(resultFrame, points1[i], displacement_points[i], Scalar(255,0,0));
            
            circle(resultFrame, points1[i], 2, Scalar(0, 0, 0), 1, 1, 0);
            
            arrowedLine(opticalFlow, points1[i], points2[i], Scalar(0,255,0));
            
            circle(opticalFlow, points1[i], 1, Scalar(255, 0, 0), 1, 1, 0);
        }
        
        cout << "Vector: " << i << " - X: " << int(points1[i].x - points2[i].x) << ", Y: " << int(points1[i].y - points2[i].y) << ", Norm: " << norm(points1[i]-points2[i]) << ", Displacement: " << norm(points1[i]-displacement_points[i]) << "\n";
        
    }
    
    //CG - Resize the images so we can see them on the screen.
    resize(img1, img1, Size(img1.cols/4, img1.rows/4));
    resize(img2, img2, Size(img2.cols/4, img2.rows/4));
    resize(resultFrame, resultFrame, Size(resultFrame.cols/2, resultFrame.rows/2));
    resize(opticalFlow, opticalFlow, Size(opticalFlow.cols/2, opticalFlow.rows/2));
    resize(roi, roi, Size(roi.cols/2, roi.rows/2));
    resize(roi2, roi2, Size(roi2.cols/2, roi2.rows/2));
    
    //CG - Create windows for display.
    namedWindow( "Image 1 ROI", WINDOW_NORMAL );
    namedWindow( "Image 2", WINDOW_NORMAL );
    namedWindow( "Result", WINDOW_NORMAL );
    namedWindow( "OF", WINDOW_NORMAL );

    //CG - Show the images on screen.
    imshow("Image 1", roi);
    imshow("Image 2", roi2);
    imshow("Result", resultFrame);
    imshow("OF", opticalFlow);
    
    //CG - Wait for the user to press a key before exiting.
    cvWaitKey(0);
    
}