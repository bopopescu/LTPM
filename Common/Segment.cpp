#include "Segment.h"

#include <boost/foreach.hpp>
#include <boost/geometry/algorithms/num_points.hpp>
#include <boost/geometry/geometries/box.hpp>

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

bool Segment::hasCVContour()
{
	contour = 0;

	
	cvFindContours(iplMask, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	CvMoments moments;
	
	if(contour == 0) return false;
	else return true;
}

void Segment::updateContour()
{
	contour = 0;

	cvFindContours(iplMask, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	CvMoments moments;
	
//	CvSeq* c = contour;
//cerr << "-------------------------- " << c << endl;
	/*for( CvSeq* c = contour; c!=NULL; c=c->h_next ){
			for(int i = 0; i < c->total; i++){
				CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, c, i );
				cerr << p->x << "," << p->y << endl;
			}
		}
*/
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


float Segment::hackAreaOfIntersectionWith(Segment & other)
{
	boost::geometry::model::box<Point> bbox1;
	boost::geometry::envelope(polygon, bbox1);

	int min_x1 = get<0>(bbox1.min_corner());
	int max_x1 = get<0>(bbox1.max_corner());
	int min_y1 = get<1>(bbox1.min_corner());
	int max_y1 = get<1>(bbox1.max_corner());

	boost::geometry::model::box<Point> bbox2;
	boost::geometry::envelope(polygon, bbox2);

	int min_x2 = get<0>(bbox2.min_corner());
	int max_x2 = get<0>(bbox2.max_corner());
	int min_y2 = get<1>(bbox2.min_corner());
	int max_y2 = get<1>(bbox2.max_corner());

	int min_x = MAX(min_x1, min_x2);
	int min_y = MAX(min_y1, min_y2);
	int max_x = MIN(max_x1, max_x2);
	int max_y = MIN(max_y1, max_y2);

	
	int intersectionArea = 0;
	for(int x = min_x; x <= max_x; x++)
		for(int y = min_y; y <= max_y; y++)
			if(boost::geometry::within(make<Point>(x, y), polygon) &&
			   boost::geometry::within(make<Point>(x, y), other.polygon))
				intersectionArea++; 
	
	return intersectionArea;
}

float Segment::areaOfIntersectionWith(Segment & other)
{
	/*cerr << "mePoly    size: " << num_points(polygon) << endl << "mePoly    area: " << boost::geometry::area(polygon) << endl;
	cerr << "otherPoly size: " << num_points(other.polygon) << endl << "otherPoly area: " << boost::geometry::area(other.polygon) << endl;
	cerr << endl;*/

	float intersectionArea = 0.0;

	try
	{
		std::deque<Polygon> output;
		boost::geometry::intersection(polygon, other.polygon, output);

		intersectionArea = 0.0;
		BOOST_FOREACH(Polygon const& p, output)
		{
			intersectionArea += boost::geometry::area(p);
		}
	}
	catch(std::exception & e)
	{
		intersectionArea = hackAreaOfIntersectionWith(other);
	}

	return intersectionArea;
}

class Match
{
public:
	Match() { };
	Match(Segment * _segment, float _intersectionArea) { segment = _segment; intersectionArea = _intersectionArea; };
	float intersectionArea;
	Segment* segment;
};

bool contains(map<Segment*, Match> _map, Segment* key)
{
	map<Segment*, Match>::iterator location = _map.find(key);
	return location != _map.end();
}

float scoreCandidate(vector<Segment> targetSegmentation, vector<Segment> candidateSegmentation)
{

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
			else if(bestTargetScore > bestMatchByTarget[bestMatchTargetSegment].intersectionArea)
				bestMatchByTarget[bestMatchTargetSegment] = Match(candidateSegment, bestTargetScore);
		}
	}

	// sum intersection areas of all pairs
	float totalMatchArea = 0.0;
	float sumOfColorScoresWeightedByIntersectionArea = 0.0;
	for(map<Segment*, Match>::iterator match = bestMatchByTarget.begin(); match != bestMatchByTarget.end(); match++)
	{
		Segment* targetSegment    = (*match).first;
		Segment* candidateSegment = (*match).second.segment;
		float matchArea = (*match).second.intersectionArea;

		totalMatchArea += matchArea;
		cerr << "segment pair area match score: " << (*match).second.intersectionArea << endl;

		float colorScore = 1.0 - Color::distance(targetSegment->color, candidateSegment->color) / sqrtf((255*255) + (255*255) + (255*255));
		sumOfColorScoresWeightedByIntersectionArea += matchArea * colorScore;
	}

	// divide total intersection area by total target polygon area
	cerr << "totalMatchArea: " << totalMatchArea << " totalTargetArea: " << totalTargetArea << endl;
	float areaScore = totalMatchArea / totalTargetArea;

	// divide weighted color match sum by total intersection area
	float colorScore = sumOfColorScoresWeightedByIntersectionArea / totalMatchArea;
	cerr << "colorScore: " << colorScore << endl;

	float match = areaScore * colorScore;
	cerr << "MATCH: " << match << endl;

	return match;
}
