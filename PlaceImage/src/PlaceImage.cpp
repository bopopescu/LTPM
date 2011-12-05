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
	vector<Segment> segments;

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

		Segment newSegment;

		vector<string> colorAndPaths;
		boost::split(colorAndPaths, lines[l], boost::is_any_of(":"));

		vector<int> colorComponents = commaSeperatedIntegersToVector(colorAndPaths[0]);
		Color color;
		color.r = colorComponents[0];
		color.g = colorComponents[1];
		color.b = colorComponents[2];
		newSegment.color = color;
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
			append(newSegment.polygon, Point(components[0], components[1]));
			//cerr << components[0] << ",, " << components[1] << endl;
			
		}
		//int static sum = 0;
		//sum += area(newSegment.polygon);
		//cerr << "polygon area: " << area(newSegment.polygon) << endl;
		//cerr << "total: " << sum << endl;

		segments.push_back(newSegment);
	}


	return segments;

}


int PlaceImage::run(){
	vector< Segment > candidateImageSegments = loadSegments(dbImageSegmentsFilename);

	vector< Segment > targetTileSegments = loadSegments(bigImageSegmentationsDirectory); // let's see if we can compare a segments to itself!

	cerr << "Compare! " << scoreCandidate(candidateImageSegments, targetTileSegments) << endl;

	// TODO for each tile
	//vector< Segment > targetTileSegments = loadSegments(bigImageSegmentationsDirectory); // TODO one file path now
	// TODO end for each

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
		std::cerr << "Usage: PlaceImage <tile_segmentations_directory> <db_image_segments>" << std::endl;
		std::exit(0);
	}
	bigImageSegmentationsDirectory = argv[1];
	dbImageSegmentsFilename  = argv[2];
}
