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

#include "prerequisites.h"
#include "root.h"
#include "renderer.h"
#include "rendersystem.h"
#include "resourceManager.h"
#include "logger.h"

int main(int argc, char** argv)
{
	// Initialize knowledge
	k::root* appRoot = new k::root();
	k::renderSystem* mRenderSystem = appRoot->getRenderSystem();
	k::materialManager* mMaterialManager = appRoot->getMaterialManager();
	k::inputManager* mInputManager = appRoot->getInputManager();

	// Doesnt matter on wii
	mRenderSystem->createWindow(800, 600);
	mRenderSystem->setWindowTitle("knowledge, the power of mind");

	#ifdef __WII__
	k::resourceManager* resourceMgr = new k::resourceManager("/knowledge/resources.cfg");
	#else
	k::resourceManager* resourceMgr = new k::resourceManager("../resources.cfg");
	#endif

	// Loading Screen
	k::bgLoadScreen* newLoadingScreen = new k::bgLoadScreen();
	assert(newLoadingScreen);

	resourceMgr->setLoadingScreen(newLoadingScreen);
	newLoadingScreen->loadBg("loading.jpg");
	newLoadingScreen->update("");


	k::resourceManager::getSingleton().loadGroup("common");
	k::resourceManager::getSingleton().loadGroup("texture");

	delete newLoadingScreen;

	/**
	 * Setup the input Manager
	 */
	assert(mInputManager != NULL);
	mInputManager->initWii(false);
	mInputManager->setupWiiMotes(1);
	mInputManager->setWiiMoteTimeout(60);
	mInputManager->setWiiMoteEmulation(true);

	// Rotations
	vec_t rquad = 0;

	#ifdef __WII__
	k::parsingFile* tevFile = new k::parsingFile("/knowledge/tev.script");

	assert(tevFile != NULL);
	k::tevManager::getSingleton().parseTevScript(tevFile);
	#endif

	// Get material
	k::material* crateMaterial = mMaterialManager->getMaterial("crate");
	assert(crateMaterial);

	// Return to normal mode
	mRenderSystem->setMatrixMode(k::MATRIXMODE_PROJECTION);
	mRenderSystem->identityMatrix();
	mRenderSystem->setPerspective(110, 
			mRenderSystem->getScreenWidth()/mRenderSystem->getScreenHeight(), 
			0.1f, 5000.0f);

	bool running = true;
	while (running)
	{
		mInputManager->feed();

		// User clicked on Close Window
		if (mInputManager->getQuitEvent() || mInputManager->getKbdKeyDown(K_KBD_ESCAPE))
			running = false;

		// Quit Application
		if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_HOME))
			running = false;

		// Clear screen, etc...
		mRenderSystem->frameStart();

		mRenderSystem->setMatrixMode(k::MATRIXMODE_MODELVIEW);
		mRenderSystem->identityMatrix();
		mRenderSystem->translateScene(0.0f, 0.0f, -4.0f);
		mRenderSystem->rotateScene(30, 1.0f, 0.0f, 0.0f);
		mRenderSystem->rotateScene(rquad, 0.0f, 1.0f, 0.0f);

		// Bind Texture
		crateMaterial->prepare();

		// Start Vertices
		mRenderSystem->startVertices(k::VERTEXMODE_QUAD);

		// Front Face
		mRenderSystem->texCoord(k::vector2(0.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f, -1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f, -1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f,  1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f,  1.0f,  1.0f));

		// Back Face
		mRenderSystem->texCoord(k::vector2(1.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f, -1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f,  1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f,  1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f, -1.0f, -1.0f));

		// Top Face
		mRenderSystem->texCoord(k::vector2(0.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f,  1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f,  1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f,  1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f,  1.0f, -1.0f));

		// Bottom Face
		mRenderSystem->texCoord(k::vector2(1.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f, -1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f, -1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f, -1.0f,  1.0f));	
		mRenderSystem->texCoord(k::vector2(1.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f, -1.0f,  1.0f));

		// Right face
		mRenderSystem->texCoord(k::vector2(1.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f, -1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f,  1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f,  1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f, -1.0f,  1.0f));

		// Left Face
		mRenderSystem->texCoord(k::vector2(0.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f, -1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f, -1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f,  1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f,  1.0f, -1.0f));

		// End
		mRenderSystem->endVertices();

		// Upload to video
		mRenderSystem->frameEnd();

		// Update Rotations
		#ifndef __WII__
		rquad += 0.08f;
		#else
		rquad += 0.5f;
		#endif
	}

	delete appRoot;
	return 0;
}

