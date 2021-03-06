#include "SuperPixelsToSegmentation.h"

#include <opencv/highgui.h>

#include <iostream>
#include <fstream>

#include <map>

#include "graph.h"
#include "Segment.h"

#include <Types.h>

#define SEGMENT_COLOR_SIMILARITY_THRESHOLD 50

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

    cerr << segData.size() << " ints, max: " << max << endl;

    delete[] memblock;
  }
  else cerr << "Unable to open file";
  //return 0;


  vector<int>::iterator currentInt = segData.begin();
  
  int dim = *currentInt;
  currentInt++;
  cerr << "dim: " << dim << endl;
  
  int prod = 1;
  vector<int> dim_vals;
  for(int d = 0; d < dim; d++)
  {
    prod *= *currentInt;
    cerr << *currentInt << ", ";
    dim_vals.push_back(*currentInt);
    currentInt++;
  }
  cerr << "prod: " << prod << endl;

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
  cerr << "width: " << seg.width << ", height: " << seg.height << endl;
  cerr << "real max: " << seg.maxLabel << endl;



  return seg;

}

std::set<int> getAdjacentSegments(Segmentation seg, int x, int y)
{
	std::set<int> adjacentLabels;

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

void printSegments(vector<Segment*> segments)
{
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

//--------------------------------------------------------------
int SuperPixelsToSegmentation::run(){
	image = 0; 
	image = cvLoadImage(imageFilename.c_str());
	if(!image)
	{
		printf("Could not load image file: %s\n",imageFilename.c_str());
		return 1;
	}

	cerr << "Loaded " << imageFilename << endl << "width: " << image->width << endl;

	///segImage.clear();
	segImage = cvCreateImage(cvSize(image->width, image->height),IPL_DEPTH_8U,3);
	Segmentation seg = segFile2Vector(segFilename);
	

	std::map<int, Segment> segments;

	Graph segmentGraph;
	
	RgbImage rgbImage(image);

	// create all segments
	for(int x = 0; x < image->width; x++)
	{
		for(int y = 0; y < image->height; y++)
		{
			int label = seg.labels[x*image->height+y];
			//cerr << "looking at pixel with label: " << label << endl;
			std::map<int, Segment>::const_iterator segmentLocation = segments.find(label);
			if(segmentLocation == segments.end())
			{

				Segment newSegment(seg, label);
				
				

				segments[label] = newSegment;
				//segmentGraph.addVertexIfNotPresent(&segments[label]);

			}
			Color pixelValue = rgbImage[y][x]; 
			segments[label].pixels.push_back(pixelValue);
			segments[label].mask[y][x] = 255;			

		}
	}

	// remove invalid segments
	vector<int> invalidLabels;
	for(std::map<int, Segment>::iterator s = segments.begin(); s != segments.end(); s++)
	{
		Segment* currentSegment = & (*s).second;
		if(!currentSegment->hasCVContour()) {
			invalidLabels.push_back((*s).first); 
		}
		else
		{
			currentSegment->updateContour();
			segmentGraph.addVertexIfNotPresent(currentSegment);
		}
	}

	for(int i = 0;  i < invalidLabels.size(); i++)
	{
		cerr << "label: " << invalidLabels[i] << " is invalid" <<  endl;
		segments.erase(invalidLabels[i]);
		if(segments.find(invalidLabels[i]) != segments.end()) cerr << invalidLabels[i] << " still in the segments map" << endl;
	}

	//if(segments.find(0) != segments.end()) cerr << 0 << " still in the segments map" << endl;
	
	// create segment adjacency edges
	for(int x = 0; x < image->width; x++)
	{
		for(int y = 0; y < image->height; y++)
		{
			int label = seg.labels[x*image->height+y];
			if(find(invalidLabels.begin(), invalidLabels.end(), label) != invalidLabels.end()) { 
				//cerr << "label: " << label << endl;
				continue;
			}

			std::set<int> adjacentLabels = getAdjacentSegments(seg, x, y);
			for(std::set<int>::iterator l = adjacentLabels.begin(); l != adjacentLabels.end(); l++)
			{
				if(find(invalidLabels.begin(), invalidLabels.end(), *l) != invalidLabels.end()) { 
					//cerr << "other label: " << *l << endl;
					continue;
				}
				if(!segmentGraph.edgeExists(&segments[label], &segments[*l]))
					segmentGraph.addEdgeAndVerticesIfNotPresent(&segments[label], &segments[*l]);
			}
		}
	}

	//if(segments.find(0) != segments.end()) cerr << 0 << " still in the segments map" << endl;

	// calculate average color, contour of each segment
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
				

		

		//cerr << "Avg Color: " << (int)currentSegment->color.r << " " << (int)currentSegment->color.g << " " << (int)currentSegment->color.g << endl;
		//cerr << "label: " << currentSegment->label << " numPixels: " << currentSegment->pixels.size() << endl;

		//cvNamedWindow("iplMask", CV_WINDOW_AUTOSIZE);
		//cvMoveWindow("iplMask", 0, 200);
		//cvShowImage("iplMask", currentSegment->iplMask);
		//cvWaitKey(0);
		//return 0;
		//currentSegment->updateContour();


		//cvDrawContours(image, currentSegment->contour, CV_RGB(255,0,0), CV_RGB(0,255,0), 10, 1, CV_AA, cvPoint(0,0));
		
		
		
		//cerr << "Label " << (*s).first << ": (first, last, average) = ("
                //     << (int) (*s).second[0].r << ", " << (int) (*s).second[(*s).second.size()-1].r << ", " << (int) segmentAvgColors[label].r << ")"
		//     << "Pixel Count: " << (*s).second.size() << endl;
	}



	// merge similar adjacent segments
	bool updatedGraph = true;
	while(updatedGraph)
	{
		std::set<Edge> edges = segmentGraph.edges();
		updatedGraph = false;
		for(std::set<Edge>::iterator e = edges.begin(); e != edges.end() && !updatedGraph; e++)
		{
			Segment* segment1 = (*e).a;
			Segment* segment2 = (*e).b;

			if(Color::distance(segment1->color, segment2->color) < SEGMENT_COLOR_SIMILARITY_THRESHOLD)
			{
				segmentGraph.collapseEdge(*e);
				updatedGraph = true;
				break;
			}
		}
	}
	// segments is now invalid-ish

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


	// make image with collapsed segmentation
	IplImage* mergedSegmentsImage = cvCreateImage(cvSize(segImage->width, segImage->height), IPL_DEPTH_8U, 3);
	vector<Segment*> collapsedSegments = segmentGraph.vertices();
	for(int n = 0; n < collapsedSegments.size(); n++)
	{
		Segment* s = collapsedSegments[n];
		cvDrawContours(mergedSegmentsImage, s->contour, CV_RGB(255, 255, 255), CV_RGB(s->color.r, s->color.g, s->color.b), 10, 1, CV_AA);
	}
	

	// save segmentation to file, yo
	printSegments(segmentGraph.vertices());
	

	// check if should visualize
	if(shouldVisualize == 0) return 0; 

	// draw graph into segmentation image
	std::set<Edge> edges = segmentGraph.edges();
	for(std::set<Edge>::iterator e = edges.begin(); e != edges.end(); e++)
	{
		
		//cvDrawContours(segImage, (*e).a->contour, CV_RGB(255,0,0), CV_RGB(0,255,0), 10, 1, CV_AA, cvPoint(0,0));

		//for(std::set<int>::iterator other_seg = (*e).second.begin(); other_seg != (*e).second.end(); other_seg++)
		//cvLine(mergedSegmentsImage, (*e).a->centroid, (*e).b->centroid, CV_RGB(0, 0, 255), 1, CV_AA); // FIXME centroid is bad sometimes (when segment is small. we blame OpenCV)
		//cerr << "a centroid: " << (*e).a->centroid.x << " b centroid: " << (*e).b->centroid.x << endl;

	}

	//segImage.update();
	cerr << endl;
	

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

	int key = -1;
	while(key != 0)
	{
		key = cvWaitKey(0);
		cerr << key << endl;
	}

	return 0;
}

SuperPixelsToSegmentation::SuperPixelsToSegmentation(int argc, char** argv)
{

	if(argc < 3)
	{
		std::cerr << "Usage: SuperPixelsToSegmentation <image_file> <BSE_seg_file>" << std::endl;
		std::exit(0);
	}
	imageFilename = argv[1];
	segFilename = argv[2];
	shouldVisualize = atoi(argv[3]);
	std::cerr << "Should Visualize: " << shouldVisualize << endl;
}
