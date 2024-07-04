#ifndef MHI_H
#define MHI_H
////////////////// this MHI conde is used to determine the initial z position
#include <opencv2/core/core.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/optflow.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <iostream>
using namespace cv;
using namespace std;
using namespace cv::motempl;
const double MHI_DURATION = 2.5;//6////2.8
const double MAX_TIME_DELTA = 4;//4
const double MIN_TIME_DELTA = 0.01;///0.05
// Global variables
extern char filename1[50];
extern vector<Mat> buf;
extern int last;
extern int num_local_1;
extern int num_local_2;
extern int right_diretcion_flag;
extern int right_number;
extern int left_number;
extern int avg_1;
extern Mat mhi, orient, mask, segmask, zplane;
extern vector<Rect> regions;
extern ofstream myfilex;
extern int direction_judge;
// char filename1[50] = { 0 };
// // number of cyclic frame buffer used for motion detection
// // (should, probably, depend on FPS)
// // ring image buffer
// vector<Mat> buf(2);
// int last = 0;
// int num_local_1 = -1;
// int num_local_2 = -1;
// int right_diretcion_flag = 0;
// int right_number = 0;
// int left_number = 0;
// int avg_1 = -1;
// // temporary images
// Mat mhi, orient, mask, segmask, zplane;
// vector<Rect> regions;
// // parameters:
// //  img - input video frame
// //  dst - resultant motion picture
// //  args - optional parameters
// //global variables
// ofstream myfilex;
// int direction_judge = -1;
// //-1 is initial
// //0 is not moving
// //1 is going inside
// //2 is going outside
int findpipettetip(Mat img_head, Point2f pipette_tip_corners[]);
void update_mhi(const Mat& img_mhi, Mat& dst, int diff_threshold, int& avg);
int use_MHI(Mat& frame, Mat& background, Mat& pip_bw, int tip_x, int tip_y);
#endif // MHI_H