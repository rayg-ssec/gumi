gumi
====

Grand Unified Matrix Interface

This project is about providing, at the minimum, library interoperability between recent NumPy, gfortran, and C++ Eigen.
The interoperability should not require re-architecting existing code.
Phase 1 is complete when these three run-times can coexist in one executable via boost::python.

Phase 2 will include a C interface, likely using macros for direct array access.

Phase 3 should include data structure interoperability with ctypes, gfortran, and C++. 
Thatmay require cross-compiling of structure definitions in order to be efficient.

