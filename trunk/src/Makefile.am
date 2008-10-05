lib_LTLIBRARIES = libknowledge.la 

libknowledge_la_SOURCES = drawable.cpp\
								  vector3.cpp\
								  md5.cpp\
								  fileAccess.cpp\
								  camera.cpp\
								  glRenderSystem.cpp\
								  texture.cpp\
								  textureManager.cpp\
								  material.cpp\
								  materialManager.cpp\
								  guiManager.cpp\
								  inputManager.cpp\
								  renderer.cpp\
								  sticker.cpp\
								  logger.cpp\
								  root.cpp

INCLUDES = -I@top_srcdir@/include $(CXXFLAGS)

libknowledge_la_CFLAGS = -I$(INCLUDES) -g
libknowledge_la_LIBADD = -lm -lGL -lGLU -lGLEW -lILU -lILUT

pkginclude_HEADERS = @top_srcdir@/include/prerequisites.h \
							@top_srcdir@/include/singleton.h \
							@top_srcdir@/include/camera.h \
							@top_srcdir@/include/singleton.h \
							@top_srcdir@/include/matrix4.h \
							@top_srcdir@/include/drawable.h \
							@top_srcdir@/include/sticker.h \
							@top_srcdir@/include/texture.h \
							@top_srcdir@/include/textureManager.h \
							@top_srcdir@/include/guiManager.h \
							@top_srcdir@/include/inputManager.h \
							@top_srcdir@/include/keysyms.h \
							@top_srcdir@/include/logger.h \
							@top_srcdir@/include/material.h \
							@top_srcdir@/include/materialManager.h \
							@top_srcdir@/include/renderer.h \
							@top_srcdir@/include/matrix3.h \
							@top_srcdir@/include/vector2.h \
							@top_srcdir@/include/vector3.h \
							@top_srcdir@/include/quaternion.h \
							@top_srcdir@/include/fileAccess.h \
							@top_srcdir@/include/rendersystem.h \
							@top_srcdir@/include/glRenderSystem.h \
							@top_srcdir@/include/root.h \
							@top_srcdir@/include/md5.h

