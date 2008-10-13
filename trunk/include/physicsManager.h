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

// #include "ode_k/ode.h"
#include <ode/ode.h>
#include "prerequisites.h"
#include "vector2.h"
#include "vector3.h"
#include "drawable.h"
#include "quaternion.h"
#include "singleton.h"

#define ATTACH_POSITION (1 << 0)
#define ATTACH_ORIENTATION (1 << 1)

namespace k
{
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
			void attachDrawable(drawable3D* att)
			{
				assert(att != NULL);

				mAttached = att;
				mAttachedParams = 0;
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

			vector3 getPosition()
			{
				const dReal* pos = dGeomGetPosition(mGeom);
				return vector3(pos[0], pos[1], pos[2]);
			}

			void setPosition(vector3 pos)
			{
				dGeomSetPosition(mGeom, pos.x, pos.y, pos.z);
			}

			quaternion getOrientation()
			{
				dQuaternion rot;
				dGeomGetQuaternion(mGeom, rot);

				return quaternion(rot[1], rot[2], rot[3], rot[0]);
			}

			void setOrientation(quaternion ori)
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

		public:
			/**
			 * Creates a base world and space each named "default"
			 * The space is a simple space.
			 */
			physicsManager();
			~physicsManager();

			static physicsManager& getSingleton();

			/**
			 * Creates a new world and if it exists,
			 * set it as the default working one.
			 *
			 * @name The name of the new world.
			 */
			void setWorld(const std::string& name);
			void setGravity(vector3 grav);

			/**
			 * Creates a new space and if it exists,
			 * set it as the default working one.
			 *
			 * @name The name of the new space.
			 */
			void setSpace(const std::string& name);

			/**
			 * Creates a new joint group.
			 * @name The name of the new joint group.
			 */
			void createJointGroup(const std::string& name);
	};
}

#endif

