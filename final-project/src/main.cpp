/*
 * DisplayImage.cpp
 *
 *  Created on: Aug 3, 2017
 *      Author: aaron
 */

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "EllipseHandDetection.h"
#include "HSVHandDetection.h"


typedef enum
{
	DETECTION_METHOD_HSV,
	DETECTION_METHOD_ELLIPSE
}	DetectionMethod_t;


static bool TrackbarValueChanged = false;

static void TrackbarValueChangeCallback(int Value, void *pParam)
{
	TrackbarValueChanged = true;
}


int main(int argc, char** argv)
{
	bool UseLiveVideo = false;
	bool ReadFromFile = false;
	bool Run = true;
	std::string InputFileName;
	DetectionMethod_t DetectionMethod = DETECTION_METHOD_HSV;
//	DetectionMethod_t DetectionMethod = DETECTION_METHOD_ELLIPSE;

	const char InputImageWindow[] = "Input Image";
	const char HSVDetectionWindow[] = "HSV Detect Results";
	const char EllipseDetectionWindow[] = "Ellipse Detect Results";
	const char *middleWindow1 = NULL;
	const char middleWindow2[] = "Canny Results";
	const char OutputImageWindow[] = "Hand Detection Results";
	cv::Mat input;
	cv::Mat mid1;
	cv::Mat mid2;
	cv::Mat mid2gray;
	cv::Mat output;
	cv::VideoCapture video;

	if(2 == argc)
	{
		ReadFromFile = true;
	}
	else if(1 == argc)
	{
		UseLiveVideo = true;
	}
	else
	{
		printf("Invalid input arguments\n");
		return -1;
	}

	if(ReadFromFile)
	{
		InputFileName = argv[1];
	}
	else if(UseLiveVideo)
	{
		video.open(0);

		if(!video.isOpened())
		{
			printf("ERROR: can not open camera\n");
			return -1;
		}
		else
		{
			// Show video information
			int VideoWidth = (int)video.get(CV_CAP_PROP_FRAME_WIDTH);
			int VideoHeight = (int)video.get(CV_CAP_PROP_FRAME_HEIGHT);

			printf("Input video: (%d x %d)\n", VideoWidth, VideoHeight);
		}
	}
	else
	{
		printf("Should not have gotten here!\n");
		return -1;
	}

	// Create window for input image
	cv::namedWindow(InputImageWindow, CV_WINDOW_AUTOSIZE);
	//Create windows for intermediary steps
	if(DETECTION_METHOD_ELLIPSE == DetectionMethod)
	{
		middleWindow1 = EllipseDetectionWindow;
	}
	else
	{
		middleWindow1 = HSVDetectionWindow;
	}

	cv::namedWindow(middleWindow1, CV_WINDOW_AUTOSIZE);
	cv::namedWindow(middleWindow2, CV_WINDOW_AUTOSIZE);
	// Create window for output image
	cv::namedWindow(OutputImageWindow, CV_WINDOW_AUTOSIZE);

	if(DETECTION_METHOD_ELLIPSE == DetectionMethod)
	{
		EllipseHandDetectionCreateTrackbarWindow(TrackbarValueChangeCallback);
	}
	else
	{
		HSVHandDetectionCreateTrackbarWindow(TrackbarValueChangeCallback);
	}

	while(Run)
	{
		if(ReadFromFile)
		{
			input = cv::imread(InputFileName);

			if(!input.data)
			{
				printf("Could not load specified image data\n");
				return -1;
			}

			if(CV_8UC3 != input.type())
			{
				printf("Input image is not RGB type\n");
				return -1;
			}
		}
		else if(UseLiveVideo)
		{
			// Get current image
			video >> input;

			if(input.empty())
			{
				printf("Unable to capture video\n");
				video.release();
				return -1;
			}

			if(CV_8UC3 != input.type())
			{
				printf("Input video is not RGB type\n");
				video.release();
				return -1;
			}
		}

		// create output image using input as a starting point
		// (cannot use cv::Mat = operator - must use cv::Mat::copyTo() function to do deep copy
		input.copyTo(mid1);

		// run hand detection on mid1 image
		if(DETECTION_METHOD_ELLIPSE == DetectionMethod)
		{
			EllipseHandDetection(mid1);
		}
		else
		{
			HSVHandDetection(mid1);
		}

		// run Canny on mid2 image
		  //first convert to grayscale
		cv::cvtColor( input, mid2gray, cv::COLOR_BGR2GRAY );
		  //blur image
		cv::blur( mid2gray, mid2, cv::Size(3,3) );
		  //run canny
		cv::Canny( mid2, mid2, 10, 80, 3 );

		// and thicken the line using a box filter
		cv::Mat mid2_2;
		cv::boxFilter(mid2, mid2_2, -1, cv::Size(3,3), cv::Point(-1, -1), false, cv::BORDER_DEFAULT);

		//AND mid1 and mid2 into output image
		cv::bitwise_and(mid1, mid2_2, output);

		// display input image
		cv::imshow(InputImageWindow, input);

		// display intermediary steps windows
		cv::imshow(middleWindow1, mid1);
		cv::imshow(middleWindow2, mid2_2);

		// display output image
		cv::imshow(OutputImageWindow, output);

		do
		{
			char KeyPress = cv::waitKey(10);

			switch(KeyPress)
			{
				case 'C':
				case 'c':
					// save output image
					cv::imwrite("input.jpg", input);
					cv::imwrite("mid1.jpg", mid1);
					cv::imwrite("mid2.jpg", mid2_2);
					cv::imwrite("output.jpg", output);
					break;

				case 27:
					Run = false;
					break;

				default:
					break;
			}
		}
		while(ReadFromFile && Run && (!TrackbarValueChanged));
	}

	if(UseLiveVideo)
	{
		video.release();
	}

	return 0;
}


