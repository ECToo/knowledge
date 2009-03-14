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

#define K_VERSION "0.1a"

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
#include <sys/time.h>

#ifndef memalign
#include <malloc.h>
#endif

#ifndef WIN32
#include "config.h"
#endif

#define DLL_EXPORT

#ifndef ATTRIBUTE_ALIGN
#define ATTRIBUTE_ALIGN(v) __attribute__((aligned(v)))
#endif

#define byteSwap4(Y) (((Y & 0xff)<<24)|((Y & 0xff00) << 8)|((Y & 0xff0000) >> 8)|((Y & 0xff000000) >> 24))

#ifdef __WII__
#ifndef __BIG_ENDIAN__
#define __BIG_ENDIAN__
#endif
#endif

#ifdef __BIG_ENDIAN__
	#define readLEInt(Y) (byteSwap4(Y))
	#define readBEInt(Y) (Y)
	#define readBEFloat(Y) (Y)
	static inline float readLEFloat(float f)
	{
		union { float f; int i; } data;
		data.i = readLEInt(data.i);
		return data.f;
	}
#else
	#define readLEInt(Y) (Y)
	#define readBEInt(Y) (byteSwap4(Y))
	#define readLEFloat(Y) (Y)
	static inline float readBEFloat(float f)
	{
		union { float f; int i; } data;
		data.i = readLEInt(data.i);
		return data.f;
	}
#endif

// Platform dependent includes
#ifndef __WII__

	// Must be included before anything else
	#ifdef WIN32
		#include <windows.h>
		#define DLL_EXPORT __declspec(dllexport)
		#define memalign __mingw_aligned_malloc
	#endif

	// OpenGL
	#include <GL/glew.h>
	#include <GL/gl.h>
	#include <GL/glu.h>

	// FreeImage
	#include <FreeImage.h>

	// SDL
	#include "SDL.h"

	// posix threads
	#include <pthread.h>

#else

	#include <ogc/video.h>
	#include <ogc/system.h>
	#include <ogc/gx.h>
	#include <ogc/gu.h>
	#include <ogc/cache.h>
	#include <ogc/pad.h>
	#include <fat.h>
	#include <sys/unistd.h>
	#include <sys/dir.h>
	#include <ogc/lwp.h>
	#include <ogc/mutex.h>
	#include <ogc/lwp_watchdog.h>
	#include <malloc.h>
	#include <wiiuse/wpad.h>
	#include <debug.h>

#endif

// Platform independent definitions
#ifndef __WII__
	typedef GLuint kTexture;
	typedef GLfloat vec_t;
	typedef GLuint kVBO;
	typedef unsigned int index_t;

	// Threads
	typedef pthread_t kthread;
	typedef pthread_mutex_t kmutex;

	#define _break(); ""
	#define MAT_ROW_MAJOR

#else

	typedef char kVBO;
	typedef GXTexObj kTexture;
	typedef f32 vec_t;
	typedef u16 index_t;

	// Threads
	typedef lwp_t kthread;
	typedef mutex_t kmutex;

	#define MAT_COLUMN_MAJOR

#endif

#endif

