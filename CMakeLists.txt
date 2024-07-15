project(libhaloc)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(catkin REQUIRED)

include_directories(
  include
  ${EIGEN3_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${catkin_INCLUDE_DIRS}
)

catkin_package(
  INCLUDE_DIRS
    include 
  LIBRARIES
    haloc
  DEPENDS
    OpenCV)

# Add the Image Hashing library
add_library(haloc
  src/haloc.cc
  src/hash.cc)
target_link_libraries(haloc
  ${EIGEN3_LIBRARIES}
  ${OpenCV_LIBRARIES}
  ${catkin_LIBRARIES})
