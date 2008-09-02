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

#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "prerequisites.h"
#include "vector3.h"
#include "texture.h"

namespace k
{
	class material
	{
		private:
			vector3 mAmbient;
			vector3 mDiffuse;
			vector3 mSpecular;

			std::list<texture*> mTextures;

		public:
			void setAmbient(const vector3& color);
			void setDiffuse(const vector3& color);
			void setSpecular(const vector3& color);

			void pushTexture(texture* tex);
			void prepare();
	};
}

#endif

