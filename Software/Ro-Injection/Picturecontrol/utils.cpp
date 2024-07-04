#include "stdafx.h"
#include "utils.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
void MatToCImage(Mat &mat, CImage &cImage)
{
	/* create new CImage */
	int width = mat.cols;
	int height = mat.rows;
	int channels = mat.channels();
	cImage.Destroy(); /* clear */
	cImage.Create(width,
				  height,		 /* positive: left-bottom-up   or negative: left-top-down */
				  8 * channels); /* numbers of bits per pixel */
	/* copy values */
	uchar *ps;
	uchar *pimg = (uchar *)cImage.GetBits(); /* A pointer to the bitmap buffer */
	/*
	 * The pitch is the distance, in bytes. represent the beginning of
	 * one bitmap line and the beginning of the next bitmap line
	 */
	int step = cImage.GetPitch();
	for (int i = 0; i < height; ++i)
	{
		ps = (mat.ptr<uchar>(i));
		for (int j = 0; j < width; ++j)
		{
			if (channels == 1) /* gray */
			{
				*(pimg + i * step + j) = ps[j];
			}
			else if (channels == 3) /* color */
			{
				for (int k = 0; k < 3; ++k)
				{
					*(pimg + i * step + j * 3 + k) = ps[j * 3 + k];
				}
			}
		}
	}
}

float caldistance(float x1, float x2, float y1, float y2)
{
	float dis;
	dis = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
	return (dis);
}

float sina(float x0, float x1, float y0, float y1) /* ///x0 represents the tip x-coordinate, x1 represents the target position x-coordinate */
{
	float sin_theta = 0.458;
	float cos_theta = 0.891;
	float sin_theta_0 = (sin_theta * (x1 - x0) / sqrt((y1 - y0) * (y1 - y0) + (x1 - x0) * (x1 - x0))) - (cos_theta * (y0 - y1) / sqrt((y1 - y0) * (y1 - y0) + (x1 - x0) * (x1 - x0)));
	/* double cos_theta_0 = cos_theta*(x1 - x0) / sqrt((y1 - y0)*(y1 - y0) + (x1 - x0)*(x1 - x0)) + sin_theta*(y1 - y0) / sqrt((y1 - y0)*(y1 - y0) + (x1 - x0)*(x1 - x0)); */
	return (sin_theta_0);
}

float cosa(float x0, float x1, float y0, float y1) /* ///x0 represents the tip x-coordinate, x1 represents the target position x-coordinate */
{
	float sin_theta = 0.458;
	float cos_theta = 0.891;
	float cos_theta_0 = cos_theta * (x1 - x0) / sqrt((y1 - y0) * (y1 - y0) + (x1 - x0) * (x1 - x0)) + sin_theta * (y0 - y1) / sqrt((y1 - y0) * (y1 - y0) + (x1 - x0) * (x1 - x0));
	return (cos_theta_0);
}