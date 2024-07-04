#include "stdafx.h"
#include <windows.h>
#include "opencv2\highgui\highgui.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv.hpp"  
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp> 
#include <stdio.h>
#include "math.h"
using namespace cv;
using namespace std;
double ave_pix(int roi_i, int roi_j, Mat template_img);
void binarizing(double thresh, int min_worm_pix_val, int max_worm_pix_val, int average_pix, Mat worm_image);
int max_pix(Mat worm_img_pix);
int min_pix(Mat worm_img_pix);
void RemoveSmallContours(vector<vector<Point>> *rough_contours, vector<Vec4i> hierarchy, vector<Point> &contours);
int findheadtail(Mat img_cont_ht, Mat worm_img_origin, vector<Point> &worm_contours, Point2f worm_corners[]);
int findmaxcont(Mat img_contour, Mat worm_img_origin, vector<Point> &worm_contours, Mat &img_har);
int findmidlines(Mat image_origin_mid, vector<Point> &worm_contours, Point &head, Point &tailint, int &head_idx, int &tail_idx);
double dis(Point p1, Point p2);
int findheadtail_2(Mat img_cont_ht, Mat worm_img_origin, vector<Point> &worm_contours_2, Point2f worm_corners_2[]);
int findhead(Mat image, Point2f worm_corners_2[]);
void RemoveSmallContours_2(vector<vector<Point>> &rough_contours, vector<Vec4i> hierarchy, int &idx_contours);
int findheadtail_3(Mat img_cont_ht, Mat worm_img_origin, Point2f worm_corners_2[]);