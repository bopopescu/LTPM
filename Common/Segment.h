#pragma once

#include <Types.h>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>

using namespace boost::geometry;

typedef model::d2::point_xy<float> Point;
typedef model::polygon<Point> Polygon;

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
	Polygon polygon;
};

float compareSegmentations(vector<Segment> a, vector<Segment> b);
