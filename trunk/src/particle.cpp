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

#include "particle.h"
#include "materialManager.h"
#include "rendersystem.h"
#include "root.h"

namespace k {
			
particle::particle()
{
	mVisible = false;
	mLastDrawTime = 0;
}

const vector3& particle::getVelocity()
{
	return mVelocity;
}

const vector3& particle::getAcceleration()
{
	return mAcceleration;
}

void particle::setVelocity(vector3& vel)
{
	mVelocity = vel;
}
			
void particle::setPosition(vector3& pos)
{
	mPosition = pos;
}

void particle::setAcceleration(vector3& accel)
{
	mAcceleration = accel;
}

bool particle::isVisible()
{
	return mVisible;
}
			
unsigned int particle::getSpawnTime()
{
	return mSpawnTime;
}

void particle::setVisibility(bool vis)
{
	mVisible = vis;
}

void particle::resetLife(long time)
{
	mSpawnTime = time;
	mLastDrawTime = time;
}
			
void particle::setRadius(vec_t radi)
{
	mRadius = radi;
}

void particle::draw(sprite* spr, long time)
{
	if (!mVisible)
		return;

	assert(spr != NULL);

	// Update sprite position
	vec_t timeScaleDiff = (time - mLastDrawTime)/1000.0f;
	mLastDrawTime = time;

	// dv/dt
	if (mAcceleration != vector3::zero)
	{
		mVelocity += mAcceleration * timeScaleDiff;
	}

	// dr/dt
	mPosition += mVelocity * timeScaleDiff;

	// sprite
	spr->setPosition(mPosition);
	spr->setRadius(mRadius);
	spr->draw();
}
			
particleEmitter::particleEmitter(unsigned int numParticles, material* mat)
{
	assert(numParticles != 0);
	assert(mat != NULL);

	mParticles = new std::vector<particle>;
	assert(mParticles != NULL);

	mTimer.reset();

	mSprite = new sprite(mat, 0);
	assert(mSprite != NULL);

	mFreeParticles = numParticles;
}

particleEmitter::particleEmitter(unsigned int numParticles, const std::string& mat)
{
	assert(numParticles != 0);

	mParticles = new std::vector<particle>(numParticles);
	mTimer.reset();

	mSprite = new sprite(materialManager::getSingleton().getMaterial(mat), 0);
	assert(mSprite != NULL);

	mFreeParticles = numParticles;
}

void particleEmitter::killParticle(particle* p)
{
	assert(p != NULL);
	p->setVisibility(false);

	mFreeParticles++;
}

void particleEmitter::setRadius(vec_t radi)
{
	mRadius = radi;
	for (std::vector<particle>::iterator it = mParticles->begin();
			it != mParticles->end(); it++)
	{
		(*it).setRadius(radi);
	}
}
			
void particleEmitter::spawnParticle(particle* p)
{
	assert(p != NULL);
	assert(mFreeParticles > 0);

	p->setVisibility(true);
	p->resetLife(mTimer.getMilliSeconds());
	p->setRadius(mRadius);

	mFreeParticles--;
}

void particleEmitter::setSpawnQuantity(unsigned int amount)
{
	mSpawnQuantity = amount;
}

void particleEmitter::setSpawnTime(long time)
{
	mSpawnTime = time;
}

void particleEmitter::setLifeTime(long time)
{
	mLifetime = time;
}

void particleEmitter::setMaterial(const std::string& mat)
{
	mMaterial = mat;
}
			
particleEmitter::~particleEmitter()
{
	delete mParticles;
}
			
particle* particleEmitter::findFreeParticle()
{
	for (std::vector<particle>::iterator it = mParticles->begin();
			it != mParticles->end(); it++)
	{
		if (!(*it).isVisible())
			return &(*it);
	}

	return NULL;
}

void particleEmitter::setPosition(vector3& pos)
{
	mPosition = pos;
}
			
pointEmitter::pointEmitter(unsigned int numParticles, material* mat)
	: particleEmitter(numParticles, mat)
{
}

pointEmitter::pointEmitter(unsigned int numParticles, const std::string& mat)
	: particleEmitter(numParticles, mat)
{
}

void pointEmitter::spawnParticle(particle* p)
{
	particleEmitter::spawnParticle(p);

	p->setPosition(mPosition);

	vector3 randVel = vector3(rand()%25 * pow(-1, rand()), rand() % 20 * 2, 0);
	p->setVelocity(randVel);
	// p->setVelocity(mVelocity);
	p->setAcceleration(mAcceleration);
}
					
void pointEmitter::feed()
{
	long timeElapsed = mTimer.getMilliSeconds();

	for (std::vector<particle>::iterator it = mParticles->begin();
			it != mParticles->end(); it++)
	{
		particle* p = &(*it);
		
		if (p->isVisible() && (timeElapsed - p->getSpawnTime()) > mLifetime)
		{
			// Particle is dead
			killParticle(p);
		}
	}
			
	// Spawn new particles
	if (mSpawnTimer.getMilliSeconds() > mSpawnTime)
	{
		// Spawn
		if (mSpawnQuantity <= mFreeParticles)
		{
			for (int i = 0; i < mSpawnQuantity; i++)
				spawnParticle(findFreeParticle());
		}

		// Reset Timer
		mSpawnTimer.reset();
	}
}

void pointEmitter::draw(camera* c)
{
	for (std::vector<particle>::iterator it = mParticles->begin();
			it != mParticles->end(); it++)
	{
		particle* p = &(*it);

		if (p->isVisible())
		{
			if (c == NULL)
			{
				renderSystem* rs = root::getSingleton().getRenderSystem();
				assert(rs != NULL);

				rs->setMatrixMode(MATRIXMODE_MODELVIEW);
				rs->identityMatrix();
			}
			else
			{
				c->copyView();
			}

			p->draw(mSprite, mTimer.getMilliSeconds());
		}
	}
}
			
void pointEmitter::setVelocity(vector3& vel)
{
	mVelocity = vel;
}

void pointEmitter::setAcceleration(vector3& accel)
{
	mAcceleration = accel;
}

particleSystem::particleSystem()
{
}

particleSystem::~particleSystem()
{
}

void particleSystem::setPosition(const vector3& pos)
{
	mPosition = pos;
}

void particleSystem::setMaterial(const std::string& mat)
{
	mMaterial = materialManager::getSingleton().getMaterial(mat);
}

void particleSystem::setMaterial(material* mat)
{
	assert(mat != NULL);
	mMaterial = mat;
}

void particleSystem::setMass(vec_t mass)
{
	mParticleMass = mass;
}

void particleSystem::pushEmitter(const std::string& name, particleEmitter* em)
{
	assert(em != NULL);
	mEmissors[name] = em;

	em->setPosition(mPosition);
}
			
void particleSystem::cycle(camera* c)
{
	std::map<std::string, particleEmitter*>::const_iterator it;
	for (it = mEmissors.begin(); it != mEmissors.end(); it++)
	{
		particleEmitter* em = it->second;

		em->feed();
		em->draw(c);
	}

	/*
	std::map<std::string, particleAffector*>::const_iterator ait;
	for (ait = mAffectors.begin(); ait != mAffectors.end(); ait++)
	{
	}
	*/
}

}

