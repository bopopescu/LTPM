#include "Segment.h"


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

	cvFindContours(iplMask, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
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
