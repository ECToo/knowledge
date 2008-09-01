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
			#ifndef __WII__
			std::map<std::string, ILuint*> mTextures;
			#else	
			std::map<std::string, GXTexObj*> mTextures;
			#endif

			#ifndef __WII__
			ILuint* getTexture(const std::string& filename);
			#else	
			GXTexObj* getTexture(const std::string& filename);
			#endif

		public:
			textureManager();
			~textureManager();

			static textureManager& getSingleton();

			texture* createTexture(const std::string& filename);
	};
}

#endif

