#include "background.h"
#include "DIP.h"
#include "LPdetector.h"



using namespace std;
using namespace cv;

BS::BS(BS::Background btype, const cv::Mat &image)
{
	
	backgroundType = btype;

	//First frame is our background
	if (backgroundType == Background::firstFrameBS)
	{
		cvtColor(image, bg, COLOR_BGR2GRAY);
		Mat resized(cvRound(bg.cols / scale), cvRound(bg.rows / scale), CV_8UC1);
		resize(bg, bg, resized.size());

		bgModel.release();
	}
	//using background subtractor
	else if (backgroundType == Background::opencvBS)
	{
		bgModel = createBackgroundSubtractorMOG2().dynamicCast<BackgroundSubtractor>();
		mask.create(Size(cvRound(image.cols / scale), cvRound(image.rows / scale)), image.type());
	}
}

void BS::carTrack(cv::Mat & image)
{
	LPdetectionClass LPd;


	if (backgroundType == Background::firstFrameBS)
	{
		Mat gray_image;
		cvtColor(image, gray_image, COLOR_BGR2GRAY);
		resize(gray_image, gray_image, bg.size());

		mask = DIP::maskFirstFrame(gray_image, bg);

		resize(mask, mask, image.size());
	}
	else if (backgroundType == Background::opencvBS)
	{
		Rect ROI(50, 150, (image.cols - 100), (image.rows - 150));
		Mat frameMask(image.size(), CV_8UC1, Scalar::all(0));
		frameMask(ROI).setTo(Scalar::all(255));
		Mat result;
		image.copyTo(result, frameMask);
		resize(result, result, Size(image.cols / 4, image.rows / 4));

		bgModel->apply(result, mask, true ? -1 : 0);

		DIP::maskOpencvBS(mask);

		bgModel->getBackgroundImage(bg);

		resize(mask, mask, image.size());
	}

	namedWindow("mask", WINDOW_NORMAL);
	resizeWindow("mask", 600, 400);
	imshow("mask", mask);
	waitKey(10);

	//find and mark licence plate 
	LPd.detectAndDraw(image, detectedObjects());
}

std::vector<cv::Rect>BS::detectedObjects()
{
	
		std::vector<std::vector<cv::Point> > contours;
		std::vector<cv::Vec4i> hierarchy;

		// find contours on the mask and then create a vector which holds the separated contours

		cv::findContours(mask, contours, hierarchy, 0, 2, cv::Point(0, 0));
		std::vector<std::vector<cv::Point> > contoursPoly(contours.size());
		std::vector<cv::Rect> objectRects;
		


		for (int i = 0; i < contours.size(); i++)
		{
			//detect only cars (area bigger then 120000)
			if (contourArea(contours[i]) > 120000)
			{
				//std::cout << " Area: " << contourArea(contours[i]) << std::endl;

				cv::approxPolyDP(cv::Mat(contours[i]), contoursPoly[i], 3, true);
				cv::Rect t = cv::boundingRect(cv::Mat(contoursPoly[i]));
				
				// *Changing box's with car size*
				/*
				Rect returnRect = Rect(t.x - padding, t.y - padding, t.width + (padding * 2), t.height + (padding * 2));
				if (returnRect.x < 0)returnRect.x = 0;
				if (returnRect.y < 0)returnRect.y = 0;
				if (returnRect.x + returnRect.width >= mask.cols)returnRect.width = mask.cols - returnRect.x;
				if (returnRect.y + returnRect.height >= mask.rows)returnRect.height = mask.rows - returnRect.y;
				*/
				
				objectRects.push_back(t);
			}
		}
		



		return  objectRects;

}


cv::Mat BS::returnMask()
{
	return mask;
}

BS::~BS()
{
	destroyAllWindows();
}
