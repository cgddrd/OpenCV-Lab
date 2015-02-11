#include <stdio.h>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv){
    
    Mat src, descriptors,dest;
    vector<KeyPoint> keypoints;
    
    src = imread(argv[1]);
    
    
    cvtColor(src, src, CV_BGR2GRAY);
    
    SIFT sift(2000,3,0.004);
    sift(src, src, keypoints, descriptors, false);
    drawKeypoints(src, keypoints, dest);
    resize(dest, dest, Size(dest.cols/3, dest.rows/3));
    
    namedWindow( "Display window", WINDOW_NORMAL );// Create a window for display.
    imshow( "Display window", dest );
    cvWaitKey(0);
    return 0;
}