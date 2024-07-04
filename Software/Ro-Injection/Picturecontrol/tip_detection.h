#ifndef TIP_DETECTION_H
#define TIP_DETECTION_H
#include "opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
using namespace cv;
class TemplateMatchingRowAndColResult
{
	public:
		int col;
		int row;
};
void Pipette_tip_detection(Mat img_pipette_source, Point2f& Pipette_tip);
void Pipette_tip_detection_ROI(Mat img_pipette_background, Mat img_pipette_source, Point2f& Pipette_tip);
#endif
