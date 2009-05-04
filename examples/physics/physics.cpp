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

// Plane Stuff
const vec_t vertices[] ATTRIBUTE_ALIGN(32) = 
{ 
	40, 0, 40, -40, 0, 40, 
	-40, 0, -40, 40, 0, -40
};

const vec_t texCoords[] ATTRIBUTE_ALIGN(32) =
{
	0, 0, 1, 0, 1, 1, 
	1, 0, 0, 1, 0, 0
};

const vec_t normals[] ATTRIBUTE_ALIGN(32) = 
{
	0, 1, 0, 0, 1, 0,
	0, 1, 0, 0, 1, 0
};

const int indices[] ATTRIBUTE_ALIGN(32) = 
{ 
	0, 2, 1, 2, 0, 3 
};

class kPlane : public k::drawable3D
{
	private:
		k::material* mMaterial;

	public:
		void setMaterial(k::material* mat)
		{
			kAssert(mat);
			mMaterial = mat;
		}

		void setMaterial(const std::string& matName)
		{
			k::materialManager* mMaterialManager = &k::materialManager::getSingleton();
			kAssert(mMaterialManager);

			setMaterial(mMaterialManager->getMaterial(matName));
		}

		void draw()
		{
			k::renderSystem* rs = k::root::getSingleton().getRenderSystem();
			kAssert(rs);

			mMaterial->prepare();

			rs->clearArrayDesc();
			rs->setVertexArray(vertices);
			rs->setVertexCount(4);

			rs->setNormalArray(normals);
			rs->setTexCoordArray(texCoords);

			rs->setIndexCount(6);
			rs->setVertexIndex((index_t*)indices);
			rs->drawArrays();

			mMaterial->finish();
		}

		k::boundingBox getAABoundingBox() 
		{
			return k::boundingBox(k::vector3::zero, k::vector3::zero);
		}

		k::boundingBox getBoundingBox() 
		{
			return k::boundingBox(k::vector3::zero, k::vector3::zero);
		}
};

void addRandomBox()
{
	k::md5model* newBoxModel = new k::md5model("physics/box.md5mesh");

	if (newBoxModel)
	{
		k::renderer* mRenderer = k::root::getSingleton().getRenderer();
		k::physicsManager* mPhysicsManager = &k::physicsManager::getSingleton();

		mRenderer->push3D(newBoxModel);

		// Box
		k::physicBox* boxPhysic = mPhysicsManager->createBox(k::vector3(8, 8, 8));
		kAssert(boxPhysic);
		boxPhysic->setPosition(k::vector3(rand()%38 - 20, rand()%20 + 15, rand()%38 - 20));
		boxPhysic->attachDrawable(newBoxModel);
	}
}

void addRandomSphere()
{
	k::md5model* newModel = new k::md5model("physics/soccer.md5mesh");

	if (newModel)
	{
		k::renderer* mRenderer = k::root::getSingleton().getRenderer();
		k::physicsManager* mPhysicsManager = &k::physicsManager::getSingleton();

		mRenderer->push3D(newModel);

		// Sphere
		k::physicSphere* ballPhysic = mPhysicsManager->createSphere(2.398);
		kAssert(ballPhysic);
		ballPhysic->setPosition(k::vector3(rand()%38 - 20, rand()%10 + 16, rand()%38 - 20));
		ballPhysic->attachDrawable(newModel);
	}
}

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

	kAssert(mRenderer);
	kAssert(mRenderSystem);

	// Doesnt matter on wii
	mRenderSystem->createWindow(800, 600);
	mRenderSystem->setWindowTitle("knowledge, the power of mind");

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
	k::resourceManager::getSingleton().loadGroup("physics");

	// Fps Counter
	k::bitmapText* fpsText = new k::bitmapText("fonts/cube_14.dat", "cube_14");
	kAssert(fpsText);
	fpsText->setPosition(k::vector2(4, 10));
	mRenderer->push2D(fpsText);

	delete newLoadingScreen;

	mRenderer->setSkyPlane("skyPlane");

	kAssert(mGuiManager);
	mGuiManager->setCursor("wiiCursor3", k::vector2(32, 32));
	
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
	newCamera->setPerspective(45, 1.333f, 0.1f, 5000.0f);
	newCamera->setPosition(k::vector3(70, 40, -70));
	newCamera->lookAt(k::vector3(0, 0, 0));
	mRenderer->setCamera(newCamera);

	// Physics
	#ifndef __WII__
	k::physicsManager* mPhysicsManager = new k::physicsManager(0.005);
	#else
	k::physicsManager* mPhysicsManager = new k::physicsManager(0.05);
	#endif

	mPhysicsManager->setGravity(k::vector3(0, -9.78, 0));

	// Plane
	kPlane* newPlane = new kPlane();
	kAssert(newPlane);
	newPlane->setMaterial("odePlane");
	mRenderer->push3D(newPlane);

	k::physicTriMesh* planePhysic = mPhysicsManager->createTriMesh(vertices, 4, indices, 6, normals);
	kAssert(planePhysic);
	planePhysic->attachDrawable(newPlane);

	// Mouse Buttons 
	bool running = true;
	bool leftHold = false;
	bool rightHold = false;

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

		if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_A))
		{
			leftHold = true;
		}
		else
		if (leftHold)
		{
			addRandomBox();
			leftHold = false;
		}

		if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_B))
		{
			rightHold = true;
		}
		else
		if (rightHold)
		{
			addRandomSphere();
			rightHold = false;
		}

		k::vector2 mousePos = mInputManager->getWiiMotePosition(0);
		mGuiManager->setCursorPos(mousePos);

		// Physics Loop
		mPhysicsManager->collideActiveSpace();
		mPhysicsManager->cycle();

		// Update FPS
		std::stringstream fpsT;
		fpsT << "fps: " << mRenderer->getLastFps();
		fpsText->setText(fpsT.str());

		mRenderer->draw();
	}

	delete appRoot;
	return 0;
}

