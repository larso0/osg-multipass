cmake_minimum_required(VERSION 3.0)
project(multipass)

# Only use Debug and Release builds
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE)

# Get all source files recursively
file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src/*)

# Create executable from source files
add_executable(multipass ${SRC})

# Find OpenSceneGraph
set(OSG_DIR "osg" CACHE PATH "Path to OSG")
set(OSG_3RDPARTY_DIR "3rdparty" CACHE PATH "Path to OSG 3rd party dependencies")
find_package(OpenSceneGraph REQUIRED osgDB osgViewer osgGA osgUtil osgText)
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})
target_link_libraries(multipass ${OPENSCENEGRAPH_LIBRARIES})