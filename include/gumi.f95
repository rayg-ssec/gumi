module gumi

use iso_c_binding
implicit none

interface 
    subroutine gumi_alloc_f1(array, ctxt, name, shape, options)
        use iso_c_binding
        real*4, dimension(:), pointer :: array
        type(c_ptr), value :: ctxt
        character(len=1024) :: name
        integer :: shape
        integer :: options
    end subroutine

    subroutine gumi_alloc_f2(array, ctxt, name, shape, options)
        use iso_c_binding
        real*4, dimension(:,:), pointer :: array
        type(c_ptr), value :: ctxt
        character(len=1024), intent(in) :: name
        integer, dimension(:), intent(in) :: shape
        integer :: options
    end subroutine

    subroutine gumi_alloc_i1(array, ctxt, name, shape, options)
        use iso_c_binding
        integer*4, dimension(:), pointer :: array
        type(c_ptr), value :: ctxt
        character(len=1024) :: name
        integer :: shape
        integer :: options
    end subroutine

    subroutine gumi_alloc_i2(array, ctxt, name, shape, options)
        use iso_c_binding
        integer*4, dimension(:,:), pointer :: array
        type(c_ptr), value :: ctxt
        character(len=1024), intent(in) :: name
        integer, dimension(:), intent(in) :: shape
        integer :: options
    end subroutine
end interface



interface 
    subroutine gumi_obtain_f1(array, ctxt, name, options)
        use iso_c_binding
        real*4, dimension(:), pointer :: array
        type(c_ptr), value :: ctxt
        character(len=1024), intent(in) :: name
        integer :: options
    end subroutine

    subroutine gumi_obtain_f2(array, ctxt, name, options)
        use iso_c_binding
        real*4, dimension(:,:), pointer :: array
        type(c_ptr), value :: ctxt
        character(len=1024), intent(in) :: name
        integer :: options
    end subroutine

    subroutine gumi_obtain_i1(array, ctxt, name, options)
        use iso_c_binding
        integer*4, dimension(:), pointer :: array
        type(c_ptr), value :: ctxt
        character(len=1024), intent(in) :: name
        integer :: options
    end subroutine

    subroutine gumi_obtain_i2(array, ctxt, name, options)
        use iso_c_binding
        integer*4, dimension(:,:), pointer :: array
        type(c_ptr), value :: ctxt
        character(len=1024), intent(in) :: name
        integer :: options
    end subroutine

end interface


interface 
    subroutine gumi_release_f1(array)
        real*4, dimension(:), pointer :: array
    end subroutine

    subroutine gumi_release_f2(array)
        real*4, dimension(:,:), pointer :: array
    end subroutine

    subroutine gumi_release_i1(array)
        integer*4, dimension(:), pointer :: array
    end subroutine

    subroutine gumi_release_i2(array)
        integer*4, dimension(:,:), pointer :: array
    end subroutine
end interface

interface
    function gumi_path(ctxt, path) result(newctxt)
        use iso_c_binding
        type(c_ptr) :: newctxt
        type(c_ptr), value :: ctxt
        character(len=1024), intent(in) :: path
    end function
end interface


end module