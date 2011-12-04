#pragma once

#include "Types.h"

class Segment
{
public:
	static Segment* combine(Segment *s1, Segment *s2);

	Segment();
	Segment(Segmentation & seg, int label);

	void updateContour();

	std::vector<Color> pixels;
	BwImage mask;
	IplImage* iplMask;

	Segmentation* seg;

	CvPoint centroid;
	Color color;
	int label;


	CvMemStorage *storage;
	CvSeq *contour;
};
