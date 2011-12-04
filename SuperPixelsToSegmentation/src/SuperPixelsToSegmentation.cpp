#include "SuperPixelsToSegmentation.h"

#include <opencv/highgui.h>

#include <iostream>
#include <fstream>

#include <map>

#include "graph.h"
#include "Segment.h"

#include "Types.h"

#define SEGMENT_COLOR_SIMILARITY_THRESHOLD 25

Segmentation segFile2Vector(string segFilename)
{
  vector<int> segData;
  ifstream file (segFilename.c_str(), ios::in|ios::binary|ios::ate);
  if (file.is_open())
  {
    size_t size = file.tellg();
    char* memblock = new char [size];
    file.seekg (0, ios::beg);
    file.read (memblock, size);
    file.close();

    int* fileInts = (int*) memblock;
    int intCount = (size * sizeof(char)) / sizeof(int);

    int max = fileInts[0];

    for(int i = 0; i < intCount; i++)
    {
      segData.push_back(fileInts[i]);
      max = MAX(max, fileInts[i]);
    }

    cout << segData.size() << " ints, max: " << max << endl;

    delete[] memblock;
  }
  else cout << "Unable to open file";
  //return 0;


  vector<int>::iterator currentInt = segData.begin();
  
  int dim = *currentInt;
  currentInt++;
  cout << "dim: " << dim << endl;
  
  int prod = 1;
  vector<int> dim_vals;
  for(int d = 0; d < dim; d++)
  {
    prod *= *currentInt;
    cout << *currentInt << ", ";
    dim_vals.push_back(*currentInt);
    currentInt++;
  }
  cout << "prod: " << prod << endl;

  //vector<int> realSegData;
  Segmentation seg;
  seg.width = dim_vals[0];
  seg.height = dim_vals[1];
  //int realMax = *currentInt;
  seg.maxLabel = *currentInt;
  for(int p = 0; p < prod; p++)
  {
    seg.maxLabel = MAX(*currentInt, seg.maxLabel);
    seg.labels.push_back(*currentInt);
    currentInt++;
  }
  cout << "width: " << seg.width << ", height: " << seg.height << endl;
  cout << "real max: " << seg.maxLabel << endl;



  return seg;

}

set<int> getAdjacentSegments(Segmentation seg, int x, int y)
{
	set<int> adjacentLabels;

	for(int neighborX = MAX(0, x-1); neighborX <= MIN(seg.width-1, x+1); neighborX++)
	{
		for(int neighborY = MAX(0, y-1); neighborY <= MIN(seg.height-1, y+1); neighborY++)
		{
			if(neighborX == x && neighborY == y)
				continue;

			if(seg.getLabel(neighborX, neighborY) != seg.getLabel(x, y))
				adjacentLabels.insert(seg.getLabel(neighborX, neighborY));
		}
	}

	return adjacentLabels;
}

//--------------------------------------------------------------
int SuperPixelsToSegmentation::run(){
	image = 0; 
	image = cvLoadImage(imageFilename.c_str());
	if(!image)
	{
		printf("Could not load image file: %s\n",imageFilename.c_str());
		return 1;
	}

	cout << "Loaded " << imageFilename << endl << "width: " << image->width << endl;

	///segImage.clear();
	segImage = cvCreateImage(cvSize(image->width, image->height),IPL_DEPTH_8U,3);
	Segmentation seg = segFile2Vector(segFilename);


	std::map<int, Segment> segments;

	//std::map<int, std::vector<Color> > segmentPixels;
	//std::map<int, IplImage*> segmentMasks;
	//std::map<int, BwImage> segmentMaskWrappers;
	//std::map<int, CvPoint> segmentCentroids;
	//std::map<int, Color> segmentAvgColors;
	//std::map<int, int> segmentPixelCounts;

	Graph segmentGraph;
	
	RgbImage rgbImage(image);

	// create all segments
	for(int x = 0; x < image->width; x++)
	{
		for(int y = 0; y < image->height; y++)
		{
			int label = seg.labels[x*image->height+y];
			//cout << "looking at pixel with label: " << label << endl;
			std::map<int, Segment>::const_iterator segmentLocation = segments.find(label);
			if(segmentLocation == segments.end())
			{
				//cout << "found first label " << label << endl;

				Segment newSegment(seg, label);
				
				
				//segmentPixels[label] = std::vector<Color>();
				//segmentMasks[label] = cvCreateImage(cvSize(seg.width, seg.height), IPL_DEPTH_8U, 1);
				//segmentMaskWrappers[label] = BwImage(segmentMasks[label]);
				segments[label] = newSegment;
				segmentGraph.addVertexIfNotPresent(&segments[label]);

			}
			Color pixelValue = rgbImage[y][x]; 
			segments[label].pixels.push_back(pixelValue);
			segments[label].mask[y][x] = 1;			

		}
	}

	// create segment adjacency edges
	for(int x = 0; x < image->width; x++)
	{
		for(int y = 0; y < image->height; y++)
		{
			int label = seg.labels[x*image->height+y];
			set<int> adjacentLabels = getAdjacentSegments(seg, x, y);
			for(set<int>::iterator l = adjacentLabels.begin(); l != adjacentLabels.end(); l++)
			{
				
				if(!segmentGraph.edgeExists(&segments[label], &segments[*l]))
					segmentGraph.addEdgeAndVerticesIfNotPresent(&segments[label], &segments[*l]);
			}
		}
	}


	// calculate average color, contour of each segment
	CvMemStorage *storage = cvCreateMemStorage(0);
	CvSeq *contours = 0;
	for(std::map<int, Segment>::iterator s = segments.begin(); s != segments.end(); s++)
	{
		

		unsigned int totalRed   = 0;
		unsigned int totalGreen = 0;
		unsigned int totalBlue  = 0;
	
		Segment* currentSegment = & (*s).second;

		for(int p = 0; p < currentSegment->pixels.size(); p++)
		{
			totalRed   += currentSegment->pixels[p].r;
			totalGreen += currentSegment->pixels[p].g;
			totalBlue  += currentSegment->pixels[p].b;
		}
		
		int currentLabel = (*s).first;

		currentSegment->color = Color(totalRed   / currentSegment->pixels.size(),
                                              totalGreen / currentSegment->pixels.size(),
                                              totalBlue  / currentSegment->pixels.size());

		cvFindContours(currentSegment->iplMask, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
		CvMoments moments;
		cvMoments(contours, &moments);
		double m00, m10, m01;

		m00 = cvGetSpatialMoment(&moments, 0,0);
		m10 = cvGetSpatialMoment(&moments, 1,0);
		m01 = cvGetSpatialMoment(&moments, 0,1);

		// TBD check that m00 != 0
		float center_x = m10/m00;
		float center_y = m01/m00;
		currentSegment->centroid = cvPoint(center_x, center_y);

		cvDrawContours(image, contours, CV_RGB(255,0,0), CV_RGB(0,255,0), 10, 1, CV_AA, cvPoint(0,0));

		//cout << "Label " << (*s).first << ": (first, last, average) = ("
                //     << (int) (*s).second[0].r << ", " << (int) (*s).second[(*s).second.size()-1].r << ", " << (int) segmentAvgColors[label].r << ")"
		//     << "Pixel Count: " << (*s).second.size() << endl;
	}

	// merge similar adjacent segments
	/*bool updatedGraph = true;
	while(updatedGraph)
	{
		set<Edge> edges = segmentGraph.edges();
		updatedGraph = false;
		for(set<Edge>::iterator e = edges.begin(); e != edges.end() && !updatedGraph; e++)
		{
			Segment* segment1 = (*e).a;
			Segment* segment2 = (*e).b;

			if(Color::distance(segment1->color, segment2->color) < SEGMENT_COLOR_SIMILARITY_THRESHOLD)
			{
				segmentGraph.collapseEdge((*e), Segment::combine(segment1, segment2));
				updatedGraph = true;
				break;
			}
		}
	}*/

	// assign segment colors to segmentation visualization image
	RgbImage rgbSegImage(segImage);
	for(int x = 0; x < image->width; x++)
	{
		for(int y = 0; y < image->height; y++)
		{
			int label = seg.labels[x*image->height+y];
			rgbSegImage[y][x] = segments[label].color;
		}
	}

	// draw graph into segmentation image
	set<Edge> edges = segmentGraph.edges();
	for(set<Edge>::iterator e = edges.begin(); e != edges.end(); e++)
	{
		//for(set<int>::iterator other_seg = (*e).second.begin(); other_seg != (*e).second.end(); other_seg++)
		cvLine(image, (*e).a->centroid, (*e).b->centroid, CV_RGB(0, 0, 255), 1, CV_AA);
	}

	//segImage.update();
	cout << endl;

	cvNamedWindow("image", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("image", 0, 300);
	cvNamedWindow("seg image", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("seg image", image->width, 300);


	cvShowImage("image", image);
	cvShowImage("seg image", segImage);

	cvWaitKey(0);

	return 0;
}

SuperPixelsToSegmentation::SuperPixelsToSegmentation(int argc, char** argv)
{

	if(argc < 3)
	{
		std::cout << "Usage: SuperPixelsToSegmentation <image_file> <BSE_seg_file>" << std::endl;
		std::exit(0);
	}
	imageFilename = argv[1];
	segFilename = argv[2];
}
