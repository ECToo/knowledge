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

#ifndef _FONT_MANAGER_H
#define _FONT_MANAGER_H

#include "prerequisites.h"
#include "fileParser.h"
#include "singleton.h"
#include "sticker.h"
#include "drawable.h"
#include "materialManager.h"

namespace k
{
	/**
	 * Doom 3 glyph structure.
	 */
	typedef struct
	{
		int height;
		int top;
		int bottom;
		int pitch;
		int xSkip;
		int imgWidth;
		int imgHeight;
		float s;
		float t;
		float s2;
		float t2;
	} doom3Glyph;

	/**
	 * \brief Base class for text drawing, where we derive the providers from.
	 */
	class DLL_EXPORT baseText : public drawable2D
	{
		protected:
			std::string mContents;

		public:
			/**
			 * Set the baseText contents.
			 * @param text The new content of baseText.
			 */
			virtual void setText(const std::string& text) { mContents = text; }

			/**
			 * Get the contents of baseText.
			 */
			virtual const std::string& getText() { return mContents; }

			/**
			 * Draw the baseText. This wont do anything on this class, only on childs.
			 */
			void draw() {}
	};

	/**
	 * \brief A 2D text drawer based on iD Software Doom 3 bitmap font format.
	 */
	class DLL_EXPORT bitmapText : public baseText
	{
		private:
			doom3Glyph mGlyphs[255];
			material* mMaterial;

			vec_t mMaxHeight;
			vec_t mLineHeight;

			vec_t _drawChar(vec_t x, vec_t y, char c);

		public:
			/**
			 * Creates a bitmapText using a .dat file and a material for images.
			 *
			 * @param datName The name of the *.dat containing the font definitions.
			 * @param matName The name of the material containing the image for the font.
			 */
			bitmapText(const std::string& datName, const std::string& matName);
			
			/**
			 * Set the bitmapText content and resize its boundingbox.
			 */
			void setText(const std::string& text);
			
			/**
			 * Destructor
			 */
			~bitmapText();

			/**
			 * Draw the bitmapText
			 */
			void draw();
	};
}

#endif

