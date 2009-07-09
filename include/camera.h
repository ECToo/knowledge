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

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "prerequisites.h"
#include "vector2.h"
#include "vector3.h"
#include "quaternion.h"

namespace k
{
	enum frustumPlanes
	{
		PLANE_NEAR = 0,
		PLANE_FAR,
		PLANE_TOP,
		PLANE_BOTTOM,
		PLANE_LEFT,
		PLANE_RIGHT,

		MAX_PLANES
	};

	class DLL_EXPORT camera
	{
		private:
			// Matrix to keep camera
			// orientation + translation
			matrix4 mFinal;
			matrix4 mFinalInverse;

			// Inverse transpose of the rotation only
			matrix4 mOrientationInverse;

			vector3 mPosition;
			quaternion mOrientation;

			/**
			 * Camera field of view.
			 * This is the angle of aperture the camera can see from
			 * its position into the view direction. The default value
			 * for this is 90.
			 */
			unsigned int mFov;
			vec_t mTanFov;

			/**
			 * The aspect ratio for the view perspective.
			 * The default value for this is 1.33 (4:3)
			 */
			vec_t mAspectRatio;

			/**
			 * The Distance of the near plane of camera.
			 * The default distance is 0.1
			 */
			vec_t mNearPlane;

			/**
			 * The distance of the far plane of camera.
			 * This is the value of the most far position
			 * of an object the camera can see from its position.
			 * The default is 1000.
			 */
			vec_t mFarPlane;

			/**
			 * A point inside each frustum plane.
			 */
			vector3 mFrustumPlanes[MAX_PLANES];

			/**
			 * The D argument for each frustum 
			 * plane.
			 */
			vec_t mFrustumDs[MAX_PLANES];

		public:
			camera();

			// Copy the camera matrix to the current
			// modelview matrix. Needs to be called
			// before drawing each object.
			void copyView() const;

			// Apply the modelview matrix to the scene
			void setView();

			/**
			 * Set camera Field of View
			 * @see mFov
			 */
			void setFov(unsigned int fov);

			/**
			 * Set the camera aspect ratio
			 * @see mAspectRatio
			 */
			void setAspectRatio(vec_t ar);

			/**
			 * Set camera planes.
			 * @see mNearPlane
			 * @see mFarPlane
			 */
			void setPlanes(vec_t nearP, vec_t farP);

			/**
			 * Set camera perspective projection parameters.
			 * @see mFov
			 * @see mAspectRatio
			 * @see mNearPlane
			 * @see mFarPlane
			 */
			void setPerspective(unsigned int fov, vec_t ar, vec_t nearP, vec_t farP);

			/**
			 * Called from the renderer to apply the camera
			 * perspective settings to the scene.
			 */
			void setPerspective();

			/**
			 * Check if a point is inside the view
			 * frustum
			 */
			bool isPointInsideFrustum(const vector3& point) const;
			bool isSphereInsideFrustum(const vector3& center, vec_t radius) const;
			bool isBoxInsideFrustum(const vector3& mins, const vector3& maxs) const;

			// Look at the vector dest
			void lookAt(vector3 dest);
			const vector3 getDirection() const;
			const vector3 getUp() const;
			const vector3 getRight() const;

			/**
			 * Generate a unit vector from camera position
			 * to where pointer coords are.
			 */
			const vector3 projectRayFrom2D(const vector2& coords) const;

			// Translations
			void setPosition(vector3 pos);
			const vector3& getPosition() const;

			// Orientation
			void setOrientation(quaternion ori);
			const quaternion& getOrientation() const;

			// Get the inverse transpose matrix
			const matrix4& getRotInverseTranspose() const;
			const matrix4& getInverseTranspose() const;
	};
}

#endif

