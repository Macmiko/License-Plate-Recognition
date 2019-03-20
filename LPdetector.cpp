#include "background.h"
#include "LPdetector.h"
#include "OCR.h"


using namespace std;
using namespace cv;

LPdetectionClass::LPdetectionClass()
{

}

LPdetectionClass::~LPdetectionClass()
{
}


void LPdetectionClass::detectAndDraw(cv::Mat &image, std::vector<cv::Rect> ROIs)
{

	//ROIs to Mats (one Mat = one detected car)
	std::vector<cv::Mat> detectedMats;

	for (int i = 0; i < ROIs.size(); i++)
	{
		cv::Mat t = cv::Mat(image, ROIs[i]);
		detectedMats.push_back(t);

	}


	for (int i = 0; i < detectedMats.size(); i++)
	{

		std::vector<cv::Rect> boundRect;

		cvtColor(detectedMats[i], img_gray, CV_BGR2GRAY);

		cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
		cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
		element = getStructuringElement(cv::MORPH_RECT, cv::Size(30, 30));
		//element = getStructuringElement(cv::MORPH_RECT, cv::Size(17, 3) ); 
		cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element);

		std::vector< std::vector< cv::Point> > LP_contours;
		cv::findContours(img_threshold, LP_contours, 0, 1);
		std::vector<std::vector<cv::Point> > contours_poly(LP_contours.size());
		for (int ii = 0; ii < LP_contours.size(); ii++)
			//delete noises
			if (LP_contours[ii].size() > 100 && contourArea(LP_contours[ii]) > 3000 && contourArea(LP_contours[ii]) < 10000) 
			{
				cv::approxPolyDP(cv::Mat(LP_contours[ii]), contours_poly[ii], 3, true);
				cv::Rect appRect(boundingRect(cv::Mat(contours_poly[ii])));

				//detect only LP and get a little bit bigger box with LP
				if (appRect.width > appRect.height && appRect.width > 160 && appRect.width < 190 && appRect.height>40 && appRect.height < 60)
				{
					//Rect returnRect = Rect(appRect.x - padding, appRect.y - padding, appRect.width + (padding * 2), appRect.height + (padding * 2));
					Rect returnRect = Rect(appRect.x +10, 
						appRect.y -4, 
						appRect.width -8, 
						appRect.height -6);
					
					if (returnRect.x < 0)returnRect.x = 0;
					if (returnRect.y < 0)returnRect.y = 0;
					if (returnRect.x + returnRect.width >= img_threshold.cols)returnRect.width = img_threshold.cols - returnRect.x;
					if (returnRect.y + returnRect.height >= img_threshold.rows)returnRect.height = img_threshold.rows - returnRect.y;
				
					boundRect.push_back(returnRect);
				}
			}
		for (int t = 0; t < boundRect.size(); t++)
		{
			cv::rectangle(detectedMats[i], boundRect[t], cv::Scalar(0, 255, 0), 3, 8, 0);
		}

		detectLP(detectedMats[i], boundRect);

	}
}

void LPdetectionClass::detectLP(cv::Mat mat, std::vector<cv::Rect> bR)
{
	OCRClass ocr;

	std::vector<cv::Mat> detectedLP;
	for (int jj = 0; jj < bR.size(); jj++)
	{
		cv::Mat t = cv::Mat(mat, bR[jj]);
		detectedLP.push_back(t);
	}
	
	/*
	namedWindow("car", WINDOW_NORMAL);
	resizeWindow("car", 600, 400);
	imshow("car", mat);
	waitKey(10);
	*/

	for (int i = 0; i < detectedLP.size(); i++)
	{
		cv::Mat gray;
		cv::cvtColor(detectedLP[i], gray, cv::COLOR_BGR2GRAY);
		cv::imshow("gray", gray);



		/* Histogram equalization improves the contrast between dark/bright areas */

		cv::Mat equalized;
		cv::equalizeHist(gray, equalized);
		cv::imshow("Hist. Eq.", equalized);

		/* Bilateral filter helps to improve the segmentation process */

		cv::Mat blur;
		cv::bilateralFilter(equalized, blur, 9, 75, 75);
		cv::imshow("Filter", blur);

		
		/* Threshold to binarize the image */

		cv::Mat thres;
		threshold(blur, thres, 80, 255, THRESH_BINARY);
		medianBlur(thres, thres, 1);
		threshold(thres, thres, 80, 255, THRESH_BINARY);

		//erode(thres, thres, Mat(), Point(-1, -1), 1);
		//dilate(thres, thres, Mat(), Point(-1, -1), 1);
		cv::imshow("Threshold", thres);
		
		ocr.detectString(thres);
		

		
	/*
		std::vector<std::vector<cv::Point> > contours;
		cv::findContours(thres, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
		//cv::findContours(thres, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		double min_area = 50;
		double max_area = 2000;
		std::vector<std::vector<cv::Point> > good_contours;
		for (size_t i = 0; i < contours.size(); i++)
		{
			double area = cv::contourArea(contours[i]);
			if (area > min_area && area < max_area)
				good_contours.push_back(contours[i]);
		}

		Mat drawing = Mat::zeros(thres.size(), CV_8UC3);

		// Figure out the average of the top/bottom heights of the largest segments
		int min_average_y = 0, max_average_y = 0, count = 0;
		for (size_t i = 0; i < good_contours.size(); i++)
		{
			std::vector<cv::Point> c = good_contours[i];
			double area = cv::contourArea(c);
			if (area > 200)
			{
				int min_y = drawing.rows, max_y = 0;
				for (size_t j = 0; j < c.size(); j++)
				{
					if (c[j].y < min_y)
						min_y = c[j].y;

					if (c[j].y > max_y)
						max_y = c[j].y;
				}
				min_average_y += min_y;
				max_average_y += max_y;
				count++;
			}
		}
		min_average_y /= count;
		max_average_y /= count;
		//std::cout << "Average min: " << min_average_y << " max: " << max_average_y << std::endl;

		// Create a new vector of contours with just the ones that fall within the min/max Y
		std::vector<std::vector<cv::Point> > final_contours;
		for (size_t i = 0; i < good_contours.size(); i++)
		{
			std::vector<cv::Point> c = good_contours[i];
			int min_y = drawing.rows, max_y = 0;
			for (size_t j = 0; j < c.size(); j++)
			{
				if (c[j].y < min_y)
					min_y = c[j].y;

				if (c[j].y > max_y)
					max_y = c[j].y;
			}

			// 5 is to add a little tolerance from the average Y coordinate
			if (min_y >= (min_average_y - 5) && (max_y <= max_average_y + 5))
				final_contours.push_back(c);
		}

		for (int jjj = 0; jjj < final_contours.size(); jjj++)
		{
			drawContours(drawing, final_contours, jjj, Scalar(255, 250, 0), 2, 8, 0, 0, Point());
		}

		cv::imshow("drawing", drawing);
		*/
	}


}