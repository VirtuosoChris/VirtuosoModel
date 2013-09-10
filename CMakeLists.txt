cmake_minimum_required(VERSION 2.8)

project(VirtuosoConsole)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wpedantic")

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

#add_subdirectory(test)
