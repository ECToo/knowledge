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

#ifndef __WII__

#include "physicsManager.h"
#include "logger.h"
#include "root.h"

namespace k {

template<> physicsManager* singleton<physicsManager>::singleton_instance = 0;

physicsManager& physicsManager::getSingleton()
{  
	kAssert(singleton_instance);
	return (*singleton_instance);  
}

physicsManager::physicsManager(vec_t stepsize, stepType_t steptype)
{
	kAssert(stepsize != 0.0);

	mStepSize = stepsize;
	mStepType = steptype;

	dInitODE();
	// dInitODE2(0);

	// Set Default World and Space
	setWorld("default");
	setSpace("default");
	setJointGroup("default");

	// Default surface parameters
	memset(&mSurfaceParams, 0, sizeof(dSurfaceParameters));
	mSurfaceParams.mode = dContactBounce;
	mSurfaceParams.mu = dInfinity;
	mSurfaceParams.bounce = 0.3;
	mSurfaceParams.bounce_vel = 0.15;
	mSurfaceParams.slip1 = 0.0;

	mPhysicObjects.clear();
}

physicsManager::~physicsManager()
{
	dCloseODE();
}

dWorldID physicsManager::_findWorld(const std::string& name)
{
	std::map<std::string, dWorldID>::const_iterator it = mWorlds.find(name);
	if (it != mWorlds.end())
	{
		return it->second;
	}
	else
	{
		return NULL;
	}
}

dSpaceID physicsManager::_findSpace(const std::string& name)
{
	std::map<std::string, dSpaceID>::const_iterator it = mSpaces.find(name);
	if (it != mSpaces.end())
	{
		return it->second;
	}
	else
	{
		return NULL;
	}
}
			
void physicsManager::setWorld(const std::string& name)
{
	dWorldID thisWorld = _findWorld(name);
	if (!thisWorld)
	{
		thisWorld = dWorldCreate();
		mWorlds[name] = thisWorld;
	}

	mActiveWorld = thisWorld;
}

void physicsManager::setGravity(vector3 grav)
{
	dWorldSetGravity(mActiveWorld, grav.x, grav.y, grav.z);
}

void physicsManager::setSpace(const std::string& name, spaceType_t type)
{
	dSpaceID thisSpace = _findSpace(name);
	if (!thisSpace)
	{
		switch (type)
		{
			default:
			case SPACETYPE_SIMPLE:
				thisSpace = dSimpleSpaceCreate(0);
				break;
			case SPACETYPE_HASH:
				thisSpace = dHashSpaceCreate(0);
				break;
		};

		mSpaces[name] = thisSpace;
	}

	mActiveSpace = thisSpace;
}

dJointGroupID physicsManager::_findJointGroup(const std::string& name)
{
	std::map<std::string, dJointGroupID>::const_iterator it = mJointGroups.find(name);
	if (it != mJointGroups.end())
	{
		return it->second;
	}
	else
	{
		return NULL;
	}
}

void physicsManager::setJointGroup(const std::string& name)
{
	dJointGroupID newJointGroup = _findJointGroup(name);
	if (!newJointGroup)
	{
		newJointGroup = dJointGroupCreate(0);
		dJointGroupEmpty(newJointGroup);

		mJointGroups[name] = newJointGroup;
	}

	mActiveJointGroup = newJointGroup;
}

void DLL_EXPORT defaultCollision(void* data, dGeomID id1, dGeomID id2)
{
	physicsManager* pMgr = &physicsManager::getSingleton();
	kAssert(pMgr != NULL);

	dSurfaceParameters params = pMgr->getParams();
	dWorldID worldId = pMgr->getActiveWorld();
	dJointGroupID jointId = pMgr->getActiveJointGroup();

	dContact contacts[128];
	unsigned int collisions = dCollide(id1, id2, 128, &contacts[0].geom, sizeof(dContact));
	for (unsigned int i = 0; i < collisions; i++)
	{
		dGeomID c1, c2;
		c1 = contacts[i].geom.g1;
		c2 = contacts[i].geom.g2;

		if (c1 == c2)
			continue;

		memcpy(&contacts[i].surface, &params, sizeof(dSurfaceParameters));
		dJointID c = dJointCreateContact(worldId, jointId, &contacts[i]);
		dJointAttach(c, dGeomGetBody(contacts[i].geom.g1), dGeomGetBody(contacts[i].geom.g2));
	}
}
			
dSurfaceParameters physicsManager::getParams()
{
	return mSurfaceParams;
}

void physicsManager::collideActiveSpace(mCallFunc_t func)
{
	dSpaceCollide(mActiveSpace, NULL, func);
}

void physicsManager::collideAllSpaces(mCallFunc_t func)
{
	std::map<std::string, dSpaceID>::const_iterator it;
	for (it = mSpaces.begin(); it != mSpaces.end(); it++)
	{
		dSpaceCollide(it->second, NULL, func);
	}
}

void physicsManager::cycle()
{
	switch (mStepType)
	{
		default:
		case STEPTYPE_QUICK:
			dWorldQuickStep(mActiveWorld, mStepSize);
			break;
		case STEPTYPE_NORMAL:
			dWorldStep(mActiveWorld, mStepSize);
			break;
	};

	// Update All objects positions
	std::list<physicObject*>::const_iterator it;
	for (it = mPhysicObjects.begin(); it != mPhysicObjects.end(); it++)
		(*it)->updateAttached();

	// Clean all joint groups
	std::map<std::string, dJointGroupID>::const_iterator jIt;
	for (jIt = mJointGroups.begin(); jIt != mJointGroups.end(); jIt++)
	{
		dJointGroupEmpty(jIt->second);
	}
}

dWorldID physicsManager::getActiveWorld()
{
	return mActiveWorld;
}

dSpaceID physicsManager::getActiveSpace()
{
	return mActiveSpace;
}

dJointGroupID physicsManager::getActiveJointGroup()
{
	return mActiveJointGroup;
}
			
physicSphere* physicsManager::createSphere(vec_t radius)
{
	physicSphere* newSphere = new physicSphere(mActiveWorld, mActiveSpace, radius);
	if (newSphere)
		mPhysicObjects.push_back(newSphere);

	return newSphere;
}

physicBox* physicsManager::createBox(vector3 length)
{
	physicBox* newBox = new physicBox(mActiveWorld, mActiveSpace, length);
	if (newBox)
		mPhysicObjects.push_back(newBox);

	return newBox;
}

physicTriMesh* physicsManager::createTriMesh(drawable3D* mesh)
{
	return NULL;
}

physicTriMesh* physicsManager::createTriMesh(const void* vertices, unsigned int numVertices, 
					const void* indices, unsigned int numIndices, const void* normals)
{
	physicTriMesh* newTriMesh = new physicTriMesh(mActiveSpace, vertices, numVertices, 
			indices, numIndices, normals);

	if (newTriMesh)
		mPhysicObjects.push_back(newTriMesh);

	return newTriMesh;
}

physicSphere::physicSphere(dWorldID wId, dSpaceID sId, vec_t radius)
{
	mGeom = dCreateSphere(sId, radius);
	mBody = dBodyCreate(wId);

	dGeomSetBody(mGeom, mBody);
}

physicSphere::~physicSphere()
{
	dGeomDestroy(mGeom);
	// TODO: Destroy Body?
}
			
physicBox::physicBox(dWorldID wId, dSpaceID sId, vector3 length)
{
	mGeom = dCreateBox(sId, length.x, length.y, length.z);
	mBody = dBodyCreate(wId);

	dGeomSetBody(mGeom, mBody);
}

physicBox::~physicBox()
{
	dGeomDestroy(mGeom);
	// TODO: Destroy Body?
}
			
physicTriMesh::physicTriMesh(dSpaceID sId, const void* vertices, unsigned int numVertices,
		const void* indices, unsigned int numIndices, const void* normals)
{
	dTriMeshDataID trimeshData = dGeomTriMeshDataCreate();
	dGeomTriMeshDataBuildSingle1(trimeshData, 
			vertices, 3 * sizeof(float), numVertices, 
			indices, numIndices, 3 * sizeof(int),
			normals);

	mGeom = dCreateTriMesh(sId, trimeshData, NULL, NULL, NULL);
}

physicTriMesh::physicTriMesh(dSpaceID sId, drawable3D* mesh)
{
	// TODO
}

physicTriMesh::~physicTriMesh()
{
	dGeomDestroy(mGeom);
}

}

#endif
// Wii is not working right now, set this for it 

