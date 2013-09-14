cmake_minimum_required(VERSION 2.8)

project(VirtuosoModel)
add_definitions(-DNO_BOOST -DGLEW_STATIC)
 
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wpedantic")

#need eigen
set(Eigen_DIR "" CACHE PATH "Eigen Include Directory")
include_directories(${Eigen_DIR})


set(GLEW_INCLUDE_DIR "" CACHE PATH "Glew Include Directory")
include_directories(${GLEW_INCLUDE_DIR})

set(GLEW_LIBRARY_PATH "GLEW" CACHE FILEPATH "GLEW STATIC LIB")


set(VIRTUOSO_MISC_PATH "" CACHE PATH "VIRTUOSO_MISC_PATH")
include_directories(${VIRTUOSO_MISC_PATH})

set(GL_CPP "" CACHE PATH "GL_CPP")
set(GL_CPP_LIB "GL_CPP_LIB" CACHE FILEPATH "GL_CPP_LIB")

include_directories(${GL_CPP})

set(SOURCESLIB 
AttributeArray.cpp
AttributeArray.h
GPUMesh.h
GPUMesh.cpp
Mesh.cpp
Mesh.h
Quad.h
Quad.cpp
MeshIO.h
MeshIO.cpp
)			

add_library(VirtuosoMesh ${SOURCESLIB})
target_link_libraries(VirtuosoMesh ${GL_CPP_LIB})
target_link_libraries(VirtuosoMesh ${GLEW_LIBRARY_PATH})

#add_subdirectory(test)
