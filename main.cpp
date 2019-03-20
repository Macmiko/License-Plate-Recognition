#include "DIP.h"
#include "background.h"
#include "Videoinput.h"
#include "VideoGUI.h"
#include "opencv2/videoio.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "OCR.h"
#include "Perspective.h"


using namespace cv;
using namespace std;


int main()
{
	//creating a constructors
	OCRClass ocr;
	VideoGUI gui = VideoGUI();

	VideoInput input("../video1.2.3.mp4");

	Mat actualFrame = input.getFrame();

	Perspective perspective(actualFrame, gui.perspective, Perspective::InitializationType::Manual, Perspective::TransformMatrixType::FromGUI);

	BS tr(BS::Background::opencvBS, actualFrame);
	tr.carTrack(actualFrame);

	while(true)
	{
		actualFrame = input.getFrame();
		if (actualFrame.empty())
			break;

		cout << "Fps: " << input.ms() << endl;
		
		if(gui.perspective=="yes")
		perspective.TransformImage(actualFrame);


		double t1 = getTickCount();
		tr.carTrack(actualFrame);
		double t2 = getTickCount();
		double time = (t2 - t1) / getTickFrequency();
		cout << "Tracking takes: " << time << endl;


		gui.show(actualFrame);

	}
	//check which number was found the most of the times
	ocr.findCorrectLP(ocr.LPs);

	cout << ocr.finalLP;


	ocr.saveLP();
	ocr.checkPermission();
	
	return 0;
}
