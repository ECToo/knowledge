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
#include "timer.h"
#include "material.h"
#include "sprite.h"
#include "camera.h"

namespace k 
{
	enum emissorType
	{
		EMISSOR_POINT,
		EMISSOR_PLANE
	};

	enum affectorType
	{
		AFFECTOR_ACCELERATOR,
		AFFECTOR_LINEAR,
		AFFECTOR_WAVE
	};

	/**
	 * Particle class.
	 */
	class particle
	{
		private:
			vector3 mPosition;

			/**
			 * This is applied to the corpse
			 * position each second. Of course
			 * that if it is called in a smaller
			 * interval, it is applied in smaller
			 * parts.
			 */
			vector3 mVelocity;
			vector3 mAcceleration;

			/**
			 * Last time we called the draw
			 * function
			 */
			unsigned long mLastDrawTime;
			
			/**
			 * The time (based on timer class)
			 * this particle was spawned.
			 */
			long mSpawnTime;

			/**
			 * Each particle has an independent radius.
			 */
			vec_t mRadius;

			/**
			 * Is this particle visible?
			 */
			bool mVisible;

		public:
			particle();

			const vector3& getVelocity();
			const vector3& getAcceleration();
			bool isVisible();

			unsigned int getSpawnTime();

			void setVelocity(vector3& vel);
			void setAcceleration(vector3& accel);
			void setPosition(vector3& pos);

			void setRadius(vec_t radi);
			void setVisibility(bool vis);
			void resetLife(long time);
			void draw(sprite* spr, long time);
	};

	class particleEmitter
	{
		protected:
			/**
			 * Particle initial radius.
			 */
			vec_t	mRadius;

			/**
			 * Lifetime (in msec) of each particle on the system.
			 */
			unsigned int mLifetime;

			/**
			 * Max number of particles on this system.
			 */
			unsigned int mMaxNumberOfParticles;

			/**
			 * This is the time (in msec) between each set of particles
			 * spawn.
			 */
			unsigned int mSpawnTime;

			/**
			 * This is the amount of particles to spawn.
			 */
			unsigned int mSpawnQuantity;

			/**
			 * Number of free particles in the system.
			 */
			unsigned int mFreeParticles;

			/**
			 * Timer to keep track of emitter
			 * events.
			 */
			timer mTimer;
			timer mSpawnTimer;

			/**
			 * The actual particles
			 */
			std::vector<particle>* mParticles;

			/**
			 * The material used by the particles.
			 */
			std::string mMaterial;

			/**
			 * This sprite is shared among the particles
			 * since they are all the same material, this is 
			 * pretty possible.
			 */
			sprite* mSprite;

			/**
			 * Point pos.
			 */
			vector3 mPosition;

			particle* findFreeParticle();

			void killParticle(particle* p);
			void spawnParticle(particle* p);

		public:
			particleEmitter(unsigned int numParticles, material* mat);
			particleEmitter(unsigned int numParticles, const std::string& mat);

			virtual ~particleEmitter();

			void setRadius(vec_t radi);
			void setSpawnQuantity(unsigned int amount);
			void setSpawnTime(long time);
			void setLifeTime(long time);
			void setMaterial(const std::string& mat);
			void setPosition(vector3& pos);

			virtual void feed() = 0;
			virtual void draw(camera* c) = 0;
	};
	
	class pointEmitter : public particleEmitter
	{
		protected:
			/**
			 * Initial velocities.
			 */
			vector3 mMinVelocity;
			vector3 mMaxVelocity;

			/**
			 * System acceleration.
			 */
			vector3 mAcceleration;

		public:
			pointEmitter(unsigned int numParticles, material* mat);
			pointEmitter(unsigned int numParticles, const std::string& mat);

			void spawnParticle(particle* p);

			void feed();
			void draw(camera* c);

			/**
			 * The emitter will release
			 * random particle velocity values between
			 * the minimum and the maximum values
			 */
			void setVelocity(vector3& min, vector3& max);
			void setAcceleration(vector3& accel);
	};

	/**
	 * The plane emitter uses a finite plane
	 * (defined by 4 vertexes) to generate particles.
	 * Each particle is positioned somewhere randomly
	 * within the plane bounds and then shoot with
	 * initial velocity and acceleration.
	 */
	class planeEmitter : public particleEmitter
	{
		protected:
			vector3 mVelocity;
			vector3 mAcceleration;

			/**
			 * Plane Vertices
			 *
			 * A--*
			 * |  |
			 * *--B
			 */
			vector3 mVertices[2];

		public:
			planeEmitter(unsigned int numParticles, material* mat);
			planeEmitter(unsigned int numParticles, const std::string& mat);

			void spawnParticle(particle* p);

			void feed();
			void draw(camera* c);

			void setBounds(vector3& a, vector3& b);
			void setVelocity(vector3& vel);
			void setAcceleration(vector3& accel);
	};

	class particleAffector
	{
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
			 * Emitters
			 */
			std::map<std::string, particleEmitter*> mEmitters;

			/**
			 * Affectors
			 */
			std::map<std::string, particleAffector*> mAffectors;

		public:
			particleSystem();
			~particleSystem();

			void setPosition(const vector3& pos);

			void setMaterial(const std::string& mat);
			void setMaterial(material* mat);

			void setMass(vec_t mass);

			void pushEmitter(const std::string& name, particleEmitter* em);

			void cycle(camera* c);
	};
}

#endif

