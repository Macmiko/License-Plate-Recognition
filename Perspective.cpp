#include <opencv2/core/utility.hpp>
#include <opencv/highgui.h>
#include <cstdlib>
#include <iostream>
#include "Perspective.h"

using namespace cv;
using namespace std;

void Perspective::TransformImage(cv::Mat & image)
{
	warpPerspective(image, image, lambda, image.size());
}

Perspective::Perspective(cv::Mat image,string perpective, Perspective::InitializationType type, Perspective::TransformMatrixType way)
{
	if (perpective == "yes")
	{
		if (way == Perspective::TransformMatrixType::FromGUI)
		{


			if (type == Perspective::InitializationType::Manual)
			{
				Mat image_for_text;
				setOutputQuad(image);
				vector<Point2f> input;
				image.copyTo(image_for_text);
				namedWindow("Source window", WINDOW_NORMAL);
				resizeWindow("Source window", 600, 400);
				setMouseCallback("Source window", mouseCallback, &input);
				while (1)
				{
					imshow("Source window", image_for_text);
					image.copyTo(image_for_text);
					switch (input.size())
					{
					case 0:
						putText(image_for_text, "Choose 4 points", Point2f(50, 50),
							FONT_HERSHEY_COMPLEX_SMALL, 3, cvScalar(0, 0, 255), 2, CV_AA);
						break;

					case 1:
						putText(image_for_text, "Choose 3 points", Point2f(50, 50),
							FONT_HERSHEY_COMPLEX_SMALL, 3, cvScalar(0, 0, 255), 2, CV_AA);
						break;

					case 2:
						putText(image_for_text, "Choose 2 points", Point2f(50, 50),
							FONT_HERSHEY_COMPLEX_SMALL, 3, cvScalar(0, 0, 255), 2, CV_AA);
						line(image_for_text,input[0],input[1],Scalar(0, 0, 0), 8, 8);
						break;

					case 3:
						putText(image_for_text, "Choose 1 point", Point2f(50, 50),
							FONT_HERSHEY_COMPLEX_SMALL, 3, cvScalar(0, 0, 255), 2, CV_AA);
						line(image_for_text, input[0], input[1], Scalar(0, 0, 0), 8, 8);
						line(image_for_text, input[1], input[2], Scalar(0, 0, 0), 8, 8);

						break;
					default:
						putText(image_for_text, "Please click space", Point2f(50, 50),
							FONT_HERSHEY_COMPLEX_SMALL, 3, cvScalar(0, 0, 255), 2, CV_AA);
						line(image_for_text, input[0], input[1], Scalar(0, 0, 0), 8, 8);
						line(image_for_text, input[1], input[2], Scalar(0, 0, 0), 8, 8);
						line(image_for_text, input[2], input[3], Scalar(0, 0, 0), 8, 8);
						line(image_for_text, input[3], input[0], Scalar(0, 0, 0), 8, 8);

						break;
					}
					if (input.size() > 0) //we have at least 1 point
					{
						for (auto it = input.begin(); it != input.end(); ++it)
						{
							circle(image, Point2f((*it).x, (*it).y), 10, Scalar(0, 0, 0), -2);

						}

					}

					if (waitKey(20) == 32)
					{
						destroyWindow("Source window");
						break;
					}

				}
				setInputQuad(input);

			}
			else if (type == Perspective::InitializationType::Auto)
			{
				setOutputQuad(image);
			}

			sortQuads();
			lambda = Mat::zeros(2, 4, CV_32FC1);
			lambda = getPerspectiveTransform(inputQuad, outputQuad);
			WriteToFile();
		}
		else if (way == Perspective::TransformMatrixType::FromFile)
		{
			ReadFromFile();
		}
	}
}

void mouseCallback(int event, int x, int y, int, void * param)
{
	vector<Point2f> *p = static_cast<vector<Point2f> *>(param);
	if (p->size() == 4)
	{
		setMouseCallback("Source Window", NULL, NULL);
	}
	else if (event == EVENT_LBUTTONDOWN)
	{
		vector<Point2f> *p = static_cast<vector<Point2f> *>(param);
		p->push_back(Point2f(x, y));
	}
}
/*points in the corners of the image*/
void Perspective::setOutputQuad(const cv::Mat & image)
{
	outputQuad[0] = Point2f(0, 0);
	outputQuad[1] = Point2f(0, float(image.rows - 1));
	outputQuad[2] = Point2f(float(image.cols - 1), 0);
	outputQuad[3] = Point2f(float(image.cols - 1), float(image.rows - 1));
}

void Perspective::setInputQuad(std::vector<cv::Point2f> points)
{
	if (points.size() < 4)
	{
		cout << "Not enough points" << endl;
		return;
	}
	for (int i = 0; i < 4; i++)
	{
		inputQuad[i] = points.at(i);
		
	}
}
/*sort input points*/
void Perspective::sortQuads()
{
	Point2f sortedQuad[4];
	float distance;
	float min = 0;

	for (int i = 0; i < 4; i++)
	{
		min= sqrt(pow((outputQuad[0].x - inputQuad[i].x), 2) + pow((outputQuad[0].y - inputQuad[i].y), 2));
		int best = 0;
		for (int j = 0; j < 4; j++)
		{
			distance = sqrt(pow((outputQuad[j].x - inputQuad[i].x), 2) + pow((outputQuad[j].y - inputQuad[i].y), 2));
			
			if (distance < min)
			{
				min = distance;
				best = j;
			}
		}
		sortedQuad[best] = inputQuad[i];
	}
	for (int i = 0; i < 4; i++)
	{
		inputQuad[i] = sortedQuad[i];
	}
}

void Perspective::WriteToFile()

{
	FileStorage fs("lambda.xml", FileStorage::WRITE);
	if (fs.isOpened())
	{
		fs << "lambda" << lambda;
		fs.release();
	}
	else
		cout << "Error: file cannot be opened" << endl;
	
}
void Perspective::ReadFromFile()
{
	FileStorage fs2("lambda.xml", FileStorage::READ);
	if (fs2.isOpened())
	{
		fs2["lambda"] >> lambda;

		fs2.release();
	}
	else
		cout << "Error: file cannot be opened" << endl;

}
Perspective::~Perspective()
{

}
