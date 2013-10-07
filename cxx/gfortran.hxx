#ifndef HXX_GFORTRAN
#define HXX_GFORTRAN
//
// gfortran.hxx
// 
// array descriptor interfaces GFortran <-> C++
//
// Purpose: provide immutable data structures describing arrays in fortran, or describing arrays to send to fortran.

//

#include <cmath>
#include <complex>
#include <stdint.h>


#ifndef __GNUC__ 
#error GNU compiler collection is default - set USING_INTEL_FORTRAN OR USING_PGI_FORTRAN
#endif


#define FORTRAN(name) name##_   
#define MODFORT(module,name) __##module##_MOD_##name

namespace gfortran
{

typedef int64_t f_index_t;

//
// type codes
//

enum _gfc_dtype {
    GFC_DTYPE_UNKNOWN = 0,
    GFC_DTYPE_INTEGER = 1,
    GFC_DTYPE_LOGICAL = 2,
    GFC_DTYPE_REAL = 3,
    GFC_DTYPE_COMPLEX = 4,
    GFC_DTYPE_DERIVED = 5,
    GFC_DTYPE_CHARACTER = 6
};


// create a compile-time lookup table
template <typename T>
struct _gfc_dtype_tab
{
    enum { GFC_DTYPE = GFC_DTYPE_UNKNOWN };
};

#define CTYPE_FTYPE(CTYPE,GFC) \
template <> \
struct _gfc_dtype_tab<CTYPE> \
{ enum { GFC_DTYPE = GFC }; };

// create a compile-time lookup accessible via _gfc_dtype_tab<ctype>.GFC_DTYPE
CTYPE_FTYPE(char, GFC_DTYPE_CHARACTER)
CTYPE_FTYPE(int8_t, GFC_DTYPE_INTEGER)
CTYPE_FTYPE(int16_t, GFC_DTYPE_INTEGER)
CTYPE_FTYPE(int32_t, GFC_DTYPE_INTEGER)
CTYPE_FTYPE(int64_t, GFC_DTYPE_INTEGER)
CTYPE_FTYPE(float, GFC_DTYPE_REAL)
CTYPE_FTYPE(double, GFC_DTYPE_REAL)
CTYPE_FTYPE(bool, GFC_DTYPE_LOGICAL)
CTYPE_FTYPE(std::complex<float>, GFC_DTYPE_COMPLEX)
CTYPE_FTYPE(std::complex<double>, GFC_DTYPE_COMPLEX)


//
// array descriptors
//

class dimension
/// A basic compile-time abstraction for a dimension providing uniform fields.
/// This way we can implement the same interface for multiple compilers.
{
private:
    const f_index_t _stride;  /// in elements, not bytes
    const f_index_t _lbound;  /// inclusive, lower bound
    const f_index_t _ubound;  /// inclusive, upper bound

    dimension(f_index_t lbound, f_index_t ubound, f_index_t stride):
        _stride(stride), 
        _lbound(lbound), 
        _ubound(ubound)
    {
    }

public:
    dimension():
        _stride(0), 
        _lbound(0), 
        _ubound(0)
    {
    }

    static dimension with_lbound_ubound_stride(f_index_t lbound, f_index_t ubound, f_index_t stride)
    {
        return dimension(lbound, ubound, stride);
    }

    static dimension with_lbound_extent_stride(f_index_t lbound, f_index_t extent, f_index_t stride)
    {
        return dimension(lbound, lbound + extent - 1, stride);
    }

    inline f_index_t stride() const
    {
        return _stride;
    }

    inline f_index_t lbound() const
    {
        return _lbound;
    }

    inline f_index_t ubound() const
    {
        return _ubound;
    }

    inline f_index_t extent() const
    {
        return _ubound + 1 - _lbound;
    }
};



template <typename ValueType, unsigned Dimensions>
class array_descriptor
{
public:  // FIXME
    ValueType *mem;
    size_t offset;
    f_index_t dtype_rank : 3;
    f_index_t dtype_type : 3;
    f_index_t dtype_size : 58; 
    dimension dim[Dimensions];        

public:
    array_descriptor():
        mem(0),
        offset(0),
        dtype_rank(0),
        dtype_type(0),
        dtype_size(0)
    {        
    }

    array_descriptor(ValueType *data, const unsigned extents[], const int base[] = 0):
        mem(data),
        dtype_rank(Dimensions),
        dtype_type(_gfc_dtype_tab<ValueType>().GFC_DTYPE),
        dtype_size(sizeof(ValueType))
    {
        // assume that last index is contiguous
        f_index_t stride = 1;
        for(int d=Dimensions-1; d>=0; --d)
        {
            dim[d] = dimension::with_lbound_extent_stride(base? base[d] : 1, extents[d], stride);
            stride *= extents[d];
        }
    }
};



// FUTURE: DRY violation
template <typename ValueType, unsigned Dimensions>
class const_array_descriptor
{
private:
    const ValueType *mem;
    size_t offset;
    f_index_t dtype_rank : 3;
    f_index_t dtype_type : 3;
    f_index_t dtype_size : 58; 
    dimension dim[Dimensions];        

public:
    const_array_descriptor(const ValueType *data, const unsigned extents[], const int base[] = 0):
        mem(data),
        dtype_rank(Dimensions),
        dtype_type(_gfc_dtype_tab<ValueType>().GFC_DTYPE),
        dtype_size(sizeof(ValueType))
    {
        // assume that last index is contiguous
        f_index_t stride = 1;
        for(int d=Dimensions-1; d>=0; --d)
        {
            dim[d] = dimension::with_lbound_extent_stride(base? base[d] : 1, extents[d], stride);
            stride *= extents[d];
        }
    }
};


}; // namespace gfortran

#endif // HXX_GFORTRAN