#include "LTPMApp.h"



LTPMApp::LTPMApp(int argc, char** argv)
{
/*	if(argc < 3)
	{
		std::cout << "Usage: LTPMApp " << std::endl;
		std::exit(0);
	}
	meshFilename = argv[1];
	skeletonFilename = argv[2];*/
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

	image.loadImage("../../Images/220px-Medved_mzoo.jpg");
	ofxCvColorImage cvInputImage;
	cvInputImage.allocate(image.width, image.height);
	cvInputImage.setFromPixels(image.getPixels(), image.width, image.height);

	ofxCvColorImage cvSegmentationImage;
	cvSegmentationImage.allocate(image.width, image.height);

	ofxCvColorImage inputCopy;

	// get width/height disregarding ROI
    IplImage* ipltemp = cvInputImage.getCvImage();
    int _width = ipltemp->width;
    int _height = ipltemp->height;


	// opencv will clober the image it detects contours on, so we want to
    // copy it into a copy before we detect contours.  That copy is allocated
    // if necessary (necessary = (a) not allocated or (b) wrong size)
	// so be careful if you pass in different sized images to "findContours"
	// there is a performance penalty, but we think there is not a memory leak
    // to worry about better to create mutiple contour finders for different
    // sizes, ie, if you are finding contours in a 640x480 image but also a
    // 320x240 image better to make two ofxCvContourFinder objects then to use
    // one, because you will get penalized less.

	if( inputCopy.getWidth() == 0 ) {
		inputCopy.allocate( _width, _height );
	} else if( inputCopy.getWidth() != _width || inputCopy.getHeight() != _height ) {
        // reallocate to new size
        inputCopy.clear();
        inputCopy.allocate( _width, _height );
	}

    inputCopy.setROI( cvInputImage.getROI() );
    inputCopy = cvInputImage;

	cvPyrMeanShiftFiltering(inputCopy.getCvImage(), cvSegmentationImage.getCvImage(), 15, 15, 1/*, (CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 5, 1)*/);
	
}

//--------------------------------------------------------------
void LTPMApp::update(){


}

//--------------------------------------------------------------
void LTPMApp::draw(){

	image.draw(0, 0);
	segImage.draw(0, 0);
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
