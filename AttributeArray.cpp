#include "AttributeArray.h"

//constructor to construct an attribute array.
///\todo should we just reserve the memory?
AttributeArray::AttributeArray(std::string n, int comps,int vert)
:
arrayPtr(new std::vector<float>()),
//array(*arrayPtr),
name(n),
//vertices(vert),
components(comps)
{
    (*arrayPtr).reserve(comps*vert);
}
///\todo why do we have the vertex count but not the numebr of components?  should be because vert ct is constant for a model
AttributeArray::AttributeArray( int vert, std::istream& i)
:
arrayPtr(new std::vector<float>()),
//array(*arrayPtr),
name(""),
//vertices(vert),///\todo
components(0)
{
    read(vert,i);
}

void AttributeArray::read(int vertices,std::istream& in)
{

     std::vector<float>& array = *arrayPtr;

    unsigned int namelength;

    in.read((char*) &namelength, sizeof(unsigned int));

    std::vector<char> c_str(namelength,' ');// = new char[namelength]; ///\todo unnecessary?

    in.read((char*)&(c_str[0]), sizeof(char)*namelength);

    name = std::string(&c_str[0]);

    in.read((char*)&components, sizeof(unsigned int));

    array.resize(components*vertices);// = new float[components * vertices];

    in.read((char*) &array[0], vertices*components*sizeof(float));

    //log<<"loading attribute:"<<name<<" with "<<components<<" components per vertex"<<std::endl;

}


void AttributeArray::write(std::ostream& out)const
{

    std::vector<float>& array = *arrayPtr;

    unsigned int namelength = name.size()+1;
    out.write((char*)&namelength, sizeof(unsigned int));
    out.write((const char*)name.c_str(), sizeof(char)*(namelength));
    out.write((const char*)&components, sizeof(unsigned int));
    out.write((char*)&array[0], numVerts()*components*sizeof(float));
}


