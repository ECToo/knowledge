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

// Temp
#include <ode/ode.h>

typedef struct
{
	dWorldID worldId;
	dJointGroupID jointId;
} contactInfo_t;

void twoBodiesCollide(void* data, dGeomID id1, dGeomID id2)
{
	contactInfo_t* cInfo = (contactInfo_t*)data;
	assert(cInfo != NULL);

	// Lets just use 255
	dContact contacts[255];
	unsigned int collisions = dCollide(id1, id2, 255, &contacts[0].geom, sizeof(dContact));
	for (unsigned int i = 0; i < collisions; i++)
	{
		dGeomID c1, c2;
		c1 = contacts[i].geom.g1;
		c2 = contacts[i].geom.g2;

		if (c1 == c2)
			continue;

		contacts[i].surface.mode = dContactBounce;
		contacts[i].surface.mu = dInfinity;
		contacts[i].surface.bounce = 0.8;
		contacts[i].surface.bounce_vel = 0.15;
		contacts[i].surface.slip1 = 0.0;

		dJointID c = dJointCreateContact(cInfo->worldId, cInfo->jointId, &contacts[i]);
		dJointAttach(c, dGeomGetBody(contacts[i].geom.g1), dGeomGetBody(contacts[i].geom.g2));
	}
}

void createSphere(dWorldID& mWorldId, dSpaceID& mSpaceId, float radius, k::vector3 pos)
{
	dGeomID mSphere = 0;
	dBodyID mSphereBody = 0;

	mSphere = dCreateSphere(mSpaceId, radius);
	mSphereBody = dBodyCreate(mWorldId);
	dGeomSetBody(mSphere, mSphereBody);
	dBodySetPosition(mSphereBody, pos.x, pos.y, pos.z);
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

	assert(mRenderer != NULL);
	assert(mRenderSystem != NULL);

	// Physics
	dWorldID mWorldId;
	dSpaceID mSpaceId;
	dJointGroupID mJointId;
	dGeomID mSphere, mPlane;
	dBodyID mSphereBody, mPlaneBody;
	dJointGroupID mJointGroupId;

	mWorldId = dWorldCreate();
	mSpaceId = dHashSpaceCreate(0);
	dWorldSetGravity(mWorldId, 0, -9.78, 0);

	// Joints
	mJointId = dJointGroupCreate(0);
	dJointGroupEmpty(mJointId);

	// To be passed as argument to twoBodiesCollide
	contactInfo_t* cInfo = new contactInfo_t;
	assert(cInfo != NULL);

	cInfo->worldId = mWorldId;
	cInfo->jointId = mJointId;

	mPlane = dCreatePlane(mSpaceId, 0, 1, -0.005, 0);
	mSphere = dCreateSphere(mSpaceId, 2.398); // model radius
	mSphereBody = dBodyCreate(mWorldId);
	dGeomSetBody(mSphere, mSphereBody);
	dBodySetPosition(mSphereBody, 0, 30, -20);

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

	// Create Model
	k::vector3 modelPosition;
	modelPosition.z = -100;

	#ifdef __WII__
	k::md5model* newModel = new k::md5model("/knowledge/soccer/soccer.md5mesh");
	#else
	k::md5model* newModel = new k::md5model("soccer.md5mesh");
	#endif

	assert(newModel != NULL);

	mRenderer->push3D(newModel);

	assert(mGuiManager != NULL);
	mGuiManager->setCursor("wiiCursor", k::vector2(32, 32));

	/**
	 * Setup the input Manager
	 */
	assert(mInputManager != NULL);
	mInputManager->initWii(false);
	mInputManager->setupWiiMotes(1);
	mInputManager->setWiiMoteTimetout(60);
	mInputManager->setWiiMoteEmulation(true);

	// Angles
	bool running = true;
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

		k::vector2 mousePos = mInputManager->getWiiMotePosition(0);
		mGuiManager->setCursorPos(mousePos);

		const dReal* modelPos = dBodyGetPosition(mSphereBody);
		const dReal* modelOri = dBodyGetQuaternion(mSphereBody);

		newModel->setPosition(k::vector3(modelPos[0], modelPos[1], modelPos[2]));
		newModel->setOrientation(k::quaternion(modelOri[1], modelOri[2], modelOri[3], modelOri[0]));

		mRenderer->draw();

		// Physics Loop
		dSpaceCollide(mSpaceId, cInfo, twoBodiesCollide);
		dWorldStepFast1(mWorldId, 0.004, 1);
		dJointGroupEmpty(mJointId);
	}

	delete appRoot;
	return 0;
}

