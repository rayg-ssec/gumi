#ifndef HXX_GFORTRAN
#define HXX_GFORTRAN
//
// gfortran.hxx
// 
// array descriptor interfaces Fortran9X <-> C++
//

namespace gfortran
{

#include <complex>
#include <stdint.h>

#include "boost/array.hpp"
#include "boost/multi_array.hpp"
#include "boost/shared_ptr.hpp"

typedef int64_t index_t;


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
// an allocator that borrows a pre-existing memory block from fortran
//

template<typename ValueType>
class borrowing
{
    ValueType *_mem;
    bool _used;
public:
    typedef ValueType value_type;
    typedef ValueType *pointer;
    typedef ValueType &reference;
    typedef const ValueType *const_pointer;
    typedef const ValueType &const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    borrowing(ValueType *start) throw(): _mem(start), _used(false) { }
    ~borrowing() { }
    
    pointer address(reference val) const { return &val; }
    const_pointer address(const_reference val) const { return &val; }


    pointer allocate(size_type count, const void* = 0)
    {
        // FIXME: throw exception if _used is already true
        _used = true;
        return static_cast<pointer>(_mem);
    }

    void deallocate(pointer ptr, size_type)
    {
        // herp. derp. hurrrh. you want what?
    }

    void construct(pointer ptr, const ValueType& val)
    {
        new ((void *)ptr) ValueType(val);
    }

    void destroy(pointer ptr)
    {
        ptr->ValueType::~ValueType();
    }


    template<class Other>
    struct rebind
    {
        typedef borrowing<Other> other;
    };

    template<class Other>
    borrowing(const borrowing<Other>&x) throw(): _mem(x._mem), _used(x._used) {}

/* 
    template<class Other>
    borrowing& operator=(const borrowing<Other>&) { return *this; }
 */


    size_type max_size() const throw() 
    { 
        return 1; 
    }
};




//
// array descriptors
//

struct descriptor_dimension
{
    index_t stride;
    index_t lbound;
    index_t ubound;
};


template <typename ValueType, unsigned Dimensions>
class array_descriptor
{
  public:
    ValueType *mem;
    size_t offset;
    index_t dtype_rank : 3;
    index_t dtype_type : 3;
    index_t dtype_size : 58; 
    descriptor_dimension dim[Dimensions];
    
    typedef boost::multi_array<ValueType, Dimensions> boost_array_t;
    typedef boost::array<typename boost_array_t::index, Dimensions> boost_shape_t;
    
    // default array_t to present is the boost multiarray
    typedef boost_array_t array_t;
    
    // allocate an array, with memory owned by C++, but descriptor points at it
    boost::shared_ptr<boost_array_t> allocate(const unsigned *dims)
    {
        boost_shape_t shape(_with_shape(dims));
        boost_array_t *p_array = new boost_array_t(shape, boost::fortran_storage_order());
        
        _copy_description_of(*p_array);
        
        return boost::shared_ptr<boost_array_t>(p_array);
    }

    boost_array_t create(const unsigned *dims)
    {
        boost_shape_t shape(_with_shape(dims));
        boost_array_t zult(shape, boost::fortran_storage_order());        
        _copy_description_of(zult);
        return zult;
    }

    
    void from_array(boost_array_t &arr)
    {
        _copy_description_of(arr);
    }
    
    // return a multi_array wrapper for a described fortran array
    boost_array_t as_array() const
    {
        int shape[Dimensions];
        int stride[Dimensions];
        for(unsigned i=0; i<Dimensions; ++i)
        {
            shape[i] = dim[i].ubound - dim[i].lbound + 1;
            stride[i] = dim[i].stride;
        }
        
        boost_array_t zult( _with_shape(shape, shape+Dimensions), 
                            boost::fortran_storage_order(), 
                            borrowing<ValueType>(mem) );
    }
    
  private:

    template<typename S>
    static boost_shape_t _with_shape(const S *start)
    {
        boost_shape_t zult;        
        std::copy(start, start+Dimensions, zult.begin());
        return zult;
    }
  
    void _copy_description_of(boost_array_t &arr)
    {
        typename boost_array_t::element *data(arr.data());        
        typename boost_array_t::element *origin(arr.origin());        
        const typename boost_array_t::index *strides(arr.strides());
        const typename boost_array_t::size_type *shape(arr.shape());
        
        mem = data;
        offset = sizeof(ValueType) * (origin - data); // FIXME: is bytes correct?
        dtype_rank = Dimensions;
        dtype_type = _gfc_dtype_tab<ValueType>::GFC_DTYPE;
        dtype_size = sizeof(ValueType);
        for(unsigned d=0; d<Dimensions; ++d)
        {
            dim[d].stride = strides[d];
            dim[d].lbound = 1;
            dim[d].ubound = shape[d];
        }
    }
    
};


}; // namespace gfortran

#endif // HXX_GFORTRAN