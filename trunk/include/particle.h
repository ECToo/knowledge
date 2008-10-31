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

#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "prerequisites.h"
#include "vector3.h"

namespace k 
{
	enum emissorType
	{
		EMISSOR_POINT,
		EMISSOR_PLANE,
		EMISSOR_CIRCUMFERENCE
	};

	enum affectorType
	{
		AFFECTOR_GRAVITY,
		AFFECTOR_LINEAR,
		AFFECTOR_WAVE
	};

	class particleEmitter
	{
		private:
			/**
			 * Particle radius
			 */
			vec_t	mParticleRadius;

			/**
			 * Lifetime (in msec) of each particle on the system.
			 */
			vec_t mParticleLifetime;

			/**
			 * Max number of particles on this system.
			 */
			vec_t mMaxNumberOfParticles;

			/**
			 * This is the time (in msec) between each particle
			 * spawn.
			 */
			vec_t mSpawnTime;
	};

	class particleSystem
	{
		private:
			/**
			 * System Position
			 */
			vector3 mPosition;

			/**
			 * Particles material
			 */
			material* mMaterial;

			/**
			 * Particle mass
			 */
			vec_t mParticleMass;

			/**
			 * Emissors
			 */
	};
}

#endif

