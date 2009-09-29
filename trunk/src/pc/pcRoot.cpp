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

#include "root.h"
#include "logger.h"
#include "glRenderSystem.h"

namespace k {

root::root(const std::string& filename)
{
	try
	{
		mLogger = new logger(filename);
		mLogger->setLogMode(LOGMODE_BOTH);
	}

	catch (...)
	{
		printf("Failed to allocate log system.");
		return;
	}

	try
	{
		// Initialize the render system
		mActiveRS = new glRenderSystem();
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
		mParticleManager = new particle::manager();
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate particleManager.");
		return;
	}
}

} // namespace k

