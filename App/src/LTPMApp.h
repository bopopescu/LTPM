#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
//#include <ofxAssimpModelLoader.h>


// listen on port 12345
//#define PORT 7110
//#define NUM_MSG_STRINGS 20

#include <map>

class LTPMApp : public ofBaseApp {
	public:

		LTPMApp(int argc, char** argv);

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);


		ofImage image;

		//ofxAssimpModelLoader body;


		//ofxOscReceiver	receiver;



};
