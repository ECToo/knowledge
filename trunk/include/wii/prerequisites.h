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

#ifndef _WII_PREREQUISITES_H_
#define _WII_PREREQUISITES_H_

#ifndef __BIG_ENDIAN__
	#define __BIG_ENDIAN__
#endif

#ifdef __HAVE_SSE3__
	#undef __HAVE_SSE3__
#endif


#include <ogc/video.h>
#include <ogc/system.h>
#include <ogc/gx.h>
#include <ogc/gu.h>
#include <ogc/conf.h>
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

typedef lwp_t 				platformThread;
typedef mutex_t 			platformMutex;
typedef GXTexObj 			platformTexturePointer;
typedef char 				platformVBO;
typedef long long			platformTimer;
typedef f32 				vec_t;
typedef u16 				index_t;

#define MAT_COLUMN_MAJOR

#endif

