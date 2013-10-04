#ifndef HXX_EIGENFORT
#define HXX_EIGENFORT

#include "descriptors.hxx"

namespace gumi {

template < typename Descriptor >
class FortranMatrix
{
    // obtain name and rank from descriptor
    typedef typename Descriptor::data_t data_t;
    enum { rank_v = Descriptor::rank_v };
    
    MatrixFromFortran( array_descriptor<
    
};


template
< 
    typename Elem, 
    unsigned Dimensions
>
class MatrixFromFortran


}


#endif
   