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
#include <unordered_map>

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

#define byteSwap4(Y) (((Y & 0xff)<<24)|((Y & 0xff00) << 8)|((Y & 0xff0000) >> 8)|((Y & 0xff000000) >> 24))
#define byteSwap2(Y) (((Y & 0xff) << 8)|((Y & 0xff00) >> 8))

#ifdef __BIG_ENDIAN__

	#define readLEShort(Y) (byteSwap2(Y))
	#define readBEShort(Y) (Y)
	#define readLEInt(Y) (byteSwap4(Y))
	#define readBEInt(Y) (Y)
	#define readBEFloat(Y) (Y)
 
	static inline float readLEFloat(const float f)
	{
		union 
		{ 
			float f; 
			int i; 
		} data;

		data.i = readLEInt(data.i);
		return data.f;
	}

#else

	#define readLEShort(Y) (Y)
	#define readBEShort(Y) (byteSwap2(Y))
	#define readLEInt(Y) (Y)
	#define readBEInt(Y) (byteSwap4(Y))
	#define readLEFloat(Y) (Y)

	static inline float readBEFloat(const float f)
	{
		union 
		{ 
			float f; 
			int i; 
		} data;

		data.i = readBEInt(data.i);
		return data.f;
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

#endif

