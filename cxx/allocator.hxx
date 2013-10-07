
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


