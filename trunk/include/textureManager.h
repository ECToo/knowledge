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

#define DEFAULT_WRAP (FLAG_CLAMP_EDGE_S | FLAG_CLAMP_EDGE_T | FLAG_CLAMP_EDGE_R)

namespace k
{
	/**
	 * This function will create a new texture based
	 * on the file type. On openGL this will be done by
	 * FreeImage and on Wii it will be done by a set of custom
	 * functions using libjpeg and libpng. wrapBits should be
	 * a bitwised set of flags for wrapping parameters. 
	 * The default wrapping is CLAMP_TO_EDGE on S,T and R.
	 * @see texFlags
	 */

	static inline bool isPowerOfTwo(unsigned int n)
	{ return ((n & (n-1)) == 0); }

	extern void unloadTexture(const kTexture* tex);
	extern texture* loadTexture(const std::string& filename, int wrapBits);
	extern texture* loadCubemap(const std::string& filename, int wrapBits);

	/**
	 * Create an texture file. Keep in mind
	 * that data must be in UNSIGNED_BYTE format.
	 *
	 * @data The texture pixels.
	 * @w Texture Width.
	 * @h Texture Height.
	 * @flags The texture flags @see texFlags
	 */
	extern texture* createRawTexture(unsigned char* data, int w, int h, int flags);

	class DLL_EXPORT textureManager : public singleton<textureManager>
	{
		private:
			std::list<texture*> mTextures;

		public:
			textureManager();
			~textureManager();

			static textureManager& getSingleton();

			bool allocateTextureData(const std::string& filename, int wrapBits = DEFAULT_WRAP);

			texture* getTexture(const std::string& filename);
			texture* createEmptyTexture();

			textureStage* createStage(unsigned short index);
			void setStageTexture(textureStage* newStage, const std::string& filename, int wrapBits = DEFAULT_WRAP);
			void setStageCubicTexture(textureStage* newStage, const std::string& filename, int wrapBits = DEFAULT_WRAP);

			textureStage* createCubicTexture(const std::string& filename, unsigned short index, int wrapBits = DEFAULT_WRAP);
			textureStage* createTexture(const std::string& filename, unsigned short index, int wrapBits = DEFAULT_WRAP);
	};
}

#endif

