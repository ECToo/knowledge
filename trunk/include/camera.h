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
#include "drawable.h"
#include "ray.h"

namespace k
{
	/**
	 * Frustum planes
	 */
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

	/**
	 * \brief Controls the rendering and transformations on the view.
	 * Controls the transformation and views
	 * of active renderer scenes. @see k::renderer
	 */
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
			/**
			 * Creates a default camera with the following options
			 * fov = 90, aspect ratio = 1.3333f, near plane = 0.1f, far plane = 5000
			 */
			camera();

			/**
			 * Copy the camera matrix to the current
			 * modelview matrix. Needs to be called 
			 * before drawing each object, handled by
			 * renderer. @see k::renderer
			 */
			void copyView() const;

			/**
			 * Apply the camera modelview over the scene modelview.
			 */
			void setView();

			/**
			 * Set camera Field of View (in degrees)
			 * @see mFov
			 * @param fov The new camera field of view.
			 */
			void setFov(unsigned int fov);

			/**
			 * Set the camera aspect ratio
			 * @see mAspectRatio
			 * @param ar The new camera aspect ratio.
			 */
			void setAspectRatio(vec_t ar);

			/**
			 * Set camera planes.
			 * @see mNearPlane
			 * @see mFarPlane
			 * @param nearP The new near plane distace.
			 * @param farP The new far plane distace.
			 */
			void setPlanes(vec_t nearP, vec_t farP);

			/**
			 * Set camera perspective projection parameters.
			 * @see mFov
			 * @see mAspectRatio
			 * @see mNearPlane
			 * @see mFarPlane
			 * @param fov The new camera field of view.
			 * @param ar The new camera aspect ratio.
			 * @param nearP The new near plane distance.
			 * @param farP The new far plane distance.
			 */
			void setPerspective(unsigned int fov, vec_t ar, vec_t nearP, vec_t farP);

			/**
			 * Called from the renderer to apply the camera
			 * perspective settings to the scene. @see k::renderer
			 */
			void setPerspective();

			/**
			 * Check if a point is inside the view
			 * frustum.
			 *
			 * @param point The point to be tested
			 */
			bool isPointInsideFrustum(const vector3& point) const;

			/**
			 * Check if a sphere is inside the view
			 * frustum.
			 *
			 * @param center The center of the sphere.
			 * @param radius The sphere radius
			 */
			bool isSphereInsideFrustum(const vector3& center, vec_t radius) const;

			/**
			 * Check if a axis aligned box is inside the view
			 * frustum.
			 *
			 * @param AABB The axis aligned box edges.
			 */
			bool isBoxInsideFrustum(const boundingBox& AABB) const;

			/**
			 * Changes the camera orientation to face a point.
			 *
			 * @param dest The looking point.
			 */
			void lookAt(vector3 dest);

			/**
			 * Get the direction camera is pointing at.
			 */
			const vector3 getDirection() const;

			/**
			 * Get the camera up vector.
			 */
			const vector3 getUp() const;

			/**
			 * Get the camera right vector.
			 */
			const vector3 getRight() const;

			/**
			 * Generate a unit vector from camera position
			 * to where pointer coords are. Note that the coordinates
			 * will be normalized to frustum coordinates (-1, 1) so you 
			 * only need to pass standards coordinate (0, getScreenWidth() / getScreenHeight)
			 *
			 * @param coords Coordinates of the pointer. 
			 */
			const ray projectRayFrom2D(const vector2& coords) const;

			/**
			 * Set camera position.
			 *
			 * @param pos The new camera position.
			 */
			void setPosition(const vector3& pos);

			/**
			 * Get the camera position.
			 */
			const vector3& getPosition() const;

			/**
			 * Set camera orientation.
			 *
			 * @param ori The new camera orientation
			 */
			void setOrientation(const quaternion& ori);

			/**
			 * Get the camera orientation
			 */
			const quaternion& getOrientation() const;

			/**
			 * Get the inverse transpose matrix of the cameras orientation.
			 */
			const matrix4& getRotInverseTranspose() const;

			/**
			 * Get the inverse transpose matrix of the camera.
			 */
			const matrix4& getInverseTranspose() const;
	};
}

#endif

