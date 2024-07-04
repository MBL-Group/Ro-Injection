#include "stdafx.h"
#include "mhi.h"
#include <ctime>
#include <cmath>
/* Global variables definition */
char filename1[50] = {0};
vector<Mat> buf(2);
int last = 0;
int num_local_1 = -1;
int num_local_2 = -1;
int right_diretcion_flag = 0;
int right_number = 0;
int left_number = 0;
int avg_1 = -1;
Mat mhi, orient, mask, segmask, zplane;
vector<Rect> regions;
ofstream myfilex;
int direction_judge = -1;
int findpipettetip(Mat img_head, Point2f pipette_tip_corners[])
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
	int dilatation_elem = 2; /* / 0 */
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
	int x_coordinate_temp = 0;
	int x_coordinate_index = 0;
	Mat img_thresh = Mat::zeros(img_bw_head.size(), CV_32FC1);
	/***************************using harris corner algorithm to detect  corners *********/
	cornerHarris(img_head, img_thresh, 15, 3, 0.04); /* 9 */
	/*
	 * imshow("harris figure", img_thresh);
	 * waitKey();
	 */
	normalize(img_thresh, img_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(img_norm, img_scale_harris); /* / Scales, calculates absolute values, and converts the result to 8-bit */
	/*
	 * imshow("harris_corner_img_coverrsion", img_scale_harris);
	 * waitKey();
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm detection demo video/harris_corner_img_coverrsion.jpg", img_scale_harris);
	 */
	threshold(img_scale_harris, img_scalebw_harris, 30, 255, CV_THRESH_BINARY); /* 120 */
	/*
	 * imshow("harris_corner_img_second", img_scalebw_harris);
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm detection demo video/harris_corner_img_second.jpg", img_scalebw_harris);
	 * Moments mu = moments(img_contours[index_contours], true);
	 * Point2f mc = Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);
	 * Point mc_int = Point(mc);
	 * circle(img_scalebw_harris, mc, 2, Scalar(255, 255, 255), 2, 8);///// head is the largest area
	 * imshow("harris_corner_img_circle", img_scalebw_harris);
	 * imwrite("F:/McGill Ph. D/Microinjection of C.elegans Project/worm detection demo video/harris_corner_img_circle.jpg", img_scalebw_harris);
	 */
	/***************************using harris corner algorithm to fdetect  corners *********/
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
		minEnclosingCircle(Mat(img_contours_2[i]), center1[i], radius1[i]);
		center2[i] = Point(center1[i]);
	}
	for (int i = 0; i < img_contours_2.size(); i++)
	{
		if (center2[i].x > x_coordinate_temp)
		{
			x_coordinate_temp = center2[i].x;
			x_coordinate_index = i;
		}
	}
	pipette_tip_corners[0].x = center2[x_coordinate_index].x + radius1[x_coordinate_index];
	pipette_tip_corners[0].y = center2[x_coordinate_index].y - radius1[x_coordinate_index];
	return (1);
}

void update_mhi(const Mat &img_mhi, Mat &dst, int diff_threshold, int &avg)
{
	double timestamp = (double)clock() / CLOCKS_PER_SEC; /* get current time in seconds */
	Size size = img_mhi.size();
	int i, idx1 = last;
	Rect comp_rect;
	double count;
	double angle;
	Point center;
	double magnitude;
	Scalar color;
	num_local_2++;
	/*
	 * allocate images at the beginning or
	 * reallocate them if the frame size is changed
	 */
	if (mhi.size() != size)
	{
		mhi = Mat::zeros(size, CV_32F);
		zplane = Mat::zeros(size, CV_8U);
		buf[0] = Mat::zeros(size, CV_8U);
		buf[1] = Mat::zeros(size, CV_8U);
	}
	cvtColor(img_mhi, buf[last], COLOR_BGR2GRAY); /* convert frame to grayscale */
	int idx2 = (last + 1) % 2;					  /* index of (last - (N-1))th frame */
	last = idx2;
	Mat silh = buf[idx2];
	absdiff(buf[idx1], buf[idx2], silh); /* get difference between frames(store the diff array to silh) */
	/* //////////////////////////////////////opencv MHI algorithm////////////////////////////////////////////////////// */
	updateMotionHistory(silh, mhi, timestamp, MHI_DURATION); /* update MHI, remember silh is diff of buf[idx1] and buf[idx2] */
	/*
	 * after this, mhi now stores the motion
	 * /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 * convert MHI to blue 8u image
	 */
	mhi.convertTo(mask, CV_8U, 255. / MHI_DURATION, (MHI_DURATION - timestamp) * 255. / MHI_DURATION);
	/* calculate motion gradient orientation and valid orientation mask */
	calcMotionGradient(mhi, mask, orient, MAX_TIME_DELTA, MIN_TIME_DELTA, 3);
	imshow("orient", orient);
	/* waitKey(); */
	int useful_point_count = 0;
	int sum_points = 0;
	int avg_value = 0;
	int largest_point_value = 0;
	int largest_x = 0;
	int largest_y = 0;
	for (int x = 15; x < orient.rows; x++)
	{
		for (int y = 0; y < orient.cols; y++)
		{
			if ((int)orient.at<float>(x, y) != 0)
			{ /* round angles from 315-360 to angles from 0-45 */
				if ((int)orient.at<float>(x, y) >= 271 && (int)orient.at<float>(x, y) <= 360)
				{
					sum_points += 360 - (int)orient.at<float>(x, y);
				}
				else
				{
					sum_points += (int)orient.at<float>(x, y);
				}
				useful_point_count++;
				if ((int)orient.at<float>(x, y) >= largest_point_value)
				{
					largest_point_value = (int)orient.at<float>(x, y);
					largest_x = x;
					largest_y = y;
				}
			}
			/* cout<<orient.at<float>(x, y)<<" "; */
		}
		/* cout << endl; */
	}
	if (useful_point_count != 0)
	{
		avg_value = sum_points / useful_point_count;
	}
	/* cout << "num of points: " << useful_point_count << " sum points: " << sum_points  << " largest: " << largest_point_value <<" avg: "<<avg_value<< endl; */
	if (useful_point_count > 0 && ((avg_value > 0 && avg_value <= 90) || (avg_value > 81 && avg_value <= 270)))
	{
		/* myfilex << num_local_2 << ":    " << avg_value << endl; */
		if (avg_value > 0 && avg_value < 90)
		{
			direction_judge = 1;
			right_number++;
			/* myfilex << avg_value << endl; */
		}
		if (avg_value > 91 && avg_value <= 270)
		{
			direction_judge = 2;

			/*if (right_diretcion_flag == 1)
			 * {
			 * myfilex << avg_value << endl;
			 * }
			 * if (right_diretcion_flag == 0)
			 * {
			 * myfilex << 0 << endl;
			 * }*/
			if (right_diretcion_flag == 0)
			{
				right_number = 0;
			}
			if (right_diretcion_flag == 1)
			{
				left_number++;
			}
		}
	}
	else
	{
		/* myfilex << num_local_2 << ":    " << 0 << endl; */
		direction_judge = 0;
	}
	if (right_number > 50) /* 30 */
	{
		right_diretcion_flag = 1;
	}
	avg = avg_value;
}

int use_MHI(Mat &frame, Mat &background, Mat &pip_bw, int tip_x, int tip_y) /* ///needs to make revisions to the MHI */
{
	num_local_1++;
	Mat motion;
	int erosion_size = 2;
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
	/*
	 * img: original video frame
	 * background: background img, with only pipette present (no worm)
	 * int x, int y: x and y coordinate of the pipette tip
	 */
	Mat grey, gray_scl;
	Mat pipette = background.clone();
	Mat copy = frame.clone();
	/* Mat frame_covered = frame.clone(); */
	grey = background - frame;
	/*
	 * imshow("gray figure", grey);
	 * waitKey();
	 */
	cvtColor(background, pipette, COLOR_BGR2GRAY);
	threshold(pipette, pipette, 185, 255, 1);
	cvtColor(pipette, pipette, COLOR_GRAY2BGR);
	/*
	 * imshow("pipette", pipette);
	 * waitKey();
	 */
	for (int y = 0; y < pipette.rows; y++)
	{
		/*Vec3b* Mi = pipette.ptr<Vec3b>(y);
		 * for (int x = 0; x < pipette.cols; x++)
		 * {
		 * if (Mi[x] == Vec3b(255, 255, 255))
		 * {
		 * grey.at<Vec3b>(y, x) = Vec3b(0, 0, 0);
		 * frame.at<Vec3b>(y, x) = Vec3b(173, 198, 172);
		 * }
		 * }*/
		float *Mi = pip_bw.ptr<float>(y);
		Vec3b *Mi1 = pipette.ptr<Vec3b>(y);
		for (int x = 0; x < pipette.cols; x++)
		{
			if ((Mi[x] == 255) && (Mi1[x] == Vec3b(255, 255, 255)))
			{
				grey.at<Vec3b>(y, x) = Vec3b(0, 0, 0);
				frame.at<Vec3b>(y, x) = Vec3b(173, 198, 172);
			}
		}
	}
	/*
	 * imshow("covered",frame_covered);
	 * grey = frame_covered;
	 * Rect ROI = Rect(440, 240, 13, 30);
	 */
	Rect ROI = Rect(tip_x - 20, tip_y - 40, 20, 45); /* //Rect ROI = Rect(440, 215, 15, 30); */
	/* Rect ROI1 = Rect(tip_x - 3, tip_y - 30, 15, 30); */
	rectangle(frame, ROI, Scalar(0, 0, 255), 2, 8, 0);
	/*
	 * imshow("rectangle", frame);
	 * Rect ROI2 = Rect(435, 230, 30, 35);//original: 435, 230, 30, 35
	 * Rect ROI3 = Rect(440, 228, 15, 37);
	 */
	Mat temp_ROI_2 = Mat(frame, ROI);
	Mat use_ROI2 = temp_ROI_2.clone();
	/*
	 * imshow("temp_ROI_2", temp_ROI_2);
	 * imshow("use_ROI2", use_ROI2);
	 * int totalPixel = 0;
	 * cvtColor(use_ROI2, use_ROI2, COLOR_RGB2GRAY);
	 * for (int x = 0; x <= ROI.width - 1; x++) {
	 * for (int y = 0; y <= ROI.height - 1; y++) {
	 *      totalPixel += use_ROI2.at<uchar>(y, x);
	 * }
	 * }
	 */
	cvtColor(grey, gray_scl, COLOR_BGR2GRAY);
	threshold(gray_scl, gray_scl, 35, 255, 1);
	/* imshow("gray_scl", gray_scl); */
	Mat temp_ROI_1 = Mat(gray_scl, ROI);
	Mat use_ROI = temp_ROI_1.clone();
	cvtColor(use_ROI, use_ROI, COLOR_GRAY2RGB);
	imshow("use_ROI", use_ROI);
	/*
	 * waitKey();
	 * /////////////////////////////////////////////////////////
	 * //////                     MHI                     //////
	 * /////////////////////////////////////////////////////////
	 */
	update_mhi(use_ROI, motion, 35, avg_1);
	/*
	 * update_mhi(use_ROI, 35, avg);
	 * /////////////////////////////////////////////////////////
	 * ///////              draw arrows                   //////
	 * /////////////////////////////////////////////////////////
	 */
	int arrow_center_x = (ROI.x * 2 + ROI.width) / 2;
	int arrow_center_y = (ROI.y * 2 + ROI.height) / 2;
	Point pt;
	pt.x = arrow_center_x;
	pt.y = arrow_center_y;
	if (direction_judge == 1)
	{
		circle(frame, pt, 10, Scalar(255, 0, 0), 2);
		line(frame, pt, Point(pt.x + 10, pt.y), Scalar(255, 0, 0), 2, LINE_4);
	}
	if (direction_judge == 2)
	{
		circle(frame, pt, 10, Scalar(255, 0, 0), 2);
		line(frame, pt, Point(pt.x - 10, pt.y), Scalar(255, 0, 0), 2, LINE_4);
	}
	if (direction_judge == 0)
	{
		avg_1 = 0;
	}
	/*
	 * /////////////////////////////////////////////////////////
	 * /////////////////////////////////////////////////////////
	 * imshow("grey", frame);
	 * waitKey();
	 * imshow("Motion", motion);
	 * waitKey();
	 */
	cout << "Avg value: " << avg_1 << endl;
	return (avg_1);
}