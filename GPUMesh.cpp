//  Copyright (c) 2013 Christopher Pugh. All rights reserved.

#include "GPUMesh.h"
#include <stdexcept>

namespace Virtuoso{

GPUMesh& GPUMesh::operator=(const GPUMesh& m)
{
    if(&m != this)
    {
        freeMesh();
        vertexCount = m.vertexCount;
        faceCount = m.faceCount;
        copycount = m.copycount;
        vbos = m.vbos;
        indexbuffer = m.indexbuffer;
        attributenames = m.attributenames;
        
        #ifdef GL_ES_BUILD
            attributecomponents = m.attributecomponents;
        #else
            vertex_array = m.vertex_array;
        #endif
    }
    
    return *this;
}

void GPUMesh::freeMesh()
{
    ///\todo test that they exist first
    glDeleteBuffers(vbos.size(), &vbos[0]);
        
    #ifndef GL_ES_BUILD
        glDeleteVertexArrays(1, & vertex_array);
    #endif
        
    glDeleteBuffers(1, & indexbuffer);
}
    
GLuint& GPUMesh::operator[](const std::string& in){
    std::vector<std::string>::iterator it =
    std::find(attributenames.begin(), attributenames.end(),in);

    if(it == attributenames.end()){
        throw std::runtime_error("attribute not present");
    }

    int index =  std::distance(attributenames.begin(), it);

    return vbos[index];
}

#ifndef GL_ES_BUILD
void GPUMesh::initialize(std::istream& in)//:adjacency(false)
{

    glGenVertexArrays(1, &vertex_array);

    glGenBuffers(1, & indexbuffer);

    if(!vertex_array || !indexbuffer) {
        throw std::runtime_error("could not allocate VBO or vertex array");
    }

    glBindVertexArray(vertex_array);

    unsigned int numAttributes = 0;

    in.read(( char*)&vertexCount, sizeof(int)); //how many vertices?
    in.read(( char*)&faceCount, sizeof(int)); //how many faces?

    Mesh::PrimitiveMode mode;
    in.read((char*)&mode, sizeof(int)); //output the primitive type

    if(mode != Mesh::TRIANGLES) {
        throw std::runtime_error("Non triangle primitives currently unsupported!");
    }

    bool indexedmesh = false;
    in.read((char*)&indexedmesh, sizeof(bool));

    if(!indexedmesh) {
        throw std::runtime_error("Non-indexed mesh not currently supported");
    }

    in.read((char*)&numAttributes, sizeof(unsigned int));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

    //create an empty vbo
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*3*faceCount, NULL, GL_STATIC_DRAW);

    //map the buffer for writing only
    void* indexptr  = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

    if(!indexptr) {
        throw std::runtime_error("Unable to map buffer!");
    }

    //read the index buffer in to the gpu
    if(indexedmesh) {
        in.read(( char*)indexptr, sizeof(unsigned int)*faceCount*Mesh::pointsperface(mode));
    }

    if(!glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER)) {
        throw std::runtime_error("Unmapping buffer returned GL_FALSE - buffer contents corrupted");
    }

    //load in the attribute arrays to the gpu one by one
    for(unsigned int i = 0; i < numAttributes; i++) {
        unsigned int namelength;

        //read in the attribute name string
        in.read((char*) &namelength, sizeof(unsigned int));
        char* c_str = new char[namelength];

        in.read((char*)c_str, sizeof(char)*namelength);

        std::string aname = std::string(c_str);

        //read how many components this attribute has
        unsigned int components = 0;
        in.read((char*)&components, sizeof(unsigned int));

        delete []c_str;

        //generate a new vbo for the attribute
        GLuint nb = 0;

        glGenBuffers(1, &nb);

        if(!nb)throw std::runtime_error("Could not allocate attribute buffer");

        //add the vbo id and the attribute name to this object
        vbos.push_back(nb);
        attributenames.push_back(aname);


        glBindBuffer(GL_ARRAY_BUFFER, nb);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*components*vertexCount, NULL, GL_STATIC_DRAW);
        glVertexAttribPointer(i, components, GL_FLOAT, GL_FALSE, 0, 0);


        //read in the attribute data from the file
        //array = new float[components * vertices];
        void* aptr  = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

        if(!aptr) {
            throw std::runtime_error("unable to map array attribute buffer");
        }

        in.read((char*) aptr, vertexCount*components*sizeof(float));

        if(!glUnmapBuffer(GL_ARRAY_BUFFER)) {
            throw std::runtime_error("Unmapping array attribute buffer returned GL_FALSE: Buffer contents corrupted");
        }

        glEnableVertexAttribArray(i);

    }
}
#endif

    
GPUMesh::~GPUMesh()
{
    if(copycount.isUnique()) {
        freeMesh();
    } 
}


void GPUMesh::initialize(const Mesh& m)
{


    vertexCount = m.numVerts;
    faceCount = m.numFaces;

#ifndef GL_ES_BUILD
    glGenVertexArrays(1, &vertex_array);
    
    if(  !vertex_array) {
        throw std::runtime_error("could not allocate VAO  ");
    }
#endif
    
    glGenBuffers(1, & indexbuffer);

    if(  !indexbuffer) {
        throw std::runtime_error("could not allocate VBO  ");
    }

    
#ifndef GL_ES_BUILD
    glBindVertexArray(vertex_array);
#endif

    //set the postions buffer
    for(unsigned int i = 0; i < m.attributes.size(); i++) {

        GLuint nb = 0;


        glGenBuffers(1, &nb);

        if(!nb)throw std::runtime_error("Could not allocate attribute buffer");

        vbos.push_back(nb);
        attributenames.push_back(m.attributes[i].name);

#ifdef GL_ES_BUILD
        attributecomponents.push_back(m.attributes[i].components);
#endif
        
        glBindBuffer(GL_ARRAY_BUFFER, nb);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.attributes[i].components*m.numVerts, &(m.attributes[i].getArray()[0]), GL_STATIC_DRAW);
        glVertexAttribPointer(i, m.attributes[i].components, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(i);

    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*3*m.numFaces, &(m.indexbuffer[0]), GL_STATIC_DRAW);

}


void GPUMesh::drawPoints(){
        //set the postions buffer
    for(unsigned int i = 0; i < vbos.size(); i++) {
            
        
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
            
        glVertexAttribPointer(i, attributecomponents[i], GL_FLOAT, GL_FALSE, 0, 0);
            
        //glEnableVertexAttribArray(i);
            
    }
        
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
        
    ///\todo crash here on quit.  need to shut down gl shit
    //bind buffers, set attribute pointers
    //glDrawElements(GL_TRIANGLES, 3*faceCount, GL_UNSIGNED_INT, 0);
    
    glDrawArrays(GL_POINTS, 0, vertexCount);
}
    
    
    
    void GPUMesh::drawLines(){
        //set the postions buffer
        for(unsigned int i = 0; i < vbos.size(); i++) {
            
            
            glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
            
            glVertexAttribPointer(i, attributecomponents[i], GL_FLOAT, GL_FALSE, 0, 0);
            
            //glEnableVertexAttribArray(i);
            
        }
        
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
        
        ///\todo crash here on quit.  need to shut down gl shit
        //bind buffers, set attribute pointers
        //glDrawElements(GL_TRIANGLES, 3*faceCount, GL_UNSIGNED_INT, 0);
        
        glDrawArrays(GL_LINES, 0, vertexCount);
    }
    
    
    
#ifndef GL_ES_BUILD

    
    void GPUMesh::push()
    {
        //bind the geometry we want to use
        glBindVertexArray(vertex_array);
        ///\todo 3???? points in tri
        glDrawElements(GL_TRIANGLES, 3*faceCount, GL_UNSIGNED_INT, 0);
        
    }

#else
    
    void GPUMesh::push()
    {
        
        //set the postions buffer
        for(unsigned int i = 0; i < vbos.size(); i++) {
            
            
            
            glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
           
            glVertexAttribPointer(i, attributecomponents[i], GL_FLOAT, GL_FALSE, 0, 0);
            
            glEnableVertexAttribArray(i);
            
        }
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

///\todo crash here on quit.  need to shut down gl shit
      //bind buffers, set attribute pointers
        glDrawElements(GL_TRIANGLES, 3*faceCount, GL_UNSIGNED_INT, 0);
        
    }
    
#endif
    
    
}
