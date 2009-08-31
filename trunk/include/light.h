/*
    Copyright (C) 2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

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

#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "prerequisites.h"
#include "color.h"
#include "vector3.h"
#include "quaternion.h"
#include "drawable.h"

namespace k {
namespace light {

enum attenuationParameters
{
	ATT_CONSTANT,
	ATT_LINEAR,
	ATT_QUADRATIC
};

enum lightSources
{
	POSITIONAL,
	DIRECTIONAL
};

/**
 * \brief A virtual light representation on knowledge.
 *
 * Those represent vertex point lights. Keep in mind
 * that in future we will introduce pixel shaders
 * for OpenGL and DirectX render systems and those
 * lights will still be valid but they will define
 * parameters to be used in the shaders.
 *
 * The light always cover the area within its range. 
 * If an object is outside the range, the light will be discarded
 * from the drawing process.
 */
class DLL_EXPORT light
{
	protected:
		/**
		 * The range in space this light covers.
		 */
		vec_t mRange;

		/**
		 * Light position
		 */
		vector3 mPosition;

		/**
		 * Light attenuation. Use the vector
		 * to define each attenuation parameter.
		 *
		 * Ex: 
		 * mAttenuation.vec[ATT_QUADRATIC] = 1.0f;
		 */
		vector3 mAttenuation;

		/**
		 * color components of the light
		 */
		color mDiffuse;
		color mSpecular;
		color mAmbient;

		/**
		 * Drawable this light is attached to.
		 */
		drawable3D* mAttach;

		/**
		 * Is this light enabled?
		 */
		bool mEnabled;

	public:
		/**
		 * Spawns a point light.
		 *
		 * Never create a light by yourself, you should
		 * ask the renderer to do it.
		 */
		light()
		{
			mEnabled = true;
			mAttach = NULL;
			mRange = 300;
			mAmbient = color(0.5, 0.5, 0.5, 1.0);
			mDiffuse = color(1.0, 1.0, 1.0, 1.0);
			mSpecular = color(0.5, 0.5, 0.5, 1.0);
			mAttenuation = vector3(1, 0, 0);
		}

		/**
		 * Set the drawable3D this light is attached
		 * to.
		 *
		 * @obj The drawable.
		 */
		void setAttach(drawable3D* obj)
		{
			mAttach = obj;
		}

		/**
		 * Get the drawable this light is attached
		 * to.
		 *
		 * @returns The Drawable3D light is attached.
		 */
		drawable3D* getAttach() const
		{
			return mAttach;
		}

		/**
		 * Set if the light is enabled or not.
		 */
		void setEnabled(bool state)
		{
			mEnabled = state;
		}

		/**
		 * Get if light is enabled or not.
		 */
		bool getEnabled() const
		{
			return mEnabled;
		}

		/**
		 * Set light position, keep in mind that
		 * the new position must be in world space.
		 *
		 * @pos New light position in world space.
		 */
		void setPosition(const vector3& pos)
		{
			mPosition = pos;
		}

		/**
		 * Get Light position
		 * @return Light position in world space.
		 */
		vector3 getPosition() const
		{
			if (mAttach)
				return mAttach->getAbsolutePosition() + mPosition;
			else
				return mPosition;
		}

		/**
		 * Check if an object position is within lights
		 * range
		 */
		bool isInLightRange(const vector3& objPosition)
		{
			vec_t dist = 0;

			if (mAttach)
				dist = (mAttach->getAbsolutePosition() + mPosition).distance(objPosition);
			else
				dist = mPosition.distance(objPosition);

			if (dist <= mRange)
				return true;
			else
				return false;
		}

		/**
		 * Set Light range.
		 */
		void setRange(vec_t range)
		{
			mRange = range;
		}

		/**
		 * Get Light Range.
		 */
		const vec_t& getRange() const
		{
			return mRange;
		}

		/**
		 * Set Attenuation.
		 * Use the vector to define each 
		 * attenuation parameter.
		 *
		 * Ex: 
		 * mAttenuation.vec[ATT_QUADRATIC] = 1.0f;
		 */
		void setAttenuation(const vector3& att)
		{
			mAttenuation = att;
		}

		/**
		 * Get light's attenuation
		 */
		const vector3& getAttenuation() const
		{
			return mAttenuation;
		}

		/**
		 * Set light diffuse color.
		 */
		void setDiffuse(const color& diff)
		{
			mDiffuse = diff;
		}

		/**
		 * Get light diffuse color. 
		 */
		const color& getDiffuse() const
		{
			return mDiffuse;
		}

		/**
		 * Set light specular color.
		 */
		void setSpecular(const color& spec)
		{
			mSpecular = spec;
		}

		/**
		 * Get light specular color. 
		 */
		const color& getSpecular() const
		{
			return mSpecular;
		}

		/**
		 * Set light ambient color.
		 */
		void setAmbient(const color& amb)
		{
			mAmbient = amb;
		}

		/**
		 * Get light ambient color. 
		 */
		const color& getAmbient() const
		{
			return mAmbient;
		}
};

} } // namespaces

#endif

