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

#ifndef _WORLD_H_
#define _WORLD_H_

#include "prerequisites.h"
#include "camera.h"

namespace k 
{
	/**
	 * \brief A basic abstraction for a world handler.
	 * This class should be a basic abstraction for world
	 * handlers. You can for example create a world handler that uses
	 * octree and insert it into the renderer so it will cull and process
	 * based on camera position.
	 */
	class DLL_EXPORT world
	{
		public:
			/**
			 * Virtual destructor
			 */
			virtual ~world() {}

			/**
			 * Draw the world from the cameras position.
			 * @param viewer The active camera on the renderer.
			 */
			virtual void draw(const camera* viewer) = 0;
	};
}

#endif

