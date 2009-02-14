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

#ifndef _ROOT_H_
#define _ROOT_H_

#include "prerequisites.h"
#include "singleton.h"
#include "rendersystem.h"
#include "textureManager.h"
#include "materialManager.h"
#include "renderer.h"
#include "guiManager.h"
#include "inputManager.h"
#include "thread.h"
#include "particle.h"
#include "timer.h"

namespace k
{
	extern std::string getExtension(const std::string& file);
	class DLL_EXPORT root : public singleton<root>
	{
		private:
			renderSystem* mActiveRS;
			textureManager* mTextureManager;
			materialManager* mMaterialManager;
			renderer* mRenderer;
			guiManager* mGuiManager;
			inputManager* mInputManager;
			particleManager* mParticleManager;

			/**
			 * Global timer
			 */
			timer mGlobalTimer;

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

			// GUI
			guiManager* getGuiManager();

			// Input
			inputManager* getInputManager();

			// Particles
			particleManager* getParticleManager();

			// The Global Timer =]
			long getGlobalTime();
	};
}

#endif

