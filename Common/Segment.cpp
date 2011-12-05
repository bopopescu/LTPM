#include "Segment.h"

#include <boost/foreach.hpp>
#include <boost/geometry/algorithms/num_points.hpp>

Segment* Segment::combine(Segment *s1, Segment *s2)
{
	Segment* newSegment = new Segment(*s1->seg, s1->label);

	IplImage* maskAddition = cvCreateImage(cvSize(s1->seg->width, s1->seg->height), IPL_DEPTH_8U, 1);
	cvAdd(s1->iplMask, s2->iplMask, maskAddition, NULL);
	cvReleaseImage(&s1->iplMask);
	newSegment->iplMask = maskAddition;
	newSegment->mask = BwImage(newSegment->iplMask);
	
	// weighted average colors
	int s1PixelCount = s1->pixels.size();
	int s2PixelCount = s2->pixels.size();
	int totalPixelCount = s1PixelCount + s2PixelCount;
	float s1Weight = float(s1PixelCount) / float(totalPixelCount);
	float s2Weight = float(s2PixelCount) / float(totalPixelCount);
	newSegment->color.r = s1->color.r * s1Weight + s2->color.r * s2Weight;
	newSegment->color.g = s1->color.g * s1Weight + s2->color.g * s2Weight;
	newSegment->color.b = s1->color.b * s1Weight + s2->color.b * s2Weight;

	newSegment->pixels.insert(newSegment->pixels.end(), s1->pixels.begin(), s1->pixels.end());
	newSegment->pixels.insert(newSegment->pixels.end(), s2->pixels.begin(), s2->pixels.end());

	newSegment->updateContour();

	return newSegment;
}

Segment::Segment()
{

}

Segment::Segment(Segmentation & _seg, int _label)
{
	storage = cvCreateMemStorage(0);
	label = _label;
	iplMask = cvCreateImage(cvSize(_seg.width, _seg.height), IPL_DEPTH_8U, 1);
	cvSet(iplMask, cvScalar(0));
	mask = BwImage(iplMask);
	seg = &_seg;
}



void Segment::updateContour()
{
	contour = 0;

	cvFindContours(iplMask, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	CvMoments moments;
	cvMoments(contour, &moments);
	double m00, m10, m01;

	m00 = cvGetSpatialMoment(&moments, 0,0);
	m10 = cvGetSpatialMoment(&moments, 1,0);
	m01 = cvGetSpatialMoment(&moments, 0,1);

	// TBD check that m00 != 0
	float center_x = m10/m00;
	float center_y = m01/m00;
	centroid = cvPoint(center_x, center_y);
}


float Segment::areaOfIntersectionWith(Segment & other)
{

	cerr << "mePoly    size: " << num_points(polygon) << endl << "mePoly    area: " << boost::geometry::area(polygon) << endl;
	cerr << "otherPoly size: " << num_points(other.polygon) << endl << "otherPoly area: " << boost::geometry::area(other.polygon) << endl;
	cerr << endl;

	std::deque<Polygon> output;

		

	boost::geometry::intersection(other.polygon, other.polygon, output);

	float intersectionArea = 0.0;

	BOOST_FOREACH(Polygon const& p, output)
	{
		intersectionArea += boost::geometry::area(p);
	}

	return intersectionArea;
}

class Match
{
public:
	Match() { };
	Match(Segment * _segment, float _score) { segment = _segment; score = _score; };
	float score;
	Segment* segment;
};

bool contains(map<Segment*, Match> _map, Segment* key)
{
	map<Segment*, Match>::iterator location = _map.find(key);
	return location != _map.end();
}

float scoreCandidate(vector<Segment> targetSegmentation, vector<Segment> candidateSegmentation)
{

	// TODO add color matching in here.

	map<Segment*, Match> bestMatchByTarget;
	float totalTargetArea = 0.0;
	for(int targetSegmentIndex = 0; targetSegmentIndex < targetSegmentation.size(); targetSegmentIndex++)
	{
		Segment* targetSegment = & targetSegmentation[targetSegmentIndex];
		totalTargetArea += area(targetSegment->polygon);
	}

	// assign each candidate segment to the target segment it most intersects with
	// pair each target segment with the best candidate segment match (most intersects)
	for(int candidateSegmentIndex = 0; candidateSegmentIndex < candidateSegmentation.size(); candidateSegmentIndex++)
	{
		Segment* candidateSegment = & candidateSegmentation[candidateSegmentIndex];
		Segment* bestMatchTargetSegment = NULL;
		float bestTargetScore = -1;
		
		for(int targetSegmentIndex = 0; targetSegmentIndex < targetSegmentation.size(); targetSegmentIndex++)
		{
			Segment* targetSegment = & targetSegmentation[targetSegmentIndex];
			float pairOverlap = candidateSegment->areaOfIntersectionWith(*targetSegment);
			if(pairOverlap > bestTargetScore)
			{
				bestTargetScore = pairOverlap;
				bestMatchTargetSegment = targetSegment;
			}
		}
		if(bestMatchTargetSegment != NULL)
		{
			if(!contains(bestMatchByTarget, bestMatchTargetSegment))
				bestMatchByTarget[bestMatchTargetSegment] = Match(candidateSegment, bestTargetScore);
			else if(bestTargetScore > bestMatchByTarget[bestMatchTargetSegment].score)
				bestMatchByTarget[bestMatchTargetSegment] = Match(candidateSegment, bestTargetScore);
		}
	}

	// sum intersection areas of all pairs
	float totalMatchArea = 0.0;
	for(map<Segment*, Match>::iterator match = bestMatchByTarget.begin(); match != bestMatchByTarget.end(); match++)
	{
		totalMatchArea += (*match).second.score;
		cerr << "match score: " << (*match).second.score << endl;
	}

	// divide total intersection area by total target polygon area
	cout << "totalMatchArea: " << totalMatchArea << " totalTargetArea: " << totalTargetArea << endl;
	float match = totalMatchArea / totalTargetArea;

	// return




	return match;
}
