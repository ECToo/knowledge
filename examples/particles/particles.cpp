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
	_break();

	// Initialize knowledge
	k::root* appRoot = new k::root();
	k::renderSystem* mRenderSystem = appRoot->getRenderSystem();
	k::renderer* mRenderer = appRoot->getRenderer();
	k::guiManager* mGuiManager = appRoot->getGuiManager();
	k::inputManager* mInputManager = appRoot->getInputManager();

	assert(mRenderer != NULL);
	assert(mRenderSystem != NULL);

	// Doesnt matter on wii
	mRenderSystem->createWindow(800, 600);
	mRenderSystem->setWindowTitle("knowledge, the power of mind");
	mRenderSystem->setClearColor(k::vector3(0.125, 0.349, 0.505));

	// Common library
	#ifdef __WII__
	k::resourceManager* resourceMgr = new k::resourceManager("/knowledge/resources.cfg");
	#else
	k::resourceManager* resourceMgr = new k::resourceManager("../resources.cfg");
	#endif

	// Loading Screen
	k::imgLoadScreen* newLoadingScreen = new k::imgLoadScreen();
	assert(newLoadingScreen);

	resourceMgr->setLoadingScreen(newLoadingScreen);
	newLoadingScreen->loadBg("loading.png");
	newLoadingScreen->setImgDimension(k::vector2(256, 256));
	newLoadingScreen->update("");

	k::resourceManager::getSingleton().loadGroup("common");
	k::resourceManager::getSingleton().loadGroup("particles");

	delete newLoadingScreen;

	// Set the skybox
	mRenderer->setSkyPlane("skyPlane");

	// Parse material file
	assert(mGuiManager != NULL);
	mGuiManager->setCursor("wiiCursor3", k::vector2(48, 48));

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
	newCamera->setPosition(k::vector3(0, 0, 0));
	newCamera->lookAt(k::vector3(0, 0, -1));
	mRenderer->setCamera(newCamera);

	// Font for frustum culling
	k::bitmapText* fpsText = new k::bitmapText("fonts/cube_14.dat", "cube_14");
	assert(fpsText != NULL);

	fpsText->setPosition(k::vector2(10, 10));
	mRenderer->push2D(fpsText);

	bool running = true;
	bool leftHold = false;

	while (running)
	{
		#ifdef WIN32
		Sleep(1000);
		#else
			#ifndef __WII__
			usleep(1000);
			#endif
		#endif

		mInputManager->feed();

		// User clicked on Close Window
		if (mInputManager->getQuitEvent() || mInputManager->getKbdKeyDown(K_KBD_ESCAPE))
			running = false;

		if ((mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_LEFT)) ||
			 mInputManager->getKbdKeyDown(K_KBD_LEFT))
		{
			k::quaternion dirQuat(1, k::vector3(0, 1, 0));
			k::quaternion ori = newCamera->getOrientation();

			newCamera->setOrientation(dirQuat * ori);
		}

		if ((mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_RIGHT)) ||
			 mInputManager->getKbdKeyDown(K_KBD_RIGHT))
		{
			k::quaternion dirQuat(-1, k::vector3(0, 1, 0));
			k::quaternion ori = newCamera->getOrientation();

			newCamera->setOrientation(dirQuat * ori);
		}

		if ((mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_UP) ||
			 mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_DOWN)) ||
			(mInputManager->getKbdKeyDown(K_KBD_w) ||
			 mInputManager->getKbdKeyDown(K_KBD_s)))
		{
			k::vector3 look = newCamera->getDirection();
			k::vector3 pos = newCamera->getPosition();

			if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_DOWN) ||
				 mInputManager->getKbdKeyDown(K_KBD_s))
			{
				pos -= look*2;
			}
			else
			{
				pos += look*2;
			}

			newCamera->setPosition(pos);
		}

		if (mInputManager->getKbdKeyDown(K_KBD_LCTRL) ||
			 mInputManager->getKbdKeyDown(K_KBD_SPACE))
		{
			k::vector3 up = newCamera->getUp();
			k::vector3 pos = newCamera->getPosition();

			if (mInputManager->getKbdKeyDown(K_KBD_LCTRL))
			{
				pos -= up * 2;
			}
			else
			{
				pos += up * 2;
			}

			newCamera->setPosition(pos);
		}

		if (mInputManager->getKbdKeyDown(K_KBD_a) ||
			 mInputManager->getKbdKeyDown(K_KBD_d))
		{
			k::vector3 right = newCamera->getRight();
			k::vector3 pos = newCamera->getPosition();

			if (mInputManager->getKbdKeyDown(K_KBD_a))
			{
				pos -= right * 2;
			}
			else
			{
				pos += right * 2;
			}

			newCamera->setPosition(pos);
		}


		// Quit Application
		if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_HOME))
		{
			running = false;
		}

		// Set Demo FPS
		std::stringstream fps;
		fps << "FPS: " << mRenderer->getLastFps();
		fps << "\nINSIDE FRUSTUM: ";

		if (newCamera->isPointInsideFrustum(k::vector3(0, -40, -10)))
			fps << "YES";
		else
			fps << "NO";

		fpsText->setText(fps.str());

		k::vector2 mousePos = mInputManager->getWiiMotePosition(0);
		mGuiManager->setCursorPos(mousePos);

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

		// Physics Loop
		mRenderer->draw();
	}

	delete appRoot;
	return 0;
}
