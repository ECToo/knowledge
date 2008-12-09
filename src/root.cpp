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
#include "textureLib.h"

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
	#else

	// Gecko
	DEBUG_Init(GDBSTUB_DEVICE_USB, 1);
	mActiveRS = new wiiRenderSystem();

	// Initialize SD card
	fatInitDefault();
	new logger("sd://knowledge/knowledge.log");
	#endif

	logger::getSingleton().setLogMode(LOGMODE_BOTH);
	mActiveRS->initialise();
	mActiveRS->configure();

	// Create the renderer
	mRenderer = new renderer();

	// Create a new Texture Manager
	mTextureManager = new textureManager();
	new textureLoader();

	// Create the Material manager
	mMaterialManager = new materialManager();

	// Create The Gui manager
	mGuiManager = new guiManager();

	// Create The Input Manager
	mInputManager = new inputManager();
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
			
guiManager* root::getGuiManager()
{
	assert(mGuiManager != NULL);
	return mGuiManager;
}

inputManager* root::getInputManager()
{
	assert(mInputManager != NULL);
	return mInputManager;
}
	
std::string getExtension(const std::string& file)
{
	char* dot = strstr(file.c_str(), ".");

	if (!dot)
	{
		return std::string("");
	}
	else
	{
		unsigned int pos = (strrchr(file.c_str(), '.') - file.c_str());
		std::string extension;
		extension.append(file.c_str() + pos, file.length() - pos); 

		return extension;
	}
}

} // namespace k

