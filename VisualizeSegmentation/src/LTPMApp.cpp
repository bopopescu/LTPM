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

vector<int> segFile2Vector(string segFilename)
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
	
	return segData;	
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
	segImage.allocate(image.width, image.height, OF_IMAGE_GRAYSCALE);
	vector<int> segData = segFile2Vector(segFilename);
	for(int x = 0; x < image.width; x++)
	{
		for(int y = 0; y < image.width; y++)
		{
			segImage.setColor(x, y, ofColor(segData[y*image.width+x]));
		}
	}
	
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
