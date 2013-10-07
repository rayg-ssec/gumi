#ifndef HXX_GUMI_BOOST
#define HXX_GUMI_BOOST

#include <vector>
#include "boost/multiarray"
#include "gfortran.hxx"


using namespace gfortran;

template<typename ValueType, unsigned Dimensions>
class GfortranArray: public array_descriptor<ValueType, Dimensions>
{
  public:

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
            dim[d] = dimension::with_lbound_ubound_stride(1, shape[d], strides[d]);
        }
    }
};

#endif // HXX_GUMI_BOOST
