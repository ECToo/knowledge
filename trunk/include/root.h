#ifndef _ROOT_H_
#define _ROOT_H_

#include "prerequisites.h"
#include "singleton.h"
#include "rendersystem.h"
#include "textureManager.h"
#include "materialManager.h"
#include "renderer.h"

namespace k
{
	class root : public singleton<root>
	{
		private:
			renderSystem* mActiveRS;
			textureManager* mTextureManager;
			materialManager* mMaterialManager;
			renderer* mRenderer;

		public:
			root();
			~root();

			// Get Root Singleton
			static root& getSingleton();

			// Register the active render system
			renderSystem* getRenderSystem();

			// Renderer
			renderer* getRenderer();

			// Materials
			materialManager* getMaterialManager();
	};
}

#endif

