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
	 * \brief Defines a particle or particle system location.
	 * It also supports parenting, so affectors and emitters
	 * can offset from original particle system position.
	 */
	class DLL_EXPORT location
	{
		protected:
			vector3 mPosition;
			const location* mParent;

		public:
			/**
			 * Constructor
			 */
			location()
			{
				mParent = NULL;
			}

			/**
			 * Return world space position, if this location has a 
			 * parent, it will return the parent + this location position.
			 */
			vector3 getAbsolutePosition() const
			{
				if (mParent)
					return mParent->getAbsolutePosition() + mPosition;
				else
					return mPosition;
			}

			/**
			 * Return only this location position, independent of parents.
			 */
			const vector3& getRelativePosition() const
			{
				return mPosition;
			}

			/**
			 * Set location position.
			 * @param[in] pos The new position.
			 */
			void setPosition(const vector3& pos)
			{
				mPosition = pos;
			}

			/**
			 * Set location parent. This location position will be derived from
			 * the parent if you use getAbsolutePosition().
			 * @param[in] parent Thew new parent.
			 */
			void setParent(location* parent)
			{
				mParent = parent;
			}
	};

	/**
	 * \brief Particle representation.
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
			/**
			 * Constructor
			 */
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

	/**
	 * \brief Particle containers.
	 */
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
			/**
			 * Constructor.
			 */
			container();

			/**
			 * Destructor
			 */
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

	/**
	 * \brief This class is responsible for emitting particles.
	 * This class cant be used by itself, instead you should derivate
	 * a emitter type of it.
	 */
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
			/**
			 * Constructor. Creates an emitter as a child of a container.
			 *
			 * @param[in] cont A valid pointer to parent container.
			 */
			emitter(container* cont);

			/**
			 * Destructor.
			 */
			virtual ~emitter();

			/**
			 * Set emitter particle radius.
			 * @param[in] radi The new particle radius.
			 */
			void setRadius(vec_t radi);

			/**
			 * Set the amout of particles this emitter will emit per go.
			 * @param[in] amount The number of particles to be emitted.
			 */
			void setSpawnQuantity(unsigned int amount);

			/**
			 * Set the time interval in which this emitter will emit particles.
			 * @param[in] time Interval between particle emissions.
			 */
			void setSpawnTime(long time);

			/**
			 * Set the life time (in milliseconds) of the particles.
			 * @param[in] time Life time of the emitted particles.
			 */
			void setLifeTime(long time);

			/**
			 * The emitter will release
			 * random particle velocity values between
			 * the minimum and the maximum values
			 */
			void setVelocity(const vector3& min, const vector3& max);

			/**
			 * Set this emitter particles minimum velocity.
			 * @param[in] min Minimum particles velocity
			 */
			void setMinVelocity(const vector3& min);

			/**
			 * Set this emitter particles maximum velocity.
			 * @param[in] max Maximum particles velocity
			 */
			void setMaxVelocity(const vector3& max);

			void baseSpawn(particle* newP);

			/**
			 * Ask the emitter to spawn new particles.
			 */
			virtual void spawnParticles() = 0;

			/**
			 * Returns the axis-aligned bounding box of the emitter
			 */
			virtual const boundingBox getAABB() const = 0;
	};
	
	/**
	 * \brief Emits particles from a space point.
	 */
	class DLL_EXPORT pointEmitter : public emitter
	{
		protected:
			/**
			 * System acceleration.
			 */
			vector3 mAcceleration;

		public:
			/**
			 * Constructor. Creates an emitter as a child of a container.
			 *
			 * @param[in] cont A valid pointer to parent container.
			 */
			pointEmitter(container* cont);

			/**
			 * Destructor.
			 */
			~pointEmitter();

			/**
			 * Ask the emitter to spawn particles.
			 */
			void spawnParticles();

			/**
			 * Set particles acceleration.
			 * @param accel New acceleration.
			 */
			void setAcceleration(const vector3& accel);

			/**
			 * Return this emitter axis-aligned bounding box.
			 */
			const boundingBox getAABB() const;
	};

	/**
	 * \brief Emits particles from a space finite plane.
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
			/**
			 * Constructor. Creates an emitter as a child of a container.
			 *
			 * @param[in] cont A valid pointer to parent container.
			 */
			planeEmitter(container* cont);

			/**
			 * Destructor.
			 */
			~planeEmitter();

			/**
			 * Ask the emitter to spawn particles.
			 */
			void spawnParticles();

			/**
			 * Set the physical limits of particles emission.
			 *
			 * @param[in] a The minimum point of the limit box.
			 * @param[in] b The maximum point of the limit box.
			 */
			void setBounds(const vector3& a, const vector3& b);

			/**
			 * Set particles acceleration.
			 * @param accel New acceleration.
			 */
			void setAcceleration(const vector3& accel);
			
			/**
			 * Return this emitter axis-aligned bounding box.
			 */
			const boundingBox getAABB() const;
	};

	/**
	 * \brief Affects particles in many ways. 
	 * This class dont work by itself, it should be derivated.
	 */
	class DLL_EXPORT affector : public location
	{
		protected:
			unsigned int mAffectedParameter;
			unsigned int mAffectedOperation;

			vector3 mFactor;

			timer mTimer;

		public:

			/**
			 * Set the affected parameter this affector will change. @see affectedTypes
			 */
			void setAffectedParameter(unsigned int param);

			/**
			 * Set the affected operation this affector will execute. @see affectedOperations
			 */
			void setAffectedOperation(unsigned int op);

			/**
			 * Set the factor for the affected operation.
			 */
			void setFactor(const vector3& factor);

			/**
			 * Set the factor for the affected operation.
			 */
			void setFactor(vec_t factor);

			/**
			 * Reset affector time counter
			 */
			void resetTimer();

			/**
			 * Ask the affector to interact with particle.
			 * @param[in] p Particle to get interaction.
			 */
			virtual void interact(particle* p) = 0;
	};

	/**
	 * \brief Affect particle parameter in a linear fashion.
	 */
	class DLL_EXPORT linearAffector : public affector
	{
		public:
			/**
			 * Ask the affector to interact with particle.
			 * @param[in] p Particle to get interaction.
			 */
			void interact(particle* p);
	};

	/**
	 * \brief Base particle System.
	 */
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
			/**
			 * Constructor.
			 */
			system();

			/**
			 * Destructor, free all allocated particles.
			 */
			~system();

			/**
			 * Get system visi bility
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
			 * Return the axis aligned bounding box of the system.
			 */
			const boundingBox& getAABoundingBox() const
			{
				return mBounds;
			}

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

	/**
	 * \brief Controls the emission and life of particles.
	 */
	class DLL_EXPORT manager : public singleton<manager>
	{
		protected:
			std::map<std::string, system*> mSystems;

		public:
			/**
			 * Constructor.
			 */
			manager();

			/**
			 * Destructor, free all allocated particle systems.
			 */
			~manager();

			/**
			 * Get a particle system by name.
			 * @param[in] name Particle system name.
			 */
			system* getSystem(const std::string& name);

			/**
			 * Allocate a particle system.
			 * @param[in] name Particle system name.
			 */
			system* allocateSystem(const std::string& name);

			/**
			 * Return an particle::manager instance of this singleton
			 */
			static manager& getSingleton();

			/**
			 * Parse particle script by path.
			 * @param[in] filename The full path of particle script (from resourceManager root).
			 */
			void parseScript(const std::string& filename);

			/**
			 * Parse particle script by parsingFile.
			 * @param[in] file A valid pointer to the script parsingFile.
			 */
			void parseScript(parsingFile* file);

			void parseSystem(parsingFile* file, const std::string& psName);

			void parsePlaneEmitter(parsingFile* file, system* mSystem, const std::string& name);
			void parsePointEmitter(parsingFile* file, system* mSystem, const std::string& name);
			void parseLinearAffector(parsingFile* file, system* system, const std::string& name);

			/**
			 * Ask the particle manager to draw all particle systems.
			 * Note: Particle systems that are not visible to the camera will not be drawn.
			 */
			void drawParticles();
	};

} // particle
} // k

#endif

