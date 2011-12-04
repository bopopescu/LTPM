#include "Segment.h"

Segment::Segment()
{

}

Segment::Segment(const Segmentation & seg, int _label)
{
	label = _label;
	iplMask = cvCreateImage(cvSize(seg.width, seg.height), IPL_DEPTH_8U, 1);
	cvSet(iplMask, cvScalar(0));
	mask = BwImage(iplMask);
}
