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
	k::parsingFile* commonMaterialFile = new k::parsingFile("/knowledge/common.material");
	#else
	k::parsingFile* commonMaterialFile = new k::parsingFile("common.material");
	#endif

	assert(commonMaterialFile != NULL);
	mMaterialManager->parseMaterialScript(commonMaterialFile);

	#ifdef __WII__
	k::parsingFile* particlesFile = new k::parsingFile("/knowledge/particles/particles.material");
	#else
	k::parsingFile* particlesFile = new k::parsingFile("particles.material");
	#endif

	assert(particlesFile != NULL);
	mMaterialManager->parseMaterialScript(particlesFile);

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
	newCamera->setPosition(k::vector3(0, 0, 20));
	newCamera->lookAt(k::vector3(0, 0, -1));
	mRenderer->setCamera(newCamera);

	// Sprite
	k::vector3 sprPos;
	k::sprite* newSpr = mRenderer->createSprite(10, mMaterialManager->getMaterial("poison"));
	assert(newSpr != NULL);

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

		sprAngle += 0.01;
		sprPos.x = cos(sprAngle)*10;
		sprPos.y = 0;
		sprPos.z = sin(sprAngle)*10 - 20;
		newSpr->setPosition(sprPos);

		k::vector2 mousePos = mInputManager->getWiiMotePosition(0);
		mGuiManager->setCursorPos(mousePos);

		// Physics Loop
		mRenderer->draw();
	}

	delete appRoot;
	return 0;
}

