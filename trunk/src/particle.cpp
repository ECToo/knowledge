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

static inline int getRandNonZero()
{
	int random = rand();
	while (!random)
		random = rand();

	return random;
}
			
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

void particle::setVelocity(const vector3& vel)
{
	mVelocity = vel;
}
			
void particle::setPosition(const vector3& pos)
{
	mPosition = pos;
}

void particle::setAcceleration(const vector3& accel)
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
	if (!mVisible || !spr)
		return;

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
	spr->rawDraw();
}
			
particleEmitter::particleEmitter()
{
	mParticles = NULL;
	mSprite = NULL;

	mTimer.reset();
	mFreeParticles = 0;
}

particleEmitter::particleEmitter(unsigned int numParticles, material* mat)
{
	kAssert(numParticles != 0);
	kAssert(mat != NULL);

	mParticles = new std::vector<particle>(numParticles);
	kAssert(mParticles != NULL);

	mTimer.reset();

	mSprite = new sprite(mat, 0);
	kAssert(mSprite != NULL);

	mFreeParticles = numParticles;
}

particleEmitter::particleEmitter(unsigned int numParticles, const std::string& mat)
{
	kAssert(numParticles != 0);

	mParticles = new std::vector<particle>(numParticles);
	mTimer.reset();

	mSprite = new sprite(mat, 0);
	kAssert(mSprite);

	mFreeParticles = numParticles;
}

void particleEmitter::killParticle(particle* p)
{
	kAssert(p != NULL);
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
	kAssert(p != NULL);
	kAssert(mFreeParticles > 0);

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
			
void particleEmitter::setNumParticles(unsigned int amount)
{
	if (mParticles)
		delete mParticles;

	mParticles = new std::vector<particle>(amount);
	kAssert(mParticles);

	mFreeParticles = amount;
}

void particleEmitter::setMaterial(const std::string& mat)
{
	mMaterial = mat;
	mMaterialPtr = materialManager::getSingleton().getMaterial(mat);

	if (mSprite)
	{
		mSprite->setMaterial(mat);
	}
	else
	{
		mSprite = new sprite(mat, 0);
		kAssert(mSprite);
	}
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

void particleEmitter::setPosition(const vector3& pos)
{
	mPosition = pos;
}

pointEmitter::pointEmitter()
	: particleEmitter()
{
}
			
pointEmitter::pointEmitter(unsigned int numParticles, material* mat)
	: particleEmitter(numParticles, mat)
{
}

pointEmitter::pointEmitter(unsigned int numParticles, const std::string& mat)
	: particleEmitter(numParticles, mat)
{
}

static inline vector3 getRandomVelocity(const vector3& mMaxVelocity, const vector3& mMinVelocity)
{
	vector3 randVel;
	if (mMaxVelocity != mMinVelocity)
	{
		int tmp;

		if ((tmp = (int)(mMaxVelocity.x - mMinVelocity.x + 1)) != 0 && (mMaxVelocity.x != mMinVelocity.x))
			randVel.x = getRandNonZero() % tmp + mMinVelocity.x;
		else
			randVel.x = mMinVelocity.x;

		if ((tmp = (int)(mMaxVelocity.y - mMinVelocity.y + 1)) != 0 && (mMaxVelocity.y != mMinVelocity.y))
			randVel.y = getRandNonZero() % tmp + mMinVelocity.y;
		else
			randVel.y = mMinVelocity.y;

		if ((tmp = (int)(mMaxVelocity.z - mMinVelocity.z + 1)) != 0 && (mMaxVelocity.z != mMinVelocity.z))
			randVel.z = getRandNonZero() % tmp + mMinVelocity.z;
		else
			randVel.z = mMinVelocity.z;
	}
	else
	{
		randVel = mMinVelocity;
	}

	return randVel;
}

void pointEmitter::spawnParticle(particle* p)
{
	particleEmitter::spawnParticle(p);

	p->setPosition(mPosition);

	/**
	 * Randomize particles velocity between minimum and max speeds
	 */
	p->setVelocity(getRandomVelocity(mMaxVelocity, mMinVelocity));
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
			for (unsigned short i = 0; i < mSpawnQuantity; i++)
				spawnParticle(findFreeParticle());
		}

		// Reset Timer
		mSpawnTimer.reset();
	}
}

void pointEmitter::draw(camera* c)
{
	if (!mSprite || !mParticles || !mMaterialPtr)
		return;
				
	renderSystem* rs = root::getSingleton().getRenderSystem();
	kAssert(rs);

	rs->setDepthMask(false);
	mMaterialPtr->prepare();

	for (std::vector<particle>::iterator it = mParticles->begin();
			it != mParticles->end(); it++)
	{
		particle* p = &(*it);

		if (p->isVisible())
		{
			if (c == NULL)
			{
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

	rs->setDepthMask(true);
	mMaterialPtr->finish();
}
			
void particleEmitter::setVelocity(const vector3& min, const vector3& max)
{
	mMinVelocity = min;
	mMaxVelocity = max;
}

void particleEmitter::setMinVelocity(const vector3& min)
{
	mMinVelocity = min;
}

void particleEmitter::setMaxVelocity(const vector3& max)
{
	mMaxVelocity = max;
}

void pointEmitter::setAcceleration(const vector3& accel)
{
	mAcceleration = accel;
}
			
planeEmitter::planeEmitter()
	: particleEmitter()
{
}

planeEmitter::planeEmitter(unsigned int numParticles, material* mat)
	: particleEmitter(numParticles, mat)
{
}

planeEmitter::planeEmitter(unsigned int numParticles, const std::string& mat)
	: particleEmitter(numParticles, mat)
{
}

/**
 * Here we set the plane bounds.
 *
 * A--*
 * |  |
 * *--B
 */

void planeEmitter::setBounds(const vector3& a, const vector3& b)
{
	mVertices[0] = a;
	mVertices[1] = b;
}

void planeEmitter::spawnParticle(particle* p)
{
	particleEmitter::spawnParticle(p);

	/**
	 * Randomize particles position between plane bounds
	 */
	vector3 randPos = mPosition;
	int tmp;

	if ((tmp = (int)(mVertices[1].x - mVertices[0].x)) != 0)
		randPos.x += getRandNonZero() % tmp + mVertices[0].x;
	else
		randPos.x += mVertices[0].x;

	if ((tmp = (int)(mVertices[1].y - mVertices[0].y)) != 0)
		randPos.y += getRandNonZero() % tmp + mVertices[0].y;
	else
		randPos.y += mVertices[0].y;

	if ((tmp = (int)(mVertices[1].z - mVertices[0].z)) != 0)
		randPos.z += getRandNonZero() % tmp + mVertices[0].z;
	else
		randPos.z += mVertices[0].z;

	p->setPosition(randPos);
	p->setVelocity(getRandomVelocity(mMinVelocity, mMaxVelocity));
	p->setAcceleration(mAcceleration);
}
					
void planeEmitter::feed()
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
			for (unsigned short i = 0; i < mSpawnQuantity; i++)
				spawnParticle(findFreeParticle());
		}

		// Reset Timer
		mSpawnTimer.reset();
	}
}

void planeEmitter::draw(camera* c)
{
	if (!mSprite || !mParticles || !mMaterialPtr)
		return;

	renderSystem* rs = root::getSingleton().getRenderSystem();
	kAssert(rs);

	rs->setDepthMask(false);
	mMaterialPtr->prepare();

	for (std::vector<particle>::iterator it = mParticles->begin();
			it != mParticles->end(); it++)
	{
		particle* p = &(*it);

		if (p->isVisible())
		{
			if (c == NULL)
			{
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

	rs->setDepthMask(true);
	mMaterialPtr->finish();
}

void planeEmitter::setAcceleration(const vector3& accel)
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

	/*
	std::map<std::string, particleAffector*>::iterator it;
	for (it = mAffectors.begin(); it != mAffectors.end(); it++)
	{
		it->second->setPosition(pos);
	}
	*/

	std::map<std::string, particleEmitter*>::iterator eIt;
	for (eIt = mEmitters.begin(); eIt != mEmitters.end(); eIt++)
	{
		eIt->second->setPosition(pos);
	}
}
			
const vector3& particleSystem::getPosition()
{
	return mPosition;
}

void particleSystem::setMaterial(const std::string& mat)
{
	mMaterial = materialManager::getSingleton().getMaterial(mat);
}

void particleSystem::setMaterial(material* mat)
{
	kAssert(mat != NULL);
	mMaterial = mat;
}

void particleSystem::setMass(vec_t mass)
{
	mParticleMass = mass;
}

void particleSystem::pushEmitter(const std::string& name, particleEmitter* em)
{
	kAssert(em != NULL);
	mEmitters[name] = em;

	em->setPosition(mPosition);
}
			
void particleSystem::cycle(camera* c)
{
	std::map<std::string, particleEmitter*>::const_iterator it;
	for (it = mEmitters.begin(); it != mEmitters.end(); it++)
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

template<> particleManager* singleton<particleManager>::singleton_instance = 0;

particleManager& particleManager::getSingleton()
{  
	kAssert(singleton_instance);
	return (*singleton_instance);  
}

particleManager::particleManager()
{
	mSystems.clear();
}

particleManager::~particleManager()
{
}

particleSystem* particleManager::getParticleSystem(const std::string& name)
{
	std::map<std::string, particleSystem*>::iterator it = mSystems.find(name);
	if (it != mSystems.end())
		return it->second;
	else
		return NULL;
}

particleSystem* particleManager::allocatePS(const std::string& name)
{
	particleSystem* newSystem = getParticleSystem(name);
	if (!newSystem)
	{
		newSystem = new particleSystem();
		if (newSystem)
		{
			mSystems[name] = newSystem;
		}
	}

	return newSystem;
}

static inline vector3 readVector(parsingFile* file)
{
	vector3 vec;

	std::string token = file->getNextToken();
	vec.x = atof(token.c_str());
	token = file->getNextToken();
	vec.y = atof(token.c_str());
	token = file->getNextToken();
	vec.z = atof(token.c_str());

	return vec;
}

static inline std::string parseEmitterGenericToken(parsingFile* file, const std::string& parsedToken, particleEmitter* system)
{
	kAssert(file);
	kAssert(system);

	// Current Token
	std::string token = parsedToken;

	if (token == "minVelocity")
	{
		system->setMinVelocity(readVector(file));
	}
	else
	if (token == "maxVelocity")
	{
		system->setMaxVelocity(readVector(file));
	}
	else
	if (token == "radius")
	{
		token = file->getNextToken();
		system->setRadius(atof(token.c_str()));
	}
	else
	if (token == "spawnQuantity")
	{
		token = file->getNextToken();
		system->setSpawnQuantity(atoi(token.c_str()));
	}
	else
	if (token == "spawnTime")
	{
		token = file->getNextToken();
		system->setSpawnTime(atoi(token.c_str()));
	}
	else
	if (token == "lifeTime")
	{
		token = file->getNextToken();
		system->setLifeTime(atoi(token.c_str()));
	}
	else
	if (token == "position")
	{
		system->setPosition(readVector(file));
	}
	else
	if (token == "quota")
	{
		token = file->getNextToken();
		system->setNumParticles(atoi(token.c_str()));
	}
	else
	if (token == "material")
	{
		token = file->getNextToken();
		system->setMaterial(token);
	}

	return token;
}

void particleManager::parsePlaneEmitter(parsingFile* file, particleSystem* system, const std::string& name)
{
	kAssert(file);
	kAssert(system);

	std::string token = file->getNextToken(); // {
	unsigned int openBraces = 1;

	planeEmitter* emitter = new planeEmitter();
	kAssert(emitter);

	while (openBraces)
	{
		if (file->eof())
			return;

		// Tokens Here =]
		token = parseEmitterGenericToken(file, token, emitter);
		if (token == "bounds")
		{
			vector3 min;
			vector3 max;

			min = readVector(file);
			max = readVector(file);

			emitter->setBounds(min, max);
		}

		token = file->getNextToken();

		// Script properties
		if (token == "}")
			openBraces--;
		else
		if (token == "{")
			openBraces++;
	}

	system->pushEmitter(name, emitter);
}

void particleManager::parsePointEmitter(parsingFile* file, particleSystem* system, const std::string& name)
{
	kAssert(file);
	kAssert(system);

	std::string token = file->getNextToken(); // {
	unsigned int openBraces = 1;

	planeEmitter* emitter = new planeEmitter();
	kAssert(emitter);

	while (openBraces)
	{
		if (file->eof())
			return;

		// Tokens Here =]
		parseEmitterGenericToken(file, token, emitter);
		token = file->getNextToken();

		// Script properties
		if (token == "}")
			openBraces--;
		else
		if (token == "{")
			openBraces++;
	}

	system->pushEmitter(name, emitter);
}
				
void particleManager::parseParticleSystem(parsingFile* file, const std::string& psName)
{
	kAssert(file);

	particleSystem* newSystem = allocatePS(psName);
	if (!newSystem)
	{
		S_LOG_INFO("Failed to allocate new particle system.");
		return;
	}

	std::string token = file->getNextToken(); // {
	unsigned int openBraces = 1;

	while (openBraces)
	{
		if (file->eof())
			return;

		if (token == "planeEmitter")
		{
			token = file->getNextToken();
			parsePlaneEmitter(file, newSystem, token);
		}
		else
		if (token == "pointEmitter")
		{
			token = file->getNextToken();
			parsePointEmitter(file, newSystem, token);
		}
		else
		if (token == "position")
		{
			newSystem->setPosition(readVector(file));
		}
		else
		if (token == "mass")
		{
			token = file->getNextToken();
			newSystem->setMass(atof(token.c_str()));
		}

		token = file->getNextToken();

		// Script properties
		if (token == "}")
			openBraces--;
		else
		if (token == "{")
			openBraces++;
	}

	S_LOG_INFO("Particle System " + psName + " created.");
}

void particleManager::parseParticleScript(const std::string& filename)
{
	parsingFile* newFile = new parsingFile(filename);

	kAssert(newFile);
	parseParticleScript(newFile);

	delete newFile;
}

void particleManager::parseParticleScript(parsingFile* file)
{
	if (!file->isReady())
	{
		S_LOG_INFO("File " + file->getFilename() + " is not ready to be parsed.");
		return;
	}

	std::string token = file->getNextToken();
	while (!file->eof())
	{
		if (token == "particleSystem")
		{
			// System name
			token = file->getNextToken();
			parseParticleSystem(file, token);
		}

		// if (token == "particleSystem")
		
		// Next Token
		token = file->getNextToken();
	}
	// while (!file->eof())
}
			
void particleManager::drawParticles(camera* active)
{
	std::map<std::string, particleSystem*>::const_iterator pIt;
	for (pIt = mSystems.begin(); pIt != mSystems.end(); pIt++)
	{
		pIt->second->cycle(active);
	}
}

}

