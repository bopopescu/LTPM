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

#include <boost/algorithm/string.hpp>

//using namespace boost::geometry;

//#include "graph.h"
#include <Segment.h>

//#include "Types.h"

//#define SEGMENT_COLOR_SIMILARITY_THRESHOLD 50

vector<int> commaSeperatedIntegersToVector(string segFileColorString)
{
	vector<string> componentStrings;
	boost::split(componentStrings, segFileColorString, boost::is_any_of(","));

	vector<int> integers;
	for(int c = 0; c < componentStrings.size(); c++)
	{
		integers.push_back(atoi(componentStrings[c].c_str()));
	}

	return integers;
}

vector< Segment > loadSegments(string segmentFilename)
{
	cerr << "Opening " << segmentFilename << endl;
	ifstream file (segmentFilename.c_str(), ios::in);
	std::stringstream buffer;
	buffer << file.rdbuf();
	string segmentFile = buffer.str();
	file.close();

	std::vector<std::string> lines;
	boost::split(lines, segmentFile, boost::is_any_of("\n"));

	for(int l = 0; l < lines.size(); l++)
	{
		if(lines[l] == "")
			continue;
		vector<string> colorAndPaths;
		boost::split(colorAndPaths, lines[l], boost::is_any_of(":"));

		vector<int> colorComponents = commaSeperatedIntegersToVector(colorAndPaths[0]);
		Color color;
		color.r = colorComponents[0];
		color.g = colorComponents[1];
		color.b = colorComponents[2];
		//cerr << "color: " << (int) color.r << " -- " << (int) color.g << " -- " << (int) color.b << endl;

		vector<string> pathStrings;
		boost::split(pathStrings, colorAndPaths[1], boost::is_any_of("&"));

		assert(pathStrings.size() == 2); // want just one path, but might need to handle internal paths at some point ...
		vector<string> pointStrings;
		boost::split(pointStrings, pathStrings[0], boost::is_any_of(";"));

		for(int p = 0; p < pointStrings.size(); p++)
		{
			if(pointStrings[p].size() == 0)
				continue;
			vector<int> components = commaSeperatedIntegersToVector(pointStrings[p]);
			//cerr << components[0] << ",, " << components[1] << endl;
			
		}
	}


	/*for(int s = 0; s < segments.size(); s++)
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
	}*/

}


int PlaceImage::run(){
	vector< Segment > segments = loadSegments(dbImageSegmentsFilename);

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
	cout << "hey" << endl;

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
