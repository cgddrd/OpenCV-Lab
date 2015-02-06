//#include <opencv2/imgcodecs.hpp>
//#include <opencv2/videoio/videoio.hpp>
//#include <opencv2/highgui/highgui.hpp>
//
//#include <iostream>
//#include <iomanip>
//#include <stdio.h>
//
//using namespace cv;
//using namespace std;
//
////hide the local functions in an anon namespace
//namespace {
//    
//    //Pointer to Pointer (i.e. array of char*) - Could also declare as char* av[]
//    void help(char** av) {
//        cout << "The program captures frames from a video file, image sequence (01.jpg, 02.jpg ... 10.jpg) or camera connected to your computer." << endl
//        << "Usage:\n" << av[0] << " <video file, image sequence or device number>" << endl
//        << "q,Q,esc -- quit" << endl
//        << "space   -- save frame" << endl << endl
//        << "\tTo capture from a camera pass the device number. To find the device number, try ls /dev/video*" << endl
//        << "\texample: " << av[0] << " 0" << endl
//        << "\tYou may also pass a video file instead of a device number" << endl
//        << "\texample: " << av[0] << " video.avi" << endl
//        << "\tYou can also pass the path to an image sequence and OpenCV will treat the sequence just like a video." << endl
//        << "\texample: " << av[0] << " right%%02d.jpg" << endl;
//    }
//    
//    // CG - Note here we are PASSING BY REFERENCE!
//    int process(VideoCapture &capture) {
//        int n = 0;
//        
//        //char filename[200];
//        
//        string filename2;
//        
//        string window_name = "video | q or esc to quit";
//        cout << "press space to save a picture. q or esc to quit" << endl;
//        namedWindow(window_name, WINDOW_KEEPRATIO); //resizable window;
//        Mat frame;
//        
//        for (;;) {
//            
//            capture >> frame;
//            if (frame.empty())
//                break;
//            
//            imshow(window_name, frame);
//            
//            char key = (char) waitKey(30); //delay N millis, usually long enough to display and capture input
//            
//            std::stringstream fmt;
//            
//            switch (key) {
//                case 'q':
//                case 'Q':
//                case 27: //escape key
//                    return 0;
//                case ' ': //Save an image
//                   // sprintf(filename,"filename%.3d.jpg",n++);
//                    
//                    fmt << "filenamesioned" << std::setfill('0') << std::setw(5) << n++ << ".jpg";
//                    
//                    imwrite(fmt.str(),frame);
//                    
//                    cout << "Saved " << fmt.str() << endl;
//                    break;
//                default:
//                    break;
//            }
//        }
//        return 0;
//    }
//}
//
//int main(int ac, char** av) {
//    
//    if (ac != 2) {
//        help(av);
//        return 1;
//    }
//    
//    std::string arg = av[1];
//    
//    //VideoCapture capture(arg);
//    
//    VideoCapture capture = VideoCapture(arg); //try to open string, this will attempt to open it as a video file or image sequence
//    
//    if (!capture.isOpened()) //if this fails, try to open as a video camera, through the use of an integer param
//        
//        capture.open(atoi(arg.c_str()));
//    
//    if (!capture.isOpened()) {
//        
//        cerr << "Failed to open the video device, video file or image sequence!\n" << endl;
//        
//        help(av);
//        
//        return 1;
//        
//    }
//    
//    return process(capture);
//}

#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <ctype.h>

using namespace cv;
using namespace std;

Mat image;

bool backprojMode = false;
bool selectObject = false;
int trackObject = 0;
bool showHist = true;
Point origin;
Rect selection;
int vmin = 10, vmax = 256, smin = 30;

static void onMouse( int event, int x, int y, int, void* )
{
    if( selectObject )
    {
        selection.x = MIN(x, origin.x);
        selection.y = MIN(y, origin.y);
        selection.width = std::abs(x - origin.x);
        selection.height = std::abs(y - origin.y);
        
//        selection.x = 20;
//        selection.y = 20;
//        selection.width = std::abs(x - origin.x);
//        selection.height = std::abs(y - origin.y);
        
        selection &= Rect(0, 0, image.cols, image.rows);
        
       // cout<< selection << endl;
    }
    
    switch( event )
    {
        case EVENT_LBUTTONDOWN:
            origin = Point(x,y);
            selection = Rect(x,y,0,0);
            selectObject = true;
            break;
        case EVENT_LBUTTONUP:
            selectObject = false;
            if( selection.width > 0 && selection.height > 0 )
                trackObject = -1;
            break;
    }
}

static void help()
{
    cout << "\nThis is a demo that shows mean-shift based tracking\n"
    "You select a color objects such as your face and it tracks it.\n"
    "This reads from video camera (0 by default, or the camera number the user enters\n"
    "Usage: \n"
    "   ./camshiftdemo [camera number]\n";
    
    cout << "\n\nHot keys: \n"
    "\tESC - quit the program\n"
    "\tc - stop the tracking\n"
    "\tb - switch to/from backprojection view\n"
    "\th - show/hide object histogram\n"
    "\tp - pause video\n"
    "To initialize tracking, select the object with mouse\n";
}

const char* keys =
{
    "{@camera_number| 0 | camera number}"
};

int main( int argc, const char** argv )
{
    help();
    
    VideoCapture cap;
    Rect trackWindow;
    int hsize = 16;
    float hranges[] = {0,180};
    const float* phranges = hranges;
    CommandLineParser parser(argc, argv, keys);
    int camNum = parser.get<int>(0);
    
    cap.open(camNum);
    
    if( !cap.isOpened() )
    {
        help();
        cout << "***Could not initialize capturing...***\n";
        cout << "Current parameter's value: \n";
        parser.printMessage();
        return -1;
    }
    
    namedWindow( "Histogram", 0 );
    namedWindow( "CamShift Demo", 0 );
    setMouseCallback( "CamShift Demo", onMouse, 0 );
    createTrackbar( "Vmin", "CamShift Demo", &vmin, 256, 0 );
    createTrackbar( "Vmax", "CamShift Demo", &vmax, 256, 0 );
    createTrackbar( "Smin", "CamShift Demo", &smin, 256, 0 );
    
    Mat frame, hsv, hue, mask, hist, histimg = Mat::zeros(200, 320, CV_8UC3), backproj;
    bool paused = false;
    
    for(;;)
    {
        if( !paused )
        {
            cap >> frame;
            if( frame.empty() )
                break;
        }
        
        frame.copyTo(image);
        
        if( !paused )
        {
            cvtColor(image, hsv, COLOR_BGR2HSV);
            
            if( trackObject )
            {
                int _vmin = vmin, _vmax = vmax;
                
                inRange(hsv, Scalar(0, smin, MIN(_vmin,_vmax)),
                        Scalar(180, 256, MAX(_vmin, _vmax)), mask);
                int ch[] = {0, 0};
                hue.create(hsv.size(), hsv.depth());
                mixChannels(&hsv, 1, &hue, 1, ch, 1);
                
                if( trackObject < 0 )
                {
                    Mat roi(hue, selection), maskroi(mask, selection);
                    calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
                    normalize(hist, hist, 0, 255, NORM_MINMAX);
                    
                    trackWindow = selection;
                    trackObject = 1;
                    
                    histimg = Scalar::all(0);
                    int binW = histimg.cols / hsize;
                    Mat buf(1, hsize, CV_8UC3);
                    for( int i = 0; i < hsize; i++ )
                        buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180./hsize), 255, 255);
                    cvtColor(buf, buf, COLOR_HSV2BGR);
                    
                    for( int i = 0; i < hsize; i++ )
                    {
                        int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows/255);
                        rectangle( histimg, Point(i*binW,histimg.rows),
                                  Point((i+1)*binW,histimg.rows - val),
                                  Scalar(buf.at<Vec3b>(i)), -1, 8 );
                    }
                }
                
                calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
                backproj &= mask;
                RotatedRect trackBox = CamShift(backproj, trackWindow,
                                                TermCriteria( TermCriteria::EPS | TermCriteria::COUNT, 10, 1 ));
                if( trackWindow.area() <= 1 )
                {
                    int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5)/6;
                    trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
                                       trackWindow.x + r, trackWindow.y + r) &
                    Rect(0, 0, cols, rows);
                }
                
                if( backprojMode )
                    cvtColor( backproj, image, COLOR_GRAY2BGR );
                ellipse( image, trackBox, Scalar(0,0,255), 3, LINE_AA );
            }
        }
        else if( trackObject < 0 )
            paused = false;
        
        if( selectObject && selection.width > 0 && selection.height > 0 )
        {
            Mat roi(image, selection);
            bitwise_not(roi, roi);
        }
        
        imshow( "CamShift Demo", image );
        imshow( "Histogram", histimg );
        
        char c = (char)waitKey(10);
        if( c == 27 )
            break;
        switch(c)
        {
            case 'b':
                backprojMode = !backprojMode;
                break;
            case 'c':
                trackObject = 0;
                histimg = Scalar::all(0);
                break;
            case 'h':
                showHist = !showHist;
                if( !showHist )
                    destroyWindow( "Histogram" );
                else
                    namedWindow( "Histogram", 1 );
                break;
            case 'p':
                paused = !paused;
                break;
            default:
                ;
        }
    }
    
    return 0;
}