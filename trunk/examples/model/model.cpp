/*
    Copyright (C) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

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

	#ifdef __WII__
	newLoadingScreen->loadBg("loading.tpl");
	#else
	newLoadingScreen->loadBg("loading.jpg");
	#endif

	newLoadingScreen->setImgDimension(k::vector2(256, 256));
	newLoadingScreen->update("");

	k::resourceManager::getSingleton().loadGroup("model");
	k::resourceManager::getSingleton().loadGroup("common");
	k::resourceManager::getSingleton().loadGroup("skies");
	
	// Angles
	int rX = 0;
	int rY = 0;

	// Load the Model
	k::vector3 modelPosition;
	modelPosition.z = -65;

	// k::light::light* tempLight = NULL;
	// k::light::light* tempLight2 = NULL;
	k::light::light* tempLight = mRenderer->createPointLight();
	k::light::light* tempLight2 = mRenderer->createPointLight();
	if (tempLight && tempLight2)
	{
		tempLight->setAmbient(k::color(0.0, 0.0, 0.0, 1.0));
		tempLight->setDiffuse(k::color(0.0, 0.0, 1.0, 1.0));
		tempLight->setSpecular(k::color(1.0, 1.0, 1.0, 1.0));

		tempLight2->setAmbient(k::color(0.0, 0.0, 0.0, 1.0));
		tempLight2->setDiffuse(k::color(1.0, 0.0, 0.0, 1.0));
		tempLight2->setSpecular(k::color(1.0, 1.0, 1.0, 1.0));
	}

	// md5 Model
	k::md5model* newModel = NULL;

	// md3 model(s)
	k::md3model* q3Model = NULL;
	k::md3model* q3ModelUpper = NULL;
	k::md3model* q3ModelHead = NULL;

	try
	{
		// Uncomment this out for marvin =]
		/*
		newModel = new k::md5model("model/marvin/marvin.md5mesh");
		newModel->attachAnimation("model/marvin/idle.md5anim", "idle");
		newModel->attachAnimation("model/marvin/walk.md5anim", "runf");
		newModel->attachAnimation("model/marvin/walk.md5anim", "runb");
		*/

		// Lets Say we want to change the model first mesh material to k_base_null material
		/*
		k::md5mesh* m1 = newModel->getMesh(0);
		if (m1) m1->setMaterial("k_base_null");
		*/

		// Uncomment this out for goku =]
		newModel = new k::md5model("model/goku.md5mesh");
		newModel->attachAnimation("model/idle.md5anim", "idle");
		newModel->attachAnimation("model/fly_f.md5anim", "runf");
		newModel->attachAnimation("model/fly_b.md5anim", "runb");

		rY = -90;
		rX = 20;

		modelPosition.y -= 50;

		newModel->setAnimation("idle");
		newModel->setAnimationFrame(0);
		mRenderer->push3D(newModel);

		/*
		 * You can uncomment those following lines to get MD3 working.
		 * I suggest you to download the monster model from knowledge Downloads
		 * section and try with it.
		 *
		 * If you want to see a Quake 3 Arena(tm) model working
		 * here, replace every occurrence of monster by xaero
		 * and copy over the xaero folder from pak0.pk3 (models/players/xaero)
		 * to the model folder. Keep in mind that you will need to create
		 * a new material script for it. Should look like this:
		 *
		 * material xaeroBody { texture { filename "model/xaero/red.tga" } }
		 * material xaeroHead { texture { filename "model/xaero/red_h.tga" } }
		 *
		q3Model = new k::md3model("model/monster/lower.md3");
		for (unsigned int i = 0; i < q3Model->getSurfacesCount(); i++)
			q3Model->getSurface(i)->setMaterial("monsterBody");

		q3ModelUpper = new k::md3model("model/monster/upper.md3");
		for (unsigned int i = 0; i < q3ModelUpper->getSurfacesCount(); i++)
			q3ModelUpper->getSurface(i)->setMaterial("monsterBody");

		q3ModelHead = new k::md3model("model/monster/head.md3");
		for (unsigned int i = 0; i < q3ModelHead->getSurfacesCount(); i++)
			q3ModelHead->getSurface(0)->setMaterial("monsterHead");
		
		// Got this from monster animation.cfg =]
		k::md3Animation_t* lowerAnim = q3Model->createAnimation("run");
		lowerAnim->firstFrame = 110;
		lowerAnim->numFrames = 9;
		lowerAnim->loopingFrames = 0;
		lowerAnim->framesPerSecond = 15;
		q3Model->setAnimation("run");

		q3Model->attach(q3ModelUpper, "tag_torso");
		q3ModelUpper->attach(q3ModelHead, "tag_head");
		q3ModelUpper->setFrame(100);

		mRenderer->push3D(q3Model);
		*/
	}
	
	catch (...)
	{
		K_LOG_INFO("Failed to allocate displaying model.");
		return 0;
	}
	
	delete newLoadingScreen;

	// Set Skyplane
	mRenderer->setSkyPlane("skyPlane");

	// Fps Counter
	k::bitmapText* fpsText;
	try
	{
		fpsText = new k::bitmapText("fonts/cube_14.dat", "cube_14");
		fpsText->setPosition(k::vector2(10, 15));
		fpsText->setZ(1);
		mRenderer->push2D(fpsText);
	}

	catch (...)
	{
		K_LOG_INFO("Failed to allocate fps text box.");
		return 0;
	}

	mGuiManager->setCursor("wiiCursor3", k::vector2(32, 32));
	mGuiManager->setSkin("guiSkin", "guiDef.xml");

	k::panelWidget* newPanel = new k::panelWidget(k::vector2(0, 0), k::vector2(370, 130));
	newPanel->setZ(0);
	mGuiManager->pushWidget(newPanel);
	mRenderer->push2D(newPanel);

	k::buttonWidget* newButton = new k::buttonWidget(k::vector2(50, 50), k::vector2(120, 30));
	newButton->setZ(1);
	newButton->setText("TESTE");
	mGuiManager->pushWidget(newButton);
	mRenderer->push2D(newButton);

	//
	k::vector2 cursorDelta;

	// Screenshot
	bool oneHold = false;
	int scrCont = 0;

	// Wire
	bool E_Hold = false;
	bool wireframe = false;

	// Bounding Boxes
	bool B_Hold = false;
	bool bounding = false;

	// Light
	bool V_Hold = false;

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

		// Light
		if (mInputManager->getKbdKeyDown(K_KBD_v))
		{
			V_Hold = true;
		}
		else
		if (V_Hold)
		{
			V_Hold = false;

			// Set Inverted wireframe
			if (tempLight)
				tempLight->setEnabled(tempLight->getEnabled() ^ 1);

			if (tempLight2)
				tempLight2->setEnabled(tempLight2->getEnabled() ^ 1);
		}

		// Bounding Boxes
		if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_2) || mInputManager->getKbdKeyDown(K_KBD_b))
		{
			B_Hold = true;
		}
		else
		if (B_Hold)
		{
			B_Hold = false;

			bounding ^= 1;

			if (newModel)
				newModel->setDrawBoundingBox(bounding);

			if (q3Model && q3ModelUpper && q3ModelHead)
			{
				q3Model->setDrawBoundingBox(bounding);
				q3ModelUpper->setDrawBoundingBox(bounding);
				q3ModelHead->setDrawBoundingBox(bounding);
			}
		}


		if (mInputManager->getKbdKeyDown(K_KBD_UP))
		{
			if (!runf)
			{
				if (newModel)
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
				if (newModel)
					newModel->setAnimation("runb");

				runb = true;
				runf = idle = false;
			}
		}
		else
		{
			if (!idle)
			{
				if (newModel)
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

		cursorDelta = mGuiManager->getCursorDeltaPosition();
			
		if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_A))
		{
			rX += cursorDelta.x;
			rY += cursorDelta.y;
		}
		else
		if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_B))
		{
			modelPosition.x += cursorDelta.x;
			modelPosition.y += cursorDelta.y;
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

		if (newModel)
		{
			newModel->setPosition(modelPosition);
			newModel->setOrientation(modelQuat);
		}
		if (q3Model)
		{
			q3Model->setPosition(modelPosition);
			q3Model->setOrientation(modelQuat);
		}

		if (tempLight)
			tempLight->setPosition(k::vector3(20, 0, 50) + modelPosition);

		if (tempLight2)
			tempLight2->setPosition(k::vector3(-20, 0, 50) + modelPosition);

		std::stringstream fpsT;
		fpsT << "fps: " << mRenderer->getLastFps() << std::endl; 
		fpsT << "press v to toggle vertex lighting" << std::endl;
		fpsT << "press e to toggle vertex wireframe" << std::endl;
		fpsT << "press a/z to zoom" << std::endl;
		fpsT << "press i,j,k,l to rotate model" << std::endl;
		fpsT << "press arrows to change animations" << std::endl;
		fpsText->setText(fpsT.str());

		mRenderer->draw();
	}
	
	delete newModel;
	delete q3Model;
	delete q3ModelUpper;
	delete q3ModelHead;

	delete fpsText;
	delete resourceMgr;
	// delete appRoot;

	return 0;
}

