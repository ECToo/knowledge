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

#ifndef _STICKER_H_
#define _STICKER_H_

#include "prerequisites.h"
#include "drawable.h"
#include "material.h"

namespace k
{
	/**
	 * \brief The sticker is a 2D element with a texture.
	 */
	class DLL_EXPORT sticker : public drawable2D
	{
		protected:
			material* mMaterial;
			
			/**
			 * This is the sticker Z position (default to 0).
			 * It should be used to prevent overdraw problems.
			 */
			vec_t mZ;

			/**
			 * Final arrays for drawing
			 */
			vec_t* mUvs;
			vec_t* mVertices;

		public:
			/**
			 * Create the sticker with a material. Keep in mind that after creating
			 * the sticker, you might want to set its dimensions with setScale() and
			 * its position with setPosition().
			 * @param matName The sticker material.
			 */
			sticker(const std::string& matName);

			/**
			 * Destructor
			 */
			~sticker();

			/**
			 * Returns the sticker material.
			 */
			material* getMaterial();

			/**
			 * Draw the sticker.
			 */
			void draw();
	};
}

#endif

