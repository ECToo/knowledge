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

