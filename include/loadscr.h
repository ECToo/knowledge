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

#ifndef _LOAD_SCR_H_
#define _LOAD_SCR_H_

#include "prerequisites.h"
#include "material.h"

namespace k
{
	/**
	 * This class is only a interface for a load screen
	 * implementation
	 */
	class DLL_EXPORT loadScreen
	{
		public:
			virtual ~loadScreen() {}
			virtual void update(const std::string& filename) = 0;
	};

	/**
	 * A load screen with a background image
	 */
	class DLL_EXPORT bgLoadScreen : public loadScreen
	{
		protected:
			material* mBackground;

		public:
			bgLoadScreen()
			{
				mBackground = NULL;
			}

			~bgLoadScreen()
			{
				delete mBackground;
			}

			void loadBg(const std::string& filename);
			void update(const std::string& filename);
	};

	/**
	 * Load screen with a centered image.
	 */
	class DLL_EXPORT imgLoadScreen : public loadScreen
	{
		protected:
			material* mImgMaterial;
			vector3 mBgColor;
			vector2 mDimension;

		public:
			imgLoadScreen()
			{
				mImgMaterial = NULL;
			}
			~imgLoadScreen()
			{
				delete mImgMaterial;
			}


			void setImgDimension(const vector2& dimension);
			void setBgColor(const vector3& color);
			void loadBg(const std::string& filename);
			void update(const std::string& filename);
	};
}

#endif
