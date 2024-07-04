#include "stdafx.h"
#include "worm track.h"
int sum_pix_val = 0;
double average_pix_val = 0;
int num = 0;
char frame_num[50] = {0};
double ave_pix(int roi_i, int roi_j, Mat template_img)
{
	for (int i = (template_img.rows / 2 - roi_i / 2); i < (template_img.rows / 2 + roi_i / 2); i++)
	{
		for (int j = (template_img.cols / 2 - roi_j / 2); j < (template_img.cols / 2 + roi_j / 2); j++)
		{
			sum_pix_val += template_img.at<uchar>(i, j);
		}
	}
	average_pix_val = sum_pix_val / (roi_i * roi_j);
	printf("%f, \n", average_pix_val);
	return (average_pix_val);
}

int max_pix(Mat worm_img_pix)
{
	int maximum_val = 0;
	for (int i = 0; i < worm_img_pix.rows; i++)
	{
		for (int j = 0; j < worm_img_pix.cols; j++)
		{
			if (worm_img_pix.at<uchar>(i, j) > maximum_val)
			{
				maximum_val = worm_img_pix.at<uchar>(i, j);
			}
		}
	}
	return (maximum_val);
}

int min_pix(Mat worm_img_pix)
{
	int minmum_val = 255;
	for (int i = 0; i < worm_img_pix.rows; i++)
	{
		for (int j = 0; j < worm_img_pix.cols; j++)
		{
			if (worm_img_pix.at<uchar>(i, j) < minmum_val)
			{
				minmum_val = worm_img_pix.at<uchar>(i, j);
			}
		}
	}
	return (minmum_val);
}

void binarizing(double thresh, int min_worm_pix_val, int max_worm_pix_val, int average_pix, Mat worm_image)
{
	GaussianBlur(worm_image, worm_image, Size(3, 3), 1, 1); /* / preprocess the image */
	for (int i = 0; i < worm_image.rows; i++)
	{
		for (int j = 0; j < worm_image.cols; j++)
		{
			/*
			 * printf("%d, ", worm_image.at<uchar>(i, j));
			 * ofstream file1;
			 * file1.open("E:/worm diameter and length calculation/C.elegans_1_5x_magnification.txt");
			 */
			double temp = (worm_image.at<uchar>(i, j) - min_worm_pix_val) * (worm_image.at<uchar>(i, j) - min_worm_pix_val) * 255.0 / ((max_worm_pix_val - min_worm_pix_val) * (max_worm_pix_val - min_worm_pix_val)); /*
																																																						* /// enhanicng the contrast by employing the power-law.
																																																						* /// in addition, the range of pixel values has been narrowed from [0,255] to [min_worm_pix_val,max_worm_pix_val]
																																																						* /// this further enhanced the contrast of image
																																																						*/
			double average_pix_temp = (average_pix - min_worm_pix_val) * (average_pix - min_worm_pix_val) * 255.0 / ((max_worm_pix_val - min_worm_pix_val) * (max_worm_pix_val - min_worm_pix_val));
			if ((int)(average_pix_temp - temp) > thresh) /* ///abs is not employed this is becasue when the background is overlapped with the worm, */
			{											 /* ////the pixel value must be smaller than the average background pixel values */
				worm_image.at<uchar>(i, j) = 255;
			}
			else
			{
				worm_image.at<uchar>(i, j) = 0;
			}
		}
		/* printf("\n "); */
	}
}

void RemoveSmallContours(vector<vector<Point>> *rough_contours, vector<Vec4i> hierarchy, vector<Point> &contours, int &idx_contours)
{
	if (!rough_contours->empty() && !hierarchy.empty())
	{
		int idx = 0;
		int maxregion = (*rough_contours)[idx].size();
		int regionnumber = idx;
		int idy = 0;									 /* /////////////////////////1113 */
		int maxregion_2 = (*rough_contours)[idy].size(); /* /////////////////1113 */
		int regionnumber_2 = idy;						 /* ////////////////1113 */
		for (; idx >= 0; idx = hierarchy[idx][0])
		{
			if ((*rough_contours)[idx].size() > maxregion)
			{
				maxregion = (*rough_contours)[idx].size();
				regionnumber = idx;
			}
		}
		for (; idy >= 0; idy = hierarchy[idy][0])
		{
			if (idy != regionnumber)
			{
				if ((*rough_contours)[idy].size() > maxregion_2)
				{
					maxregion_2 = (*rough_contours)[idy].size();
					regionnumber_2 = idy;
				}
			}
		}
		Moments mu = moments((*rough_contours)[regionnumber], true);
		Point2f mc = Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);
		Moments mu1 = moments((*rough_contours)[regionnumber_2], true);
		Point2f mc1 = Point2f(mu1.m10 / mu1.m00, mu1.m01 / mu1.m00);
		if (mc1.x < mc.x)
		{
			regionnumber = regionnumber_2;
		}
		contours = (*rough_contours)[regionnumber];
		idx_contours = regionnumber;
	}
}

void RemoveSmallContours_2(vector<vector<Point>> &rough_contours, vector<Vec4i> hierarchy, int &idx_contours)
{
	if (rough_contours.size() != 0)
	{
		int area_max = 0;
		int regionnumber = 0;
		/* double area_temp1[20]; */
		vector<double> area_temp1(rough_contours.size());
		for (int i = 0; i < rough_contours.size(); i++)
		{
			area_temp1[i] = contourArea(rough_contours[i], false);
		}
		for (int i = 0; i < rough_contours.size(); i++)
		{
			if (area_temp1[i] > area_max)
			{
				area_max = area_temp1[i];
				regionnumber = i;
			}
		}
		idx_contours = regionnumber;
	}
}

int findmaxcont(Mat img_con, Mat worm_img_origin, vector<Point> &worm_contours, Mat &img_har)
{
	Mat img_con_ori = img_con.clone();
	Mat worm_img_origin_1 = worm_img_origin.clone();
	Mat img_har_1 = Mat::zeros(img_con.size(), CV_32FC1);
	vector<vector<Point>> img_contours;
	vector<Vec4i> img_hierarchy;
	vector<Point> smooth_contours, contour, tab, dist_contour;
	int index_contours = 0;
	findContours(img_con, img_contours, img_hierarchy, RETR_CCOMP, CHAIN_APPROX_NONE);
	if (img_contours.size() == 0)
	{
		return (0);
	}
	RemoveSmallContours(&img_contours, img_hierarchy, worm_contours, index_contours); /* / find the maximum region which is the worm body */

	/*for (int i = 0; i < img_contours.size(); i++)
	 * {
	 * if (i != index_contours)
	 * {
	 * drawContours(img_con_ori, img_contours, i, Scalar(0), FILLED, 8, img_hierarchy);
	 * }
	 * }*/
	drawContours(img_har_1, img_contours, index_contours, Scalar(255), FILLED, 8, img_hierarchy);
	img_har = img_har_1.clone();
	/*
	 * imshow("contours_worm_2", img_har);
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm rotation project/0227worm rotation_2/contours_worm_2.jpg", img_har);
	 * imshow("contours_worm_1", img_con_ori);///// only worm body is left
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm rotation project/0227worm rotation_2/only worm left_binary.jpg", img_con_ori);
	 * drawContours(worm_img_origin_1, img_contours, index_contours, Scalar(0), FILLED, 8, img_hierarchy);
	 * imwrite("only worm.jpg", worm_img_origin);
	 * Moments mu = moments(worm_contours, true);
	 * Point2f mc = Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);
	 * ofstream file2;
	 * file2.open("F:/McGill Ph. D/Microinjection of C.elegans Project/worm rotation project/0227worm rotation_2/mass.txt");
	 * file2 << mc.x << "," << mc.y;
	 * circle(worm_img_origin_1, mc, 2, Scalar(255, 0, 255), 1, 8);
	 * imshow("contours_worm", worm_img_origin_1);
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm rotation project/0227worm rotation_2/contours_worm.jpg", worm_img_origin);
	 * waitKey();
	 */
	return (1);
}

int findheadtail(Mat img_cont_ht, Mat worm_img_origin, vector<Point> &worm_contours1, Point2f worm_corners[])
{
	Mat img_thresh = Mat::zeros(img_cont_ht.size(), CV_32FC1);
	Mat img_norm, img_scale_harris, img_scalebw_harris;
	if (worm_contours1.size() == 0)
	{
		return (0);
	}
	/***************************using harris corner algorithm to detect  corners *********/
	cornerHarris(img_cont_ht, img_thresh, 15, 3, 0.04); /* 9 */
	normalize(img_thresh, img_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(img_norm, img_scale_harris); /* /Scales, calculates absolute values, and converts the result to 8-bit */
	/* imshow("intestine_img_original", img_scalebw_harris); */
	threshold(img_scale_harris, img_scalebw_harris, 50, 255, CV_THRESH_BINARY); /* 110 */
	/*
	 * imshow("intestine_img_second", img_scalebw_harris);
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm detection demo video/intestine_img_second.jpg", img_scalebw_harris);
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm rotation project/0227worm rotation_2/harris_worm.jpg", img_scalebw_harris);
	 */
	/***************************using harris corner algorithm to fdetect  corners *********/
	/***************based on processed images, we found corners*********/
	vector<vector<Point>> dif_contours1;
	vector<Vec4i> hierarchy1;
	double area_temp[30] = {0};
	double max_area = 0;
	double second_max_area = 0;
	int idx_max_area = 0;
	int idx_second_max_area = 0;
	int idx_tail_based_distance = 0;
	double length_center_area = 0;
	double length_center_area_temp = 0;
	double dis_temp[30] = {0};
	double max_dis = 0;
	int idx_max_dis_1 = 0;
	int idx_max_dis_2 = 0;
	findContours(img_scalebw_harris, dif_contours1, hierarchy1, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	vector<vector<Point>> poly_contours_findingcn(dif_contours1.size());
	vector<Point2f> center_findingcn(dif_contours1.size());
	vector<float> radius_findingcn(dif_contours1.size());
	for (int i = 0; i < dif_contours1.size(); i++)
	{
		approxPolyDP(Mat(dif_contours1[i]), poly_contours_findingcn[i], 2, true);
		minEnclosingCircle(Mat(poly_contours_findingcn[i]), center_findingcn[i], radius_findingcn[i]);
	}
	/***************based on processed images, we found that head/tail corner provides the largest distance *********/
	/****************************finding the center of a circle enclosing these two areas*******************************/
	/**************************center of these two circles can help us find the coarse position of head and tail*********/
	vector<vector<Point>> poly_contours1(dif_contours1.size());
	vector<Point2f> center1(dif_contours1.size());
	vector<float> radius1(dif_contours1.size());
	for (int i = 0; i < dif_contours1.size(); i++)
	{
		approxPolyDP(Mat(dif_contours1[i]), poly_contours1[i], 2, true);
		minEnclosingCircle(Mat(poly_contours1[i]), center1[i], radius1[0]);
	}
	for (int i = 0; i < dif_contours1.size(); i++)
	{
		if (dis(center1[i], center1[0]) > max_dis)
		{
			idx_max_dis_1 = i;
			max_dis = dis(center1[i], center1[0]);
		}
	}
	max_dis = 0;
	for (int i = 0; i < dif_contours1.size(); i++)
	{
		if (dis(center1[i], center1[idx_max_dis_1]) > max_dis)
		{
			idx_max_dis_2 = i;
			max_dis = dis(center1[i], center1[idx_max_dis_1]);
		}
	}
	max_dis = 0;
	circle(worm_img_origin, center1[idx_max_dis_1], 2, Scalar(255, 255, 255), 2, 8); /* /// */
	circle(worm_img_origin, center1[idx_max_dis_2], 2, Scalar(255, 255, 255), 2, 8); /* /// */
	/*
	 * printf("%f, %f,\n", center1[0].x, center1[0].y);
	 * printf("%f, %f,\n", center1[1].x, center1[1].y);
	 * printf("%f, %f,\n", worm_contours1[22].x, worm_contours1[22].y);
	 */
	worm_corners[0].x = center1[idx_max_dis_1].x;
	worm_corners[0].y = center1[idx_max_dis_1].y;
	worm_corners[1].x = center1[idx_max_dis_2].x;
	worm_corners[1].y = center1[idx_max_dis_2].y;
	/*
	 * imshow("headtail image����intestine", worm_img_origin);
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm detection demo video/headtail image����intestine.jpg", worm_img_origin);
	 * waitKey();
	 */
	/**************************center of these two circles can help us find the coarse position of head and tail*********/
	/*************************finding the center of a circle enclosing these two areas**********************/
	return (1);
}

int findheadtail_2(Mat img_cont_ht, Mat worm_img_origin, vector<Point> &worm_contours_2, Point2f worm_corners_2[])
{
	Mat img_thresh = Mat::zeros(img_cont_ht.size(), CV_32FC1);
	Mat img_norm, img_scale_harris, img_scalebw_harris;
	if (worm_contours_2.size() == 0)
	{
		return (0);
	}
	/***************************using harris corner algorithm to detect  corners *********/
	cornerHarris(img_cont_ht, img_thresh, 15, 3, 0.04); /* 9 */
	normalize(img_thresh, img_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(img_norm, img_scale_harris);								/* /Scales, calculates absolute values, and converts the result to 8-bit */
	threshold(img_scale_harris, img_scalebw_harris, 60, 255, CV_THRESH_BINARY); /* 85 */
	/*
	 * imshow("harris_corner_img_second", img_scalebw_harris);
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm rotation project/0227worm rotation_2/harris_worm.jpg", img_scalebw_harris);
	 */
	/***************************using harris corner algorithm to detect  corners *********/
	/***************based on processed images, we found corners*********/
	vector<vector<Point>> dif_contours1;
	vector<Vec4i> hierarchy1;
	double area_temp[30] = {0};
	double max_area = 0;
	double second_max_area = 0;
	int idx_max_area = 0;
	int idx_second_max_area = 0;
	int idx_tail_based_distance = 0;
	double length_center_area = 0;
	double length_center_area_temp = 0;
	double dis_temp[30] = {0};
	double max_dis = 0;
	int idx_max_dis_1 = 0;
	int idx_max_dis_2 = 0;
	findContours(img_scalebw_harris, dif_contours1, hierarchy1, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	vector<vector<Point>> poly_contours_findingcn(dif_contours1.size());
	vector<Point2f> center_findingcn(dif_contours1.size());
	vector<float> radius_findingcn(dif_contours1.size());
	for (int i = 0; i < dif_contours1.size(); i++)
	{
		approxPolyDP(Mat(dif_contours1[i]), poly_contours_findingcn[i], 2, true);
		minEnclosingCircle(Mat(poly_contours_findingcn[i]), center_findingcn[i], radius_findingcn[i]);
	}
	/***************based on processed images, we found that head corner provides a largest area with large value*********/
	/*********based on processed images, we found that tail corner provides a second largest area with large value*******/
	/****************************finding the center of a circle enclosing these two areas*******************************/
	/**************************center of these two circles can help us find the coarse position of head and tail*********/
	vector<vector<Point>> poly_contours1(dif_contours1.size());
	vector<Point2f> center1(dif_contours1.size());
	vector<float> radius1(dif_contours1.size());
	for (int i = 0; i < dif_contours1.size(); i++)
	{
		approxPolyDP(Mat(dif_contours1[i]), poly_contours1[i], 2, true);
		minEnclosingCircle(Mat(poly_contours1[i]), center1[i], radius1[0]);
	}
	for (int i = 0; i < dif_contours1.size(); i++)
	{
		if (dis(center1[i], center1[0]) > max_dis)
		{
			idx_max_dis_1 = i;
			max_dis = dis(center1[i], center1[0]);
		}
	}
	max_dis = 0;
	for (int i = 0; i < dif_contours1.size(); i++)
	{
		if (dis(center1[i], center1[idx_max_dis_1]) > max_dis)
		{
			idx_max_dis_2 = i;
			max_dis = dis(center1[i], center1[idx_max_dis_1]);
		}
	}
	max_dis = 0;
	circle(worm_img_origin, center1[idx_max_dis_1], 2, Scalar(255, 255, 0), 2, 8); /* /// head is the largest area */
	circle(worm_img_origin, center1[idx_max_dis_2], 2, Scalar(255, 255, 0), 2, 8); /* /// tail is the second area */
	worm_corners_2[0].x = center1[idx_max_dis_1].x;
	worm_corners_2[0].y = center1[idx_max_dis_1].y;
	worm_corners_2[1].x = center1[idx_max_dis_2].x;
	worm_corners_2[1].y = center1[idx_max_dis_2].y;
	/*
	 * printf("%f, %f,\n", center1[idx_max_dis_1].x, center1[idx_max_dis_1].y);
	 * printf("%f, %f,\n", center1[idx_max_dis_2].x, center1[idx_max_dis_2].y);
	 * printf("%f, %f,\n", worm_corners_2[0].x, worm_corners_2[0].y);
	 * printf("%f, %f,\n", worm_corners_2[1].x, worm_corners_2[1].y);
	 * imshow("headtail image", worm_img_origin);
	 * waitKey();
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm detection demo video/headtail image.jpg", worm_img_origin);
	 */
	/**************************center of these two circles can help us find the coarse position of head and tail*********/
	/*************************finding the center of a circle enclosing these two areas**********************/
	/*******************using dot product of two vectors to determine which point is tail or head corner****************/
	return (1);
}

double dis(Point p1, Point p2)
{
	double a = (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y);
	return (a);
}

int findmidlines(Mat image_origin_mid, vector<Point> &worm_contours, Point &head, Point &tailint, int &head_idx, int &tail_idx)
{
	/* notes: when the findcontours algorithm is employed, the points of finding contours are stored in counter -closckwise order */
	int length_worm_boundary = 0;
	int pt_num_top_boundary = 0;
	int pt_num_bot_boundary = 0;
	double ratio_top_boundary = 0;
	double ratio_bot_boundary = 0;
	vector<Point> top_boundary(102);
	vector<Point> bot_boundary(102);
	vector<Point> mid_line(102);
	int top_boundary_picking_point[102] = {0};
	int bot_boundary_picking_point[102] = {0};
	double radius_worm[102] = {0};
	double distance_mid_line[101] = {0};
	double volume_worm = 0;
	double Pi = 3.14159265358979323846;
	double mid_length = 0;
	length_worm_boundary = worm_contours.size();
	Mat img_line_show = image_origin_mid.clone();
	ofstream file1;
	file1.open("F:/McGill Ph. D/Microinjection of C.elegans Project/worm rotation project/0227worm rotation_2/1.txt");
	/************************counting the number of points on the top boundary of worm body***************/
	if (tail_idx <= head_idx) /* /////// if tail_idx<head_idx, the starting point is on the bottom boundary */
	{
		pt_num_top_boundary = head_idx - tail_idx - 1;
		pt_num_bot_boundary = tail_idx + length_worm_boundary - head_idx - 1;
	}
	else
	{
		pt_num_bot_boundary = tail_idx - head_idx - 1;
		pt_num_top_boundary = head_idx + length_worm_boundary - tail_idx - 1;
	}
	/************************counting the number of points on the top boundary of worm body***************/
	/****************************taking 100 points of each boundary for calculating the midline***********/
	ratio_top_boundary = pt_num_top_boundary / 100.0;
	ratio_bot_boundary = pt_num_bot_boundary / 100.0;
	for (int i = 0; i < 102; i++)
	{
		if (i == 0)
		{
			top_boundary_picking_point[i] = (int)i * ratio_top_boundary;
			bot_boundary_picking_point[i] = (int)i * ratio_bot_boundary;
			top_boundary[0] = worm_contours[head_idx];
			bot_boundary[0] = worm_contours[head_idx];
		}
		else if (i == 101)
		{
			top_boundary_picking_point[i] = 0;
			bot_boundary_picking_point[i] = 0;
			top_boundary[101] = worm_contours[tail_idx];
			bot_boundary[101] = worm_contours[tail_idx];
		}
		else
		{
			top_boundary_picking_point[i] = (int)i * ratio_top_boundary;
			bot_boundary_picking_point[i] = (int)i * ratio_bot_boundary;
			top_boundary[i] = worm_contours[(head_idx - top_boundary_picking_point[i] + length_worm_boundary) % length_worm_boundary];
			bot_boundary[i] = worm_contours[(head_idx + top_boundary_picking_point[i]) % length_worm_boundary];
		}
		mid_line[i] = (top_boundary[i] + bot_boundary[i]) / 2;
		if (i == 0 || i == 101)
		{
			radius_worm[i] = 0;
		}
		else
		{
			radius_worm[i] = sqrt((top_boundary[i].x - mid_line[i].x) * (top_boundary[i].x - mid_line[i].x) + (top_boundary[i].y - mid_line[i].y) * (top_boundary[i].y - mid_line[i].y));
		}
		printf("radius: %f,\n", radius_worm[i]);
		file1 << "    " << radius_worm[i] << "\n";
		circle(image_origin_mid, top_boundary[i], 1, Scalar(255, 255, 255), 1, 8);
		circle(image_origin_mid, bot_boundary[i], 1, Scalar(255, 255, 255), 1, 8);
		circle(image_origin_mid, mid_line[i], 1, Scalar(255, 255, 255), 1, 8);
	}
	/****************************taking 100 points of each boundary for calculating the midline***********/
	/* imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm rotation project/0227worm rotation_2/image_origin_mid.jpg", image_origin_mid); */
	/**************************** calculating the distance between each point of  midline***********/
	file1 << "    " << "\n";
	for (int i = 0; i < 101; i++)
	{
		distance_mid_line[i] = sqrt((mid_line[i].x - mid_line[i + 1].x) * (mid_line[i].x - mid_line[i + 1].x) + (mid_line[i].y - mid_line[i + 1].y) * (mid_line[i].y - mid_line[i + 1].y));
		mid_length += distance_mid_line[i];
		printf("distance: %f, %d, %d,%d, %d, \n", distance_mid_line[i], mid_line[i].x, mid_line[i].y, mid_line[i + 1].x, mid_line[i + 1].y);
		file1 << "    " << mid_length << " " << mid_line[i].x << " " << mid_line[i].y << "\n";
	}
	/**************************** calculating the distance between each point of  midline***********/
	/**************************** calculating the volume of worm body**********************/
	for (int i = 0; i < 101; i++)
	{
		volume_worm += Pi * distance_mid_line[i] * (radius_worm[i] * radius_worm[i] + radius_worm[i + 1] * radius_worm[i] + radius_worm[i + 1] * radius_worm[i + 1]) / 3;
	}
	file1 << " volume(pixel3):   " << volume_worm << "\n";
	/**************************** calculating the volume of worm body**********************/
	for (int i = 0; i < length_worm_boundary; i++)
	{
		line(img_line_show, worm_contours[i % length_worm_boundary], worm_contours[(i + 1) % length_worm_boundary], Scalar(255, 255, 255), 1, 8);
	}
	/*
	 * imshow("all lines", img_line_show);
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm rotation project/0227worm rotation_2/all lines.jpg", img_line_show);
	 * imshow("clockwise or anti", image_origin_mid);
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm rotation project/0227worm rotation_2/clockwise or anti.jpg", image_origin_mid);
	 * waitKey();
	 */
	return (1);
}

int findhead(Mat img_head, Point2f worm_head_corners[]) /* ////can use the head_up flag to determine whether the top or bottom corenr is head/tail */
{
	/*************Initialize the erosion element*********/
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
	Mat img_bw_head, img_norm, img_scale_harris, img_scalebw_harris;
	int dis_max = 0;
	int idx_max_dis_1 = 0;
	Mat img_bw_head_2 = Mat::zeros(img_head.size(), CV_32FC1);
	threshold(img_head, img_bw_head, 8, 255, CV_THRESH_BINARY); /* 10 */
	erode(img_bw_head, img_bw_head, erosion_element);
	erode(img_bw_head, img_bw_head, erosion_element);
	/*
	 * imshow("worm1", img_bw_head);
	 * waitKey();
	 */
	dilate(img_bw_head, img_bw_head, dilatation_element);
	/* imshow("IMAGE HEAD", img_bw_head); */
	Mat ucharMat1;
	img_bw_head.convertTo(ucharMat1, CV_8UC1);
	/*
	 * waitKey();
	 * sprintf_s(frame_num, "%d IMAGE HEAD.jpg", num);
	 * imwrite(frame_num, img_bw_head);
	 * Mat img_bw_head_2 = img_bw_head.clone();
	 */
	vector<vector<Point>> img_contours;
	vector<Vec4i> img_hierarchy;
	vector<Point> smooth_contours, contour, tab, dist_contour, worm_contours;
	int index_contours = 0;
	findContours(ucharMat1, img_contours, img_hierarchy, RETR_CCOMP, CHAIN_APPROX_NONE);
	if (img_contours.size() == 0)
	{
		return (0);
	}
	RemoveSmallContours_2(img_contours, img_hierarchy, index_contours); /* / find the maximum region which is the worm body */
	drawContours(img_bw_head_2, img_contours, index_contours, Scalar(255), FILLED, 8, img_hierarchy);

	/*for (int i = 0; i < img_contours.size(); i++)
	 * {
	 * if (i != index_contours)
	 * {
	 * drawContours(img_bw_head_2, img_contours, i, Scalar(0), FILLED, 8, img_hierarchy);
	 * }
	 * }
	 */
	/*
	 * imshow("filled figure", img_bw_head_2);
	 * sprintf_s(frame_num, "%d filled figure.jpg", num);
	 * imwrite(frame_num, img_bw_head_2);
	 * waitKey();
	 */
	Mat img_thresh = Mat::zeros(img_bw_head.size(), CV_32FC1);
	/***************************using harris corner algorithm to detect  corners *********/
	cornerHarris(img_bw_head_2, img_thresh, 5, 3, 0.04); /* 9 */
	/* imshow("harris figure", img_thresh); */
	normalize(img_thresh, img_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(img_norm, img_scale_harris); /* /Scales, calculates absolute values, and converts the result to 8-bit */
	/*
	 * imshow("harris_corner_img_coverrsion", img_scale_harris);
	 * sprintf_s(frame_num, "%d harris_corner_img_coverrsion.jpg", num);
	 * imwrite(frame_num, img_scale_harris);
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm detection demo video/harris_corner_img_coverrsion.jpg", img_scale_harris);
	 */
	threshold(img_scale_harris, img_scalebw_harris, 100, 255, CV_THRESH_BINARY); /* 120 */
	/*
	 * imshow("harris_corner_img_second", img_scalebw_harris);
	 * sprintf_s(frame_num, "%d harris_corner_img_second.jpg", num);
	 * imwrite(frame_num, img_scalebw_harris);
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm detection demo video/harris_corner_img_second.jpg", img_scalebw_harris);
	 */
	Moments mu = moments(img_contours[index_contours], true);
	Point2f mc = Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);
	Point mc_int = Point(mc);
	circle(img_scalebw_harris, mc, 2, Scalar(255, 255, 255), 2, 8); /* /// head is the largest area */
	/*
	 * imshow("harris_corner_img_circle", img_scalebw_harris);
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm detection demo video/harris_corner_img_circle.jpg", img_scalebw_harris);
	 * sprintf_s(frame_num, "%d harris_corner_img_circle.jpg", num);
	 * imwrite(frame_num, img_scalebw_harris);
	 */
	/***************************using harris corner algorithm to detect  corners *********/
	/* waitKey(); */
	vector<vector<Point>> img_contours_2;
	vector<Vec4i> img_hierarchy_2;
	vector<Point> smooth_contours_2, contour_2;
	findContours(img_scalebw_harris, img_contours_2, img_hierarchy_2, RETR_CCOMP, CHAIN_APPROX_NONE);
	vector<vector<Point>> poly_contours1(img_contours_2.size());
	vector<Point2f> center1(img_contours_2.size());
	vector<float> radius1(img_contours_2.size());
	vector<Point> center2(img_contours_2.size());
	for (int i = 0; i < img_contours_2.size(); i++)
	{
		approxPolyDP(Mat(img_contours_2[i]), poly_contours1[i], 2, true);
		minEnclosingCircle(Mat(img_contours_2[i]), center1[i], radius1[0]);
		center2[i] = Point(center1[i]);
	}
	for (int i = 0; i < img_contours_2.size(); i++)
	{
		if (dis(center2[i], mc_int) > dis_max)
		{
			if ((center2[i].y > 10) && (center2[i].y < 90) && (center2[i].x > 10) && (center2[i].x < 90))
			{
				idx_max_dis_1 = i;
				dis_max = dis(center2[i], mc_int);
			}
		}
	}
	worm_head_corners[0].x = center2[idx_max_dis_1].x;
	worm_head_corners[0].y = center2[idx_max_dis_1].y;
	/*******************using dot product of two vectors to determine which point is tail or head corner****************/
	/*
	 * circle(img_head, worm_head_corners[0], 2, Scalar(255, 255, 255), 2, 8);///// head is the largest area
	 * imshow("img head_final", img_head);
	 * sprintf_s(frame_num, "%d image_head_final.jpg", num);
	 * imwrite(frame_num, img_head);
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm detection demo video/img head_final.jpg", img_head);
	 * waitKey();
	 */
	num++;
	return (1);
}

int findheadtail_3(Mat img_cont_ht, Mat worm_img_origin, Point2f worm_corners_2[])
{
	Mat img_thresh = Mat::zeros(img_cont_ht.size(), CV_32FC1);
	Mat img_norm, img_scale_harris, img_scalebw_harris;
	/***************************using harris corner algorithm to detect  corners *********/
	cornerHarris(img_cont_ht, img_thresh, 9, 3, 0.04); /* 9 */
	normalize(img_thresh, img_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(img_norm, img_scale_harris);								/* /Scales, calculates absolute values, and converts the result to 8-bit */
	threshold(img_scale_harris, img_scalebw_harris, 75, 255, CV_THRESH_BINARY); /* 80 */
	/*
	 * imshow("harris_corner_img_second", img_scalebw_harris);
	 * waitKey();
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm rotation project/0227worm rotation_2/harris_worm.jpg", img_scalebw_harris);
	 */
	/***************************using harris corner algorithm to fdetect  corners *********/
	/***************based on processed images, we found corners*********/
	vector<vector<Point>> dif_contours1;
	vector<Vec4i> hierarchy1;
	double area_temp[30] = {0};
	double max_area = 0;
	double second_max_area = 0;
	int idx_max_area = 0;
	int idx_second_max_area = 0;
	int idx_tail_based_distance = 0;
	double length_center_area = 0;
	double length_center_area_temp = 0;
	double dis_temp[30] = {0};
	double max_dis = 0;
	int idx_max_dis_1 = 0;
	int idx_max_dis_2 = 0;
	findContours(img_scalebw_harris, dif_contours1, hierarchy1, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	vector<vector<Point>> poly_contours_findingcn(dif_contours1.size());
	vector<Point2f> center_findingcn(dif_contours1.size());
	vector<float> radius_findingcn(dif_contours1.size());
	for (int i = 0; i < dif_contours1.size(); i++)
	{
		approxPolyDP(Mat(dif_contours1[i]), poly_contours_findingcn[i], 2, true);
		minEnclosingCircle(Mat(poly_contours_findingcn[i]), center_findingcn[i], radius_findingcn[i]);
	}
	/***************based on processed images, we found that head corner provides a largest area with large value*********/
	/*********based on processed images, we found that tail corner provides a second largest area with large value*******/
	/****************************finding the center of a circle enclosing these two areas*******************************/
	/**************************center of these two circles can help us find the coarse position of head and tail*********/
	vector<vector<Point>> poly_contours1(dif_contours1.size());
	vector<Point2f> center1(dif_contours1.size());
	vector<float> radius1(dif_contours1.size());
	for (int i = 0; i < dif_contours1.size(); i++)
	{
		approxPolyDP(Mat(dif_contours1[i]), poly_contours1[i], 2, true);
		minEnclosingCircle(Mat(poly_contours1[i]), center1[i], radius1[0]);
	}
	for (int i = 0; i < dif_contours1.size(); i++)
	{
		if (dis(center1[i], center1[0]) > max_dis)
		{
			idx_max_dis_1 = i;
			max_dis = dis(center1[i], center1[0]);
		}
	}
	max_dis = 0;
	for (int i = 0; i < dif_contours1.size(); i++)
	{
		if (dis(center1[i], center1[idx_max_dis_1]) > max_dis)
		{
			idx_max_dis_2 = i;
			max_dis = dis(center1[i], center1[idx_max_dis_1]);
		}
	}
	max_dis = 0;
	/*
	 * circle(worm_img_origin, center1[idx_max_dis_1], 2, Scalar(255, 255, 0), 2, 8);///// head is the largest area
	 * circle(worm_img_origin, center1[idx_max_dis_2], 2, Scalar(255, 255, 0), 2, 8);///// tail is the second area
	 */
	worm_corners_2[0].x = center1[idx_max_dis_1].x;
	worm_corners_2[0].y = (int)center1[idx_max_dis_1].y;
	worm_corners_2[1].x = center1[idx_max_dis_2].x;
	worm_corners_2[1].y = center1[idx_max_dis_2].y;
	/*
	 * printf("%f, %f,\n", center1[idx_max_dis_1].x, center1[idx_max_dis_1].y);
	 * printf("%f, %f,\n", center1[idx_max_dis_2].x, center1[idx_max_dis_2].y);
	 * printf("%f, %f,\n", worm_corners_2[0].x, worm_corners_2[0].y);
	 * printf("%f, %f,\n", worm_corners_2[1].x, worm_corners_2[1].y);
	 * imshow("headtail image", worm_img_origin);
	 * waitKey();
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm detection demo video/headtail image.jpg", worm_img_origin);
	 */
	/**************************center of these two circles can help us find the coarse position of head and tail*********/
	/*************************finding the center of a circle enclosing these two areas**********************/
	/*******************using dot product of two vectors to determine which point is tail or head corner****************/
	return (1);
}