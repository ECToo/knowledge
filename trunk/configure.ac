AC_INIT(src/md5.cpp)
AM_INIT_AUTOMAKE(libknowledge, 0.1)
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4])
AM_SANITY_CHECK
CFLAGS=""
CXXFLAGS="-Wall -O3 -g"
AC_PATH_PROG(SDL_CONFIG, sdl-config)
if test -n "$SDL_CONFIG"; then
    LIBS="$LIBS `$SDL_CONFIG --libs`"
	     CXXFLAGS="$CXXFLAGS `$SDL_CONFIG --cflags`"
		  CPPFLAGS="$CXXFLAGS"
		  fi
		  AC_CHECK_LIB([SDL], [SDL_Init], ,
		  AC_MSG_ERROR([ *** Unable to find SDL library (http://www.libsdl.org/)]))
		  AC_CHECK_HEADERS([SDL.h], ,
		  AC_MSG_ERROR([ *** SDL library found but cannot find headers (http://www.libsdl.org/)]))
AC_PATH_PROG(ODE_CONFIG, ode-config)
if test -n "$ODE_CONFIG"; then
    LIBS="$LIBS `$ODE_CONFIG --libs`"
	 CXXFLAGS="$CXXFLAGS `$ODE_CONFIG --cflags`"
fi
		  
AC_CHECK_LIB([GLEW], [glewInit], ,
AC_MSG_ERROR([ *** Unable to find GLEW library (http://glew.sourceforge.net)]))
AC_CHECK_LIB([freeimage], [FreeImage_DeInitialise], ,
AC_MSG_ERROR([ *** Unable to find FreeImage library (http://freeimage.sourceforge.net)]))
AC_OUTPUT(Makefile src/Makefile \
			 include/Makefile \
			 examples/Makefile \
			 examples/bsp/Makefile \
			 examples/model/Makefile \
			 examples/texture/Makefile \
			 examples/physics/Makefile \
			 examples/lightmap/Makefile \
			 examples/particles/Makefile \
          knowledge.pc)
