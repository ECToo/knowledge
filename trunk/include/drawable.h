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

#ifndef _DRAWABLE_H_
#define _DRAWABLE_H_

#include "prerequisites.h"
#include "vector2.h"
#include "vector3.h"
#include "quaternion.h"

namespace k 
{
	class DLL_EXPORT drawable2D
	{
		protected:
			vector2 mPosition;
			vec_t mZ;

			/**
			 * Width x Height
			 */
			vector2 mScale;

			/**
			 * Rotation in degrees
			 */
			vec_t mRotation;

			// Another Drawable this one
			// is attached to
			const drawable2D* mAttach;

		public:
			drawable2D();
			virtual ~drawable2D();

			void setPosition(const vector2& pos);
			void setRotation(const vec_t rot);
			void setScale(const vector2& size);
			void setZ(vec_t z);

			void attach(const drawable2D* target);
			const drawable2D* getRoot() const;

			const vector2& getRelativePosition() const;
			const vec_t getRelativeRotation() const;

			vector2 getAbsolutePosition() const;
			vec_t getAbsoluteRotation() const;

			const vector2& getDimension() const;
			const vec_t getZ() const;

			virtual void draw() = 0;
	};

	class DLL_EXPORT boundingBox
	{
		private:
			vector3 mMins;
			vector3 mMaxs;

		public:
			boundingBox();
			boundingBox(const vector3& min, const vector3& max);

			boundingBox operator + (const boundingBox& b);
			boundingBox& operator += (const boundingBox& b);

			boundingBox operator - (const boundingBox& b);
			boundingBox& operator -= (const boundingBox& b);

			void setTestMins(const vector3& min);
			void setTestMaxs(const vector3& max);
			void setTest(const vector3& dist);

			void setMins(const vector3& min);
			void setMaxs(const vector3& max);

			const vector3& getMins() const;
			const vector3& getMaxs() const;

			void draw() const;
	};

	class DLL_EXPORT drawable3D
	{
		protected:
			vector3 mScale;
			vector3 mPosition;
			quaternion mOrientation;

			// Draws bounding box? Default to false
			bool mDrawAABB;

			// Another Drawable this one
			// is attached to
			const drawable3D* mAttach;

		public:
			drawable3D();
			virtual ~drawable3D();

			void setOrientation(const quaternion& orientation);
			void setScale(const vector3& scale);
			void setScale(const vec_t scale);
			void setPosition(const vector3& pos);

			void attach(const drawable3D* target);
			const drawable3D* getRoot() const;

			const vector3& getScale() const;

			vector3 getAbsolutePosition() const;
			quaternion getAbsoluteOrientation() const;

			const vector3& getRelativePosition() const;
			const quaternion& getRelativeOrientation() const;

			void setDrawBoundingBox(bool option);
			bool getDrawBoundingBox() const;

			virtual void draw() = 0;
			virtual bool isOpaque() const = 0;

			virtual boundingBox getAABoundingBox() const = 0;
			virtual boundingBox getBoundingBox() const = 0;
	};
}

#endif

