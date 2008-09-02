lib_LTLIBRARIES = libknowledge.la 

libknowledge_la_SOURCES = drawable.cpp\
								  md5.cpp\
								  fileAccess.cpp\
								  glRenderSystem.cpp\
								  texture.cpp\
								  textureManager.cpp\
								  material.cpp\
								  materialManager.cpp\
								  renderer.cpp\
								  sticker.cpp\
								  logger.cpp\
								  root.cpp

INCLUDES = -I@top_srcdir@/include $(CXXFLAGS)

libknowledge_la_CFLAGS = -I$(INCLUDES) -g
libknowledge_la_LIBADD = -lm -lGL -lGLU -lGLEW -lILU -lILUT

pkginclude_HEADERS = @top_srcdir@/include/prerequisites.h \
							@top_srcdir@/include/singleton.h \
							@top_srcdir@/include/vector2.h \
							@top_srcdir@/include/vector3.h \
							@top_srcdir@/include/quaternion.h \
							@top_srcdir@/include/fileAccess.h \
							@top_srcdir@/include/rendersystem.h \
							@top_srcdir@/include/glRenderSystem.h \
							@top_srcdir@/include/root.h \
							@top_srcdir@/include/md5.h

