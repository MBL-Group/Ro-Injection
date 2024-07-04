#include "stdafx.h"
#include "tip_detection.h"
#include <iostream>
using namespace cv;
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
void Pipette_tip_detection(Mat img_pipette_source, Point2f &Pipette_tip)
{
	
	Mat template_img_back = imread("C:/background.jpg");
	cvtColor(template_img_back, template_img_back, CV_RGB2GRAY);
	float ratio_max = 0;
	int contour_index_1 = 0;
	int corner_index = 0;
	double max_x_coordinate = 0;
	int flag_conversion = 0;
	int idy_temp = 0;
	int idy_prev = 0;
	int flag_background = 0;
	int background_ave_pix = 0;
	/*************Initialize the erosion element*********/
	int erosion_type;
	int erosion_size = 1;
	int erosion_elem = 0;
	if (erosion_elem == 0)
	{
		erosion_type = MORPH_RECT;
	}
	else if (erosion_elem == 1)
	{
		erosion_type = MORPH_CROSS;
	}
	else if (erosion_elem == 2)
	{
		erosion_type = MORPH_ELLIPSE;
	}
	Mat erosion_element = getStructuringElement(erosion_type, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
	Mat template_img_back_pipette_bw;
	/*************Initialize the dilation element*********/
	int dilatation_type;
	int dilatation_size = 1;
	int dilatation_elem = 0;
	if (dilatation_elem == 0)
	{
		dilatation_type = MORPH_RECT;
	}
	else if (dilatation_elem == 1)
	{
		dilatation_type = MORPH_CROSS;
	}
	else if (dilatation_elem == 2)
	{
		dilatation_type = MORPH_ELLIPSE;
	}
	long t1, t2;
	Mat dilatation_element = getStructuringElement(erosion_type, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
	/************* dilation element Initialization ends*********/
	Mat template_img_thresh_pipette;
	Mat img_norm_pipette, img_scale_pipette, img_scalebw_pipette;
	Mat template_img_back_pipette_ori = img_pipette_source.clone();
	cvtColor(template_img_back_pipette_ori, template_img_back_pipette_ori, CV_RGB2GRAY);
	Mat template_img_back_pipette = template_img_back - template_img_back_pipette_ori;
	
	Mat template_img_back_pipette_ROI = template_img_back_pipette(Rect(0, 0, 328, 490));
	
	cv::threshold(template_img_back_pipette_ROI, template_img_thresh_pipette, 15, 255, THRESH_BINARY); /* /30 2021-0215 full image processing was changed with ROI process */
	imshow("template", template_img_thresh_pipette);
	
	dilate(template_img_thresh_pipette, template_img_thresh_pipette, dilatation_element);
	dilate(template_img_thresh_pipette, template_img_thresh_pipette, dilatation_element);
	
	erode(template_img_thresh_pipette, template_img_thresh_pipette, erosion_element);
	
	erode(template_img_thresh_pipette, template_img_thresh_pipette, erosion_element);
	imshow("template_2", template_img_thresh_pipette);
	
	Mat imageContours = Mat::zeros(template_img_thresh_pipette.size(), CV_8UC1);
	vector<vector<Point>> dif_contours_pipette;
	vector<Vec4i> hierarchy_pipette;
	
	findContours(template_img_thresh_pipette, dif_contours_pipette, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	
	vector<float> ratio(dif_contours_pipette.size());
	for (int i = 0; i < dif_contours_pipette.size(); i++)
	{
		RotatedRect rect = minAreaRect(dif_contours_pipette[i]);
		Point2f P[4];
		float dis[2] = {0};
		rect.points(P);
		if (P[0].x < 200 || P[1].x < 200 || P[2].x < 200 || P[3].x < 200) /* ////P[0].x<400;20210215 */
		{
			dis[0] = (P[0].x - P[1].x) * (P[0].x - P[1].x) + (P[0].y - P[1].y) * (P[0].y - P[1].y);
			dis[1] = (P[1].x - P[2].x) * (P[1].x - P[2].x) + (P[1].y - P[2].y) * (P[1].y - P[2].y);
			if ((dis[0] > 3) && (dis[1] > 3))
			{
				if (dis[0] < dis[1])
				{
					ratio[i] = dis[1] / dis[0];
				}
				else
				{
					ratio[i] = dis[0] / dis[1];
				}
			}
			else
			{
				ratio[i] = 0;
			}
		}
	}
	for (int i = 0; i < dif_contours_pipette.size(); i++)
	{
		if ((ratio[i] > ratio_max))
		{
			ratio_max = ratio[i];
			contour_index_1 = i;
		}
	}
	
	drawContours(imageContours, dif_contours_pipette, contour_index_1, Scalar(255), CV_FILLED, 8, hierarchy_pipette);
	
	dilate(imageContours, imageContours, dilatation_element);
	erode(imageContours, imageContours, erosion_element);
	dilate(imageContours, imageContours, dilatation_element);
	erode(imageContours, imageContours, erosion_element);
	
	Mat img_thresh_pipette = Mat::zeros(template_img_thresh_pipette.size(), CV_32FC1);
	cornerHarris(imageContours, img_thresh_pipette, 5, 3, 0.04);
	normalize(img_thresh_pipette, img_norm_pipette, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(img_norm_pipette, img_scale_pipette);
	
	cv::threshold(img_scale_pipette, img_scalebw_pipette, 50, 255, CV_THRESH_BINARY); /* 230//150 */
	
	vector<vector<Point>> dif_contours_pipette_2;
	vector<Vec4i> hierarchy_pipette_2;
	findContours(img_scalebw_pipette, dif_contours_pipette_2, hierarchy_pipette_2, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	
	vector<vector<Point>> poly_contours_pipette_2(dif_contours_pipette_2.size());
	vector<Point2f> center_pipette_2(dif_contours_pipette_2.size());
	vector<float> radius_pipette_2(dif_contours_pipette_2.size());
	Point pipette;
	for (int i = 0; i < dif_contours_pipette_2.size(); i++)
	{
		
		minEnclosingCircle(Mat(dif_contours_pipette_2[i]), center_pipette_2[i], radius_pipette_2[i]);
	}
	for (int i = 0; i < dif_contours_pipette_2.size(); i++)
	{
		if (center_pipette_2[i].x > max_x_coordinate)
		{
			max_x_coordinate = center_pipette_2[i].x;
			corner_index = i;
		}
	}
	
	cvtColor(img_pipette_source, img_pipette_source, CV_RGB2GRAY); 
	
	Pipette_tip.x = int(center_pipette_2[corner_index].x);
	Pipette_tip.y = int(center_pipette_2[corner_index].y);
	circle(img_pipette_source, Pipette_tip, 3, Scalar(255, 0, 255), 3);
	imshow("final image", img_pipette_source);
	
	return;
}

void Pipette_tip_detection_ROI(Mat img_pipette_background, Mat img_pipette_source, Point2f &Pipette_tip)
{
	int erosion_type;
	int erosion_size = 1; /* 1 */
	int erosion_elem = 2; /* 0  if using the rectangle erosion type, sometimes the part of feature will be rectangle which will result in distirtation */
	if (erosion_elem == 0)
	{
		erosion_type = MORPH_RECT;
	}
	else if (erosion_elem == 1)
	{
		erosion_type = MORPH_CROSS;
	}
	else if (erosion_elem == 2)
	{
		erosion_type = MORPH_ELLIPSE;
	}
	Mat erosion_element = getStructuringElement(erosion_type, Size(2, 2), Point(erosion_size, erosion_size)); /* / 2 * erosion_size + 1 */
	/*************Initialize the dilation element*********/
	int dilatation_type;
	int dilatation_size = 1;
	int dilatation_elem = 2; /* /0 */
	if (dilatation_elem == 0)
	{
		dilatation_type = MORPH_RECT;
	}
	else if (dilatation_elem == 1)
	{
		dilatation_type = MORPH_CROSS;
	}
	else if (dilatation_elem == 2)
	{
		dilatation_type = MORPH_ELLIPSE;
	}
	long t1, t2;
	Mat dilatation_element1 = getStructuringElement(erosion_type, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
	Mat dilatation_element = getStructuringElement(erosion_type, Size(2, 2), Point(erosion_size, erosion_size));
	Mat ROI_pipette_tip_ROI = img_pipette_source.clone();
	Mat ROI_pipette_tip = ROI_pipette_tip_ROI(Rect(Pipette_tip.x - 20, Pipette_tip.y - 20, 40, 40));
	Mat ROI_pipette_tip_background = img_pipette_background(Rect(Pipette_tip.x - 20, Pipette_tip.y - 20, 40, 40));
	
	cvtColor(ROI_pipette_tip, ROI_pipette_tip, CV_RGB2GRAY);
	double max_x_coordinate = 0;
	int corner_index = 0;
	
	Mat template_img_thresh_pipette_ROI, img_norm_pipette_ROI, img_scale_pipette_ROI, img_scaleBW_pipette_ROI;
	double area_comp;
	double area_temp = 0;
	int area_index = 0;
	Mat ROI_pipette_tip_sub = ROI_pipette_tip_background - ROI_pipette_tip;
	
	threshold(ROI_pipette_tip_sub, template_img_thresh_pipette_ROI, 10, 255, THRESH_BINARY); /* 30 */
	dilate(template_img_thresh_pipette_ROI, template_img_thresh_pipette_ROI, dilatation_element);
	dilate(template_img_thresh_pipette_ROI, template_img_thresh_pipette_ROI, dilatation_element);
	erode(template_img_thresh_pipette_ROI, template_img_thresh_pipette_ROI, erosion_element);
	erode(template_img_thresh_pipette_ROI, template_img_thresh_pipette_ROI, erosion_element);
	
	Mat imageContours_ROI = Mat::zeros(template_img_thresh_pipette_ROI.size(), CV_8UC1);
	Mat img_thresh_pipette_ROI = Mat::zeros(template_img_thresh_pipette_ROI.size(), CV_32FC1);
	vector<vector<Point>> dif_contours_pipette_ROI;
	vector<Vec4i> hierarchy_pipette_ROI;
	findContours(template_img_thresh_pipette_ROI, dif_contours_pipette_ROI, hierarchy_pipette_ROI, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	if (dif_contours_pipette_ROI.size() > 0)
	{
		for (int i = 0; i < dif_contours_pipette_ROI.size(); i++) /* /////////////////////////////size <=0 */
		{
			area_comp = contourArea(dif_contours_pipette_ROI[i], false);
			if (area_comp > area_temp)
			{
				area_temp = area_comp;
				area_index = i;
			}
		}
		drawContours(imageContours_ROI, dif_contours_pipette_ROI, area_index, Scalar(255), CV_FILLED, 8, hierarchy_pipette_ROI);
		cornerHarris(imageContours_ROI, img_thresh_pipette_ROI, 5, 3, 0.04); /* 5,3 */
		normalize(img_thresh_pipette_ROI, img_norm_pipette_ROI, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
		convertScaleAbs(img_norm_pipette_ROI, img_scale_pipette_ROI);
		threshold(img_scale_pipette_ROI, img_scaleBW_pipette_ROI, 80, 255, CV_THRESH_BINARY); /* 120 */
		
		vector<vector<Point>> dif_contours_pipette_2;
		vector<Vec4i> hierarchy_pipette_2;
		findContours(img_scaleBW_pipette_ROI, dif_contours_pipette_2, hierarchy_pipette_2, RETR_EXTERNAL, CHAIN_APPROX_NONE);
		
		if (dif_contours_pipette_2.size() <= 0)
		{
			threshold(img_scale_pipette_ROI, img_scaleBW_pipette_ROI, 50, 255, CV_THRESH_BINARY);
			
			findContours(img_scaleBW_pipette_ROI, dif_contours_pipette_2, hierarchy_pipette_2, RETR_EXTERNAL, CHAIN_APPROX_NONE);
		}
		if (dif_contours_pipette_2.size() <= 0)
		{
			threshold(img_scale_pipette_ROI, img_scaleBW_pipette_ROI, 30, 255, CV_THRESH_BINARY);
			
			findContours(img_scaleBW_pipette_ROI, dif_contours_pipette_2, hierarchy_pipette_2, RETR_EXTERNAL, CHAIN_APPROX_NONE);
		}
		vector<vector<Point>> poly_contours_pipette_2(dif_contours_pipette_2.size());
		vector<Point2f> center_pipette_2(dif_contours_pipette_2.size());
		vector<float> radius_pipette_2(dif_contours_pipette_2.size());
		Point pipette;
		for (int i = 0; i < dif_contours_pipette_2.size(); i++)
		{
			
			minEnclosingCircle(Mat(dif_contours_pipette_2[i]), center_pipette_2[i], radius_pipette_2[i]);
		}
		for (int i = 0; i < dif_contours_pipette_2.size(); i++)
		{
			if (center_pipette_2[i].x > max_x_coordinate)
			{
				max_x_coordinate = center_pipette_2[i].x;
				corner_index = i;
			}
		}
		Pipette_tip.x = center_pipette_2[corner_index].x + Pipette_tip.x - 20;
		Pipette_tip.y = center_pipette_2[corner_index].y + Pipette_tip.y - 20;
		circle(img_pipette_source, Point(center_pipette_2[corner_index].x + Pipette_tip.x - 20, center_pipette_2[corner_index].y + Pipette_tip.y - 20), 2, Scalar(255, 0, 255), 1);
		
	}
	
}