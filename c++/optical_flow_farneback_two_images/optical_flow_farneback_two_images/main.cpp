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

static void drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step,
                           double, const Scalar& color)
{
    for(int y = 0; y < cflowmap.rows; y += step)
        for(int x = 0; x < cflowmap.cols; x += step)
        {
            const Point2f& fxy = flow.at<Point2f>(y, x);
            line(cflowmap, Point(x,y), Point(cvRound(x+fxy.x), cvRound(y+fxy.y)),
                 color);
            circle(cflowmap, Point(x,y), 2, color, -1);
        }
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
    
    Mat frame, grayFrames, rgbFrames, prevGrayFrame, resultFrame, flow, cflow;
    
    //CG - Calculate a central column through the two images that has a width of 10% of the original images.
    double centre_point = img1.cols / 2;
    double width_ten_percent = img1.cols * 0.10;
    double half_width_ten_percent = width_ten_percent / 2;
    
    //CG - Extract the central column ROI from the two images ready to perform feature detection and optical flow analysis on them.
    Mat roi = img1( Rect(centre_point - half_width_ten_percent,0,width_ten_percent,img1.rows) );
    Mat roi2 = img2( Rect(centre_point - half_width_ten_percent,0,width_ten_percent,img1.rows) );
    
    roi = img1;
    roi2 = img2;
    
    //CG - Convert the first ROI to gray scale so we can perform Shi-Tomasi feature detection.
    cvtColor(roi, prevGrayFrame, cv::COLOR_BGR2GRAY);
    
    img2.copyTo(resultFrame);
    
    cvtColor(roi2, grayFrames, cv::COLOR_BGR2GRAY);
    
    calcOpticalFlowFarneback(prevGrayFrame, grayFrames, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
    
    cvtColor(prevGrayFrame, cflow, COLOR_GRAY2BGR);
    drawOptFlowMap(flow, cflow, 5, 1.5, Scalar(0, 255, 0));
    
    resize(cflow, cflow, Size(cflow.cols/2, cflow.rows/2));
    
    //CG - Create windows for display.
    namedWindow( "Image 1", WINDOW_NORMAL );
    //    namedWindow( "Image 2", WINDOW_NORMAL );
    //    namedWindow( "Result", WINDOW_NORMAL );
    //    namedWindow( "OF", WINDOW_NORMAL );
    
    //CG - Show the images on screen.
    imshow("Image 1", cflow);
    //    imshow("Image 2", roi2);
    //    imshow("Result", resultFrame);
    //    imshow("OF", opticalFlow);
    
    //CG - Wait for the user to press a key before exiting.
    cvWaitKey(0);
    
}