#include "LTPMApp.h"

#include <iostream>
#include <fstream>


LTPMApp::LTPMApp(int argc, char** argv)
{

	if(argc < 3)
	{
		std::cout << "Usage: LTPMApp <image_file> <BSE_seg_file>" << std::endl;
		std::exit(0);
	}
	imageFilename = argv[1];
	segFilename = argv[2];
}

typedef struct Segmentation
{
  int maxLabel;
  vector<int> labels;
} Segmentation;

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
  //int realMax = *currentInt;
  seg.maxLabel = *currentInt;
  for(int p = 0; p < prod; p++)
  {
    seg.maxLabel = MAX(*currentInt, seg.maxLabel);
    seg.labels.push_back(*currentInt);
    currentInt++;
  }
  cout << "real max: " << seg.maxLabel << endl;



  return seg;


/*
  vector<unsigned int> d;


  ///fid = fopen(filename,'r');
  ifstream file (segFilename.c_str(), ios::in|ios::binary|ios::ate);
  if (file.is_open())
  {
    ///dim = fread(fid,1,'uint');
    unsigned int dim;
    file.read((char*)&dim, sizeof(unsigned int));

    ///for i = 1:dim
    ///  d(i) = fread(fid,1,'uint');
    ///end;
    for(int i = 0; i < dim; i++)
    {
      unsigned int intI;
      file >> intI;
      d.push_back(intI);
    }


/*
    [B,count] = fread(fid,prod(d),'int');
    B = reshape(B,fliplr(d));

dim = fread(fid,1,'uint');

if (~feof(fid)) %collection of arrays.
  A{1} = B;
  ct = 2;
  while (~feof(fid))
    for i = 1:dim
      d(i) = fread(fid,1,'uint');
    end;
    [B,count] = fread(fid,prod(d),'int');
    A{ct} = reshape(B,fliplr(d));
    ct = ct + 1;
    dim = fread(fid,1,'uint');
  end;
else %else a single array 
  A = B;
end;

*/

/*
    cout << "dim: " << dim << endl << "d: ";
    for(int i = 0; i < d.size(); i++)
      cout << d[i] << ", " << endl;
*/

 // }

}

//--------------------------------------------------------------
void LTPMApp::setup(){
	ofDisableDataPath();

    //receiver.setup( PORT );

	ofSetVerticalSync(true);

	// this uses depth information for occlusion
	// rather than always drawing things on top of each other
//	glEnable(GL_DEPTH_TEST);

	// this sets the camera's distance from the object
//	cam.setDistance(100);

	image.loadImage(imageFilename);
	segImage.clear();
	segImage.allocate(image.width, image.height, OF_IMAGE_COLOR);
	Segmentation seg = segFile2Vector(segFilename);
        
	for(int x = 0; x < image.width; x++)
	{
		for(int y = 0; y < image.height; y++)
		{
			float val = float(seg.labels[x*image.height+y]) / float(seg.maxLabel);
			ofColor color(int(val * 255));
			//cout << segData[x*image.width+y] << ", ";
			segImage.setColor(x, y, color);
		}
	}
	segImage.update();
	cout << endl;
	
}

//--------------------------------------------------------------
void LTPMApp::update(){


}

//--------------------------------------------------------------
void LTPMApp::draw(){

	image.draw(0, 0);
	segImage.draw(image.width, 0);
}

//--------------------------------------------------------------
/*vector<vector<ofPoint>> LTPMApp::getSegments(const ofImage & image)
{
	
}*/

//--------------------------------------------------------------
void LTPMApp::keyPressed(int key){
/*	switch(key) {
		case 'M':
		case 'm':
			if(cam.getMouseInputEnabled()) cam.disableMouseInput();
			else cam.enableMouseInput();
			break;

		case 'F':
		case 'f':
			ofToggleFullscreen();
			break;
	}*/
}

//--------------------------------------------------------------
void LTPMApp::keyReleased(int key){

}

//--------------------------------------------------------------
void LTPMApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void LTPMApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void LTPMApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void LTPMApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void LTPMApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void LTPMApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void LTPMApp::dragEvent(ofDragInfo dragInfo){

}
