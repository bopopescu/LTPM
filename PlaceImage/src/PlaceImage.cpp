#include "PlaceImage.h"

//#include <opencv/highgui.h>

#include <iostream>
#include <fstream>

#include <map>
#include <vector>

using namespace std;

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/domains/gis/io/wkt/wkt.hpp>

#include <boost/foreach.hpp>

//using namespace boost::geometry;

//#include "graph.h"
//#include "Segment.h"

//#include "Types.h"

//#define SEGMENT_COLOR_SIMILARITY_THRESHOLD 50

/*
vector< vector<Point> > loadSegments(string segmentFilename)
{
	vector<int> segData;
	ifstream file (segFilename.c_str(), ios::in|ios::binary|ios::ate);

	for(int s = 0; s < segments.size(); s++)
	{
		Segment* seg = segments[s];
		cout << (int) seg->color.r << "," << (int) seg->color.g << "," << (int) seg->color.b << ":";
		
		for( CvSeq* c = seg->contour; c!=NULL; c=c->h_next ){
			for(int i = 0; i < c->total; i++){
				CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, c, i );
				cout << p->x << "," << p->y << ";";
			}
			cout << "&";
		}
		cout << endl;
	}
}
*/

int PlaceImage::run(){


typedef boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<double> > polygon;

    polygon green, blue;

    boost::geometry::read_wkt(
        "POLYGON((2 1.3,2.4 1.7,2.8 1.8,3.4 1.2,3.9 1.6,3.4 2,4.1 3,5.3 2.6,5.4 1.2,4.9 0.8,2.9 0.7,2 1.3)"
            "(4.0 2.0, 4.2 1.4, 4.8 1.9, 4.4 2.2, 4.0 2.0))", green);

    boost::geometry::read_wkt(
        "POLYGON((4.0 -0.5 , 3.5 1.0 , 2.0 1.5 , 3.5 2.0 , 4.0 3.5 , 4.5 2.0 , 6.0 1.5 , 4.5 1.0 , 4.0 -0.5))", blue);

    std::deque<polygon> output;
    boost::geometry::intersection(green, blue, output);

    int i = 0;
    std::cout << "green && blue:" << std::endl;
    BOOST_FOREACH(polygon const& p, output)
    {
        std::cout << i++ << ": " << boost::geometry::area(p) << std::endl;
    }


/*
	int top = 200;
	cvNamedWindow("image", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("image", 0, top);
	cvNamedWindow("seg image", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("seg image", image->width, top);
	cvNamedWindow("collapsed segments", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("collapsed segments", image->width, top + image->height);

	cvShowImage("image", image);
	cvShowImage("seg image", segImage);
	cvShowImage("collapsed segments", mergedSegmentsImage);

	cvWaitKey(0);
*/
	return 0;
}

PlaceImage::PlaceImage(int argc, char** argv)
{

	if(argc < 3)
	{
		std::cerr << "Usage: PlaceImage <big_image_segments> <db_image_segments>" << std::endl;
		std::exit(0);
	}
	bigImageSegmentsFilename = argv[1];
	dbImageSegmentsFilename  = argv[2];
}
