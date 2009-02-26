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

	k::resourceManager::getSingleton().loadGroup("model");
	k::resourceManager::getSingleton().loadGroup("common");

	// Load the Model
	k::vector3 modelPosition;
	modelPosition.z = -100;

	#ifdef __WII__
	// k::md5model* newModel = new k::md5model("sd:/knowledge/model/goku.md5mesh");
	k::md5model* newModel = new k::md5model("sd:/knowledge/model/torus.md5mesh");
	#else
	// k::md5model* newModel = new k::md5model("goku.md5mesh");
	k::md5model* newModel = new k::md5model("torus.md5mesh");
	#endif

	kAssert(newModel);
	newModel->getMesh(0)->setMaterial("donutMetal");

	/*
	#ifdef __WII__
	newModel->attachAnimation("sd:/knowledge/model/idle.md5anim", "idle");
	newModel->attachAnimation("sd:/knowledge/model/fly_f.md5anim", "runf");
	newModel->attachAnimation("sd:/knowledge/model/fly_b.md5anim", "runb");
	#else
	newModel->attachAnimation("idle.md5anim", "idle");
	newModel->attachAnimation("fly_f.md5anim", "runf");
	newModel->attachAnimation("fly_b.md5anim", "runb");
	#endif
	*/

	delete newLoadingScreen;

	// Set Skyplane
	mRenderer->setSkyPlane("skyPlane");

	// Fps Counter
	k::bitmapText* fpsText = new k::bitmapText("fonts/cube_14.dat", "cube_14");
	assert(fpsText != NULL);
	fpsText->setPosition(k::vector2(4, 10));
	mRenderer->push2D(fpsText);

	/*
	// Set Model animations
	newModel->setAnimation("idle");
	newModel->setAnimationFrame(10);
	*/
	
	mRenderer->push3D(newModel);

	assert(mGuiManager != NULL);
	mGuiManager->setCursor("wiiCursor3", k::vector2(32, 32));

	// Angles
	int rX = 0;
	int rY = 0;
	int lastX = 0; 
	int lastY = 0;
	int dX = 0;
	int dY = 0;

	// Screenshot
	bool oneHold = false;
	int scrCont = 0;

	// Anims
	bool runf = false;
	bool runb = false;
	bool idle = true;

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
		{
			if (!runf)
			{
				newModel->setAnimation("runf");
				runf = true;
				runb = idle = false;
			}
		}
		else
		if (mInputManager->getKbdKeyDown(K_KBD_DOWN))
		{
			if (!runb)
			{
				newModel->setAnimation("runb");
				runb = true;
				runf = idle = false;
			}
		}
		else
		{
			if (!idle)
			{
				newModel->setAnimation("idle");
				idle = true;
				runf = runb = false;
			}
		}

		if (mInputManager->getKbdKeyDown(K_KBD_a) || mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_PLUS))
		{
			#ifdef __WII__
			modelPosition.z += 0.5;
			#else
			modelPosition.z += 0.1;
			#endif
		}
		else
		if (mInputManager->getKbdKeyDown(K_KBD_z) || mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_MINUS))
		{
			#ifdef __WII__
			modelPosition.z -= 0.5;
			#else
			modelPosition.z -= 0.1;
			#endif
		}

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
		dY = lastY - mousePos.y;

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
			modelPosition.y += dY;
		}

		if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_1) || mInputManager->getKbdKeyDown(K_KBD_F12))
		{
			oneHold = true;
		}
		else
		if (oneHold)
		{
			oneHold = false;

			std::stringstream shot;
			shot << k::resourceManager::getSingleton().getRoot();
			shot << "screenshot_" << scrCont++ << ".jpg";

			mRenderSystem->screenshot(shot.str().c_str());
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

		std::stringstream fpsT;
		fpsT << "fps: " << mRenderer->getLastFps();
		fpsText->setText(fpsT.str());

		mRenderer->draw();
	}

	// Clean up
	// _break();
	
	delete newModel;
	delete fpsText;
	delete resourceMgr;
	delete appRoot;

	return 0;
}

