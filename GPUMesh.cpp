#include "StaticMesh.h"
#include <stdexcept>

GLuint& GPUMesh::operator[](const std::string& in){
    std::vector<std::string>::iterator it =
    std::find(attributenames.begin(), attributenames.end(),in);

    if(it == attributenames.end()){
        throw std::runtime_error("attribute not present");///\todo throw name too
    }

    int index =  std::distance(attributenames.begin(), it);

    return vbos[index];
}


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


GPUMesh::~GPUMesh()
{

    if(copycount.isUnique()) {

        ///\todo test that they exist first
        glDeleteBuffers(vbos.size(), &vbos[0]);
        glDeleteVertexArrays(1, & vertex_array);
        glDeleteBuffers(1, & indexbuffer);
    }
}

void GPUMesh::push()
{
    //bind the geometry we want to use
    glBindVertexArray(vertex_array);

    glDrawElements(GL_TRIANGLES, 3*faceCount, GL_UNSIGNED_INT, 0);

}


void GPUMesh::initialize(const Mesh& m/*, bool adj*/)//:adjacency(adj)
{

  //  modelingMatrix = Eigen::Matrix4f::Identity();
  //  normalMatrix = Eigen::Matrix3f::Identity();

    vertexCount = m.numVerts;
    faceCount = m.numFaces;

    glGenVertexArrays(1, &vertex_array);
    glGenBuffers(1, & indexbuffer);

    if(!vertex_array || !indexbuffer) {
        throw std::runtime_error("could not allocate VBO or vertex array");
    }

    glBindVertexArray(vertex_array);


    //set the postions buffer
    for(unsigned int i = 0; i < m.attributes.size(); i++) {

        GLuint nb = 0;


        glGenBuffers(1, &nb);

        if(!nb)throw std::runtime_error("Could not allocate attribute buffer");

        // attributenames.push_back(m.attributes[i].)

        vbos.push_back(nb);
        attributenames.push_back(m.attributes[i].name);

        std::cout<<"m.attributes[i].name:"<<m.attributes[i].name<<std::endl;

        glBindBuffer(GL_ARRAY_BUFFER, nb);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.attributes[i].components*m.numVerts, &(m.attributes[i].getArray()[0]), GL_STATIC_DRAW);
        glVertexAttribPointer(i, m.attributes[i].components, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(i);

    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*3*m.numFaces, &(m.indexbuffer[0]), GL_STATIC_DRAW);
//}
}