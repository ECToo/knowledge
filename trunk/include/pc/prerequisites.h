/*
    Copyright (C) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

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

#ifndef _PREREQUISITES_PC_H_
#define _PREREQUISITES_PC_H_

extern "C" 
{
	// Must be included before anything else
	#ifdef WIN32

		#include <windows.h>
		#define DLL_EXPORT __declspec(dllexport)

		/**
		 * Windows for some reason dont respect aligned_malloc.
		 */
		#define memalign(X, Y) malloc(Y)

	#endif

	// OpenGL
	#include <GL/glew.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <dirent.h>
	
	// FreeImage
	#include <FreeImage.h>

	// SDL
	#include "SDL.h"

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

