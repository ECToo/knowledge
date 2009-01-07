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

#ifndef _THREAD_H_
#define _THREAD_H_

#include "prerequisites.h"
#include "singleton.h"

namespace k
{
	/**
	 * This file gives a basic implementation
	 * over the threads API. Its not
	 * meant to be a full featured thread system,
	 * instead it should offer a basic set of functions
	 * to work on threads for various platforms.
	 */
	extern void createKThread(kthread* t, void* (*start)(void*), void* arg);
	extern void destroyKThread(kthread* t);
	extern void joinKThread(kthread* t);

	extern void createKMutex(kmutex* m);
	extern void lockKMutex(kmutex* m);
	extern void unlockKMutex(kmutex* m);
	extern void destroyKMutex(kmutex* m);
}

#endif

