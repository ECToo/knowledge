lib_LTLIBRARIES = libknowledge.la 

libknowledge_la_SOURCES = drawable.cpp\
								  vector3.cpp\
								  md3.cpp\
								  md5.cpp\
								  fileParser.cpp\
								  camera.cpp\
								  textureManager.cpp\
								  material.cpp\
								  materialManager.cpp\
								  guiManager.cpp\
								  fontManager.cpp\
								  inputManager.cpp\
								  gameState.cpp\
								  physicsManager.cpp\
								  resourceManager.cpp\
								  renderer.cpp\
								  bsp46.cpp\
								  sticker.cpp\
								  sprite.cpp\
								  pthread.cpp\
								  loadscr.cpp\
								  particle.cpp\
								  logger.cpp\
								  root.cpp\
								  tinystr.cpp\
								  tinyxml.cpp\
								  tinyxmlerror.cpp\
								  tinyxmlparser.cpp\
								  pc/glRenderSystem.cpp\
								  pc/glTexture.cpp\
								  pc/glMaterialStage.cpp\
								  pc/pcTimer.cpp\
								  pc/pcRoot.cpp\
								  pc/pcResourceManager.cpp

INCLUDES = -I@top_srcdir@/include -I@top_srcdir@/include/pc -I@top_srcdir@/include/wii $(CXXFLAGS)

libknowledge_la_CFLAGS = -I$(INCLUDES) -g
libknowledge_la_CXXFLAGS = -I$(INCLUDES)
libknowledge_la_LIBADD = -lode -lm -lGL -lGLU -lGLEW -lfreeimage
libknowledge_la_LDFLAGS = -pthread

pkginclude_HEADERS = @top_srcdir@/include/bsp46.h \
@top_srcdir@/include/camera.h \
@top_srcdir@/include/config.h \
@top_srcdir@/include/drawable.h \
@top_srcdir@/include/fileParser.h \
@top_srcdir@/include/fontManager.h \
@top_srcdir@/include/gameState.h \
@top_srcdir@/include/guiManager.h \
@top_srcdir@/include/inputManager.h \
@top_srcdir@/include/keysyms.h \
@top_srcdir@/include/knowledge.h \
@top_srcdir@/include/loadscr.h \
@top_srcdir@/include/logger.h \
@top_srcdir@/include/material.h \
@top_srcdir@/include/materialManager.h \
@top_srcdir@/include/matrix3.h \
@top_srcdir@/include/matrix4.h \
@top_srcdir@/include/md3.h \
@top_srcdir@/include/md5.h \
@top_srcdir@/include/particle.h \
@top_srcdir@/include/physicsManager.h \
@top_srcdir@/include/pngu.h \
@top_srcdir@/include/prerequisites.h \
@top_srcdir@/include/quaternion.h \
@top_srcdir@/include/renderer.h \
@top_srcdir@/include/rendersystem.h \
@top_srcdir@/include/resourceManager.h \
@top_srcdir@/include/root.h \
@top_srcdir@/include/singleton.h \
@top_srcdir@/include/sprite.h \
@top_srcdir@/include/sticker.h \
@top_srcdir@/include/texture.h \
@top_srcdir@/include/textureManager.h \
@top_srcdir@/include/thread.h \
@top_srcdir@/include/timer.h \
@top_srcdir@/include/tinystr.h \
@top_srcdir@/include/tinyxml.h \
@top_srcdir@/include/vector2.h \
@top_srcdir@/include/vector3.h \
@top_srcdir@/include/world.h

pcdir = $(pkgincludedir)/pc
pc_HEADERS = @top_srcdir@/include/pc/glRenderSystem.h \
@top_srcdir@/include/pc/prerequisites.h 

wiidir = $(pkgincludedir)/wii
wii_HEADERS = @top_srcdir@/include/wii/prerequisites.h \
@top_srcdir@/include/wii/tev.h \
@top_srcdir@/include/wii/wiiRenderSystem.h \
@top_srcdir@/include/wii/wiiVector3.h

