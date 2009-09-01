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
	/**
	 * Helper function, returns if an string represents a number or not.
	 */
	extern bool DLL_EXPORT isNumeric(const std::string& str);
	
	/**
	 * Helper function, returns a file extension.
	 */
	extern std::string DLL_EXPORT getExtension(const std::string& file);

	/**
	 * Helper function, returns the hash key for a filename
	 */
	extern unsigned int DLL_EXPORT getHashKey(const std::string& filename);

	/**
	 * \brief The root is the class responsible to look over other classes.
	 * Basically the root is the class that spawns the many other needed classes
	 * for knowledge. It is responsible to keep the pointers and give you references
	 * to wich classes are working and allocate and deallocate them.
	 */
	class DLL_EXPORT root : public singleton<root>
	{
		private:
			renderSystem* mActiveRS;
			textureManager* mTextureManager;
			materialManager* mMaterialManager;
			renderer* mRenderer;
			guiManager* mGuiManager;
			inputManager* mInputManager;
			particle::manager* mParticleManager;
			logger* mLogger;

			/**
			 * Global timer
			 */
			timer mGlobalTimer;

		public:
			/**
			 * Create the knowledge Root and initialize the log system
			 * saving all the logs to a file.
			 *
			 * @param filename The log destination file path.
			 */
			root(const std::string& filename = "knowledge.log");

			/**
			 * Destructor.
			 */
			~root();

			/**
			 * Get root singleton instance.
			 */
			static root& getSingleton();

			/**
			 * Returns the active renderSystem.
			 */
			renderSystem* getRenderSystem();

			/**
			 * Returns the active renderer
			 */
			renderer* getRenderer();

			/**
			 * Returns the materialmanager instance.
			 */
			materialManager* getMaterialManager();

			/**
			 * Returns the guiManager instance.
			 */
			guiManager* getGuiManager();

			/**
			 * Returns the inputManager instance.
			 */
			inputManager* getInputManager();

			/**
			 * Returns the particle::manager instance.
			 */
			particle::manager* getParticleManager();

			/**
			 * The global root timer.
			 * @see timer
			 */
			long getGlobalTime();
	};
}

#endif

