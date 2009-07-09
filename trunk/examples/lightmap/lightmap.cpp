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
	k::root* appRoot;
	try
	{
		appRoot = new k::root();
	}

	catch (...)
	{
		printf("Failed to create app root.");
		return 0;
	}

	k::renderSystem* mRenderSystem = appRoot->getRenderSystem();
	k::renderer* mRenderer = appRoot->getRenderer();
	k::guiManager* mGuiManager = appRoot->getGuiManager();
	k::inputManager* mInputManager = appRoot->getInputManager();
	k::materialManager* mMaterialManager = appRoot->getMaterialManager();

	// Doesnt matter on wii
	mRenderSystem->createWindow(800, 600);
	mRenderSystem->setWindowTitle("knowledge, the power of mind");

	// Input Manager
	mInputManager->initWii(false);
	mInputManager->setupWiiMotes(1);
	mInputManager->setWiiMoteTimeout(60);
	mInputManager->setWiiMoteEmulation(true);
	mInputManager->setPointerLock(false);

	#ifdef __WII__
	chdir("sd:/knowledge/model/");
	#endif

	// Initialize resources
	k::resourceManager* resourceMgr;
	try
	{
		resourceMgr = new k::resourceManager("../resources.cfg");
	}

	catch (...)
	{
		K_LOG_INFO("Failed to allocate resource manager.");
		return 0;
	}

	// Loading Screen
	k::imgLoadScreen* newLoadingScreen;
	try
	{
		newLoadingScreen = new k::imgLoadScreen();
	}

	catch (...)
	{
		K_LOG_INFO("Failed to create loading screen.");
		return 0;
	}

	resourceMgr->setLoadingScreen(newLoadingScreen);
	newLoadingScreen->loadBg("loading.jpg");
	newLoadingScreen->setImgDimension(k::vector2(256, 256));
	newLoadingScreen->update("");

	k::resourceManager::getSingleton().loadGroup("common");
	k::resourceManager::getSingleton().loadGroup("skies");
	k::resourceManager::getSingleton().loadGroup("lightmap");

	mRenderer->setSkyPlane("skyPlane");

	k::md5model* newModel;
	try
	{
		newModel = new k::md5model("lightmap/boxscene.md5mesh");
		mRenderer->push3D(newModel);
	}

	catch (...)
	{
		K_LOG_INFO("Failed to allocate new model.");
		return 0;
	}

	// Parse material file
	mGuiManager->setCursor("wiiCursor3", k::vector2(32, 32));

	// Setup Camera
	k::camera* newCamera;
	try
	{
		newCamera = new k::camera();
	
		#define CAM_DIST 30
		newCamera->setPosition(k::vector3(CAM_DIST, CAM_DIST / 2, 0));
		newCamera->lookAt(k::vector3(0, 0, 0));
		mRenderer->setCamera(newCamera);
	}

	catch (...)
	{
		K_LOG_INFO("Failed to create scene camera.");
		return 0;
	}

	// Fps Counter
	k::bitmapText* fpsText;
	try
	{
		fpsText = new k::bitmapText("fonts/cube_14.dat", "cube_14");
		fpsText->setPosition(k::vector2(4, 10));
		mRenderer->push2D(fpsText);
	}

	catch (...)
	{
		K_LOG_INFO("Failed to allocate fps text box.");
		return 0;
	}

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
				k::md5mesh* m1 = newModel->getMesh(0);
				if (m1) m1->setMaterial(mMaterialManager->getMaterial("boxLitShader"));

				k::md5mesh* m2 = newModel->getMesh(1);
				if (m2) m2->setMaterial(mMaterialManager->getMaterial("planeLitShader"));

				lightmap = true;
			}
			else
			{
				k::md5mesh* m1 = newModel->getMesh(0);
				if (m1) m1->setMaterial(mMaterialManager->getMaterial("boxShader"));

				k::md5mesh* m2 = newModel->getMesh(1);
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

		// Loop
		mRenderer->draw();
	}
	
	delete newCamera;
	delete newModel;
	delete fpsText;
	delete resourceMgr;
	delete appRoot;

	return 0;
}

