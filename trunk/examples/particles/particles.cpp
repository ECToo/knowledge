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
#include "renderer.h"
#include "rendersystem.h"
#include "logger.h"
#include "resourceManager.h"
#include "particle.h"

int main(int argc, char** argv)
{
	// Initialize knowledge
	k::root* appRoot = new k::root();
	k::renderSystem* mRenderSystem = appRoot->getRenderSystem();
	k::renderer* mRenderer = appRoot->getRenderer();
	k::materialManager* mMaterialManager = appRoot->getMaterialManager();
	k::guiManager* mGuiManager = appRoot->getGuiManager();
	k::inputManager* mInputManager = appRoot->getInputManager();

	assert(mRenderer != NULL);
	assert(mRenderSystem != NULL);

	// Doesnt matter on wii
	mRenderSystem->createWindow(800, 600);
	mRenderSystem->setDepthTest(true);

	// Common library
	#ifdef __WII__
	new k::resourceManager("/knowledge/resources.cfg");
	#else
	new k::resourceManager("../resources.cfg");
	#endif

	k::resourceManager::getSingleton().loadGroup("common");
	k::resourceManager::getSingleton().loadGroup("particles");

	// Parse material file
	assert(mGuiManager != NULL);
	mGuiManager->setCursor("wiiCursor", k::vector2(48, 48));

	/**
	 * Setup the input Manager
	 */
	assert(mInputManager != NULL);
	mInputManager->initWii(false);
	mInputManager->setupWiiMotes(1);
	mInputManager->setWiiMoteTimeout(60);
	mInputManager->setWiiMoteEmulation(true);

	// Setup Camera
	k::camera* newCamera = new k::camera();
	assert(newCamera != NULL);
	newCamera->setPosition(k::vector3(0, 0, 10));
	newCamera->lookAt(k::vector3(0, 0, -1));
	mRenderer->setCamera(newCamera);

	// Particles
	k::pointEmitter* pE = new k::pointEmitter(2000, "poison");
	assert(pE != NULL);

	k::particleSystem* pS = new k::particleSystem();
	assert(pS != NULL);

	k::vector3 pVel = k::vector3(0, 20, 0);
	pE->setVelocity(pVel);
	pE->setRadius(4);
	pE->setSpawnQuantity(10);
	pE->setSpawnTime(100);
	pE->setLifeTime(2000);

	k::vector3 pSPos = k::vector3(0, 0, -50);
	pS->setPosition(pSPos);
	pS->pushEmitter("test", pE);
	mRenderer->pushParticle(pS);

	bool running = true;
	bool leftHold = false;
	vec_t sprAngle = 0;

	while (running)
	{
		mInputManager->feed();

		// User clicked on Close Window
		if (mInputManager->getQuitEvent() || mInputManager->getKbdKeyDown(K_KBD_ESCAPE))
			running = false;

		// Quit Application
		if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_HOME))
		{
			running = false;
		}

		if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_B))
		{
			leftHold = true;
		}
		else
		if (leftHold)
		{
			// Do Something
			leftHold = false;
		}

		k::vector2 mousePos = mInputManager->getWiiMotePosition(0);
		mGuiManager->setCursorPos(mousePos);

		// Physics Loop
		mRenderer->draw();
	}

	delete appRoot;
	return 0;
}

