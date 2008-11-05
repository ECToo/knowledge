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
#include "resourceManager.h"

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
	k::resourceManager::getSingleton().loadGroup("lightmap");

	#ifdef __WII__
	k::parsingFile* tevFile = new k::parsingFile("/knowledge/tev.script");

	assert(tevFile != NULL);
	k::tevManager::getSingleton().parseTevScript(tevFile);
	#endif

	#ifdef __WII__
	k::md5model* boxscene = new k::md5model("/knowledge/lightmap/boxscene.md5mesh");
	#else
	k::md5model* boxscene = new k::md5model("boxscene.md5mesh");
	#endif

	assert(boxscene != NULL);
	mRenderer->push3D(boxscene);

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
	newCamera->setPosition(k::vector3(10, 8, 0));
	newCamera->lookAt(k::vector3(0, 0, 0));
	mRenderer->setCamera(newCamera);

	bool running = true;
	bool leftHold = false;
	bool lightmap = false;
	vec_t camAngle = 0;
	k::vector2 lastMousePos;

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
			// Action
			if (!lightmap)
			{
				k::md5mesh* m1 = boxscene->getMesh(0);
				if (m1) m1->setMaterial(mMaterialManager->getMaterial("boxLitShader"));

				k::md5mesh* m2 = boxscene->getMesh(1);
				if (m2) m2->setMaterial(mMaterialManager->getMaterial("planeLitShader"));

				lightmap = true;
			}
			else
			{
				k::md5mesh* m1 = boxscene->getMesh(0);
				if (m1) m1->setMaterial(mMaterialManager->getMaterial("boxShader"));

				k::md5mesh* m2 = boxscene->getMesh(1);
				if (m2) m2->setMaterial(mMaterialManager->getMaterial("planeShader"));

				lightmap = false;
			}

			leftHold = false;
		}

		k::vector2 mousePos = mInputManager->getWiiMotePosition(0);
		if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_A))
		{
			k::vector2 mouseDiff = mousePos - lastMousePos;
			
			// relative mouse pos
			camAngle += mouseDiff.x/30.0f;

			k::vector3 camPos = newCamera->getPosition();
			camPos.x = cos(camAngle)*10;
			camPos.y = 8;
			camPos.z = sin(camAngle)*10;

			newCamera->setPosition(camPos);
			newCamera->lookAt(k::vector3(0, 0, 0));

			lastMousePos = mInputManager->getWiiMotePosition(0);
		}
		mGuiManager->setCursorPos(mousePos);

		// Physics Loop
		mRenderer->draw();
	}

	delete appRoot;
	return 0;
}

