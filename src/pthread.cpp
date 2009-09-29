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

