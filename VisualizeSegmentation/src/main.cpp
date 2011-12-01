#include <ofAppGlutWindow.h>
#include "LTPMApp.h"

int main(int argc, char** argv) {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 640, 480, OF_WINDOW);
	ofRunApp(new LTPMApp(argc, argv));
}
