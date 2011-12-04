#pragma once

#include "Types.h"

class Segment
{
public:
	Segment();
	Segment(const Segmentation & seg, int label);

	std::vector<Color> pixels;
	BwImage mask;
	IplImage* iplMask;

	CvPoint centroid;
	Color color;
	int label;
};
