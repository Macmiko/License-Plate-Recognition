#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/video/background_segm.hpp"

class BS
{
public:

	enum class Background {
		opencvBS,
		firstFrameBS
	};
private:
	Background backgroundType;
	cv::Mat bg;
	cv::Mat mask;

	const int scale = 4;

	//int padding = 2;

	cv::Ptr<cv::BackgroundSubtractor> bgModel;

public:
	BS(BS::Background btype, const cv::Mat &image);
	~BS();
	void carTrack(cv::Mat &image);
	cv::Mat returnMask();
	std::vector<cv::Rect> detectedObjects();


};

