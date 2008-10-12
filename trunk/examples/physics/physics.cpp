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

// Temp - Need physics Wrapper
#include "ode_k/ode.h"

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
			rs->vertex(k::vector3(-20, 0, -20));
			rs->texCoord(k::vector2(0, 1));
			rs->vertex(k::vector3(-20, 0, 20));
			rs->texCoord(k::vector2(1, 1));
			rs->vertex(k::vector3(20, 0, 20));
			rs->texCoord(k::vector2(1, -1));
			rs->vertex(k::vector3(20, 0, -20));
			rs->endVertices();
		}
};

typedef struct
{
	dWorldID worldId;
	dJointGroupID jointId;
} contactInfo_t;

void twoBodiesCollide(void* data, dGeomID id1, dGeomID id2)
{
	contactInfo_t* cInfo = (contactInfo_t*)data;
	assert(cInfo != NULL);

	dContact contacts[128];
	unsigned int collisions = dCollide(id1, id2, 128, &contacts[0].geom, sizeof(dContact));
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

	#ifdef __WII__
	k::md5model* newModel = new k::md5model("/knowledge/soccer/soccer.md5mesh");
	#else
	k::md5model* newModel = new k::md5model("soccer.md5mesh");
	#endif

	assert(newModel != NULL);
	mRenderer->push3D(newModel);

	assert(mGuiManager != NULL);
	mGuiManager->setCursor("wiiCursor", k::vector2(48, 48));

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
	dInitODE2(0);
	dWorldID mWorldId;
	dSpaceID mSpaceId;
	dJointGroupID mJointId;
	dGeomID mSphere, mPlane;
	dBodyID mSphereBody;

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

	// Plane Stuff
	const vec_t vertices[4][3] = { 
		{20, 0, 20}, 
		{-20, 0, 20}, 
		{-20, 0, -20}, 
		{20, 0, -20}
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

	dTriMeshDataID trimeshData;
	trimeshData = dGeomTriMeshDataCreate();
	dGeomTriMeshDataBuildSingle1(trimeshData, 
			vertices, 3 * sizeof(vec_t), 4, 
			indices, 6, 3 * sizeof(int),
			normals);

	mPlane = dCreateTriMesh(mSpaceId, trimeshData, NULL, NULL, NULL);

	// Sphere
	mSphere = dCreateSphere(mSpaceId, 2.398); // model radius
	mSphereBody = dBodyCreate(mWorldId);
	dGeomSetBody(mSphere, mSphereBody);
	dBodySetPosition(mSphereBody, 0, 30, 0);

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

		const dReal* planePos = dGeomGetPosition(mPlane);
		newPlane->setPosition(k::vector3(planePos[0], planePos[1], planePos[2]));
	
		mRenderer->draw();

		// Physics Loop
		dSpaceCollide(mSpaceId, cInfo, twoBodiesCollide);

		#ifdef __WII__
		dWorldQuickStep(mWorldId, 0.05);
		#else
		dWorldQuickStep(mWorldId, 0.005);
		#endif

		dJointGroupEmpty(mJointId);
	}

	delete appRoot;
	return 0;
}

