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

#ifdef __WII__
#include "thread.h"
#include "logger.h"

namespace k {

void createplatformThread(platformThread* t, void* (*start)(void*), void* arg)
{
	LWP_CreateThread(t, start, arg, NULL, 0, 0);
}

void destroyplatformThread(platformThread* t)
{
	LWP_SuspendThread(*t);
}

void joinplatformThread(platformThread* t)
{
	LWP_JoinThread(*t, NULL);
}

void createplatformMutex(platformMutex* m)
{
	LWP_MutexInit(m, false);	
}
	
void lockplatformMutex(platformMutex* m)
{
	LWP_MutexLock(*m);
}

void unlockplatformMutex(platformMutex* m)
{
	LWP_MutexUnlock(*m);
}

void destroyplatformMutex(platformMutex* m)
{
	LWP_MutexDestroy(*m);
}

}

#endif
