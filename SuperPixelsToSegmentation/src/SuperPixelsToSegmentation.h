#pragma once

#include <opencv/cv.h>

#include <string>

using namespace std;

class SuperPixelsToSegmentation {
	public:

		SuperPixelsToSegmentation(int argc, char** argv);

		int run();

		string imageFilename;
		string segFilename;
		
		IplImage* image;
		IplImage* segImage;
};
