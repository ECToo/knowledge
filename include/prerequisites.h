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

#ifndef _PREREQUISITES_H
#define _PREREQUISITES_H

#define K_VERSION "0.1a"

// Standard C++
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <list>
#include <vector>
#include <map>
#include <algorithm>

// Standard C Libraries
#include <cmath>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdarg>

extern "C" 
{
	#include <sys/time.h>
	#include <malloc.h>
}

#ifndef ATTRIBUTE_ALIGN
	#define ATTRIBUTE_ALIGN(v) __attribute__((aligned(v)))
#endif

#ifndef ATTRIBUTE_PACKED
	#define ATTRIBUTE_PACKED __attribute__((packed))
#endif

#ifdef __BIG_ENDIAN__

	static inline short readLEShort(const short num)
	{
		char b1, b2;

		b1 = num & 0xff;
		b2 = (num >> 8) & 0xff;

		return (b1 << 8) + b2;
	}
	
	static inline short readBEShort(const short num)
	{	return num; }

	static inline int readLEInt(const int num)
	{
		char b[4];

		b[0] = num & 0xff;
		b[1] = (num >> 8) & 0xff;
		b[2] = (num >> 16) & 0xff;
		b[3] = (num >> 24) & 0xff;

		return ((int)b[0] << 24) + ((int)b[1] << 16) + ((int)b[2] << 8) + b[3];
	}
	
	static inline int readBEInt(const int num)
	{ return num; }
 
	static inline float readBEFloat(const float f)
	{ return f; }

	static inline float readLEFloat(const float f)
	{
		union 
		{ 
			float f; 
			char b[4]; 
		} in, out;

		in.f = f;

		out.b[0] = in.b[3];
		out.b[1] = in.b[2];
		out.b[2] = in.b[1];
		out.b[3] = in.b[0];

		return out.f;
	}

#else

	#ifdef __WII__
	#error "Wii is not set as big endian, please define __BIG_ENDIAN__ !"
	#endif

	static inline short readBEShort(const short num)
	{
		char b1, b2;

		b1 = num & 0xff;
		b2 = (num >> 8) & 0xff;

		return (b1 << 8) + b2;
	}
	
	static inline short readLEShort(const short num)
	{	return num; }

	static inline int readBEInt(const int num)
	{
		char b[4];

		b[0] = num & 0xff;
		b[1] = (num >> 8) & 0xff;
		b[2] = (num >> 16) & 0xff;
		b[3] = (num >> 24) & 0xff;

		return ((int)b[0] << 24) + ((int)b[1] << 16) + ((int)b[2] << 8) + b[3];
	}
	
	static inline int readLEInt(const int num)
	{ return num; }
 
	static inline float readLEFloat(const float f)
	{ return f; }

	static inline float readBEFloat(const float f)
	{
		union 
		{ 
			float f; 
			char b[4]; 
		} in, out;

		in.f = f;

		out.b[0] = in.b[3];
		out.b[1] = in.b[2];
		out.b[2] = in.b[1];
		out.b[3] = in.b[0];

		return out.f;
	}

#endif

#ifndef __WII__
	#include "pc/prerequisites.h"
#else
	#include "wii/prerequisites.h"
#endif

#ifndef DLL_EXPORT
	#define DLL_EXPORT
#endif

/**
 * Windows for some reason dont respect aligned_malloc.
 * OSX also dont see memalign.
 */
#ifndef memalign
		#define memalign(X, Y) malloc(Y)
#endif

#endif

