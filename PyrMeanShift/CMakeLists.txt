cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(PyrMeanShiftSegmentation)

find_package(OpenCV)

include_directories(
${OpenCV_INCLUDE_DIR}
)

link_directories(
  #${OPENCV_LIBRARIES}
)


add_executable (PyrMeanShiftSegmentation
  src/PyrMeanShiftSegmentation.cpp
)

target_link_libraries (PyrMeanShiftSegmentation
  ${OPENCV_LIBS}
  opencv_highgui
)
