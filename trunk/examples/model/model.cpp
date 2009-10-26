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

class modelExample : public k::inputEventHandler, public k::signalHandler
{
	private:
		k::root* appRoot;
		k::renderSystem* mRenderSystem;
		k::renderer* mRenderer;
		k::guiManager* mGuiManager;
		k::inputManager* mInputManager;
			
		k::inputPeripheral* mouse;
		k::inputKeyboard* kb;
		k::inputWiimote* wiimote;
			
		k::resourceManager* resourceMgr;
			
		// Positions
		k::vector3 modelPosition;
		k::vector2 cursorDelta;

		// Angles
		int rX;
		int rY;
			
		// Wire
		bool wireframe;
		bool bounding;

		// Light
		bool V_Hold;

		// Anims
		bool runf;
		bool runb;
		bool idle;

		// is our app running?
		bool running;

		// number of screenshots
		unsigned int scrCont;
			
		// GUI
		k::panelWidget* newPanel;
		k::bitmapText* fpsText;

		// md5 Model
		k::md5model* newModel;

		// md3 model(s)
		k::md3model* q3Model;
		k::md3model* q3ModelUpper;
		k::md3model* q3ModelHead;

		// Lights
		k::light::light* tempLight;
		k::light::light* tempLight2;

	public:
		modelExample()
		{
			// Zero pointers
			appRoot = NULL;
			mRenderSystem = NULL;
			mRenderer = NULL;
			mGuiManager = NULL;
			mInputManager = NULL;
			mouse = NULL;
			kb = NULL;
			wiimote = NULL;
			resourceMgr = NULL;
			newPanel = NULL;
			fpsText = NULL;
			newModel = NULL;
			q3Model = NULL;
			q3ModelUpper = NULL;
			q3ModelHead = NULL;
			tempLight = NULL;
			tempLight2 = NULL;
			scrCont = 0;

			// Initialize knowledge
			try
			{
				appRoot = new k::root();
			}

			catch (...)
			{
				printf("Failed to create app root.");
				return;
			}

			mRenderSystem = appRoot->getRenderSystem();
			mRenderer = appRoot->getRenderer();
			mGuiManager = appRoot->getGuiManager();
			mInputManager = appRoot->getInputManager();
		
			// Doesnt matter on wii
			mRenderSystem->createWindow(800, 600);
			mRenderSystem->setWindowTitle("knowledge, the power of mind");

			// Input Manager - mouse and keyboard devices
			mouse = mInputManager->getDevice(k::INPUT_MOUSE);
			kb = (k::inputKeyboard*)mInputManager->getDevice(k::INPUT_KEYBOARD);
			if (kb)
			{
				k::inputFunctionPtr downPtr(this, (k::inputFunction)&modelExample::keyDown);
				k::inputFunctionPtr upPtr(this, (k::inputFunction)&modelExample::keyUp);
		
				kb->pushEvent(k::HANDLER_DOWN, downPtr);
				kb->pushEvent(k::HANDLER_UP, upPtr);
			}

			wiimote = (k::inputWiimote*)mInputManager->getDevice(k::INPUT_WIIMOTE_1);
			if (wiimote)
			{
				k::inputFunctionPtr downPtr(this, (k::inputFunction)&modelExample::keyDown);
				k::inputFunctionPtr upPtr(this, (k::inputFunction)&modelExample::keyUp);
		
				wiimote->pushEvent(k::HANDLER_DOWN, downPtr);
				wiimote->pushEvent(k::HANDLER_UP, upPtr);
			}

			#ifdef __WII__
			chdir("sd:/knowledge/model/");
			#endif

			// Initialize resources
			try
			{
				resourceMgr = new k::resourceManager("../resources.cfg");
			}
	
			catch (...)
			{
				K_LOG_INFO("Failed to allocate resource manager.");
				return;
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
				return;
			}

			resourceMgr->setLoadingScreen(newLoadingScreen);
			newLoadingScreen->loadBg("loading.jpg");

			newLoadingScreen->setImgDimension(k::vector2(256, 256));
			newLoadingScreen->update("");

			k::resourceManager::getSingleton().loadGroup("model");
			k::resourceManager::getSingleton().loadGroup("common");
			k::resourceManager::getSingleton().loadGroup("skies");
	
			// Angles
			rX = 0;
			rY = 0;

			// Load the Model
			modelPosition.z = -65;
		
			tempLight = mRenderer->createPointLight();
			tempLight2 = mRenderer->createPointLight();
			if (tempLight && tempLight2)
			{
				tempLight->setAmbient(k::color(0.0, 0.0, 0.0, 1.0));
				tempLight->setDiffuse(k::color(0.0, 0.0, 1.0, 1.0));
				tempLight->setSpecular(k::color(1.0, 1.0, 1.0, 1.0));

				tempLight2->setAmbient(k::color(0.0, 0.0, 0.0, 1.0));
				tempLight2->setDiffuse(k::color(1.0, 0.0, 0.0, 1.0));
				tempLight2->setSpecular(k::color(1.0, 1.0, 1.0, 1.0));
			}

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
				return;
			}
			
			delete newLoadingScreen;

			// Set Skyplane
			mRenderer->setSkyPlane("skyPlane");

			// Fps Counter
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
				return;
			}

			mGuiManager->setCursor("wiiCursor3", k::vector2(32, 32));
			mGuiManager->setSkin("guiSkin", "guiDef.xml");

			newPanel = new k::panelWidget(k::vector2(0, 0), k::vector2(370, 130));
			newPanel->setZ(0);
			mGuiManager->pushWidget(newPanel);
			mRenderer->push2D(newPanel);

			/**
			 * Used to test GUI buttons, uncomment if you wanna see a nice button =D
			 *
			k::buttonWidget* newTestBtn = new k::buttonWidget(k::vector2(180, 180), k::vector2(150, 20));
			newTestBtn->setText("TEST");
			mGuiManager->pushWidget(newTestBtn);	
			mRenderer->push2D(newTestBtn);

			// Clicked Signal
			k::signalHandlerInfo_t* clickedInfo = new k::signalHandlerInfo_t;
			clickedInfo->handler = this;
			clickedInfo->function = (k::signalFunctionPtr)(&modelExample::buttonClicked);
			clickedInfo->userData = NULL;
			newTestBtn->connect("clicked", clickedInfo);
			*/

			// Wire
			wireframe = false;
			bounding = false;

			// Anims
			runf = false;
			runb = false;
			idle = true;

			// We are running
			running = true;
		}

		~modelExample()
		{
			delete newModel;
			delete q3Model;
			delete q3ModelUpper;
			delete q3ModelHead;

			delete fpsText;
			delete resourceMgr;
			delete appRoot;
			delete newPanel;
		}

		void loop()
		{
			while (running)
			{
				mInputManager->feed();

				// Set Anim to idle
				if (kb && !kb->isKeyDown(k::KB_up) && !kb->isKeyDown(k::KB_down))
				{
					if (!idle)
					{
						if (newModel)
							newModel->setAnimation("idle");
	
						idle = true;
						runb = runf = false;
					}
				}

				// User clicked on Close Window
				if (mInputManager->getQuitEvent())
					running = false;

				if (mouse)
					cursorDelta = mouse->getDeltaPosition();

				if (mouse && mouse->isButtonDown(k::MOUSE_LEFT))
				{
					rX += cursorDelta.x;
					rY += cursorDelta.y;
				}
				else
				if (mouse && mouse->isButtonDown(k::MOUSE_RIGHT))
				{
					modelPosition.x += cursorDelta.x;
					modelPosition.y += cursorDelta.y;
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
		}

		/* Used to test GUI buttons
		bool buttonClicked(k::signalInfo_t info, void* userData)
		{
			printf("Clicked button %d\n", info.mouseState.buttons);
			return false;
		}
		*/

		bool keyDown(unsigned int key, k::inputPeripheral* kbd)
		{
			switch (key)
			{
				case k::KB_left:
					modelPosition.x--;
					break;
				case k::KB_right:
					modelPosition.x++;
					break;
				case k::KB_up:
					if (!runf)
					{
						if (newModel)
							newModel->setAnimation("runf");

						runf = true;
						runb = idle = false;
					}
					break;
				case k::KB_down:
					if (!runb)
					{
						if (newModel)
							newModel->setAnimation("runb");

						runb = true;
						runf = idle = false;
					}
					break;
				case k::KB_a:
					modelPosition.z += 5;
					break;
				case k::KB_z:
					modelPosition.z -= 5;
					break;
				case k::KB_i:
					rY += 1;
					break;
				case k::KB_k:
					rY -= 1;
					break;
				case k::KB_j:
					rX += 1;
					break;
				case k::KB_l:
					rX -= 1;
					break;
			}

			return false;
		}

		bool keyUp(unsigned int key, k::inputPeripheral* kbd)
		{
			switch (key)
			{
				// Quit
				case k::KB_escape:
				case k::WIIMOTE_HOME:
					running = false;
					break;

				// Wireframe
				case k::KB_e:
					wireframe ^= 1;
					mRenderSystem->setWireFrame(wireframe);
					break;

				// Bounding box
				case k::KB_b:
					bounding ^= 1;

					if (newModel)
						newModel->setDrawBoundingBox(bounding);

					if (q3Model && q3ModelUpper && q3ModelHead)
					{
						q3Model->setDrawBoundingBox(bounding);
						q3ModelUpper->setDrawBoundingBox(bounding);
						q3ModelHead->setDrawBoundingBox(bounding);
					}
					break;

				// Screenshot
				case k::WIIMOTE_1:
				case k::KB_f12:
					{
						std::stringstream shot;
						shot << k::resourceManager::getSingleton().getRoot();
						shot << "screenshot_" << scrCont++ << ".png";

						mRenderSystem->screenshot(shot.str().c_str());
					}
					break;

				// Vertex Light
				case k::KB_v:
					if (tempLight)
						tempLight->setEnabled(tempLight->getEnabled() ^ 1);
					if (tempLight2)
						tempLight2->setEnabled(tempLight2->getEnabled() ^ 1);
					break;
			}

			return false;
		}
};

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char** argv)
#endif
{
	modelExample newApp;
	newApp.loop();

	return 0;
}

