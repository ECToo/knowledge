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

#include "prerequisites.h"
#include "md5.h"
#include "root.h"
#include "renderer.h"
#include "rendersystem.h"
#include "logger.h"
#include "physicsManager.h"

class kPlane : public k::drawable3D
{
	public:
		k::material* mMaterial;

		void draw()
		{
			k::renderSystem* rs = k::root::getSingleton().getRenderSystem();
			assert(rs != NULL);

			mMaterial->prepare();

			rs->startVertices(k::VERTEXMODE_QUAD);
			rs->texCoord(k::vector2(0, -1));
			rs->vertex(k::vector3(-40, 0, -40));
			rs->texCoord(k::vector2(0, 1));
			rs->vertex(k::vector3(-40, 0, 40));
			rs->texCoord(k::vector2(1, 1));
			rs->vertex(k::vector3(40, 0, 40));
			rs->texCoord(k::vector2(1, -1));
			rs->vertex(k::vector3(40, 0, -40));
			rs->endVertices();
		}
};

void addRandomBox()
{
	#ifdef __WII__
	k::md5model* newBoxModel = new k::md5model("/knowledge/soccer/box.md5mesh");
	#else
	k::md5model* newBoxModel = new k::md5model("box.md5mesh");
	#endif

	if (newBoxModel)
	{
		k::renderer* mRenderer = k::root::getSingleton().getRenderer();
		k::physicsManager* mPhysicsManager = &k::physicsManager::getSingleton();

		mRenderer->push3D(newBoxModel);

		// Box
		k::physicBox* boxPhysic = mPhysicsManager->createBox(k::vector3(8, 8, 8));
		assert(boxPhysic != NULL);
		boxPhysic->setPosition(k::vector3(rand()%38 - 20, 20, rand()%38 - 20));
		boxPhysic->attachDrawable(newBoxModel);
	}
}

void addRandomSphere()
{
	#ifdef __WII__
	k::md5model* newModel = new k::md5model("/knowledge/soccer/soccer.md5mesh");
	#else
	k::md5model* newModel = new k::md5model("soccer.md5mesh");
	#endif

	if (newModel)
	{
		k::renderer* mRenderer = k::root::getSingleton().getRenderer();
		k::physicsManager* mPhysicsManager = &k::physicsManager::getSingleton();

		mRenderer->push3D(newModel);

		// Sphere
		k::physicSphere* ballPhysic = mPhysicsManager->createSphere(2.398);
		assert(ballPhysic != NULL);
		ballPhysic->setPosition(k::vector3(rand()%38 - 20, 16, rand()%38 - 20));
		ballPhysic->attachDrawable(newModel);
	}
}

int main(int argc, char** argv)
{
	// Initialize knowledge
	k::root* appRoot = new k::root();
	k::renderSystem* mRenderSystem = appRoot->getRenderSystem();
	k::renderer* mRenderer = appRoot->getRenderer();
	k::materialManager* mMaterialManager = appRoot->getMaterialManager();
	k::guiManager* mGuiManager = appRoot->getGuiManager();
	k::inputManager* mInputManager = appRoot->getInputManager();

	#ifndef __WII__
	k::physicsManager* mPhysicsManager = new k::physicsManager(0.005);
	#else
	k::physicsManager* mPhysicsManager = new k::physicsManager(0.05);
	#endif

	assert(mRenderer != NULL);
	assert(mRenderSystem != NULL);

	// Doesnt matter on wii
	mRenderSystem->createWindow(800, 600);
	mRenderSystem->setDepthTest(true);

	// Common library
	#ifdef __WII__
	k::parsingFile* commonMaterialFile = new k::parsingFile("/knowledge/common.material");
	#else
	k::parsingFile* commonMaterialFile = new k::parsingFile("common.material");
	#endif

	assert(commonMaterialFile != NULL);
	mMaterialManager->parseMaterialScript(commonMaterialFile);

	// Parse material file
	#ifdef __WII__
	k::parsingFile* matFile = new k::parsingFile("/knowledge/soccer/soccer.material");
	#else
	k::parsingFile* matFile = new k::parsingFile("soccer.material");
	#endif

	mMaterialManager->parseMaterialScript(matFile);

	assert(mGuiManager != NULL);
	mGuiManager->setCursor("wiiCursor", k::vector2(48, 48));

	k::sticker* odeLogo = new k::sticker("odeLogo");
	odeLogo->setScale(k::vector2(128, 128));
	odeLogo->setPosition(k::vector2(10, 10));
	mRenderer->push2D(odeLogo);

	// Make The plane
	kPlane* newPlane = new kPlane();
	assert(newPlane != NULL);
	newPlane->mMaterial = k::materialManager::getSingleton().createMaterial("odePlane");
	assert(newPlane->mMaterial != NULL);
	mRenderer->push3D(newPlane);
	
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
	newCamera->setPosition(k::vector3(14, 18, -14));
	newCamera->lookAt(k::vector3(0, 0, 0));
	mRenderer->setCamera(newCamera);

	// Physics
	mPhysicsManager->setGravity(k::vector3(0, -9.78, 0));

	// Plane Stuff
	const vec_t vertices[4][3] = { 
		{40, 0, 40}, 
		{-40, 0, 40}, 
		{-40, 0, -40}, 
		{40, 0, -40}
	};

	const vec_t normals[4][3] = {
		{0, 1, 0},
		{0, 1, 0},
		{0, 1, 0},
		{0, 1, 0}
	};

	const int indices[6] = { 
		0, 2, 1, 
		2, 0, 3 
	};

	// Plane
	k::physicTriMesh* planePhysic = mPhysicsManager->createTriMesh(vertices, 4, indices, 6, normals);
	assert(planePhysic != NULL);
	planePhysic->attachDrawable(newPlane);

	// Angles
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

		mRenderer->draw();

		// Physics Loop
		mPhysicsManager->collideActiveSpace();
		mPhysicsManager->cycle();
	}

	delete appRoot;
	return 0;
}

