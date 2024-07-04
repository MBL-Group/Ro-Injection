#include <pylon/PylonIncludes.h>
#include <pylon/AviCompressionOptions.h>
#include <pylon/PylonGUI.h>
#include <pylon/1394/Basler1394InstantCamera.h>
#include "opencv2\highgui\highgui.hpp"
#include "opencv.hpp"
#include <opencv2/opencv.hpp>
#pragma once
typedef Pylon::CBasler1394InstantCamera Camera_t;
using namespace Basler_IIDC1394CameraParams;
/* Namespace for using pylon objects. */
using namespace Pylon;
using namespace GenApi;
using namespace std;
using namespace cv;
class Camera
{
public:
	Camera();
	~Camera();
	void initialize();
	void run();
	static DWORD WINAPI StaticThreadFunc(LPVOID p);
	Mat frame;
};
DWORD WINAPI camera_run(LPVOID lpParameter);