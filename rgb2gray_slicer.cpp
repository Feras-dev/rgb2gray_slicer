/**
 * @file rgb2gray_slicer.cpp
 * @author Feras Alshehri (falshehri@mail.csuchico.edu)
 * @brief a program to convert a 3 channel, RGB video into grayscale frames in PGM format,
 * one for each channel.
 * @version 0.1
 * @date 2022-03-01
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <iostream>
#include <string>
#include <unistd.h>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#define GRAY_SCALE_TYPE 0
#define TEXT_1_LOCATION (Point(20, 20))
#define TEXT_2_LOCATION (Point(20, 40))
#define TEXT_LINE_THICKNESS 1
#define TEXT_LINE_SCALE_FACTOR 1.2
#define BGR_WHITE (Scalar(255, 255, 255))
#define WIN_TITLE_1 "source frame"
#define WIN_TITLE_2 "gray frame"
#define WIN_SIZE_SCALE_DOWN 6
#define SYSTEM_ERROR (-1)
#define ESCAPE_KEY (27)

using namespace std;
using namespace cv;

// file global
static string       out_filepath = "";
static unsigned int frame_num    = 0;
static unsigned int channel      = 0; // 0 = gray-scale,
                                      // 1 = red channel only,
                                      // 2 = green channel only,
                                      // 3 = blue channel only

/**
 * @brief main loop to run during runtime and control the state machine.
 *
 */
static void mainLoop(string file_path) {
    char winInput;               // holds user input while window is in-focus
    Mat  frame_src;              // source frame
    Mat  frame_gray_single_band; // frame in gray scale based on a single band.

    // initialize video stream from video file provided
    VideoCapture cap(file_path);

    if (!cap.isOpened()) { exit(SYSTEM_ERROR); }

    // set the specific channel number to retrive
    cap.set(CAP_PROP_CHANNEL, channel);

    // create a window to display our video
    namedWindow(WIN_TITLE_1);
    namedWindow(WIN_TITLE_2);

    while (1) {
        // store the frame captured from the video file into our frame
        cap >> frame_src;

        // once all frames have been consumed, break.
        if (frame_src.empty()) { break; }

        // break on ECS key
        if ((winInput = waitKey(10)) == ESCAPE_KEY) { break; }

        // convert into grayscale
        cvtColor(frame_src, frame_gray_single_band, COLOR_BGR2GRAY);

        // display source and processed frame
        imshow(WIN_TITLE_1, frame_src);
        imshow(WIN_TITLE_2, frame_gray_single_band);

        // write out the frame to disk as pgm file
        out_filepath = "./frames_out_chan_";
        out_filepath += to_string(channel);
        out_filepath += "/frame_out";
        out_filepath += to_string(frame_num);
        out_filepath += ".pgm";

        imwrite(out_filepath, frame_gray_single_band);

        // increment frame number
        ++frame_num;
    }

    // clean up
    cap.release();
    frame_src.release();
    frame_gray_single_band.release();
    destroyWindow(WIN_TITLE_1);
    destroyWindow(WIN_TITLE_2);

    return;
}

/**
 * @brief entry point of program.
 *
 * @param argc count of cmdl args.
 * @param argv list of strings, each representing a cmdl arg.
 * @return int returns 0 upon successful execution.
 */
int main(int argc, char **argv) {
    // define command line args
    CommandLineParser parser(argc, argv,
                             "{@video        | | path to video file to process.}"
                             "{help         h|false|show help message}");

    parser.printMessage();
    cout << "The program will process each frame of the video given, and output it as a "
            "PGM file under ./frames_out/*.pgm.\n";
    cout << "Press 'ESC' at any moment to exit program.\n";

    string file_path = parser.get<string>("@video");

    // run the program for each supported channel
    // 0 = blue channel only,
    // 1 = green channel only,
    // 2 = red channel only
    // TODO: use an enum instead to improve readability and scalability
    for (int chan = 0; chan < 3; ++chan) {
        channel   = chan;
        frame_num = 0;

        switch (channel) {
        case 0:
            cout << "\t >> Processing GRAY_SCALE: based on BLUE channel only";
            break;

        case 1:
            cout << "\t >> Processing GRAY_SCALE: based on GREEN channel only";
            break;

        case 2:
            cout << "\t >> Processing GRAY_SCALE: based on RED channel only";
            break;

        default:
            break;
        }

        cout << "(chan=" << channel << ")\n";

        mainLoop(file_path);
    }

    cout << ">> Exiting!" << endl;

    return 0;
}