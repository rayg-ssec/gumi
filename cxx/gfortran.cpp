#include <iostream>
#include "boost/shared_ptr.hpp"
#include "boost/multi_array.hpp"
#include "gfortran.hpp"

using namespace std;
using namespace boost;
using namespace gfortran;

typedef array_descriptor<float, 2> real4_2d;

int main(int argc, char *argv[])
{
    real4_2d fromcpp;
    unsigned size[] = {4,5};
    real4_2d::array_t arr(fromcpp.create(size));    
    arr[2][2] = 7.0;
    shared_ptr<real4_2d::array_t> pmatey(fromcpp.allocate(size));
    real4_2d::array_t &matey(*pmatey);
    matey[2][2] = 8.975;
    
    return 0;
}

