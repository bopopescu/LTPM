#pragma once

//#include <opencv/cv.h>

#include <string>

using namespace std;

class PlaceImage {
	public:

		PlaceImage(int argc, char** argv);

		int run();

		string bigImageSegmentationsDirectory;
		string dbImageSegmentsFilename;
		
};
