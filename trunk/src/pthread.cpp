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

#ifndef __WII__
#include "thread.h"
#include "logger.h"

namespace k {

void createKThread(platformThread* t, void* (*start)(void*), void* arg)
{
	kAssert(t);
	kAssert(start);

	if (pthread_create(t, NULL, start, arg) < 0)
		S_LOG_INFO("Failed to spawn a new thread");
}

void destroyKThread(platformThread* t)
{
	pthread_exit(t);
}

void joinKThread(platformThread* t)
{
	kAssert(t);
	pthread_join(*t, NULL);
}

void createKMutex(platformMutex* m)
{
	kAssert(m);
	pthread_mutex_init(m, NULL);
}
	
void lockKMutex(platformMutex* m)
{
	kAssert(m);
	pthread_mutex_lock(m);
}

void unlockKMutex(platformMutex* m)
{
	kAssert(m);
	pthread_mutex_unlock(m);
}

void destroyKMutex(platformMutex* m)
{
	kAssert(m);
	pthread_mutex_destroy(m);
}

}

#endif

