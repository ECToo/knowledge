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

#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "prerequisites.h"

#include "drawable.h"
#include "fileParser.h"
#include "material.h"
#include "sprite.h"
#include "timer.h"
#include "vector3.h"

namespace k {
namespace particle
{
	/**
	 * What paremeters the affectors
	 * can change in a particle.
	 */
	enum affectedTypes
	{
		AFF_POS = 0,
		AFF_VELOCITY,
		AFF_ACCELERATION,

		AFF_RADIUS,
		AFF_LIFETIME,

		MAX_AFFECTED
	};

	/**
	 * Bounds for emitters/affectors
	 */
	enum boundIndex
	{
		BOUND_MIN = 0,
		BOUND_MAX
	};

	/**
	 * How the factors affect
	 * the particle parameters
	 */
	enum affectedOperations
	{
		AFF_SUM = 0,
		AFF_SUB,
		AFF_MUL,
		AFF_DIV,

		MAX_AFFECT_OPERATIONS
	};

	/**
	 * Defines a particle or particle system location.
	 * It also supports parenting, so affectors and emitters
	 * can offset from original particle system position.
	 */
	class DLL_EXPORT location
	{
		protected:
			vector3 mPosition;
			const location* mParent;

		public:
			location()
			{
				mParent = NULL;
			}

			vector3 getAbsolutePosition() const
			{
				if (mParent)
					return mParent->getAbsolutePosition() + mPosition;
				else
					return mPosition;
			}

			const vector3& getRelativePosition() const
			{
				return mPosition;
			}

			void setPosition(const vector3& pos)
			{
				mPosition = pos;
			}

			void setParent(location* parent)
			{
				mParent = parent;
			}
	};

	/**
	 * Particle class.
	 */
	class DLL_EXPORT particle : public location
	{
		protected:
			/**
			 * Velocity change particle's position on each dt.
			 */
			vector3 mVelocity;

			/**
			 * Acceleration changes particle's velocity on each dt.
			 */
			vector3 mAcceleration;

			/**
			 * Last time we called the draw function.
			 *
			 * On each iteration, we can get dt as the difference
			 * between local time and mLastDrawTime.
			 */
			unsigned long mLastDrawTime;
			
			/**
			 * The time particle was spawned.
			 * The particle's life is defined by
			 * actual time - mSpawnTime.
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

			/**
			 * Life time of this particle.
			 */
			unsigned int mLifeTime;

		public:
			particle();

			/**
			 * Particle Velocity
			 */
			const vector3& getVelocity() const;

			/**
			 * Particle Acceleration
			 */
			const vector3& getAcceleration() const;

			/**
			 * Is this particle visible? If not, its
			 * not going to be drawn.
			 */
			bool isVisible() const;

			/**
			 * Time particle spawned. 
			 * This time is relative to the container mTime.
			 */
			unsigned int getSpawnTime() const;

			/**
			 * How much time this particle can live.
			 */
			unsigned int getLifeTime() const;

			/**
			 * Get particle radius.
			 */
			vec_t getRadius() const;

			/**
			 * Set particle velocity.
			 */
			void setVelocity(const vector3& vel);

			/**
			 * Set particle acceleration.
			 */
			void setAcceleration(const vector3& accel);

			/**
			 * Set particle life time. This is the amount
			 * of time (in milliseconds) the particle can live.
			 */
			void setLifeTime(unsigned int time);

			/**
			 * Set particle radius.
			 */
			void setRadius(vec_t radi);

			/**
			 * Set the particle visibility. If the particle
			 * is not visible, then its considered dead
			 * and will not be drawn.
			 */
			void setVisibility(bool vis);

			/**
			 * Change particle mSpawnTime to time.
			 */
			void resetLife(long time);

			/**
			 * Calculates dv/dt and dr/dt and update them.
			 * @time Actual time on container.
			 */
			void updatePhysics(long time);

			/**
			 * Draw particle, specifying the sprite used.
			 */
			void draw(sprite* spr);
	};

	class DLL_EXPORT container
	{
		protected:
			/**
			 * The actual particles
			 */
			std::vector<particle>* mParticles;

			/**
			 * This sprite is shared among the particles
			 * since they are all the same material, this is 
			 * pretty possible.
			 */
			sprite* mSprite;

			/**
			 * If we are using ARB point sprites, dont use
			 * sprite at all, build an array of particle
			 * positions and call it, F-A-S-T-ER.
			 */
			vec_t* mVertexPositions;

			/**
			 * Timer to update particle positions
			 * and keep track of events.
			 */
			timer mTimer;

			/**
			 * Max number of particles on this system.
			 */
			unsigned int mMaxNumberOfParticles;

			/**
			 * Number of free particles.
			 */
			unsigned int mFreeParticles;

			/**
			 * Material used to draw the particles.
			 */
			material* mMaterial;

		public:
			container();
			~container();
			
			/**
			 * Set particle quota.
			 */
			void setNumParticles(unsigned int amount);

			/**
			 * Set particles material.
			 */
			void setMaterial(const std::string& name);

			/**
			 * Set particles material.
			 */
			void setMaterial(material* mat);

			/**
			 * Returns a free particle.
			 */
			particle* findFreeParticle();

			/**
			 * Spawn a new particle
			 */
			particle* spawnParticle();

			/**
			 * Remove dead particles, update other
			 * positions.
			 */
			void update();

			/**
			 * Draw the particles on this container.
			 */
			void draw();
	};

	class DLL_EXPORT emitter : public location
	{
		protected:
			vector3 mMinVelocity;
			vector3 mMaxVelocity;

			/**
			 * Particle initial radius.
			 */
			vec_t	mRadius;

			/**
			 * Lifetime (in msec) of each particle on the system.
			 */
			unsigned int mLifetime;

			/**
			 * This is the time (in msec) between each set of particles
			 * spawn.
			 */
			long mSpawnTime;

			/**
			 * This is the amount of particles to spawn.
			 */
			unsigned int mSpawnQuantity;

			/**
			 * Timer to keep track of emitter
			 * events. Time that particles spawned
			 */
			timer mSpawnTimer;

			/**
			 * The particle container this emitter is child of.
			 */
			container* mContainer;

		public:
			emitter(container* cont);
			virtual ~emitter();

			void setRadius(vec_t radi);
			void setSpawnQuantity(unsigned int amount);
			void setSpawnTime(long time);
			void setLifeTime(long time);

			/**
			 * The emitter will release
			 * random particle velocity values between
			 * the minimum and the maximum values
			 */
			void setVelocity(const vector3& min, const vector3& max);
			void setMinVelocity(const vector3& min);
			void setMaxVelocity(const vector3& min);

			void baseSpawn(particle* newP);

			virtual void spawnParticles() = 0;
			virtual const boundingBox getAABB() const = 0;
	};
	
	class DLL_EXPORT pointEmitter : public emitter
	{
		protected:
			/**
			 * System acceleration.
			 */
			vector3 mAcceleration;

		public:
			pointEmitter(container* cont);
			~pointEmitter();

			void spawnParticles();
			void setAcceleration(const vector3& accel);

			const boundingBox getAABB() const;
	};

	/**
	 * The plane emitter uses a finite plane
	 * (defined by 4 vertexes) to generate particles.
	 * Each particle is positioned somewhere randomly
	 * within the plane bounds and then shoot with
	 * initial velocity and acceleration.
	 */
	class DLL_EXPORT planeEmitter : public emitter
	{
		protected:
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
			planeEmitter(container* cont);
			~planeEmitter();

			void spawnParticles();
			void setBounds(const vector3& a, const vector3& b);
			void setAcceleration(const vector3& accel);
			
			const boundingBox getAABB() const;
	};

	class DLL_EXPORT affector : public location
	{
		protected:
			unsigned int mAffectedParameter;
			unsigned int mAffectedOperation;

			vector3 mFactor;

			timer mTimer;

		public:

			void setAffectedParameter(unsigned int param);
			void setAffectedOperation(unsigned int op);

			void setFactor(const vector3& factor);
			void setFactor(vec_t factor);

			virtual void interact(particle* p) = 0;
	};

	/**
	 * Affect particle parameter in a linear fashion.
	 */
	class DLL_EXPORT linearAffector : public affector
	{
		public:
			void interact(particle* p);
	};

	class DLL_EXPORT system : public location, public container
	{
		protected:
			/**
			 * Particle mass
			 */
			vec_t mParticleMass;

			/**
			 * Emitters
			 */
			std::map<std::string, emitter*> mEmitters;

			/**
			 * Affectors
			 */
			std::map<std::string, affector*> mAffectors;

			/**
			 * System bounding box.
			 */
			boundingBox mBounds;

			/**
			 * System visibility.
			 */
			bool mIsVisible;

		public:
			system();
			~system();

			/**
			 * Get system visibility
			 */
			const bool getVisible() const;

			/**
			 * Set System visibility
			 */
			void setVisible(bool v);

			/**
			 * Calculate AA bounding box for this system.
			 */
			void _calculateAABB();

			/**
			 * Set system material, taken from materialManager
			 */
			void setMaterial(const std::string& mat);

			/**
			 * Set system material from pointer.
			 */
			void setMaterial(material* mat);

			/**
			 * Set particles mass.
			 */
			void setMass(vec_t mass);

			/**
			 * Add a new emitter on the system.
			 */
			void pushEmitter(const std::string& name, emitter* em);

			/**
			 * Add a new affector on the system.
			 */
			void pushAffector(const std::string& name, affector* aff);

			/**
			 * Update and draw everything
			 */
			void cycle();

			/**
			 * Draw system bounding box.
			 */
			void drawBounds();
	};

	class DLL_EXPORT manager : public singleton<manager>
	{
		protected:
			std::map<std::string, system*> mSystems;

		public:
			manager();
			~manager();

			system* getSystem(const std::string& name);
			system* allocateSystem(const std::string& name);

			static manager& getSingleton();

			void parseScript(const std::string& filename);
			void parseScript(parsingFile* file);

			void parseSystem(parsingFile* file, const std::string& psName);

			void parsePlaneEmitter(parsingFile* file, system* mSystem, const std::string& name);
			void parsePointEmitter(parsingFile* file, system* mSystem, const std::string& name);
			void parseLinearAffector(parsingFile* file, system* system, const std::string& name);

			void drawParticles();
	};

} // particle
} // k

#endif

