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

#include "knowledge.h"

k::renderSystem* mRenderSystem;
k::sticker* texSticker;
k::material* crateMaterial;
vec_t rquad;

static inline void drawScene(bool rtt)
{
	// Clear screen, etc...
	mRenderSystem->frameStart();

	mRenderSystem->setMatrixMode(k::MATRIXMODE_PROJECTION);
	mRenderSystem->setPerspective(110, 1.33f, 0.1f, 5000.0f);

	mRenderSystem->setMatrixMode(k::MATRIXMODE_MODELVIEW);
	mRenderSystem->identityMatrix();
	mRenderSystem->translateScene(0.0f, 0.0f, -4.0f);
	mRenderSystem->rotateScene(30, 1.0f, 0.0f, 0.0f);
	mRenderSystem->rotateScene(rquad, 0.0f, 1.0f, 0.0f);

	// Bind Texture
	crateMaterial->start();

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
	crateMaterial->finish();

	if (!rtt)
	{
		mRenderSystem->setMatrixMode(k::MATRIXMODE_MODELVIEW);
		mRenderSystem->identityMatrix();

		mRenderSystem->setMatrixMode(k::MATRIXMODE_PROJECTION);
		mRenderSystem->setOrthographic(0, mRenderSystem->getScreenWidth(), 
			mRenderSystem->getScreenHeight(), 0, -128, 128);

		texSticker->draw();
	}

	// Upload to video
	mRenderSystem->frameEnd();
}

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char** argv)
#endif
{
	// Initialize knowledge
	k::root* appRoot = new k::root();
	k::materialManager* mMaterialManager = appRoot->getMaterialManager();
	k::inputManager* mInputManager = appRoot->getInputManager();
	mRenderSystem = appRoot->getRenderSystem();

	// Doesnt matter on wii
	mRenderSystem->createWindow(800, 600);
	mRenderSystem->setWindowTitle("knowledge, the power of mind");

	#ifdef __WII__
	k::resourceManager* resourceMgr = new k::resourceManager("/knowledge/resources.cfg");
	#else
	k::resourceManager* resourceMgr = new k::resourceManager("../resources.cfg");
	#endif

	// Loading Screen
	k::imgLoadScreen* newLoadingScreen = new k::imgLoadScreen();
	kAssert(newLoadingScreen);

	resourceMgr->setLoadingScreen(newLoadingScreen);
	newLoadingScreen->loadBg("loading.jpg");
	newLoadingScreen->setImgDimension(k::vector2(256, 256));
	newLoadingScreen->update("");

	// Create Texture for RTT
	platformTexturePointer rttTarget;
	mRenderSystem->genTexture(128, 128, 3, &rttTarget);
	mRenderSystem->setRttSize(128, 128);

	// Create Material for RTT
	try
	{
		k::texture* newTexture = new k::texture(&rttTarget, 128, 128, k::TEX_RGB);
		k::material* rttMaterial = mMaterialManager->createMaterial("rttMaterial", newTexture);
		kAssert(rttMaterial);
	}

	catch (...)
	{
		K_LOG_INFO("Failed to create texture material.");
		return 0;
	}

	// Create a sticker ;)
	texSticker = new k::sticker("rttMaterial");
	kAssert(texSticker);

	texSticker->setScale(k::vector2(128, 128));
	
	// Load textures
	k::resourceManager::getSingleton().loadGroup("common");
	k::resourceManager::getSingleton().loadGroup("texture");

	delete newLoadingScreen;

	/**
	 * Setup the input Manager
	 */
	kAssert(mInputManager);
	mInputManager->initWii(false);
	mInputManager->setupWiiMotes(1);
	mInputManager->setWiiMoteTimeout(60);
	mInputManager->setWiiMoteEmulation(true);

	// Get material
	crateMaterial = mMaterialManager->getMaterial("crate");
	kAssert(crateMaterial);

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

		// Draw
		mRenderSystem->setViewPort(0, 0, 128, 128);
		mRenderSystem->setRttTarget(&rttTarget);
		mRenderSystem->setRenderToTexture(true);
		drawScene(true);

		mRenderSystem->setViewPort(0, 0, mRenderSystem->getScreenWidth(), mRenderSystem->getScreenHeight());
		drawScene(false);
	
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

