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

