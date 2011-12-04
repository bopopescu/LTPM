#include "SuperPixelsToSegmentation.h"

#include <opencv/highgui.h>

#include <iostream>
#include <fstream>

#include <map>

#include "graph.h"



class Segmentation
{
public:
  int maxLabel;
  vector<int> labels;
  int width;
  int height;

  int getLabel(int x, int y) { return labels[x*height+y]; }
};

class Color
{
public:
  Color() {};
  Color(int _r, int _g, int _b) { r = _r; g = _g; b = _b; }
  unsigned char r;
  unsigned char g;
  unsigned char b;
};


template<class T> class Image
{
  private:
  IplImage* imgp;
  public:
  Image(IplImage* img=0) {imgp=img;}
  ~Image(){imgp=0;}
  void operator=(IplImage* img) {imgp=img;}
  inline T* operator[](const int rowIndx) {
    return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));}
};

typedef struct{
  float b,g,r;
} RgbPixelFloat;

typedef Image<Color>       RgbImage;
typedef Image<RgbPixelFloat>  RgbImageFloat;
typedef Image<unsigned char>  BwImage;
typedef Image<float>          BwImageFloat;


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

	std::map<int, std::vector<Color> > segmentPixels;
	std::map<int, Color > segmentAvgColors;
	std::map<int, int> segmentPixelCounts;

	Graph segmentGraph;
	
	RgbImage rgbImage(image);

	for(int x = 0; x < image->width; x++)
	{
		for(int y = 0; y < image->height; y++)
		{
			int label = seg.labels[x*image->height+y];
			float val = float(label) / float(seg.maxLabel);
			std::map<int, std::vector<Color> >::const_iterator labelColor = segmentPixels.find(label);
			if(labelColor == segmentPixels.end())
			{
				cout << "found first label " << label << endl;
				segmentGraph.addVertexIfNotPresent(label);
				segmentPixels[label] = std::vector<Color>();

			}
			Color pixelValue = rgbImage[y][x]; 
			segmentPixels[label].push_back(pixelValue);

			set<int> adjacentLabels = getAdjacentSegments(seg, x, y);
			for(set<int>::iterator l = adjacentLabels.begin(); l != adjacentLabels.end(); l++)
			{
				if(!segmentGraph.edgeExists(label, *l))
					segmentGraph.addEdgeAndVerticesIfNotPresent(label, *l);
			}
				

		}
	}

	// calculate average color in each segment
	for(std::map<int, std::vector<Color> >::iterator s = segmentPixels.begin(); s != segmentPixels.end(); s++)
	{
		unsigned int totalRed   = 0;
		unsigned int totalGreen = 0;
		unsigned int totalBlue  = 0;
	
		for(int p = 0; p < (*s).second.size(); p++)
		{
			totalRed   += (*s).second[p].r;
			totalGreen += (*s).second[p].g;
			totalBlue  += (*s).second[p].b;
		}
		
		int label = (*s).first;

		segmentAvgColors[label] = Color(totalRed   / (*s).second.size(),
                                                       totalGreen / (*s).second.size(),
                                                       totalBlue  / (*s).second.size());

		
		cout << "Label " << (*s).first << ": (first, last, average) = ("
                     << (int) (*s).second[0].r << ", " << (int) (*s).second[(*s).second.size()-1].r << ", " << (int) segmentAvgColors[label].r << ")"
		     << "Pixel Count: " << (*s).second.size() << endl;
	}

	// merge similar adjacent segments
	for(int x = 0; x < image->width; x++)
	{
		for(int y = 0; y < image->height; y++)
		{
			
		}
	}

	// assign segment colors to segmentation visualization image
	RgbImage rgbSegImage(segImage);
	for(int x = 0; x < image->width; x++)
	{
		for(int y = 0; y < image->height; y++)
		{
			int label = seg.labels[x*image->height+y];
			rgbSegImage[y][x] = segmentAvgColors[label];
		}
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
