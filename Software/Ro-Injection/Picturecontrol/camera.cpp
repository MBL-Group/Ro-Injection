#include "stdafx.h"
#include "camera.h"
Camera::Camera()
{
}

void Camera::initialize()
{
	PylonAutoInitTerm autoInitTerm;
	CImage myImage1;
	int exitCode = 0;
	int num = 0;
	/* //>>>>>>>>>>> this section is used to compresss the image which will be written in files<<<<<<<<<<<<<<<<<<<<//// */
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); /* choose jpeg type */
	compression_params.push_back(95);					   /* number indicates the quality of figure you want: 100 means the best quality. default value of jpeg compressed is 95 if this section is not employed */
	/* / *************Initialize the erosion element********* /// */
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
	/* / *************Initialize the dilation element********* /// */
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
	Mat dilatation_element1 = getStructuringElement(erosion_type, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
	Mat dilatation_element = getStructuringElement(erosion_type, Size(2, 2), Point(erosion_size, erosion_size));
}

DWORD WINAPI Camera::StaticThreadFunc(LPVOID p)
{
	Camera *instance = static_cast<Camera *>(p);
	instance->run();
	return (0);
}

void Camera::run()
{
	/* TODO: Add your control notification handler code here */
	PylonAutoInitTerm autoInitTerm;
	CImage myImage1;
	int exitCode = 0;
	int num = 0;
	/* //>>>>>>>>>>> this section is used to compresss the image which will be written in files<<<<<<<<<<<<<<<<<<<<//// */
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); /* choose jpeg type */
	compression_params.push_back(95);					   /* number indicates the quality of figure you want: 100 means the best quality. default value of jpeg compressed is 95 if this section is not employed */
	/* / *************Initialize the erosion element********* /// */
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
	/* / *************Initialize the dilation element********* /// */
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
	Mat dilatation_element1 = getStructuringElement(erosion_type, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
	Mat dilatation_element = getStructuringElement(erosion_type, Size(2, 2), Point(erosion_size, erosion_size));
	/* ///>>>>>>>>>> grabbing picture from camera<<<<<<<<<// */
	try
	{
		CvVideoWriter *video = NULL; /* Create an video object to store the images. */
		IplImage *frame_1;			 /* this type of variable is necessary in the funtion "cvWriteFrame()", use it to store the vonverted result from Mat variable */
		/* Only look for cameras supported by Camera_t */
		CDeviceInfo info;
		info.SetDeviceClass(Camera_t::DeviceClass());
		/* Create an instant camera object with the first found camera device that matches the specified device class. */
		Camera_t camera(CTlFactory::GetInstance().CreateFirstDevice(info));
		/* Print the model name of the camera. */
		cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;

		/*
		 * ==========================================Parameter Configuration Start===================================
		 * Open the camera for accessing the parameters.
		 */
		camera.Open();
		camera.OffsetX.SetValue(camera.OffsetX.GetMin());
		camera.OffsetY.SetValue(camera.OffsetY.GetMin());
		camera.Width.SetValue(658);
		camera.Height.SetValue(490);
		camera.PixelFormat.SetValue(PixelFormat_YUV422Packed);
		camera.GainRaw.SetValue(510);
		camera.ExposureTimeAbs.SetValue(462);
		camera.BlackLevelRaw.SetValue(100);
		camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Red);
		camera.BalanceRatioAbs.SetValue(1.09375);
		camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Green);
		camera.BalanceRatioAbs.SetValue(1.421875);
		camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Blue);
		camera.BalanceRatioAbs.SetValue(3.984375);
		/* **************setting parameters for video recording **************** // */
		CIntegerPtr width(camera.GetNodeMap().GetNode("Width"));
		CIntegerPtr height(camera.GetNodeMap().GetNode("Height"));
		CEnumerationPtr pixelFormat(camera.GetNodeMap().GetNode("PixelFormat"));
		/* **************setting parameters for video recording **************** // */
		camera.MaxNumBuffer = 10;
		camera.StartGrabbing();
		CGrabResultPtr ptrGrabResult;
		CPylonImage target;
		CImageFormatConverter converter;
		converter.OutputPixelFormat = PixelType_BGR8packed;
		converter.OutputBitAlignment = OutputBitAlignment_MsbAligned;
		/* Create a window to display the images */
		namedWindow("Camera Output", WINDOW_NORMAL);
		while (camera.IsGrabbing())
		{
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				/* Translate the message and dispatch it to WindowProc() */
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			/* If the message is WM_QUIT, exit the while loop */
			if (msg.message == WM_QUIT)
				break;
			camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
			if (ptrGrabResult->GrabSucceeded())
			{
				/* /// ********convertion of the picture format to bgr******** // */
				converter.Convert(target, ptrGrabResult);
				/* ///>>>>>>>>>convertion of the picture format to bgr<<<<<<// */
				Mat new_frame(target.GetHeight(), target.GetWidth(), CV_8UC3, target.GetBuffer(), Mat::AUTO_STEP);
				new_frame.copyTo(frame);
				/* Display the frame */
				imshow("Camera Output", new_frame);
				/* imwrite("C:/Users/panpe/Desktop/test_camera_thread.jpg", new_frame);///////under 10x objective background image includes only the pipette tip for MHI detection */
				/* Wait for 30 milliseconds to allow the window to be updated */
				if (waitKey(30) >= 0)
					break;
			}
		}
	}
	catch (GenICam::GenericException &e)
	{
		/* Error handling. */
		cerr << "An exception occurred." << endl
			 << e.GetDescription() << endl;
		exitCode = 1;
	}
	/* Comment the following two lines to disable waiting on exit. */
	cerr << endl
		 << "Press Enter to exit." << endl;
	while (cin.get() != '\n')
		;

	/*
	 * PylonTerminate();
	 * ReleaseMutex(hMutex);
	 * return exitCode;
	 */
}

DWORD WINAPI camera_run(LPVOID lpParameter)
{
	/* TODO: Add your control notification handler code here */
	PylonAutoInitTerm autoInitTerm;
	CImage myImage1;
	int exitCode = 0;
	int num = 0;
	/* //>>>>>>>>>>> this section is used to compresss the image which will be written in files<<<<<<<<<<<<<<<<<<<<//// */
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); /* choose jpeg type */
	compression_params.push_back(95);					   /* number indicates the quality of figure you want: 100 means the best quality. default value of jpeg compressed is 95 if this section is not employed */
	/* / *************Initialize the erosion element********* /// */
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
	/* / *************Initialize the dilation element********* /// */
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
	Mat dilatation_element1 = getStructuringElement(erosion_type, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
	Mat dilatation_element = getStructuringElement(erosion_type, Size(2, 2), Point(erosion_size, erosion_size));
	/* ///>>>>>>>>>> grabbing picture from camera<<<<<<<<<// */
	try
	{
		CvVideoWriter *video = NULL; /* Create an video object to store the images. */
		IplImage *frame_1;			 /* this type of variable is necessary in the funtion "cvWriteFrame()", use it to store the vonverted result from Mat variable */
		/* Only look for cameras supported by Camera_t */
		CDeviceInfo info;
		info.SetDeviceClass(Camera_t::DeviceClass());
		/* Create an instant camera object with the first found camera device that matches the specified device class. */
		Camera_t camera(CTlFactory::GetInstance().CreateFirstDevice(info));
		/* Print the model name of the camera. */
		cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;

		/*
		 * ==========================================Parameter Configuration Start===================================
		 * Open the camera for accessing the parameters.
		 */
		camera.Open();
		camera.OffsetX.SetValue(camera.OffsetX.GetMin());
		camera.OffsetY.SetValue(camera.OffsetY.GetMin());
		camera.Width.SetValue(658);
		camera.Height.SetValue(490);
		camera.PixelFormat.SetValue(PixelFormat_YUV422Packed);
		camera.GainRaw.SetValue(510);
		camera.ExposureTimeAbs.SetValue(462);
		camera.BlackLevelRaw.SetValue(100);
		camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Red);
		camera.BalanceRatioAbs.SetValue(1.09375);
		camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Green);
		camera.BalanceRatioAbs.SetValue(1.421875);
		camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Blue);
		camera.BalanceRatioAbs.SetValue(3.984375);
		/* **************setting parameters for video recording **************** // */
		CIntegerPtr width(camera.GetNodeMap().GetNode("Width"));
		CIntegerPtr height(camera.GetNodeMap().GetNode("Height"));
		CEnumerationPtr pixelFormat(camera.GetNodeMap().GetNode("PixelFormat"));
		/* **************setting parameters for video recording **************** // */
		camera.MaxNumBuffer = 10;
		camera.StartGrabbing();
		CGrabResultPtr ptrGrabResult;
		CPylonImage target;
		CImageFormatConverter converter;
		converter.OutputPixelFormat = PixelType_BGR8packed;
		converter.OutputBitAlignment = OutputBitAlignment_MsbAligned;
		/* Create a window to display the images */
		namedWindow("Camera Output", WINDOW_NORMAL);
		while (camera.IsGrabbing())
		{
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				/* Translate the message and dispatch it to WindowProc() */
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			/* If the message is WM_QUIT, exit the while loop */
			if (msg.message == WM_QUIT)
				break;
			camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
			if (ptrGrabResult->GrabSucceeded())
			{
				/* /// ********convertion of the picture format to bgr******** // */
				converter.Convert(target, ptrGrabResult);
				/* ///>>>>>>>>>convertion of the picture format to bgr<<<<<<// */
				Mat frame(target.GetHeight(), target.GetWidth(), CV_8UC3, target.GetBuffer(), Mat::AUTO_STEP);
				/* Display the frame */
				imshow("Camera Output", frame);
				/* Wait for 30 milliseconds to allow the window to be updated */
				if (waitKey(30) >= 0)
					break;
			}
		}
	}
	catch (GenICam::GenericException &e)
	{
		/* Error handling. */
		cerr << "An exception occurred." << endl
			 << e.GetDescription() << endl;
		exitCode = 1;
	}
	/* Comment the following two lines to disable waiting on exit. */
	cerr << endl
		 << "Press Enter to exit." << endl;
	while (cin.get() != '\n')
		;

	/*
	 * PylonTerminate();
	 * ReleaseMutex(hMutex);
	 */
	return (exitCode);
}

Camera::~Camera()
{
}