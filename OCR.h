#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <fstream>  
#include <iostream>  
#include <string> 
#include <stdio.h>
#include <string.h>

class OCRClass
{

private:
	tesseract::TessBaseAPI tess;
	char* output;
	int output_size = 0;

public:
	OCRClass();
	~OCRClass();

	void detectString(cv::Mat &imageOfLP);
	void saveLP();
	void findCorrectLP(std::vector<std::string> v);
	void checkPermission();
	
	static std::vector<std::string> LPs;
	
	std::string finalLP;


};


