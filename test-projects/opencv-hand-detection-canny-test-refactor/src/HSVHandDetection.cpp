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

#include "HSVHandDetection.h"

// In GIMP color chooser, HSV ranges are as follows:
// Hue: 0 to 360
// Saturation: 0 to 100
// Value: 0 to 100
// In OpenCV, HSV ranges are as follows:
// Hue: 0 to 180
// Saturation: 0 to 255
// Value: 0 to 255

// From number01.jpg in GIMP:
// Hue range from 11 to 24
// Saturation range from 38 to 68
// Value range from 47 to 94
// This translates to OpenCV ranges of:
// Hue: 5 to 12 (use 5 to 15)
// Saturation: 97 to 240 (use 90 to 240)
// Value: 120 to 240 (use 120 to 240)

//#define _USE_ORIGINAL_HSV_LIMITS

#ifdef _USE_ORIGINAL_HSV_LIMITS
static const cv::Scalar HSVMin = cv::Scalar(0, 30, 80);
static const cv::Scalar HSVMax = cv::Scalar(20, 150, 255);
#else
//static const cv::Scalar HSVMin = cv::Scalar(10, 30, 50);
//static const cv::Scalar HSVMax = cv::Scalar(25, 200, 255);
static const cv::Scalar HSVMin = cv::Scalar(0, 20, 20);
static const cv::Scalar HSVMax = cv::Scalar(40, 255, 255);
#endif	//_USE_ORIGINAL_HSV_LIMITS


void HSVHandDetection(cv::Mat &Image)
{
	cv::cvtColor(Image, Image, CV_BGR2HSV);
	cv::inRange(Image, HSVMin, HSVMax, Image);
}


