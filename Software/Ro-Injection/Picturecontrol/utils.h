#ifndef UTILS_H
#define UTILS_H
#pragma once
#include<cmath>;
#include "opencv2\highgui\highgui.hpp"
#include "opencv.hpp"  
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
void MatToCImage(Mat &mat, CImage &cImage);
float caldistance(float x1, float x2, float y1, float y2);
float sina(float x0, float x1, float y0, float y1);
float cosa(float x0, float x1, float y0, float y1);
#endif