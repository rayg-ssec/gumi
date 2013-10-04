// g++ -c -Wall -I../include test_descriptors.cxx 

#include <iostream>
#include "descriptors.hxx"

using namespace std;

typedef gumi::fortran::array_descriptor<int, 1> f_int1d;
typedef gumi::fortran::array_descriptor<int, 2> f_int2d;
typedef gumi::fortran::array_descriptor<int, 3> f_int3d;

extern "C"
void FORTRAN(inspect2i)( const f_int2d *A )
{
    cout << "::inspect2i called\n";
    cout << A->data << endl;
    cout << A->dim[0] << endl;
    cout << A->dim[1] << endl;
}

// a fortran subroutine that returns an array
extern "C" 
void FORTRAN(f_returns_array)( f_int2d *A );

// fortran calls C++ calls fortran
extern "C"
void FORTRAN(bounce2i)( )
{
    f_int2d Q;
    FORTRAN(f_returns_array)(&Q);
    cout << "returned array from bounce2i\n";
    FORTRAN(inspect2i)(&Q);
    cout << "53703 == " << Q.data[0] << endl;
}

