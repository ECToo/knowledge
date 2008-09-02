#ifndef _PREREQUISITES_H
#define _PREREQUISITES_H

// Standard
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <fstream>
#include <cassert>
#include <list>
#include <vector>
#include <map>
#include <stack>

// Platform dependent includes
#ifndef __WII__
	// OpenGL
	#include <GL/gl.h>
	#include <GL/glu.h>

	// DevIL
	#include <IL/il.h>
	#include <IL/ilu.h>
	#include <IL/ilut.h>

	// SDL
	#include "SDL.h"

	#ifdef WIN32
		#include <windows.h>
	#endif
#else
	#include <ogc/video.h>
	#include <ogc/system.h>
	#include <ogc/gx.h>
	#include <ogc/cache.h>
	#include <fat.h>
	#include <sys/unistd.h>
	#include <malloc.h>
#endif

// Platform independent definitions
#ifndef __WII__
	typedef GLfloat vec_t;
#else
	typedef float vec_t;
#endif

// Global Constants

#endif

