#ifndef HXX_GUMI
#define HXX_GUMI

#include <string>

namespace gumi { 

typedef void *token;
typedef std::string name;

template <typename MatrixT>
class const_share 
{

  public:
    const_share(const Matrix &T, const name &)
    {
    }
    
    token as_token() const;
};

template <typename MatrixT>
class share 
{
    share(MatrixT &T)
    {
    }    
    
    token as_token() const;
};

template <typename MatrixT>
class const_access // something that is const_shareed
{ 
    const_access( const name & )
    {
    }
    
    const_access( token )
    {
    }

    operator MatrixT() const
    {
    }
};


template <typename MatrixT>
class access // something that is shared
{ 
    access( const name & )
    {
    }
    
    access( token )
    {
    }
    
    operator const MatrixT() const
    {
    }
};



  
} // namespace

#endif // HXX_GUMI