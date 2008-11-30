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

#ifndef _TEXTURE_MANAGER_H
#define _TEXTURE_MANAGER_H

#include "prerequisites.h"
#include "singleton.h"
#include "texture.h"

namespace k
{
	class textureManager : public singleton<textureManager>
	{
		private:
			std::map<std::string, texture*> mTextures;

		public:
			textureManager();
			~textureManager();

			static textureManager& getSingleton();

			void allocateTextureData(const std::string& filename);
			void deallocateTextureData(const std::string& filename);

			texture* getTexture(const std::string& filename);
			textureStage* createCubicTexture(const std::string& filename, unsigned short index);
			textureStage* createTexture(const std::string& filename, unsigned short index);
	};
}

#endif

