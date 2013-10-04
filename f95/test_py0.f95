module py0
! gfortran-fsf-4.6 -bundle -o test_py.so test_py0.f95


type :: sunflower_samurai
    SEQUENCE
    REAL*4 :: honor
    INTEGER*4 :: vigilance
    REAL*8 :: determination
end type

contains

subroutine py(aout)
    REAL*4, DIMENSION(:,:) :: aout
    INTEGER :: row,col
    PRINT *, SIZE(aout,1), SIZE(aout,2)
    do row=1,SIZE(aout,1)
        do col=1,SIZE(aout,2)
            aout(row,col) = row*2 + col
        end do
    end do
end subroutine

subroutine py_takes_my_array(aout)
    REAL*4, DIMENSION(:,:), POINTER :: aout
    INTEGER :: row,col
    ALLOCATE(aout(6,8))
    PRINT *, SIZE(aout,1), SIZE(aout,2)
    do row=1,SIZE(aout,1)
        do col=1,SIZE(aout,2)
            aout(row,col) = row*2 + col
        end do
    end do
end subroutine

subroutine py_receives_struct_array(aout)
    type(sunflower_samurai), dimension(:), POINTER :: aout
    INTEGER :: row
    allocate(aout(6))
    do row=1,size(aout,1)
        aout(row)%vigilance = row
        aout(row)%honor = sqrt(1.4142+row)
        aout(row)%determination = 90329032.853
    end do
end subroutine

subroutine py_makes_struct_array(aout)
! this part doesn't work consistently since we don't know the type code; there are also some 0 vs 1 indexing issues
    type(sunflower_samurai), dimension(:), POINTER :: aout
    INTEGER :: row
    do row=1,size(aout,1)
        aout(row)%vigilance = aout(row)%vigilance * 2
        aout(row)%determination = 90329032.5
        aout(row)%honor = -row
    end do
end subroutine

end module
