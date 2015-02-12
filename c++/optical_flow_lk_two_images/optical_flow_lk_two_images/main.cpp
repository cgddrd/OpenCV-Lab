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
   // namedWindow(rawWindow, WINDOW_AUTOSIZE);
    double angle;
    
    
   // img1.copyTo(prevGrayFrame);
    
    cvtColor(img1, prevGrayFrame, cv::COLOR_BGR2GRAY);
    goodFeaturesToTrack(prevGrayFrame, points1, MAX_COUNT, 0.01, 5, Mat(), 3, 0, 0.04);
    
    img2.copyTo(resultFrame);
    
    cvtColor(img2, grayFrames, cv::COLOR_BGR2GRAY);
    
    calcOpticalFlowPyrLK(prevGrayFrame, grayFrames, points1, points2, status, err, winSize, 3, termcrit, 0, 0.001);
    
    for (i = k = 0; i < points2.size(); i++) {
        
        cout << "Optical Flow Difference... X is " << int(points1[i].x - points2[i].x) << "\t Y is " << int(points1[i].y - points2[i].y) << "\t\t" << i << "\n";
        
        if ((points1[i].x - points2[i].x) > 0) {
            
            // CG - Note: Scalar COLOUR values use the format (B,G,R).
            
            //line(resultFrame, points1[i], points2[i], Scalar(0, 0, 255), 1, 1, 0);
            
            arrowedLine(resultFrame, points1[i], points2[i], Scalar(0,0,255));
            
            circle(resultFrame, points1[i], 2, Scalar(255, 0, 0), 1, 1, 0);
            
            //line(opticalFlow, points1[i], points2[i], Scalar(0, 0, 255), 1, 1, 0);
            
            arrowedLine(opticalFlow, points1[i], points2[i], Scalar(0,0,255));
            
            circle(opticalFlow, points1[i], 1, Scalar(255, 0, 0), 1, 1, 0);
            
        } else {
            
            //line(resultFrame, points1[i], points2[i], Scalar(0, 255, 0), 1, 1, 0);
            
            arrowedLine(resultFrame, points1[i], points2[i], Scalar(0,255,0));
            
            circle(resultFrame, points1[i], 2, Scalar(0, 0, 0), 1, 1, 0);
            
            //line(opticalFlow, points1[i], points2[i], Scalar(0, 255, 0), 1, 1, 0);
            
            arrowedLine(opticalFlow, points1[i], points2[i], Scalar(0,255,0));
            
            circle(opticalFlow, points1[i], 1, Scalar(255, 0, 0), 1, 1, 0);
        }
        
        points1[k++] = points1[i];
        
        cout << "i = " << i << " k = " << k << "\n";
        
    }

    
//    if (needToInit) {
//        
//        goodFeaturesToTrack(grayFrames, points1, MAX_COUNT, 0.01, 5, Mat(), 3, 0, 0.04);
//        needToInit = false;
//        
//    } else if (!points2.empty()) {
//        
//        calcOpticalFlowPyrLK(prevGrayFrame, grayFrames, points2, points1,
//                             status, err, winSize, 3, termcrit, 0, 0.001);
//        
//        for (i = k = 0; i < points2.size(); i++) {
//            cout << "Optical Flow Difference... X is "
//            << int(points1[i].x - points2[i].x) << "\t Y is "
//            << int(points1[i].y - points2[i].y) << "\t\t" << i
//            << "\n";
//            
//            if ((points1[i].x - points2[i].x) > 0) {
//                line(rgbFrames, points1[i], points2[i], Scalar(0, 0, 255),
//                     1, 1, 0);
//                
//                circle(rgbFrames, points1[i], 2, Scalar(255, 0, 0), 1, 1,
//                       0);
//                
//                line(opticalFlow, points1[i], points2[i], Scalar(0, 0, 255),
//                     1, 1, 0);
//                circle(opticalFlow, points1[i], 1, Scalar(255, 0, 0), 1, 1,
//                       0);
//            } else {
//                line(rgbFrames, points1[i], points2[i], Scalar(0, 255, 0),
//                     1, 1, 0);
//                
//                circle(rgbFrames, points1[i], 2, Scalar(255, 0, 0), 1, 1,
//                       0);
//                
//                line(opticalFlow, points1[i], points2[i], Scalar(0, 255, 0),
//                     1, 1, 0);
//                circle(opticalFlow, points1[i], 1, Scalar(255, 0, 0), 1, 1,
//                       0);
//            }
//            points1[k++] = points1[i];
//            
//        }
//        
//        goodFeaturesToTrack(grayFrames, points1, MAX_COUNT, 0.01, 10, Mat(),
//                            3, 0, 0.04);
//        
//    }
    
    resize(img1, img1, Size(img1.cols/4, img1.rows/4));
    
    namedWindow( "Image 1", WINDOW_NORMAL );// Create a window for display.
    
    resize(img2, img2, Size(img2.cols/4, img2.rows/4));
    
    namedWindow( "Image 2", WINDOW_NORMAL );// Create a window for display.
    
    resize(resultFrame, resultFrame, Size(resultFrame.cols/2, resultFrame.rows/2));
    
    resize(opticalFlow, opticalFlow, Size(opticalFlow.cols/2, opticalFlow.rows/2));
    
    namedWindow( "Result", WINDOW_NORMAL );// Create a window for display.
    
    namedWindow( "OF", WINDOW_NORMAL );// Create a window for display.
    
   // while(1) {
        
        
        imshow("Image 1", img1);
    
        imshow("Image 2", img2);
        
        imshow("Result", resultFrame);
    
        imshow("OF", opticalFlow);
    //}
   
    cvWaitKey(0);
    
    // imshow(opticalFlowWindow, opticalFlow);
    
    //std::swap(points2, points1);
    
   // points1.clear();
    //grayFrames.copyTo(prevGrayFrame);
    
//    while (1) {
//        
//        cap >> frame;
//        frame.copyTo(rgbFrames);
//        cvtColor(rgbFrames, grayFrames, cv::COLOR_BGR2GRAY);
//        
//        if (needToInit) {
//            
//            goodFeaturesToTrack(grayFrames, points1, MAX_COUNT, 0.01, 5, Mat(), 3, 0, 0.04);
//            needToInit = false;
//            
//        } else if (!points2.empty()) {
//            
//            cout << "\n\n\nCalculating  calcOpticalFlowPyrLK\n\n\n\n\n";
//            calcOpticalFlowPyrLK(prevGrayFrame, grayFrames, points2, points1,
//                                 status, err, winSize, 3, termcrit, 0, 0.001);
//            
//            for (i = k = 0; i < points2.size(); i++) {
//                cout << "Optical Flow Difference... X is "
//                << int(points1[i].x - points2[i].x) << "\t Y is "
//                << int(points1[i].y - points2[i].y) << "\t\t" << i
//                << "\n";
//                
//                if ((points1[i].x - points2[i].x) > 0) {
//                    line(rgbFrames, points1[i], points2[i], Scalar(0, 0, 255),
//                         1, 1, 0);
//                    
//                    circle(rgbFrames, points1[i], 2, Scalar(255, 0, 0), 1, 1,
//                           0);
//                    
//                    line(opticalFlow, points1[i], points2[i], Scalar(0, 0, 255),
//                         1, 1, 0);
//                    circle(opticalFlow, points1[i], 1, Scalar(255, 0, 0), 1, 1,
//                           0);
//                } else {
//                    line(rgbFrames, points1[i], points2[i], Scalar(0, 255, 0),
//                         1, 1, 0);
//                    
//                    circle(rgbFrames, points1[i], 2, Scalar(255, 0, 0), 1, 1,
//                           0);
//                    
//                    line(opticalFlow, points1[i], points2[i], Scalar(0, 255, 0),
//                         1, 1, 0);
//                    circle(opticalFlow, points1[i], 1, Scalar(255, 0, 0), 1, 1,
//                           0);
//                }
//                points1[k++] = points1[i];
//                
//            }
//            
//            goodFeaturesToTrack(grayFrames, points1, MAX_COUNT, 0.01, 10, Mat(),
//                                3, 0, 0.04);
//            
//        }
//        
//        imshow(rawWindow, rgbFrames);
//        // imshow(opticalFlowWindow, opticalFlow);
//        
//        std::swap(points2, points1);
//        
//        points1.clear();
//        grayFrames.copyTo(prevGrayFrame);
//        
//        keyPressed = waitKey(10);
//        if (keyPressed == 27) {
//            break;
//        } else if (keyPressed == 'r') {
//            opticalFlow = Mat(cap.get(CV_CAP_PROP_FRAME_HEIGHT),
//                              cap.get(CV_CAP_PROP_FRAME_HEIGHT), CV_32FC3);
//        }
//        
//    }
}