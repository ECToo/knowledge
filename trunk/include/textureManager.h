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

#ifndef _TEXTURE_MANAGER_H
#define _TEXTURE_MANAGER_H

#include "prerequisites.h"
#include "singleton.h"
#include "texture.h"

#define DEFAULT_WRAP (FLAG_CLAMP_EDGE_S | FLAG_CLAMP_EDGE_T | FLAG_CLAMP_EDGE_R)

namespace k
{
	static inline bool isPowerOfTwo(unsigned int n)
	{ return ((n & (n-1)) == 0); }

	typedef std::map<int, texture*> textureHash;

	/**
	 * \brief Handling texture loading and data allocation.
	 */
	class DLL_EXPORT textureManager : public singleton<textureManager>
	{
		private:
			textureHash mTextures;

		public:
			textureManager();
			~textureManager();

			/**
			 * Returns an instance to the textureManager singleton.
			 */
			static textureManager& getSingleton();

			/**
			 * Allocate a texture by file path.
			 * @param filename The full path of texture filename.
			 */
			texture* allocateTexture(const std::string& filename, int wrapBits = DEFAULT_WRAP);

			/**
			 * Get a texture by file path.
			 * @param filename The full path of texture filename.
			 */
			texture* getTexture(const std::string& filename);

			/**
			 * Create system textures. (k_base_white, k_base_black, k_base_null)
			 */
			void createSystemTextures();
	};
}

#endif

