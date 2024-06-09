cmake_minimum_required(VERSION 3.0)
project(VCWORK)

# Find OpenCV package
find_package(OpenCV REQUIRED)
# Include directories from OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})

# Create an executable
add_executable(VCWORK main.cpp vc.c)

# Link OpenCV Libraries
target_link_libraries(VCWORK ${OpenCV_LIBS})

#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2") # Optimization level 2
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP") # Use multiple processors
