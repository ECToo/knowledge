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
#include "md5.h"
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

	// Parse material file
	#ifdef __WII__
	k::parsingFile* matFile = new k::parsingFile("/knowledge/goku/goku.material");
	#else
	k::parsingFile* matFile = new k::parsingFile("goku.material");
	#endif

	mMaterialManager->parseMaterialScript(matFile);

	// Create Model
	k::vector3 modelPosition;
	modelPosition.z = -100;

	#ifdef __WII__
	k::md5model* newModel = new k::md5model("/knowledge/goku/goku.md5mesh");
	#else
	k::md5model* newModel = new k::md5model("goku.md5mesh");
	#endif

	assert(newModel != NULL);
	mRenderer->push3D(newModel);

	assert(mGuiManager != NULL);
	mGuiManager->setCursor("wiiCursor", k::vector2(32, 32));

	/**
	 * Setup the input Manager
	 */
	assert(mInputManager != NULL);
	mInputManager->initWii(false);
	mInputManager->setupWiiMotes(1);
	mInputManager->setWiiMoteTimetout(60);
	mInputManager->setWiiMoteEmulation(true);

	// Angles
	int rX = 0;
	int rY = 0;
	int lastX = 0; 
	int lastY = 0;
	int dX = 0;
	int dY = 0;

	bool running = true;
	while (running)
	{
		mInputManager->feed();

		// User clicked on Close Window
		if (mInputManager->getQuitEvent() || mInputManager->getKbdKeyDown(K_KBD_ESCAPE))
			running = false;

		if (mInputManager->getKbdKeyDown(K_KBD_LEFT))
			modelPosition.x++;
		else
		if (mInputManager->getKbdKeyDown(K_KBD_RIGHT))
			modelPosition.x--;

		if (mInputManager->getKbdKeyDown(K_KBD_UP))
			modelPosition.y++;
		else
		if (mInputManager->getKbdKeyDown(K_KBD_DOWN))
			modelPosition.y--;

		if (mInputManager->getKbdKeyDown(K_KBD_a) || mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_PLUS))
			modelPosition.z += 0.3;
		else
		if (mInputManager->getKbdKeyDown(K_KBD_z) || mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_MINUS))
			modelPosition.z -= 0.3;

		if (mInputManager->getKbdKeyDown(K_KBD_i) || mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_UP))
			rY += 1;
		else
		if (mInputManager->getKbdKeyDown(K_KBD_k) || mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_DOWN))
			rY -= 1;

		if (mInputManager->getKbdKeyDown(K_KBD_j) || mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_RIGHT))
			rX += 1;
		else
		if (mInputManager->getKbdKeyDown(K_KBD_l) || mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_LEFT))
			rX -= 1;

		k::vector2 mousePos = mInputManager->getWiiMotePosition(0);
		mGuiManager->setCursorPos(mousePos);

		dX = mousePos.x - lastX;
		dY = mousePos.y - lastY;

		#ifndef __WII__
		dY = -dY;
		#endif

		lastX = mousePos.x;
		lastY = mousePos.y;

		if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_A))
		{
			rX += dX;
			rY += dY;
		}
		else
		if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_B))
		{
			modelPosition.x += dX;
			modelPosition.y -= dY;
		}

		// Quit Application
		if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_HOME))
		{
			running = false;
		}

		// Model Rotation
		k::quaternion yQuat = k::quaternion(rY, k::vector3(1, 0, 0));
		k::quaternion xQuat = k::quaternion(rX, k::vector3(0, 1, 0));
		k::quaternion modelQuat = xQuat * yQuat;

		newModel->setPosition(modelPosition);
		newModel->setOrientation(modelQuat);

		mRenderer->draw();
	}

	delete appRoot;
	return 0;
}

