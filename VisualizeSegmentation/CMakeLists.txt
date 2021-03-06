cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(LTPM)

include_directories(
  ../Libraries/of_preRelease_v007_linux64/libs/openFrameworks
  ../Libraries/of_preRelease_v007_linux64/libs/openFrameworks/app
  ../Libraries/of_preRelease_v007_linux64/libs/openFrameworks/3d
  ../Libraries/of_preRelease_v007_linux64/libs/openFrameworks/communication
  ../Libraries/of_preRelease_v007_linux64/libs/openFrameworks/gl
  ../Libraries/of_preRelease_v007_linux64/libs/openFrameworks/graphics
  ../Libraries/of_preRelease_v007_linux64/libs/openFrameworks/utils
  ../Libraries/of_preRelease_v007_linux64/libs/openFrameworks/types
  ../Libraries/of_preRelease_v007_linux64/libs/openFrameworks/math
  ../Libraries/of_preRelease_v007_linux64/libs/openFrameworks/events
  ../Libraries/of_preRelease_v007_linux64/libs/openFrameworks/sound
  ../Libraries/of_preRelease_v007_linux64/libs/openFrameworks/video
  ../Libraries/of_preRelease_v007_linux64/addons/ofxOsc/src
  ../Libraries/of_preRelease_v007_linux64/libs/tess2/include
  ../Libraries/of_preRelease_v007_linux64/libs/poco/include
  ../Libraries/of_preRelease_v007_linux64/libs/cairo/include
  ../Libraries/of_preRelease_v007_linux64/libs/cairo/include/cairo
  ../Libraries/of_preRelease_v007_linux64/libs/kiss/include
  #../Libraries/of_preRelease_v007_linux64/addons/ofxOpenCv/libs/opencv/include/opencv
  #../Libraries/of_preRelease_v007_linux64/addons/ofxOpenCv/libs/opencv/include
  #../Libraries/of_preRelease_v007_linux64/addons/ofxOpenCv/src
  ../Libraries/of_preRelease_v007_linux64/addons/ofxOsc/libs/oscpack/include/osc
  ../Libraries/of_preRelease_v007_linux64/addons/ofxOsc/libs/oscpack/include/ip
  ../Libraries/of_preRelease_v007_linux64/addons/ofxAssimpModelLoader/src
  ../Libraries/of_preRelease_v007_linux64/addons/ofxAssimpModelLoader/libs/assimp/include
  /usr/include/gstreamer-0.10
  /usr/include/glib-2.0
  /usr/lib/x86_64-linux-gnu/glib-2.0/include
  /usr/include/libxml2
)

cmake_policy(SET CMP0015 NEW) # so the oF relative path works
link_directories(
  ../Libraries/of_preRelease_v007_linux64/libs/openFrameworksCompiled/lib/linux64 
  ../Libraries/of_preRelease_v007_linux64/libs/tess2/lib/linux64 
  ../Libraries/of_preRelease_v007_linux64/libs/kiss/lib/linux64
  ../Libraries/of_preRelease_v007_linux64/libs/poco/lib/linux64
  #../Libraries/of_preRelease_v007_linux64/addons/ofxOpenCv/libs/opencv/lib/linux64
  #../Libraries/of_preRelease_v007_linux64/addons/ofxAssimpModelLoader/libs/assimp/lib/linux64
)


add_executable (LTPM
  src/main.cpp
  src/LTPMApp.cpp
  #../Libraries/of_preRelease_v007_linux64/addons/ofxOpenCv/src/ofxCvImage.cpp
  #../Libraries/of_preRelease_v007_linux64/addons/ofxOpenCv/src/ofxCvGrayscaleImage.cpp
  #../Libraries/of_preRelease_v007_linux64/addons/ofxOpenCv/src/ofxCvContourFinder.cpp
  #../Libraries/of_preRelease_v007_linux64/addons/ofxOsc/src/ofxOscMessage.cpp
  #../Libraries/of_preRelease_v007_linux64/addons/ofxOsc/src/ofxOscReceiver.cpp
  #../Libraries/of_preRelease_v007_linux64/addons/ofxOsc/src/ofxOscSender.cpp
  #../Libraries/of_preRelease_v007_linux64/addons/ofxOsc/src/ofxOscBundle.cpp
  #../Libraries/of_preRelease_v007_linux64/addons/ofxAssimpModelLoader/src/ofxAssimpModelLoader.cpp
)

target_link_libraries (LTPM
  openFrameworks openal mpg123 tess2 cairo
  #opencv_core
  #opencv_lapack
  #opencv_flann
  #opencv_contrib
  #opencv_calib3d
  #opencv_features2d
  #opencv_imgproc
  #opencv_objdetect
  #opencv_legacy
  #opencv_highgui
  GL kiss GLEW PocoNet PocoUtil PocoXML PocoFoundation sndfile freeimage glut GLU #oscpack assimp
)
