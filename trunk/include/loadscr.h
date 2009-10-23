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

#ifndef _LOAD_SCR_H_
#define _LOAD_SCR_H_

#include "prerequisites.h"
#include "material.h"
#include "color.h"

namespace k
{
	/**
	 * \brief This class is only a interface for a load screen implementation
	 */
	class DLL_EXPORT loadScreen
	{
		public:
			/**
			 * Destructor to be overloaded.
			 */
			virtual ~loadScreen() {}

			/**
			 * Force the drawing of screen.
			 * @param[in] filename Last file loaded by resource manager.
			 */
			virtual void update(const std::string& filename) = 0;
	};

	/**
	 * \brief A load screen with a background image.
	 */
	class DLL_EXPORT bgLoadScreen : public loadScreen
	{
		protected:
			material* mBackground;

		public:
			/**
			 * Constructor
			 */
			bgLoadScreen()
			{
				mBackground = NULL;
			}

			/**
			 * Destructor, deallocates background material.
			 */
			~bgLoadScreen()
			{
				delete mBackground;
			}

			/**
			 * Load a background image with path set to filename.
			 * @param filename The path to the image to be loaded, 
			 * keep in mind that path will check root in resourceManager
			 */
			void loadBg(const std::string& filename);

			/**
			 * Force redraw of the load screen parsing the last file loaded.
			 * @param[in] filename The last file loaded by the resouce manager.
			 */
			void update(const std::string& filename);
	};

	/**
	 * \brief Load screen with a centered image.
	 */
	class DLL_EXPORT imgLoadScreen : public loadScreen
	{
		protected:
			material* mImgMaterial;
			color mBgColor;
			vector2 mDimension;

			// for drawing arrays
			vec_t* mVertices;

		public:
			/**
			 * Constructor
			 */
			imgLoadScreen()
			{
				mImgMaterial = NULL;

				mVertices = (vec_t*) memalign(32, sizeof(vec_t) * 12);
				if (!mVertices)
				{
					S_LOG_INFO("Failed to allocate vertex array for load screen.");
					return;
				}

				mVertices[0] = mVertices[1] = mVertices[4] = mVertices[9] = 0.0f;
				mVertices[2] = mVertices[5] = mVertices[8] = mVertices[11] = -0.5f;
			}

			/**
			 * Destructor, deallocates image material
			 */
			~imgLoadScreen()
			{
				delete mImgMaterial;

				// deallocate memalign'ed data.
				if (mVertices)
					free(mVertices);
			}

			/**
			 * Set the image dimensions in pixels
			 * @param dimension The dimensions of the new centered screen image.
			 */
			void setImgDimension(const vector2& dimension);

			/**
			 * Set the color of background.
			 * @param clr The Background color.
			 */
			void setBgColor(const color& clr);

			/**
			 * Load a background image with path set to filename.
			 * @param filename The path to the image to be loaded, 
			 * keep in mind that path will check root in resourceManager
			 */
			void loadBg(const std::string& filename);

			/**
			 * Force redraw of the load screen parsing the last file loaded.
			 * @param[in] filename The last file loaded by the resouce manager.
			 */
			void update(const std::string& filename);
	};
}

#endif
