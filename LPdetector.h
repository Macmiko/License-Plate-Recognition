#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/video/background_segm.hpp"

class LPdetectionClass
{

private:
	
cv::Mat img_gray, img_sobel, img_threshold, element;
int padding = 7;
public:
	LPdetectionClass();
	~LPdetectionClass();
	void detectAndDraw(cv::Mat &image, std::vector<cv::Rect> ROIs);
	void detectLP(cv::Mat mat, std::vector<cv::Rect> bR);

};

