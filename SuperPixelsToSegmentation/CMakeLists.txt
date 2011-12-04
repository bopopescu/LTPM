cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(SuperPixelsToSegmentation)

find_package(OpenCV)

include_directories(
${OpenCV_INCLUDE_DIR}
)

link_directories(
)


add_executable (SuperPixelsToSegmentation
  src/main.cpp
  src/SuperPixelsToSegmentation.cpp
)

target_link_libraries (SuperPixelsToSegmentation
  ${OPENCV_LIBS}
  #opencv_core
  #opencv_lapack
  #opencv_flann
  #opencv_contrib
  #opencv_calib3d
  #opencv_features2d
  #opencv_imgproc
  #opencv_objdetect
  #opencv_legacy
  opencv_highgui
)
