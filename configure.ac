AC_INIT([libwtk-sdl2], [0.0.1], [muesli4@gmail.com])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([-Wall foreign])

AC_SUBST([POSIX_LIB_NAME], ["wtk-sdl2"])
AC_SUBST([AM_CXXFLAGS], ["-std=c++17 -std=gnu++17 -Wall"])

AC_PROG_CXX
AM_PROG_AR

AM_PROG_LIBTOOL

PKG_CHECK_MODULES(sdl2, sdl2)
PKG_CHECK_MODULES(SDL2_ttf, SDL2_ttf)
PKG_CHECK_MODULES(SDL2_image, SDL2_image)

# AX_BOOST_BASE([1.35.0],,[AC_MSG_ERROR([boost was not found])])

AC_CONFIG_FILES([Makefile src/Makefile include/Makefile libwtk-sdl2.pc])

#AM_GNU_GETTEXT([external])
#AM_GNU_GETTEXT_VERSION([0.19.7])

# generate configure script to produce Makefiles
AC_OUTPUT
