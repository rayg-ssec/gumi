#include <stdint.h>
#include <cstdlib>
// #include "gumi.hxx"

//
// fortran calls
//  these are interface'd in gumi.f95 / gumi.mod but implemented here
//

const size_t NAME_LEN=1024;



// FUTURE: move these to their own header file, one for each compiler, 
// and converge on a set of common calls that mask off compiler-isms

// g f o r t r a n 4.5



extern "C"
void MODFORT(gumi,gumi_alloc_f1)
(
    fortran_array_descriptor<float, 1> *array, 
    void *ctxt, 
    const char name[NAME_LEN], 
    const int32_t *shape, 
    const int32_t *options 
)
{
    
}
