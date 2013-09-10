#ifndef VIRTUOSO_ATTRIBUTEARRAY_MESH_H
#define VIRTUOSO_ATTRIBUTEARRAY_MESH_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

//#include <se/core/CopyCounter.h>
#include <sstream>
#include <memory>

#include <Eigen/core>

#include <stdexcept>

#include <array>
//#include <Eigen/vector>

///\todo should this be a IS-A with the vector?
///\todo integer indices
class AttributeArray
{

    ///\todo verify this works: this should improve load times
    std::shared_ptr< std::vector<float> > arrayPtr;

    public:

    ///\todo copy construction bullsh
    //std::vector<float>& array; //reference goes to vector.  vector itself is never overwritten even if memory buffer is

    ///:) works for me, const nazi chris!
    ///\todo get rid of most of the getters and setters because fuck that... this one is syntactic sugar
    std::vector<float>& getArray() {return *arrayPtr;}
    const std::vector<float>& getArray()const{return *arrayPtr;}

    ///\todo maybe factor this all out, into parent class, and have human readable text header for models?
    std::string name; //name of attribute array
   // unsigned int vertices; //number of vertices in array
    unsigned int components; //number of components per vertex

    //constructor to construct an attribute array.  does not allocate any memory yet
    AttributeArray(std::string n, int comps,int vert=0);

    AttributeArray(int vert, std::istream& i);

    void read(int vert,std::istream& in);

    void write(std::ostream& out)const;

    bool operator==(const std::string& str)const{
        return str == name;
    }

    unsigned int numVerts()const{///\todo this div should be ok
        return getArray().size() / components;
    }

    bool dataValid()const{///has to be exactly along component borders
        return !(getArray().size() % components);
    }

    void reserve(unsigned int vertsToReserve){
        getArray().reserve(vertsToReserve * components);
    }

    template <int x>
    struct Inserter{

        std::shared_ptr<std::vector<float> > ptr;///\todo this CANT be right???!?!?!?
        unsigned int components;



        void insertVec(const Eigen::Matrix<float, x,1>& in){

            unsigned int oldSize = ptr->size();

            ptr->resize( oldSize + components );

            Eigen::Map<Eigen::Matrix<float,  x,1> > map( & ((*ptr)[oldSize]) );

            map = in;


        }

/*
         void insertVec(const Eigen::Matrix<float,1,x>& in){

            unsigned int oldSize = ptr->size();

            ptr->resize( oldSize + components );

            Eigen::Map<Eigen::Matrix<float, 1, x> > map( & ((*ptr)[oldSize]) );

            map = in;

        }*/


/*
        template <typename... Args>
        void insert(Args... args){

            insertVec( {(static_cast<float>(args))...} );

        }
*/
/*
       // template <class std::initializer_list<float> >
        inline void insert(const std::initializer_list<float>& abc)
        {
            insertVec(Eigen::Vector3f(abc));
        }

*/
        void vertex(const Eigen::Matrix<float,  x, 1>& in){
           insertVec(in);
        }

        Inserter(AttributeArray& arr)
        :ptr(arr.arrayPtr),
        components(arr.components)
        {

            if(components != x){
                throw std::runtime_error("MISMATCH IN ATTRIBUTE ARRAY AND ARG CT");
            }

        }


    };






};

/*
    template<>
    template<>
    inline void AttributeArray::Inserter<3>::insert<float... entries>(entries... in){

      //  std::array<float,x>{{args...}};
        //insertEig({{args...} });
    }
*/


#endif
