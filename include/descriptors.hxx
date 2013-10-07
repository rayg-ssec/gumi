#ifndef HXX_DESCRIPTORS
#define HXX_DESCRIPTORS

// gumi::fortran_dimension : a struct describing the layout of a given array dimension
// gumi::fortran_array_descriptor< element-type, number-of-dimensions > : struct template
#include <iostream>

namespace gumi {

namespace fortran { 

#if defined(USING_INTEL_FORTRAN)

#error ifort not yet supported

#elseif defined(USING_PGI_FORTRAN)

#error PGI not yet supported

#else // assume gfortran

#ifndef __GNUC__ 
#error GNU compiler collection is default - set USING_INTEL_FORTRAN OR USING_PGI_FORTRAN
#endif

#define FORTRAN(name) name##_   
#define MODFORT(module,name) __##module##_MOD_##name

typedef long fortran_index_t;
typedef long fortran_size_t;

struct array_dimension
{
    fortran_index_t stride_, lbound_, ubound_;
    
    inline fortran_index_t stride() const { return stride_; }
    inline void set_stride(fortran_index_t newstride) { stride_ = newstride; }
        
    inline fortran_index_t lbound() const { return lbound_; }
    inline void set_lbound(fortran_index_t newlbound) { lbound_ = newlbound; }

    inline fortran_index_t ubound() const { return ubound_; }
    inline void set_ubound(fortran_index_t newubound) { ubound_ = newubound; }

    inline fortran_size_t extent() const { return fortran_size_t(ubound_ + 1 - lbound_); }
    inline void set_extent(fortran_size_t newextent) { ubound_ = lbound_ + newextent - 1; }
};


inline std::ostream &operator<<(std::ostream &out, const array_dimension &dim )
{
    out << "<gfort_dim stride " << dim.stride_ 
        << " lbound " << dim.lbound_
        << " ubound " << dim.ubound_ 
        << ">";
    return out;
}

                       
template
< 
    typename Element, 
    unsigned DIMENSIONS 
>
struct array_descriptor
{
    // fields per libgfortran.h
    Element *data;
    fortran_size_t offset;
    fortran_index_t dtype;
    array_dimension dim[DIMENSIONS];
    
    // compile-time class constants usable by other templates
    typedef Element data_t;
    enum { rank_v=DIMENSIONS };    
    
    inline void print( std::ostream &out )
    {
        out << "<gfort_array" << DIMENSIONS 
            << " data " << data
            << " offset " << offset 
            << " dtype " << dtype << ' ';
        for( unsigned dex = 0; dex < DIMENSIONS; ++dex )
            out << dim[dex] << ' ';
    }
};

template< typename Desc >
inline std::ostream &operator<<(std::ostream &out, const Desc &dim )
{
    dim.print(out);
    return out;
}

#endif /* compiler type */
}; // namespace gumi::fortran

}; // namespace gumi

#endif /* HXX_DESCRIPTORS */