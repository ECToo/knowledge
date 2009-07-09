noinst_PROGRAMS = physics

physics_SOURCES = physics.cpp

INCLUDES = -I$(top_srcdir)/include/  -I$(top_srcdir)/include/pc/  -I$(top_srcdir)/include/wii/

physics_CFLAGS = -I$(INCLUDES) -g 
physics_LDFLAGS = -L$(top_builddir)/src 
physics_LDADD = -lknowledge -lode

