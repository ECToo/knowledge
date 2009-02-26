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

	delete newLoadingScreen;

	// Set Skyplane
	mRenderer->setSkyPlane("skyPlane");

	// Fps Counter
	k::bitmapText* fpsText = new k::bitmapText("fonts/cube_14.dat", "cube_14");
	assert(fpsText != NULL);
	fpsText->setPosition(k::vector2(4, 10));
	mRenderer->push2D(fpsText);

	assert(mGuiManager != NULL);
	mGuiManager->setCursor("wiiCursor3", k::vector2(32, 32));

	bool running = true;
	while (running)
	{
		mInputManager->feed();

		// User clicked on Close Window
		if (mInputManager->getQuitEvent() || mInputManager->getKbdKeyDown(K_KBD_ESCAPE))
			running = false;

		// Feed Mouse
		k::vector2 mousePos = mInputManager->getWiiMotePosition(0);
		mGuiManager->setCursorPos(mousePos);

		// Quit Application
		if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_HOME))
		{
			running = false;
		}

		// FPS Counter
		std::stringstream fpsT;
		fpsT << "fps: " << mRenderer->getLastFps();
		fpsText->setText(fpsT.str());

		// Draw Everything
		mRenderer->draw();
	}

	delete fpsText;
	delete resourceMgr;
	delete appRoot;

	return 0;
}

