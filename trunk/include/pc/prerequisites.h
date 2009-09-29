/*
Copyright (c) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _PREREQUISITES_PC_H_
#define _PREREQUISITES_PC_H_

extern "C" 
{
	// Must be included before anything else
	#ifdef WIN32

		#include <windows.h>
		#define DLL_EXPORT __declspec(dllexport)

	#endif

	// OpenGL
	#include <GL/glew.h>
	#include <GL/gl.h>
	#include <GL/glu.h>

	// Temporary SDL
	#include "SDL.h"

	// Temporary FreeImage
	#include "FreeImage.h"
	
	// posix threads
	#include <pthread.h>
}
	
typedef pthread_t 			platformThread;
typedef pthread_mutex_t 	platformMutex;
typedef GLuint 				platformTexturePointer;
typedef GLuint 				platformVBO;
typedef struct timeval		platformTimer;
typedef GLfloat 				vec_t;
typedef unsigned int 		index_t;

#define MAT_ROW_MAJOR

#endif

