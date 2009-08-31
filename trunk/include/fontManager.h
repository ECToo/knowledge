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
	class DLL_EXPORT baseText
	{
		public:
			void setText(const std::string& text) {}
	};

	/**
	 * \brief A 2D text drawer based on iD Software Doom 3 bitmap font format.
	 */
	class DLL_EXPORT bitmapText : public baseText, public drawable2D
	{
		private:
			doom3Glyph mGlyphs[255];
			material* mMaterial;

			std::string mContents;
			vec_t mMaxHeight;

			vec_t _drawChar(vec_t x, vec_t y, char c);

		public:
			/**
			 * Creates a bitmapText using a .dat file and a material for images.
			 *
			 * @param datName The name of the *.dat containing the font definitions.
			 * @param matName The name of the material containing the image for the font.
			 */
			bitmapText(const std::string& datName, const std::string& matName);
			~bitmapText();

			/**
			 * Set the bitmapText contents.
			 *
			 * @param text The new bitmapText content.
			 */
			void setText(const std::string& text);

			/**
			 * Draw the bitmapText
			 */
			void draw();
	};
}

#endif

