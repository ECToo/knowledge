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
	k::guiManager* mGuiManager = appRoot->getGuiManager();
	k::inputManager* mInputManager = appRoot->getInputManager();

	assert(mRenderer != NULL);
	assert(mRenderSystem != NULL);

	// Doesnt matter on wii
	mRenderSystem->createWindow(800, 600);
	mRenderSystem->setWindowTitle("knowledge, the power of mind");

	// Input Manager
	assert(mInputManager != NULL);
	mInputManager->initWii(false);
	mInputManager->setupWiiMotes(1);
	mInputManager->setWiiMoteTimeout(60);
	mInputManager->setWiiMoteEmulation(true);
	// mInputManager->setPointerLock(true);

	// Initialize resources
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
	k::resourceManager::getSingleton().loadGroup("bsp");

	k::q3Bsp testeBsp;
	testeBsp.loadQ3Bsp("tc_closecombat.bsp");

	mRenderer->setWorld(&testeBsp);

	delete newLoadingScreen;

	// Set Skyplane
	// mRenderer->setSkyPlane("skyPlane");
	mRenderer->setSkyPlane("q3tc_nightsky2_lf");

	// Setup Camera
	k::camera* newCamera = new k::camera();
	assert(newCamera != NULL);
	newCamera->setPlanes(0.1f, 5000.0f);
	newCamera->setPosition(k::vector3(0, 0, 0));
	newCamera->lookAt(k::vector3(0, 0, -1));
	mRenderer->setCamera(newCamera);

	// Fps Counter
	k::bitmapText* fpsText = new k::bitmapText("fonts/cube_14.dat", "cube_14");
	assert(fpsText != NULL);
	fpsText->setPosition(k::vector2(4, 10));
	mRenderer->push2D(fpsText);

	assert(mGuiManager != NULL);
	mGuiManager->setCursor("wiiCursor3", k::vector2(32, 32));
	mGuiManager->setCursorPos(k::vector2(mRenderSystem->getScreenWidth()/2, mRenderSystem->getScreenHeight()/2));	

	bool L_Hold = false;
	bool E_Hold = false;
	bool wireframe = false;
	bool running = true;

	mInputManager->feed();
	k::vector2 oldMousePos = mInputManager->getWiiMotePosition(0);

	while (running)
	{
		mInputManager->feed();

		// User clicked on Close Window
		if (mInputManager->getQuitEvent() || mInputManager->getKbdKeyDown(K_KBD_ESCAPE))
			running = false;

		// Feed Mouse
		k::vector2 mousePos = mInputManager->getWiiMotePosition(0);

		// Quit Application
		if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_HOME))
		{
			running = false;
		}

		// Lightmaps
		if (mInputManager->getKbdKeyDown(K_KBD_l))
		{
			L_Hold = true;
		}
		else
		if (L_Hold)
		{
			L_Hold = false;
			testeBsp.setLightmapsDrawing(testeBsp.isDrawingLightmaps() ^ 1);
		}

		// Wireframe
		if (mInputManager->getKbdKeyDown(K_KBD_e))
		{
			E_Hold = true;
		}
		else
		if (E_Hold)
		{
			E_Hold = false;

			// Set Inverted wireframe
			wireframe ^= 1;
			mRenderSystem->setWireFrame(wireframe);
		}

		// Camera
		if ((mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_A) ||
			 mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_B)) ||
			(mInputManager->getKbdKeyDown(K_KBD_w) ||
			 mInputManager->getKbdKeyDown(K_KBD_s)))
		{
			k::vector3 look = newCamera->getDirection();
			k::vector3 pos = newCamera->getPosition();

			if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_B) ||
				 mInputManager->getKbdKeyDown(K_KBD_s))
			{
				pos -= look * 5;
			}
			else
			{
				pos += look * 5;
			}

			k::particleSystem* ps = k::particleManager::getSingleton().getParticleSystem("rain");
			if (ps)
			{
				k::vector3 psPos = ps->getPosition();
				psPos.x = pos.x;
				psPos.z = pos.z;

				ps->setPosition(psPos);
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

			k::particleSystem* ps = k::particleManager::getSingleton().getParticleSystem("rain");
			if (ps)
			{
				k::vector3 psPos = ps->getPosition();
				psPos.x = pos.x;
				psPos.z = pos.z;

				ps->setPosition(psPos);
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

		k::vector2 diffMousePos = mousePos - oldMousePos;
		if (diffMousePos.x)
		{
			k::quaternion dir1Quat(-diffMousePos.x, newCamera->getUp());
			k::quaternion dir2Quat(-diffMousePos.y, newCamera->getRight());

			k::quaternion ori = newCamera->getOrientation();
			k::quaternion finalRot = dir2Quat * dir1Quat;
			finalRot.normalize();

			newCamera->setOrientation(finalRot * ori);
		}
		oldMousePos = mousePos;

		// TEMP
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

		if (mInputManager->getKbdKeyDown(K_KBD_UP))
		{
			k::quaternion dirQuat(1, newCamera->getRight());
			k::quaternion ori = newCamera->getOrientation();

			newCamera->setOrientation(dirQuat * ori);
		}
		if (mInputManager->getKbdKeyDown(K_KBD_DOWN))
		{
			k::quaternion dirQuat(-1, newCamera->getRight());
			k::quaternion ori = newCamera->getOrientation();

			newCamera->setOrientation(dirQuat * ori);
		}

		// FPS Counter
		std::stringstream fpsT;
		fpsT << "fps: " << mRenderer->getLastFps();
		fpsText->setText(fpsT.str());

		// Draw Everything
		mRenderer->draw();
	}

	delete newCamera;
	delete fpsText;
	delete resourceMgr;
	delete appRoot;

	return 0;
}

