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

#ifndef _FONT_MANAGER_H
#define _FONT_MANAGER_H

#include "prerequisites.h"
#include "fileAccess.h"
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
	 * Base class, where to 
	 * derive the providers from.
	 */
	class baseText
	{
		public:
			void setText(const std::string& text) {}
	};

	class bitmapText : public baseText, public drawable2D
	{
		private:
			doom3Glyph mGlyphs[255];
			material* mMaterial;

			std::string mContents;

			vec_t _drawChar(vec_t x, vec_t y, char c);

		public:
			bitmapText(const std::string& datName, const std::string& matName);
			~bitmapText();

			void setText(const std::string& text);
			void draw();
	};

	/*
	class bitmapProvider : public fontProvider
	{
		private:
			std::map<doom3Font, int> mBitmaps;

		public:
			bitmapProvider(const char* file);
			void drawText(int size, const std::string& text);
	};

	class fontManager : public singleton<fontManager>
	{
		protected:
			std::map<fontProvider*, std::string> mProviders;

		public:
			fontManager()
			{
				mProviders.clear();
			}

			~fontManager();

			// Return the singleton
			static fontManager& getSingleton();

			void loadFont(const std::string& filename);
			void selectFont(const std::string& fontName);
	};
	*/
}

#endif

