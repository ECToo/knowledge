AC_INIT(src/md5.cpp)
AM_INIT_AUTOMAKE(knowledge, 0.1)
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4])
AM_SANITY_CHECK
CFLAGS=""
CXXFLAGS="-Wall -O2 -g -std=c++0x"
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

AC_CHECK_LIB([GLEW], [glewInit], ,
AC_MSG_ERROR([ *** Unable to find GLEW library (http://glew.sourceforge.net)]))
AC_CHECK_LIB([freeimage], [FreeImage_DeInitialise], ,
AC_MSG_ERROR([ *** Unable to find FreeImage library (http://freeimage.sourceforge.net)]))
AC_MSG_CHECKING(Support for SSE3)
AC_TRY_RUN([#include <stdlib.h>
int main()
{
	unsigned long op = 1;
	unsigned int ecx;

	__asm__ __volatile__ ("cpuid" : "=c" (ecx) : "a" (op));
	if (ecx & 0x00000001)
		exit(0);
	else
		exit(1);
}]
, echo "#ifndef _KNOWLEDGE_CONFIG_H_" > include/config.h
echo "#define _KNOWLEDGE_CONFIG_H_" >> include/config.h
echo "#define __HAVE_SSE3__" >> include/config.h
echo "#endif" >> include/config.h
CXXFLAGS="$CXXFLAGS -msse -msse2 -msse3 -ffast-math"
AC_MSG_RESULT(yes), AC_MSG_RESULT(no)
echo "#ifndef _KNOWLEDGE_CONFIG_H_" > include/config.h
echo "#define _KNOWLEDGE_CONFIG_H_" >> include/config.h
echo "#undef __HAVE_SSE3__" >> include/config.h
echo "#endif" >> include/config.h
)

AC_OUTPUT(Makefile src/Makefile \
			 include/Makefile \
			 examples/Makefile \
			 examples/bsp/Makefile \
			 examples/model/Makefile \
			 examples/texture/Makefile \
			 examples/lightmap/Makefile \
			 examples/particles/Makefile \
          knowledge.pc)
