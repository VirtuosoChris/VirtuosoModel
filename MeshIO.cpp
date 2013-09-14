#include "MeshIO.h"
/*
Virtuoso::Mesh Virtuoso::VBO::loadMesh(std::istream& in)
{
    Mesh rval;

    unsigned int numAttributes = 0;

    in.read(( char*)&rval.numVerts, sizeof(int)); //how many vertices?
    in.read(( char*)&rval.numFaces, sizeof(int)); //how many faces?
    in.read((char*)&rval.mode, sizeof(int)); //output the primitive type
    in.read((char*)&rval.indexedmesh, sizeof(bool));
    in.read((char*)&numAttributes, sizeof(unsigned int));

    if(indexedmesh){

        indexbuffer.resize(numFaces * pointsperface(mode));

        in.read(( char*) &(indexbuffer[0]), sizeof(unsigned int)*numFaces*pointsperface(mode));

    }

    for(unsigned int ppsp = 0; ppsp < numAttributes; ppsp++){
        attributes.push_back(AttributeArray(numVerts, in));
    }

}
*/
