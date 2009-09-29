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
			/**
			 * Constructor.
			 */
			textureManager();

			/**
			 * Destructor. Free all allocated textures.
			 */
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

