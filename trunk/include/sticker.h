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

#ifndef _STICKER_H_
#define _STICKER_H_

#include "prerequisites.h"
#include "drawable.h"
#include "material.h"

namespace k
{
	class DLL_EXPORT sticker : public drawable2D
	{
		protected:
			material* mMaterial;
			
			/**
			 * This is the sticker Z position (default to 0).
			 * It should be used to prevent overdraw problems.
			 */
			vec_t mZ;

		public:
			sticker(const std::string& matName);
			~sticker();

			material* getMaterial();
			void draw();
	};
}

#endif

