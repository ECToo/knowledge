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

#ifndef _PHYSICS_MANAGER_H_
#define _PHYSICS_MANAGER_H_

#include "prerequisites.h"
#include "vector2.h"
#include "vector3.h"
#include "drawable.h"
#include "quaternion.h"
#include "singleton.h"

#ifndef __WII__
#include <ode/ode.h>
#else
#include "ode/ode.h"
#endif

#define ATTACH_POSITION (1 << 0)
#define ATTACH_ORIENTATION (1 << 1)
#define ATTACH_ALL (ATTACH_POSITION | ATTACH_ORIENTATION)

namespace k
{
	typedef void(*mCallFunc_t)(void*, dGeomID,dGeomID);
	void defaultCollision(void*, dGeomID, dGeomID);

	/**
	 * A physic representation of some object - base class.
	 */
	class physicObject
	{
		protected:
			/**
			 * A drawable 3d this object is attached to.
			 * If this variable is set, each update the
			 * physicObject receive will also update
			 * the desired params on attached drawable.
			 */
			drawable3D* mAttached;

			/**
			 * Following the bit sets, parameters
			 * for attached object.
			 */
			char mAttachedParams;

			/** 
			 * Base geometry
			 */
			dGeomID mGeom;

		public:
			void attachDrawable(drawable3D* att, char attachParams = ATTACH_ALL)
			{
				assert(att != NULL);

				att->setPosition(getPosition());
				att->setOrientation(getOrientation());

				mAttached = att;
				mAttachedParams = attachParams;
			}

			void detachDrawable()
			{
				mAttached = NULL;
				mAttachedParams = 0;
			}

			void setAttachParams(char param)
			{
				mAttachedParams = param;
			}

			dGeomID getODEGeom()
			{
				return mGeom;
			}

			virtual vector3 getPosition()
			{
				const dReal* pos = dGeomGetPosition(mGeom);
				return vector3(pos[0], pos[1], pos[2]);
			}

			virtual void setPosition(vector3 pos)
			{
				dGeomSetPosition(mGeom, pos.x, pos.y, pos.z);
			}

			virtual quaternion getOrientation()
			{
				dQuaternion rot;
				dGeomGetQuaternion(mGeom, rot);

				return quaternion(rot[1], rot[2], rot[3], rot[0]);
			}

			virtual void setOrientation(quaternion ori)
			{
				dQuaternion rot;
				rot[0] = ori.w;
				rot[1] = ori.x;
				rot[2] = ori.y;
				rot[3] = ori.z;

				dGeomSetQuaternion(mGeom, rot);
			}

			void updateAttached() 
			{
				if (mAttached)
				{
					if (mAttachedParams & ATTACH_POSITION)
						mAttached->setPosition(getPosition());

					if (mAttachedParams & ATTACH_ORIENTATION)
						mAttached->setOrientation(getOrientation());
				}
			}
	};

	class physicSphere : public physicObject
	{
		private:
			dBodyID mBody;

		public:
			physicSphere(dWorldID wId, dSpaceID sId, vec_t radius);
			~physicSphere();
	};

	class physicBox : public physicObject
	{
		private:
			dBodyID mBody;

		public:
			physicBox(dWorldID wId, dSpaceID sId, vector3 length);
			~physicBox();
	};

	class physicTriMesh : public physicObject
	{
		public:
			physicTriMesh(dSpaceID sId, const void* vertices, unsigned int numVertices,
					const void* indices, unsigned int numIndices, const void* normals = NULL);

			physicTriMesh(dSpaceID sId, drawable3D* mesh);
			~physicTriMesh();
	};

	typedef enum
	{
		STEPTYPE_NORMAL,
		STEPTYPE_QUICK
	} stepType_t;

	typedef enum
	{
		SPACETYPE_SIMPLE,
		SPACETYPE_HASH
	} spaceType_t;

	class physicsManager : public singleton<physicsManager>
	{
		private:
			/**
			 * List of the worlds, defined by name
			 * simulations happen inside worlds and
			 * inside spaces of these worlds
			 */
			std::map<std::string, dWorldID> mWorlds;

			/**
			 * List of the spaces, defined by name
			 * simulations happen inside spaces.
			 */
			std::map<std::string, dSpaceID> mSpaces;

			/**
			 * List of joint groups, joint groups are 
			 * connected to worlds and spaces to calculate
			 * collisions
			 */
			std::map<std::string, dJointGroupID> mJointGroups;

			/**
			 * Internal, Active world
			 */
			dWorldID mActiveWorld;

			/**
			 * Internal, Active space
			 */
			dSpaceID mActiveSpace;

			/**
			 * Internal, Active joint group
			 */
			dJointGroupID mActiveJointGroup;

			/**
			 * Fixed Step size
			 */
			vec_t mStepSize;
			stepType_t mStepType;

			/**
			 * The general collision function is just a
			 * function that handle collision bouncing.
			 * The params for the collision, like mu, bounce,
			 * bounce_vel and slip can all be specified in a 
			 * call to the setCollisionParameters function.
			 */
			dSurfaceParameters mSurfaceParams;

			/**
			 * All allocated geometries
			 */
			std::list<physicObject*> mPhysicObjects;

		public:
			/**
			 * Creates a base world and space each named "default"
			 * The space is a simple space.
			 */
			physicsManager(vec_t stepsize, stepType_t steptype = STEPTYPE_QUICK);
			~physicsManager();

			static physicsManager& getSingleton();

			/**
			 * Creates a new world and if it exists,
			 * set it as the default working one.
			 *
			 * @name The name of the new world.
			 */
			void setWorld(const std::string& name);

			/**
			 * Set the active world gravity
			 */
			void setGravity(vector3 grav);

			/**
			 * Creates a new space and if it exists,
			 * set it as the default working one.
			 *
			 * @name The name of the new space.
			 */
			void setSpace(const std::string& name, spaceType_t type = SPACETYPE_SIMPLE);

			/**
			 * Creates a new joint group and set it as 
			 * the default joint group.
			 * @name The name of the new joint group.
			 */
			void setJointGroup(const std::string& name);

			/**
			 * Call collisions on active space.
			 * @func The function to call when the collision happens.
			 */
			void collideActiveSpace(mCallFunc_t func = defaultCollision);

			/**
			 * Call collisions on all defined spaces.
			 * This function is really not suggested, since if the
			 * user is specifying lots of spaces it should control
			 * then individually, anyway, it exists for testing
			 * purposes.
			 *
			 * @func The function to call when the collision happens.
			 */
			void collideAllSpaces(mCallFunc_t func = defaultCollision);

			/**
			 * Step the integrator.
			 */
			void cycle();

			/**
			 * Get SurfaceParameters for collision
			 */
			dSurfaceParameters getParams();

			/**
			 * Find the active world id
			 */
			dWorldID getActiveWorld();
			
			/**
			 * Find the active space id
			 */
			dSpaceID getActiveSpace();

			/** 
			 * Find the active joint group
			 */
			dJointGroupID getActiveJointGroup();

			/**
			 * Find a world already created. 
			 * @return NULL if not found, otherwise returns the world id
			 */
			dWorldID _findWorld(const std::string& name);

			/**
			 * Find a space already created. 
			 * @return NULL if not found, otherwise returns the space id
			 */
			dSpaceID _findSpace(const std::string& name);

			/**
			 * Find a jointgroup already created. 
			 * @return NULL if not found, otherwise returns the jointgroup id
			 */
			dJointGroupID _findJointGroup(const std::string& name);

			//
			// GEOMETRIES
			//
			physicSphere* createSphere(vec_t radius);
			physicBox* createBox(vector3 length);
			physicTriMesh* createTriMesh(drawable3D* mesh);
			physicTriMesh* createTriMesh(const void* vertices, unsigned int numVertices, 
					const void* indices, unsigned int numIndices, const void* normals);
	};
}

#endif

