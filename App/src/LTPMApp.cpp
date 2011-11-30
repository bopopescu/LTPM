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

}

//--------------------------------------------------------------
void LTPMApp::update(){


}

//--------------------------------------------------------------
void LTPMApp::draw(){

image.draw(0, 0);

}

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
