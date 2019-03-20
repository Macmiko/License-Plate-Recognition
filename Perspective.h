#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"
#include <iostream>
#include <string>
#include <cmath>



class Perspective
{
private:
	cv::Point2f inputQuad[4];
	cv::Point2f outputQuad[4];
	cv::Mat lambda;
public:
	enum class InitializationType {
		Manual,
		Auto
	};
	enum class TransformMatrixType {
		FromGUI,
		FromFile
	};
	void TransformImage(cv::Mat &image);
	Perspective(cv::Mat image,std::string perspective, Perspective::InitializationType type, Perspective::TransformMatrixType way);
	~Perspective();
private:
	void setOutputQuad(const cv::Mat & image);
	void setInputQuad(std::vector<cv::Point2f> points);
	void sortQuads();
	void WriteToFile();
	void ReadFromFile();

};

static void mouseCallback(int event, int x, int y, int, void* param);

