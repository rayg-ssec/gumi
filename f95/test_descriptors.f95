! gfortran -o test_descriptors -Wall test_descriptors.f95 ../cxx/test_descriptors.o -lstdc++

! C++ calls this function which returns an array as an inout
! yes it has a memory leak
subroutine f_returns_array(Q)
    integer, dimension(:,:), pointer :: Q
    allocate( Q(9,7) )
    Q = 53703
end subroutine


program margorp
    ! we call this C++ function with a fortran-constructed array
    interface
        subroutine inspect2i(Q)
            integer, dimension(:,:), intent(in) :: Q
        end subroutine
    end interface

    ! we call this C++ function, it calls f_returns_array
    interface
        subroutine bounce2i()
        end subroutine
    end interface
    

    integer, dimension(4,5) :: towel

    call inspect2i(towel)
    call bounce2i()
    
end program

