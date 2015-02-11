#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <iomanip>
#include <stdio.h>

using namespace cv;
using namespace std;

//hide the local functions in an anon namespace
namespace {

    //Pointer to Pointer (i.e. array of char*) - Could also declare as char* av[]
    void help(char** av) {
        cout << "The program captures frames from a video file, image sequence (01.jpg, 02.jpg ... 10.jpg) or camera connected to your computer." << endl
        << "Usage:\n" << av[0] << " <video file, image sequence or device number>" << endl
        << "q,Q,esc -- quit" << endl
        << "space   -- save frame" << endl << endl
        << "\tTo capture from a camera pass the device number. To find the device number, try ls /dev/video*" << endl
        << "\texample: " << av[0] << " 0" << endl
        << "\tYou may also pass a video file instead of a device number" << endl
        << "\texample: " << av[0] << " video.avi" << endl
        << "\tYou can also pass the path to an image sequence and OpenCV will treat the sequence just like a video." << endl
        << "\texample: " << av[0] << " right%%02d.jpg" << endl;
    }

    // CG - Note here we are PASSING BY REFERENCE!
    int process(VideoCapture &capture) {
        int n = 0;

        //char filename[200];

        string filename2;

        string window_name = "video | q or esc to quit";
        cout << "press space to save a picture. q or esc to quit" << endl;
        namedWindow(window_name, CV_WINDOW_KEEPRATIO); //resizable window;
        Mat frame;

        for (;;) {

            capture >> frame;
            if (frame.empty())
                break;

            imshow(window_name, frame);

            char key = (char) waitKey(30); //delay N millis, usually long enough to display and capture input

            std::stringstream fmt;

            switch (key) {
                case 'q':
                case 'Q':
                case 27: //escape key
                    return 0;
                case ' ': //Save an image
                   // sprintf(filename,"filename%.3d.jpg",n++);

                    fmt << "filenamesioned" << std::setfill('0') << std::setw(5) << n++ << ".jpg";

                    imwrite(fmt.str(),frame);

                    cout << "Saved " << fmt.str() << endl;
                    break;
                default:
                    break;
            }
        }
        return 0;
    }
}

int main(int ac, char** av) {

    if (ac != 2) {
        help(av);
        return 1;
    }

    std::string arg = av[1];

    //VideoCapture capture(arg);

    VideoCapture capture = VideoCapture(arg); //try to open string, this will attempt to open it as a video file or image sequence

    if (!capture.isOpened()) //if this fails, try to open as a video camera, through the use of an integer param

        capture.open(atoi(arg.c_str()));

    if (!capture.isOpened()) {

        cerr << "Failed to open the video device, video file or image sequence!\n" << endl;

        help(av);

        return 1;

    }

    return process(capture);
}