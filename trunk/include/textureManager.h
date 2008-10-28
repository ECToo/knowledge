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
	#ifdef __WII__
	struct wiiTexture
	{
		unsigned int mWidth;
		unsigned int mHeight;

		GXTexObj* mData;
	};
	#endif

	class textureManager : public singleton<textureManager>
	{
		private:
			#ifndef __WII__
			std::map<std::string, ILuint*> mTextures;
			#else	
			std::map<std::string, wiiTexture*> mTextures;
			#endif

			#ifndef __WII__
			ILuint* getTexture(const std::string& filename);
			#else	
			texture* createTexturePNG(const std::string& filename, unsigned short index);
			texture* createTextureJPG(const std::string& filename, unsigned short index);

			wiiTexture* getTexture(const std::string& filename);
			#endif

		public:
			textureManager();
			~textureManager();

			static textureManager& getSingleton();

			texture* createTexture(const std::string& filename, unsigned short index);
	};
}

#endif

