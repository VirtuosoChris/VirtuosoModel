Virtuoso Model Library by Christopher Pugh
===========================================
Classes to build models and upload them to the GPU. 

Depends on OpenGL 

This code works and is useful, so I've decided to upload it, but this part of the code is going to end up getting an overhaul at some point.  The Assimp importer needs a fair bit of cleanup before I want to share it but it shouldn't change too much. 

However, I feel like the following revisions need to occur with this lib: 
-Make it so you don't have to predeclare vertex counts? 
-build in iterators to build models with interleaved arrays
-add support for mixed datatypes for attribute arrays (not just float)
-support for non-indexed mesh
-merge mesh classes
-able to enable and disable arrays
-triangle iterators : eg (a list of tris from an indexed mesh)

enum-count pairs:: multiple primitive types, push into a loop? 

mesh = gpuMesh

document vbo format


