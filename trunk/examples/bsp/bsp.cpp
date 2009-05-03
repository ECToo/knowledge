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

class speedmd3 : public k::md3model
{
	private:
		k::vector3 mSpeed;
		vec_t mRadius;

	public:
		speedmd3(const std::string& filename)
		: k::md3model(filename) 
		{}

		void setRadius(vec_t rad)
		{ mRadius = rad; }

		void setSpeed(const k::vector3 speed)
		{ mSpeed = speed; }

		k::vector3 getSpeed() const
		{ return mSpeed; }

		vec_t getRadius() const
		{ return mRadius;	}
};

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
	mInputManager->setPointerLock(true);

	// Initialize resources
	#ifdef __WII__
	chdir("sd:/knowledge/bsp/");
	#endif

	k::resourceManager* resourceMgr;
	try
	{
		resourceMgr = new k::resourceManager("../resources.cfg");
	}
	catch (...)
	{
		K_LOG_INFO("Failed to allocate resourceManager.");
		return 1;
	}

	// Loading Screen
	k::imgLoadScreen* newLoadingScreen;
	try
	{
		newLoadingScreen = new k::imgLoadScreen();
	}
	catch (...)
	{
		K_LOG_INFO("Failed to allocate load screen.");
		return 1;
	}

	resourceMgr->setLoadingScreen(newLoadingScreen);
	newLoadingScreen->loadBg("loading.png");
	newLoadingScreen->setImgDimension(k::vector2(256, 256));
	newLoadingScreen->update("");

	k::resourceManager::getSingleton().loadGroup("common");
	k::resourceManager::getSingleton().loadGroup("bsp");
	k::resourceManager::getSingleton().loadGroup("skies");

	// Test model
	speedmd3* sphere;
	try
	{
		sphere = new speedmd3("space_models/sphere.md3");

		k::boundingBox sphereBB = sphere->getBoundingBox();
		sphere->setRadius((sphereBB.getMaxs() - sphereBB.getMins() / 2.0f).length());
		
		mRenderer->push3D(sphere);
	}
	catch (...)
	{
		K_LOG_INFO("Failed to allocate sphere.");
		return 1;
	}

	/*
	 * We dont need the box *right* now
	 *
	k::md3model* box;
	try
	{
		box = new k::md3model("space_models/playerBox.md3");
		mRenderer->push3D(box);
	}
	catch (...)
	{
		K_LOG_INFO("Failed to allocate box.");
		return 1;
	}
	*/

	k::q3Bsp testeBsp;
	// testeBsp.loadQ3Bsp("southcity.bsp");
	//testeBsp.loadQ3Bsp("the_cell.bsp");
	testeBsp.loadQ3Bsp("tc_closecombat.bsp");
	kAssert(testeBsp.getLoadSuccess());

	mRenderer->setWorld(&testeBsp);
	mRenderer->setSkyBox("graveyard");

	delete newLoadingScreen;

	// Setup Camera
	k::camera* newCamera;
	try
	{
		newCamera = new k::camera();
		newCamera->setPlanes(0.1f, 5000.0f);
		newCamera->setPosition(k::vector3(0, 0, 0));
		newCamera->lookAt(k::vector3(0, 0, -1));
		mRenderer->setCamera(newCamera);
	}
	catch (...)
	{
		K_LOG_INFO("Failed to allocate new camera.");
		return 1;
	}

	// Find a random spawn point on the map
	std::vector<k::vector3> spawnPoints;
	bool foundSpawn = false;

	const std::list<k::q3Entity> mapEntities = testeBsp.getEntities();
	std::list<k::q3Entity>::const_iterator it = mapEntities.begin();
	for (; it != mapEntities.end(); it++)
	{
		if ((*it).getValue("classname") == std::string("info_player_deathmatch"))
		{
			float temp;
			k::vector3 origin((*it).getValue("origin"));
			temp = -origin.y;
			origin.y = origin.z;
			origin.z = temp;

			spawnPoints.push_back(origin);
			foundSpawn = true;
		}
	}

	if (foundSpawn)
	{
		srand(time(NULL));
		unsigned int choosen = rand() % spawnPoints.size();
		newCamera->setPosition(spawnPoints[choosen]);
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
		K_LOG_INFO("Failed to allocate fps text.");
		return 1;
	}

	mGuiManager->setCursor("wiiCursor3", k::vector2(32, 32));
	mGuiManager->setCursorPos(k::vector2(mRenderSystem->getScreenWidth()/2, mRenderSystem->getScreenHeight()/2));	

	bool L_Hold = false;
	bool E_Hold = false;
	bool wireframe = false;
	bool running = true;

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
				if (mInputManager->getKbdKeyDown(K_KBD_s))
					pos -= look * 5;
				else
				{
					// box->setPosition(newCamera->getPosition() + newCamera->getDirection() * 100);
				}
			}
			else
			{
				if (mInputManager->getKbdKeyDown(K_KBD_w))
					pos += look * 5;
				else
				{
					sphere->setPosition(newCamera->getPosition());
					sphere->setSpeed(newCamera->getDirection() * 2);
				}
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
		
		k::vector2 diffMousePos = mInputManager->getWiiMoteMotion(0);
		if (diffMousePos.x)
		{
			k::quaternion dirQuat(-diffMousePos.x, k::vector3(0, 1, 0));
			k::quaternion ori = newCamera->getOrientation();

			newCamera->setOrientation(dirQuat * ori);
		}
		if (diffMousePos.y)
		{
			k::quaternion dirQuat(-diffMousePos.y, newCamera->getRight());
			k::quaternion ori = newCamera->getOrientation();

			newCamera->setOrientation(dirQuat * ori);
		}

		// Gravity
		sphere->setSpeed(sphere->getSpeed() + k::vector3(0, -0.05, 0));

		// Feed sphere speed
		k::q3BspTrace newTrace = testeBsp.traceSphere(sphere->getPosition(), sphere->getPosition() + sphere->getSpeed() * 2, sphere->getRadius()); 
		if (newTrace.fraction == 1.0f && newTrace.startsOut)
			sphere->setPosition(sphere->getPosition() + sphere->getSpeed());
		else
		{
			sphere->setSpeed(sphere->getSpeed().reflect(newTrace.planeNormal) * 0.95);
		}

		// FPS Counter
		std::stringstream fpsT;
		fpsT << "fps: " << mRenderer->getLastFps();
		fpsText->setText(fpsT.str());

		// Draw Everything
		mRenderer->draw();
	}

	delete mInputManager;
	delete newCamera;
	delete fpsText;
	delete resourceMgr;
	delete appRoot;

	return 0;
}	

