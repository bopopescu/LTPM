cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(PlaceImage)

find_package(OpenCV)

include_directories(
  ${OpenCV_INCLUDE_DIR}
  ../Libraries/boost_1_48_0
  ../Common
)

link_directories(
)


add_executable (PlaceImage
  src/main.cpp
  src/PlaceImage.cpp
  ../Common/Segment.cpp
#  src/Segment.cpp
)

target_link_libraries (PlaceImage
  #${OPENCV_LIBS}
  opencv_core
  #opencv_lapack
  #opencv_flann
  #opencv_contrib
  #opencv_calib3d
  #opencv_features2d
  opencv_imgproc
  #opencv_objdetect
  #opencv_legacy
  #opencv_highgui
)
