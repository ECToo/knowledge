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

#include "root.h"
#include "logger.h"
#include "wiiRenderSystem.h"

namespace k {

root::root(const std::string& filename)
{
	CON_EnableGecko(1, false);
	DEBUG_Init(GDBSTUB_DEVICE_USB, 1);
	fatInitDefault();

	try
	{
		new logger(filename);
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
		mActiveRS = new wiiRenderSystem();
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
		// Create the tev manager
		new tevManager();
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate tev manager.");
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
		mParticleManager = new particle::manager();
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate particleManager.");
		return;
	}
}

} //namespace k

