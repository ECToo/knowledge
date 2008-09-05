/*
    Copyright (C) 2008 Rômulo Fernandes Machado <romulo@castorgroup.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
	typedef unsigned int index_t;
#else
	typedef float vec_t;
	typedef u16 index_t;
#endif

// Global Constants

#endif

