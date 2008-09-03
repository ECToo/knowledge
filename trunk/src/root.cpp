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

#include "root.h"
#include "logger.h"
#include "glRenderSystem.h"
#include "wiiRenderSystem.h"

namespace k {

template<> root* singleton<root>::singleton_instance = 0;

root& root::getSingleton()
{  
	assert(singleton_instance);
	return (*singleton_instance);  
}

root::root()
{
	// Initialize the render system
	#ifndef __WII__
	mActiveRS = new glRenderSystem();
	new logger("knowledge.log");
	logger::getSingleton().setLogMode(LOGMODE_BOTH);
	#else
	mActiveRS = new wiiRenderSystem();

	// Initialize SD card
	fatInitDefault();
	new logger("/knowledge/knowledge.log");
	logger::getSingleton().setLogMode(LOGMODE_FILE);
	#endif

	mActiveRS->initialise();
	mActiveRS->configure();

	// Create the renderer
	mRenderer = new renderer();

	// Create a new Texture Manager
	mTextureManager = new textureManager();

	// Create the Material manager
	mMaterialManager = new materialManager();
}

root::~root()
{
	delete mActiveRS;
	delete mRenderer;
	delete mTextureManager;
	delete mMaterialManager;
}
			
renderSystem* root::getRenderSystem()
{
	return mActiveRS;
}
			
renderer* root::getRenderer()
{
	assert(mRenderer != NULL);
	return mRenderer;
}

materialManager* root::getMaterialManager()
{
	assert(mMaterialManager != NULL);
	return mMaterialManager;
}

} // namespace k

