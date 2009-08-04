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

