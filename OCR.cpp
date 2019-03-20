#include "OCR.h"
#include "LPdetector.h"


using namespace std;
using namespace cv;

std::vector<std::string> OCRClass::LPs;

OCRClass::OCRClass()
{
}

OCRClass::~OCRClass()
{

}

void OCRClass::detectString(cv::Mat &imageOfLP)
{
	//OCR on detected LP
	tess.Init(NULL, "eng", tesseract::OEM_DEFAULT);
	tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
	tess.SetImage((uchar*)imageOfLP.data, imageOfLP.cols, imageOfLP.rows, 1, imageOfLP.cols);

	// Get the text
	output = tess.GetUTF8Text();
	
	//delete spaces from char
	int j=0;

	
	for (int i = 0; i < strlen(output)-2; i++)
		if (output[i] != ' ')
		{
			output[j++] = output[i];
		}
	output[j] = '\0';

	while (output[output_size] != '\0') output_size++;
	
	//create vector of strings with detected lp
	if (output_size == 7)
	{
	LPs.push_back(output);
	}
	/*
	cout << LPs.size() << endl;
	cout << output << endl;
	cout << output_size << endl;
	*/
}

void OCRClass::saveLP()
{
	ofstream LP;
	LP.open("Result.txt");
	if (!LP) { cout << "Error" << endl; }
	else {
		LP << finalLP << endl;
	}
	LP.close();
}

void OCRClass::findCorrectLP(std::vector<std::string> v)
{
	std::vector<int> whichString(v.size(), 0);

	for (int i = 1; i < v.size(); i++)
	{
		for (int j = 0; j < i; j++)
		{
			if (v[i].compare(v[j]) == 0)
			{
				whichString[j] += 1;
			}
		}
	}
	int max = 0;
	int index = 0;
	for (int i = 1; i < whichString.size(); i++)
	{
		if (whichString[i]>max)
		{
			max = whichString[i];
			index = i;
		}
	}
	
	
	finalLP = v[index];
	
}

void OCRClass::checkPermission()
{
	string line;
	ifstream myfile("car_plates.txt");
	bool check=false;
	cv::Mat pic = cv::Mat::zeros(200, 500, CV_8UC3);

	if (myfile)  // same as: if (myfile.good())
	{
		while (getline(myfile, line))  // same as: while (getline( myfile, line ).good())
		{
			if (line == finalLP)
			{
				cv::putText(pic, "Your license plate: " +finalLP, cv::Point(20, 60), CV_FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 1, 8, false);
				cv::putText(pic, "ACCESS GRANTED", cv::Point(50, 120), CV_FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(0, 255, 0), 1, 8, false);

				imshow("result", pic);
				waitKey(0);
				check = true;
				break;
			}
			
		}

		if (check == false)
		{
			cv::putText(pic, "Your license plate: " + finalLP, cv::Point(20, 60), CV_FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 1, 8, false);
			cv::putText(pic, "ACCESS DENIED", cv::Point(50, 120), CV_FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(0, 0, 255), 1, 8, false);
			imshow("result", pic);
			waitKey(0);
		}
		myfile.close();
	}
	else cout << "Error with the file" << endl;;
}

