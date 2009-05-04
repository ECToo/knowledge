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

#include "knowledge.h"

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char** argv)
#endif
{
	// Initialize knowledge
	k::root* appRoot = new k::root();
	k::renderSystem* mRenderSystem = appRoot->getRenderSystem();
	k::renderer* mRenderer = appRoot->getRenderer();
	k::materialManager* mMaterialManager = appRoot->getMaterialManager();
	k::guiManager* mGuiManager = appRoot->getGuiManager();
	k::inputManager* mInputManager = appRoot->getInputManager();

	kAssert(mRenderer);
	kAssert(mRenderSystem);

	// Doesnt matter on wii
	mRenderSystem->createWindow(800, 600);
	mRenderSystem->setWindowTitle("knowledge, the power of mind");
	mRenderSystem->setDepthTest(true);

	// Common library
	#ifdef __WII__
	k::resourceManager* resourceMgr = new k::resourceManager("/knowledge/resources.cfg");
	#else
	k::resourceManager* resourceMgr = new k::resourceManager("../resources.cfg");
	#endif

	// Loading Screen
	k::imgLoadScreen* newLoadingScreen = new k::imgLoadScreen();
	kAssert(newLoadingScreen);

	resourceMgr->setLoadingScreen(newLoadingScreen);
	newLoadingScreen->loadBg("loading.png");
	newLoadingScreen->setImgDimension(k::vector2(256, 256));
	newLoadingScreen->update("");

	k::resourceManager::getSingleton().loadGroup("common");
	k::resourceManager::getSingleton().loadGroup("skies");
	k::resourceManager::getSingleton().loadGroup("lightmap");

	mRenderer->setSkyPlane("skyPlane");

	k::md5model* boxscene = new k::md5model("lightmap/boxscene.md5mesh");
	mRenderer->push3D(boxscene);

	// Parse material file
	kAssert(mGuiManager);
	mGuiManager->setCursor("wiiCursor", k::vector2(48, 48));

	/**
	 * Setup the input Manager
	 */
	kAssert(mInputManager);
	mInputManager->initWii(false);
	mInputManager->setupWiiMotes(1);
	mInputManager->setWiiMoteTimeout(60);
	mInputManager->setWiiMoteEmulation(true);

	// Setup Camera
	k::camera* newCamera = new k::camera();
	kAssert(newCamera);

	#define CAM_DIST 30
	newCamera->setPosition(k::vector3(CAM_DIST, CAM_DIST / 2, 0));
	newCamera->lookAt(k::vector3(0, 0, 0));
	mRenderer->setCamera(newCamera);

	// FPS
	k::bitmapText* fpsText = new k::bitmapText("fonts/cube_14.dat", "cube_14");
	assert(fpsText != NULL);

	fpsText->setPosition(k::vector2(10, 10));
	mRenderer->push2D(fpsText);

	bool running = true;
	bool leftHold = false;
	bool lightmap = false;
	vec_t camAngle = 0;
	k::vector2 lastMousePos;

	delete newLoadingScreen;

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
			camAngle += mouseDiff.x / 60.0f;

			k::vector3 camPos = newCamera->getPosition();
			camPos.x = cos(camAngle) * CAM_DIST;
			camPos.y = CAM_DIST / 2;
			camPos.z = sin(camAngle) * CAM_DIST;

			newCamera->setPosition(camPos);
			newCamera->lookAt(k::vector3(0, 0, 0));

			lastMousePos = mInputManager->getWiiMotePosition(0);
		}
		mGuiManager->setCursorPos(mousePos);

		// Set Demo FPS
		std::stringstream fps;
		fps << "FPS: " << mRenderer->getLastFps();
		fpsText->setText(fps.str());

		// Physics Loop
		mRenderer->draw();
	}

	delete appRoot;
	return 0;
}

