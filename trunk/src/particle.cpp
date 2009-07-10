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

#include "particle.h"
#include "materialManager.h"
#include "rendersystem.h"
#include "root.h"

namespace k {
namespace particle {

static inline int getRandNonZero()
{
	int random = rand();
	while (!random)
		random = rand();

	return random;
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
			
particle::particle()
{
	mVisible = false;

	mLastDrawTime = 0;
	mSpawnTime = 0;
	mLifeTime = 0;
	mRadius = 0;

	mParent = NULL;
}

const vector3& particle::getVelocity() const
{
	return mVelocity;
}

const vector3& particle::getAcceleration() const
{
	return mAcceleration;
}

bool particle::isVisible() const
{
	return mVisible;
}

unsigned int particle::getSpawnTime() const
{
	return mSpawnTime;
}

unsigned int particle::getLifeTime() const
{
	return mLifeTime;
}
			
vec_t particle::getRadius() const
{
	return mRadius;
}

void particle::setVelocity(const vector3& vel)
{
	mVelocity = vel;
}
			
void particle::setAcceleration(const vector3& accel)
{
	mAcceleration = accel;
}

void particle::setLifeTime(unsigned int time)
{
	mLifeTime = time;
}

void particle::setRadius(vec_t radi)
{
	mRadius = radi;
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

void particle::updatePhysics(long time)
{
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
}

void particle::draw(sprite* spr)
{
	spr->setPosition(getPosition());
	spr->setRadius(mRadius);
	spr->rawDraw();
}
			
container::container()
{
	mParticles = NULL;
	mSprite = NULL;
	mVertexPositions = NULL;

	mMaxNumberOfParticles = 0;
	mMaterial = 0;

	mTimer.reset();
}

container::~container()
{
	if (mParticles)
		delete mParticles;

	if (mVertexPositions)
		delete [] mVertexPositions;

	if (mSprite)
		delete mSprite;
}
			
void container::setNumParticles(unsigned int numParticles)
{
	if (!numParticles)
	{
		S_LOG_INFO("The number of particles for the system cant be 0.");
		return;
	}

	try
	{
		mParticles = new std::vector<particle>(numParticles);

		renderSystem* rs = root::getSingleton().getRenderSystem();
		if (rs->getPointSpriteSupport())
		{
			mVertexPositions = new vec_t[numParticles * 3];
		}
		else
		{
			mSprite = new sprite();
		}	
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate particle container.");
		return;
	}

	mMaxNumberOfParticles = numParticles;
	mFreeParticles = numParticles;
}

void container::setMaterial(const std::string& name)
{
	mMaterial = materialManager::getSingleton().getMaterial(name);
}

void container::setMaterial(material* mat)
{
	kAssert(mat);
	mMaterial = mat;
}

particle* container::findFreeParticle()
{
	std::vector<particle>::iterator it;
	for (it = mParticles->begin(); it != mParticles->end(); it++)
	{
		if (!(*it).isVisible())
			return &(*it);
	}

	return NULL;
}
			
particle* container::spawnParticle()
{
	particle* newP = findFreeParticle();
	if (newP)
	{
		mFreeParticles--;

		newP->setVisibility(true);
		newP->resetLife(mTimer.getMilliSeconds());
	}

	return newP;
}

void container::update()
{
	long timeElapsed = mTimer.getMilliSeconds();

	if (!mParticles)
	{
		S_LOG_INFO("No particles in the system, maybe you forgot to set quota?");
		return;
	}

	std::vector<particle>::iterator it;
	for (it = mParticles->begin(); it != mParticles->end(); it++)
	{
		particle* p = &(*it);
		if (!p->isVisible())
			continue;

		if ((timeElapsed - p->getSpawnTime()) >= p->getLifeTime())
		{
			// Particle is dead - timed out.
			p->setVisibility(false);
			mFreeParticles++;
		}
	}
}

void container::draw()
{
	if (!mParticles)
	{
		S_LOG_INFO("No particles in the system, maybe you forgot to set quota?");
		return;
	}

	renderSystem* rs = root::getSingleton().getRenderSystem();
	if (rs->getPointSpriteSupport())
	{
		kAssert(mVertexPositions);
	}
	else
	{
		kAssert(mSprite);
	}

	kAssert(mParticles);
	kAssert(mMaterial);

	mMaterial->start();
	rs->setDepthMask(false); // FIXME: We need a "proper" way of doing this

	if (rs->getPointSpriteSupport())
	{
		std::vector<particle>::iterator it = mParticles->begin();

		vec_t particleOldRadius = it->getRadius(); 
		vec_t particleRadius = it->getRadius(); 

		while (it != mParticles->end())
		{
			unsigned int particleIndex = 0;
			for (; it != mParticles->end(); it++)
			{
				particle* p = &(*it);
				if (!p->isVisible())
					continue;

				if (p->getRadius() != particleRadius)
				{
					particleOldRadius = particleRadius;
					particleRadius = p->getRadius();
					break;
				}

				p->updatePhysics(mTimer.getMilliSeconds());
				const vector3& pos = p->getPosition();

				mVertexPositions[particleIndex * 3 + 0] = pos.x;
				mVertexPositions[particleIndex * 3 + 1] = pos.y;
				mVertexPositions[particleIndex * 3 + 2] = pos.z;

				particleIndex++;		
			}
	
			camera* haveCamera = root::getSingleton().getRenderer()->getCamera();
			if (!haveCamera)
			{
				rs->setMatrixMode(MATRIXMODE_MODELVIEW);
				rs->identityMatrix();
			}
			else
			{
				haveCamera->copyView();
			}
					
			rs->setPointSprite(true);
			rs->setPointSpriteSize(particleOldRadius);
		
			rs->drawPointSprites(mVertexPositions, particleIndex);

			rs->setPointSprite(false);
		}
	}
	else
	{
		std::vector<particle>::iterator it;
		for (it = mParticles->begin(); it != mParticles->end(); it++)
		{
			particle* p = &(*it);
			if (!p->isVisible())
				continue;

			camera* haveCamera = root::getSingleton().getRenderer()->getCamera();
			if (!haveCamera)
			{
				rs->setMatrixMode(MATRIXMODE_MODELVIEW);
				rs->identityMatrix();
			}
			else
			{
				haveCamera->copyView();
			}

			p->updatePhysics(mTimer.getMilliSeconds());
			p->draw(mSprite);
		}
	}

	mMaterial->finish();
	rs->setDepthMask(true); // FIXME
}
			
emitter::emitter(container* cont)
{
	kAssert(cont);
	mContainer = cont;
}

emitter::~emitter()
{}

void emitter::setRadius(vec_t radi)
{
	mRadius = radi;
}

void emitter::setSpawnQuantity(unsigned int amount)
{
	mSpawnQuantity = amount;
}

void emitter::setSpawnTime(long time)
{
	mSpawnTime = time;
}

void emitter::setLifeTime(long time)
{
	mLifetime = time;
}
			
void emitter::setVelocity(const vector3& min, const vector3& max)
{
	mMinVelocity = min;
	mMaxVelocity = max;
}

void emitter::setMinVelocity(const vector3& min)
{
	mMinVelocity = min;
}

void emitter::setMaxVelocity(const vector3& max)
{
	mMaxVelocity = max;
}

void emitter::baseSpawn(particle* newP)
{
	newP->setParent(this);
	newP->setRadius(mRadius);
	newP->setLifeTime(mLifetime);
}

pointEmitter::pointEmitter(container* cont)
	: emitter(cont)
{
}
			
pointEmitter::~pointEmitter()
{
}

void pointEmitter::spawnParticles()
{
	// Spawn new particles
	if (mSpawnTimer.getMilliSeconds() > mSpawnTime)
	{
		for (unsigned short i = 0; i < mSpawnQuantity; i++)
		{
			particle* newP = mContainer->spawnParticle();
			if (!newP)
				break;


			baseSpawn(newP);
			newP->setPosition(vector3::zero);
	
			/**
			 * Randomize particles velocity between minimum and max speeds
			 */
			newP->setVelocity(getRandomVelocity(mMaxVelocity, mMinVelocity));
			newP->setAcceleration(mAcceleration);
		}

		// Reset Timer
		mSpawnTimer.reset();
	}
}

void pointEmitter::setAcceleration(const vector3& accel)
{
	mAcceleration = accel;
}
			
planeEmitter::planeEmitter(container* cont)
	: emitter(cont)
{
}

planeEmitter::~planeEmitter()
{
}

void planeEmitter::spawnParticles()
{
	// Spawn new particles
	if (mSpawnTimer.getMilliSeconds() > mSpawnTime)
	{
		// Spawn
		for (unsigned short i = 0; i < mSpawnQuantity; i++)
		{
			particle* newP = mContainer->spawnParticle();
			if (!newP)
				break;

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


			baseSpawn(newP);
			newP->setPosition(randPos);
			newP->setVelocity(getRandomVelocity(mMinVelocity, mMaxVelocity));
			newP->setAcceleration(mAcceleration);
		}

		// Reset Timer
		mSpawnTimer.reset();
	}
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
					
void planeEmitter::setAcceleration(const vector3& accel)
{
	mAcceleration = accel;
}
			
void affector::setAffectedParameter(unsigned int param)
{
	mAffectedParameter = param;
}
			
void affector::setAffectedOperation(unsigned int op)
{
	mAffectedOperation = op;
}

void affector::setFactor(const vector3& factor)
{
	mFactor = factor;
}

void affector::setFactor(vec_t factor)
{
	mFactor.x = factor;
}

void linearAffector::interact(particle* p)
{
	switch (mAffectedParameter)
	{
		case AFF_POS:
			switch (mAffectedOperation)
			{
				case AFF_SUM:
					p->setPosition(p->getRelativePosition() + mFactor);
					break;
				case AFF_SUB:
					p->setPosition(p->getRelativePosition() - mFactor);
					break;
				case AFF_MUL:
					p->setPosition(p->getRelativePosition() * mFactor.x);
					break;
				case AFF_DIV:
					p->setPosition(p->getRelativePosition() / mFactor.x);
					break;
			}
			break;
		case AFF_ACCELERATION:
			switch (mAffectedOperation)
			{
				case AFF_SUM:
					p->setAcceleration(p->getAcceleration() + mFactor);
					break;
				case AFF_SUB:
					p->setAcceleration(p->getAcceleration() - mFactor);
					break;
				case AFF_MUL:
					p->setAcceleration(p->getAcceleration() * mFactor.x);
					break;
				case AFF_DIV:
					p->setAcceleration(p->getAcceleration() / mFactor.x);
					break;
			}
			break;
		case AFF_VELOCITY:
			switch (mAffectedOperation)
			{
				case AFF_SUM:
					p->setVelocity(p->getVelocity() + mFactor);
					break;
				case AFF_SUB:
					p->setVelocity(p->getVelocity() - mFactor);
					break;
				case AFF_MUL:
					p->setVelocity(p->getVelocity() * mFactor.x);
					break;
				case AFF_DIV:
					p->setVelocity(p->getVelocity() / mFactor.x);
					break;
			}
			break;
		case AFF_RADIUS:
			switch (mAffectedOperation)
			{
				case AFF_SUM:
					p->setRadius(p->getRadius() + mFactor.x);
					break;
				case AFF_SUB:
					p->setRadius(p->getRadius() - mFactor.x);
					break;
				case AFF_MUL:
					p->setRadius(p->getRadius() * mFactor.x);
					break;
				case AFF_DIV:
					p->setRadius(p->getRadius() / mFactor.x);
					break;
			}
			break;
		case AFF_LIFETIME:
			switch (mAffectedOperation)
			{
				case AFF_SUM:
					p->setLifeTime(p->getLifeTime() + mFactor.x);
					break;
				case AFF_SUB:
					p->setLifeTime(p->getLifeTime() - mFactor.x);
					break;
				case AFF_MUL:
					p->setLifeTime(p->getLifeTime() * mFactor.x);
					break;
				case AFF_DIV:
					p->setLifeTime(p->getLifeTime() / mFactor.x);
					break;
			}
			break;
	}
}

system::system()
{
	mParent = NULL;
}

system::~system()
{
}

void system::setMaterial(const std::string& mat)
{
	mMaterial = materialManager::getSingleton().getMaterial(mat);
}

void system::setMaterial(material* mat)
{
	kAssert(mat);
	mMaterial = mat;
}

void system::setMass(vec_t mass)
{
	mParticleMass = mass;
}

void system::pushEmitter(const std::string& name, emitter* em)
{
	kAssert(em);
	mEmitters[name] = em;
	em->setParent(&(*this));
}
			
void system::pushAffector(const std::string& name, affector* aff)
{
	kAssert(aff);
	mAffectors[name] = aff;
	aff->setParent(&(*this));
}
			
void system::cycle()
{
	if (!mParticles)
		return;

	// Container update
	update();
	
	// Spawn Particles
	std::map<std::string, emitter*>::const_iterator emIt;
	for (emIt = mEmitters.begin(); emIt != mEmitters.end(); emIt++)
	{
		emIt->second->spawnParticles();
	}

	std::vector<particle>::iterator pIt;
	for (pIt = mParticles->begin(); pIt != mParticles->end(); pIt++)
	{
		if (!pIt->isVisible())
			continue;

		std::map<std::string, affector*>::const_iterator affIt;
		for (affIt = mAffectors.begin(); affIt != mAffectors.end(); affIt++)
		{ 
			affIt->second->interact(&(*pIt));
		}
	}

	// Draw container
	draw();
}

template<> manager* singleton<manager>::singleton_instance = 0;

manager& manager::getSingleton()
{  
	kAssert(singleton_instance);
	return (*singleton_instance);  
}

manager::manager()
{
	mSystems.clear();
}

manager::~manager()
{
}

system* manager::getSystem(const std::string& name)
{
	std::map<std::string, system*>::const_iterator it = mSystems.find(name);
	if (it != mSystems.end())
		return it->second;
	else
		return NULL;
}

system* manager::allocateSystem(const std::string& name)
{
	system* newSystem = getSystem(name);
	if (!newSystem)
	{
		try 
		{
			newSystem = new system();
			mSystems[name] = newSystem;
		}
		catch (...)
		{
			S_LOG_INFO("Failed to allocate a new particle system.");
			return NULL;
		}
	}

	return newSystem;
}

static inline std::string parseEmitterGenericToken(parsingFile* file, const std::string& parsedToken, emitter* system)
{
	kAssert(file);
	kAssert(system);

	// Current Token
	std::string token = parsedToken;

	if (token == "minVelocity")
	{
		system->setMinVelocity(file->getVector3());
	}
	else
	if (token == "maxVelocity")
	{
		system->setMaxVelocity(file->getVector3());
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
		system->setPosition(file->getVector3());
	}

	return token;
}

void manager::parsePlaneEmitter(parsingFile* file, system* system, const std::string& name)
{
	kAssert(file);
	kAssert(system);

	std::string token = file->getNextToken(); // {
	unsigned int openBraces = 1;

	planeEmitter* emit;

	try
	{
		emit = new planeEmitter(system);
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate plane particle emitter.");
		return;
	}

	while (openBraces)
	{
		if (file->eof())
			return;

		// Tokens Here =]
		token = parseEmitterGenericToken(file, token, emit);
		if (token == "bounds")
		{
			vector3 min;
			vector3 max;

			min = file->getVector3();
			max = file->getVector3();

			emit->setBounds(min, max);
		}

		token = file->getNextToken();

		// Script properties
		if (token == "}")
			openBraces--;
		else
		if (token == "{")
			openBraces++;
	}

	system->pushEmitter(name, emit);
}

void manager::parsePointEmitter(parsingFile* file, system* system, const std::string& name)
{
	kAssert(file);
	kAssert(system);

	std::string token = file->getNextToken(); // {
	unsigned int openBraces = 1;

	pointEmitter* emit;

	try
	{
		emit = new pointEmitter(system);
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate plane particle emitter.");
		return;
	}

	while (openBraces)
	{
		if (file->eof())
			return;

		// Tokens Here =]
		parseEmitterGenericToken(file, token, emit);
		token = file->getNextToken();

		// Script properties
		if (token == "}")
			openBraces--;
		else
		if (token == "{")
			openBraces++;
	}

	system->pushEmitter(name, emit);
}

static inline std::string parseAffectorGenericToken(parsingFile* file, const std::string& parsedToken, affector* system)
{
	kAssert(file);
	kAssert(system);

	// Current Token
	std::string token = parsedToken;

	if ((token == "param") || (token == "parameter"))
	{
		token = file->getNextToken();

		if (token == "position")
			system->setAffectedParameter(AFF_POS);
		else
		if (token == "velocity")
			system->setAffectedParameter(AFF_VELOCITY);
		else
		if (token == "acceleration")
			system->setAffectedParameter(AFF_ACCELERATION);
		else
		if (token == "radius")
			system->setAffectedParameter(AFF_RADIUS);
		else
		if (token == "lifeTime")
			system->setAffectedParameter(AFF_LIFETIME);
	}
	else
	if ((token == "op") || (token == "operation"))
	{
		token = file->getNextToken();

		if ((token == "sum") || (token == "add"))
			system->setAffectedOperation(AFF_SUM);
		else
		if ((token == "sub") || (token == "diff"))
			system->setAffectedOperation(AFF_SUB);
		else
		if (token == "mul")
			system->setAffectedOperation(AFF_MUL);
		else
		if (token == "div")
			system->setAffectedOperation(AFF_DIV);
	}
	else
	if (token == "factor")
	{
		token = file->getNextToken();
		vec_t value = atof(token.c_str());

		system->setFactor(value);
	}
	else
	if (token == "factorVector")
	{
		vector3 value = file->getVector3();
		system->setFactor(value);
	}

	return token;
}

void manager::parseLinearAffector(parsingFile* file, system* system, const std::string& name)
{
	kAssert(file);
	kAssert(system);

	std::string token = file->getNextToken(); // {
	unsigned int openBraces = 1;

	linearAffector* aff;

	try
	{
		aff = new linearAffector();
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate linear particle affector.");
		return;
	}

	while (openBraces)
	{
		if (file->eof())
			return;

		// Tokens Here =]
		parseAffectorGenericToken(file, token, aff);
		token = file->getNextToken();

		// Script properties
		if (token == "}")
			openBraces--;
		else
		if (token == "{")
			openBraces++;
	}

	system->pushAffector(name, aff);
}
				
void manager::parseSystem(parsingFile* file, const std::string& psName)
{
	kAssert(file);
	system* newSystem;

	try 
	{
		newSystem = allocateSystem(psName);
	}

	catch (...)
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
		if (token == "linearAffector")
		{
			token = file->getNextToken();
			parseLinearAffector(file, newSystem, token);
		}
		else
		if (token == "position")
		{
			newSystem->setPosition(file->getVector3());
		}
		else
		if (token == "mass")
		{
			token = file->getNextToken();
			newSystem->setMass(atof(token.c_str()));
		}
		else
		if (token == "quota")
		{
			token = file->getNextToken();
			newSystem->setNumParticles(atoi(token.c_str()));
		}
		else
		if (token == "material")
		{
			token = file->getNextToken();
			newSystem->setMaterial(token);
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

void manager::parseScript(const std::string& filename)
{
	parsingFile* newFile = new parsingFile(filename);
	if (newFile)
	{
		parseScript(newFile);
		delete newFile;
	}
}

void manager::parseScript(parsingFile* file)
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
			parseSystem(file, token);
		}

		// if (token == "particleSystem")
		
		// Next Token
		token = file->getNextToken();
	}
	// while (!file->eof())
}
			
void manager::drawParticles()
{
	std::map<std::string, system*>::const_iterator pIt;
	for (pIt = mSystems.begin(); pIt != mSystems.end(); pIt++)
	{
		pIt->second->cycle();
	}
}

} // particle
} // k

