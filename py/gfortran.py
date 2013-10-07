#!/usr/bin/env python
# encoding: utf-8
"""
gfortran_arrays.py

from libgfortran.h
Note: as currently implemented, this requires 64-bit object code.

FIXMEs
    handle offset properly
    handle more varieties of indexing
    handle slicing more completely, i.e. striding within larger arrays with offset


typedef struct dimension
{
  index_t _stride;
  index_t _lbound;
  index_t _ubound;
}
dimension;

#define GFC_ARRAY_DESCRIPTOR(r, type) \
struct {\
  type *data;\
  size_t offset;\
  index_t dtype;\
  dimension dim[r];\
}


See gfortran trans-types.{c,h} for all the guts and glory.

http://gcc.gnu.org/ml/gcc-patches/2007-04/msg00692.html
The dtype member is formatted as follows:
     rank = dtype & GFC_DTYPE_RANK_MASK // 3 bits
     type = (dtype & GFC_DTYPE_TYPE_MASK) >> GFC_DTYPE_TYPE_SHIFT // 3 bits
     size = dtype >> GFC_DTYPE_SIZE_SHIFT

/* FIXME: Increase to 15 for Fortran 2008. Also needs changes to
   GFC_DTYPE_RANK_MASK. See PR 36825.  */
#define GFC_MAX_DIMENSIONS 7

#define GFC_DTYPE_RANK_MASK 0x07
#define GFC_DTYPE_TYPE_SHIFT 3
#define GFC_DTYPE_TYPE_MASK 0x38
#define GFC_DTYPE_SIZE_SHIFT 6


Created by Keoni on 2010-11-23.
Copyright (c) 2010 University of Wisconsin SSEC. All rights reserved.
"""

import sys
import os
from ctypes import *
from numpy import ndarray, zeros
from numpy.ctypeslib import ndpointer, as_array, as_ctypes
from operator import mul

index_t = c_int64
size_t = c_int64

# from libgfortran.h
GFC_DTYPE_UNKNOWN = 0
GFC_DTYPE_INTEGER = 1
GFC_DTYPE_LOGICAL = 2
GFC_DTYPE_REAL = 3
GFC_DTYPE_COMPLEX = 4
GFC_DTYPE_DERIVED = 5
GFC_DTYPE_CHARACTER = 6

GFC_DTYPE_LIST = (  (GFC_DTYPE_INTEGER, c_int, c_int32, c_int64, c_int16),
                    (GFC_DTYPE_LOGICAL, c_bool),
                    (GFC_DTYPE_REAL, c_float, c_double),
                    (GFC_DTYPE_CHARACTER, c_char)
                    )

def _dtype_list_to_tab():
    "invert the above list to a type lookup"
    for kind in GFC_DTYPE_LIST:
        for ctype in kind[1:]:
            yield ctype, kind[0]
GFC_DTYPE_TAB = dict(_dtype_list_to_tab())

def _dtype_list_to_ctypes():
    "invert the above list to a type lookup"
    for kind in GFC_DTYPE_LIST:
        for ctype in kind[1:]:
            yield (kind[0], sizeof(ctype)), ctype

# given (gfortran-type-enum, size) give ctypes type
CTYPES_DTYPE_TAB = dict(_dtype_list_to_ctypes())



class dimension(Structure):
    _fields_ = [ ('stride', index_t),
                 ('lbound', index_t),
                 ('ubound', index_t) ]

    # create property accessor, eventually
    # we'll use this to make compatible implementations for other compilers
    @property
    def extent(self):
        return self.ubound + 1 - self.lbound

# def _base_ctype(ctarr, shape):
#     "find the base ctypes class for a multidimensional array found using npc.ctypeslib.as_ctypes"
#     t = ctarr._type_
#     for n in shape[1:]:
#         t = t._type_
#     return t

def array_descriptor(ctype, rank):
    """equivalent of template<dtype, rank> array_descriptor for ctypes
    result of calling this routine can be used within other ctypes Structures
    FUTURE: memoize this
    """
    assert(rank > 0)
    class descriptor_struct(Structure):
        _fields_ = [ ('mem', POINTER(ctype)),
                     ('offset', size_t),
                     ('dtype_rank', index_t, 3),
                     ('dtype_type', index_t, 3),
                     ('dtype_size', index_t, sizeof(index_t)*8-6),
                     ('dim', dimension * rank), ]

        def allocate(self, dims, _ctype=ctype, _rank=rank):
            "allocate this descriptor and return a numpy array wrapper"
            nelems = reduce(mul, dims, 1)
            buftype = _ctype * nelems
            data = buftype()
            rank = len(dims)
            assert(_rank==rank)
            self.mem = data
            self.dtype_rank = rank
            self.dtype_size = csize = sizeof(ctype)
            self.dtype_type = GFC_DTYPE_TAB.get(ctype, GFC_DTYPE_DERIVED)
            arr = ndarray(dtype=_ctype, buffer=data, order='F', shape=dims)
            axes = list(self.dim)
            for width, axis, stride in zip(dims, axes, arr.strides):
                axis.stride = stride / csize # numpy strides are in bytes
                axis.lbound = 1
                axis.ubound = width
            return arr

        def as_array(self, _ctype=ctype):
            "given that fortran has already allocated this array, provide an numpy wrapper"
            axes = list(self.dim)
            csize = sizeof(_ctype)
            strides = tuple([(ax.stride * csize) for ax in axes])
            dims = tuple([(ax.ubound - ax.lbound +1) for ax in axes])
            #print strides, dims
            nelems = reduce(mul, dims, 1)
            # print dims, nelems
            # FUTURE: handle offset value, also handle possibility that offset and stride information
            # may result in a mem block that's larger than nelems!
            data = cast(self.mem, POINTER(_ctype*nelems))
            arr = ndarray(dtype=_ctype, buffer=data.contents, strides=strides, shape=dims)
            return arr

        def from_array(self, arr, _ctype=ctype):
            "set up a descriptor from a compatible numpy array, preferably a fortran-order numpy array"
            dims = arr.shape
            rank = len(dims)
            nelems = reduce(mul, dims, 1)
            assert(len(self.dim) == rank)
            ctarr = as_ctypes(arr)
            # compatibility sanity check
            assert(arr.dtype == _ctype)
            # ctype = _base_ctype(ctarr, arr.shape)
            ct = cast(ctarr, type(self.mem))
            csize = sizeof(_ctype)
            self.mem = ct
            self.dtype_rank = rank
            self.dtype_size = csize
            self.dtype_type = GFC_DTYPE_TAB.get(_ctype, GFC_DTYPE_DERIVED)
            axes = list(self.dim)
            for width, axis, stride in zip(dims, axes, arr.strides):
                axis.stride = stride / csize
                axis.lbound = 1
                axis.ubound = width
        @staticmethod
        def with_array(arr):
            elf = descriptor_struct()
            elf.from_array(arr)
            return elf

    return descriptor_struct

class array_descriptor_header(Structure):
    "an array descriptor header with unknown dimension"
    _fields_ = [    ('mem', c_void_p),
                    ('offset', size_t),
                    ('dtype_rank', index_t, 3),
                    ('dtype_type', index_t, 3),
                    ('dtype_size', index_t, sizeof(index_t)*8-6) ]
                    # dimension structures trail but we don't know them until we read dtype

def from_pointer(vp):
    """given a c_void_p pointer to an arbitrary fortran-created array descriptor,
    return a pythonic descriptor with .as_array() etc
    """
    hdr = cast(vp, POINTER(array_descriptor_header))
    # find out what type array we have
    ctype = CTYPES_DTYPE_TAB[ (hdr.dtype_type, hdr.dtype_size) ]
    # recast as the proper descriptor
    desctype = array_descriptor( ctype, hdr.dtype_rank )
    hdr = cast(vp, POINTER(desctype))
    # now build a returnable array wrapper
    return hdr

class any_array(array_descriptor_header):
    "declare incoming arrays as POINTER(any_array) optionally for fortran-calls-python case"
    def as_array(self):
        # find out what type array we have
        ctype = CTYPES_DTYPE_TAB[ (self.dtype_type, self.dtype_size) ]
        # recast as the proper descriptor
        desctype = array_descriptor( ctype, self.dtype_rank )
        hdr = cast(self, desctype)
        # now build a returnable array wrapper
        return hdr.as_array()


def new_array(ctype, *dims):
    "preserve fortran array indexing in fortran, C array indexing in python"
    # first we create a descriptor type for an N-dimensional array of C's
    desctype = array_descriptor(ctype, len(dims))
    # then we make an instance of that descriptor, which we could pass to fortran
    desc = desctype()
    # then we allocate an array and update the descriptor to reflect it
    arr = desc.allocate(dims)
    return arr, desc, desctype


def mod_sym(module, symbol):
    "return mangled symbol for a F9X module"
    return '__' + module + '_MOD_' + symbol


class sunflower_samurai(Structure):
    _fields_ = [
        ('honor', c_float),
        ('vigilance', c_int),
        ('determination', c_double)
    ]

    def __str__(self):
        from pprint import pformat
        return '<honor: %s vigilance: %s determination: %s>' % (self.honor, self.vigilance, self.determination)

def main():
    py0 = cdll['./test_py.so']

    # test case where python-creates-array, fortran-fills, python-reads
    pyfunc = py0[mod_sym('py0', 'py')]
    H, W = 5, 4
    arr, ardesc, desctype = new_array(c_float, H, W)
    pyfunc.argtypes = [POINTER(desctype)]
    pyfunc(ardesc)
    print arr

    # test case where python-creates-null-pointer, fortran-allocates-and-fills, python-reads
    myfunc = py0[mod_sym('py0', 'py_takes_my_array')]
    myfunc.argtypes = [POINTER(desctype)]
    lz = desctype() # create landing zone
    myfunc(lz)
    # get numpy wrapper backed by fortran-allocated memory
    arr = lz.as_array()
    print arr

    # test from_array; create a numpy array, put it in a compatible descriptor, call fortran
    gnarr = zeros( (9,4), dtype = c_float )
    desc = desctype()
    desc.from_array(gnarr)
    pyfunc(desc)
    print gnarr

    struct_array_type = array_descriptor(sunflower_samurai, 1)
    structs = struct_array_type()
    prsa = py0[mod_sym('py0', 'py_receives_struct_array')]
    prsa.argtypes = [POINTER(struct_array_type)]

    prsa(structs)
    arr = structs.as_array()
    for elem in arr:
        print elem
    print 'dtype is %s' % structs.dtype_type

# does not work consistently - indexing 0 vs 1 / segfaults / dtype assumptions?
    #destructs = struct_array_type()
    #arr = destructs.allocate((4,))
    #pmsa = py0[mod_sym('py0', 'py_makes_struct_array')]
    #pmsa.argtypes = [POINTER(struct_array_type)]
    #pmsa(destructs)
    #for elem in arr:
    #    print elem





if __name__ == '__main__':
    main()
