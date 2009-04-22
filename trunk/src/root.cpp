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
	kAssert(singleton_instance);
	return (*singleton_instance);  
}

root::root()
{
	#ifndef __WII__
	const std::string logPath("knowledge.log");
	#else
	const std::string logPath("sd:/knowledge/knowledge.log");
	#endif

	#ifdef __WII__
		CON_EnableGecko(1, false);
		DEBUG_Init(GDBSTUB_DEVICE_USB, 1);
	#endif

	#ifdef __WII__
		fatInitDefault();
	#endif

	try
	{
		new logger(logPath);
		logger::getSingleton().setLogMode(LOGMODE_BOTH);
	}

	catch (...)
	{
		printf("Failed to allocate log system.");
		return;
	}

	try
	{
		// Initialize the render system
		#ifndef __WII__
			mActiveRS = new glRenderSystem();
		#else
			mActiveRS = new wiiRenderSystem();
		#endif

		mActiveRS->initialize();
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate renderSystem.");
		return;
	}

	try
	{
		// Create the renderer
		mRenderer = new renderer();
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate renderer.");
		return;
	}

	try
	{
		// Create a new Texture Manager
		mTextureManager = new textureManager();
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate textureManager.");
		return;
	}

	try
	{
		// Create the Material manager
		mMaterialManager = new materialManager();
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate materialManager.");
		return;
	}

	try
	{
		// Create The Gui manager
		mGuiManager = new guiManager();
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate guiManager.");
		return;
	}

	try
	{
		// Create The Input Manager
		mInputManager = new inputManager();
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate inputManager.");
		return;
	}

	try
	{
		// Create Particle Manager
		mParticleManager = new particleManager();
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate particleManager.");
		return;
	}

}

root::~root()
{
	delete mActiveRS;
	delete mRenderer;
	delete mTextureManager;
	delete mMaterialManager;
}
			
particleManager* root::getParticleManager()
{
	return mParticleManager;
}
			
renderSystem* root::getRenderSystem()
{
	return mActiveRS;
}
			
renderer* root::getRenderer()
{
	return mRenderer;
}

materialManager* root::getMaterialManager()
{
	return mMaterialManager;
}
			
guiManager* root::getGuiManager()
{
	return mGuiManager;
}

inputManager* root::getInputManager()
{
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
			
long root::getGlobalTime()
{
	return mGlobalTimer.getMilliSeconds();
}

} // namespace k

