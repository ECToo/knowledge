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

#include "md3.h"
#include "logger.h"
#include "root.h"
#include "materialManager.h"
#include "resourceManager.h"
#include "camera.h"

namespace k {

md3Surface::md3Surface()
{
	mMaterial = NULL;

	mFrameCount = mVerticesCount = mIndicesCount = mUVCount = 0;
	mVertices = NULL;
	mIndices = NULL;
	mUVs = NULL;
}

md3Surface::~md3Surface()
{
	if (mVertices) 
		free(mVertices);

	if (mIndices) 
		free(mIndices);

	if (mUVs) 
		free(mUVs);
}

void md3Surface::draw(short frameNum)
{
	renderSystem* rs = root::getSingleton().getRenderSystem();

	if (mMaterial)
		mMaterial->start();

	rs->clearArrayDesc();
	rs->setVertexArray(mVertices[frameNum * mVerticesCount].pos.vec, sizeof(md3RealVertex));
	rs->setVertexCount(mVerticesCount);

	rs->setTexCoordArray(mUVs[0].uv, sizeof(md3TexCoord_t));
	rs->setNormalArray(mVertices[frameNum * mVerticesCount].normal.vec, sizeof(md3RealVertex));
		
	rs->setVertexIndex(mIndices[0].indices);
	rs->setIndexCount(mIndicesCount);

	rs->drawArrays();

	if (mMaterial)
		mMaterial->finish();
}

bool md3Surface::isOpaque() const
{
	if (mMaterial)
		return mMaterial->isOpaque();
	else
		return true;
}
		
void md3Surface::setMaterial(material* mat)
{
	kAssert(mat);
	mMaterial = mat;
}

void md3Surface::setMaterial(const std::string& matName)
{
	material* destMaterial = materialManager::getSingleton().getMaterial(matName);
	kAssert(destMaterial);

	mMaterial = destMaterial;
}
		
bool md3Surface::allocateIndices(unsigned int i)
{
	kAssert(i);

	// delete [] mIndices;
	if (mIndices)
		free(mIndices);

	mIndicesCount = i;

	try
	{
		// mIndices = new md3Triangle_t[i];
		mIndices = (md3Triangle_t*) memalign(32, sizeof(md3Triangle_t) * i);
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate surface indices.");
		return false;
	}

	return true;
}
		
bool md3Surface::allocateVertices(unsigned int i)
{
	kAssert(i);

	// delete [] mVertices;
	if (mVertices)
		free(mVertices);

	mVerticesCount = i;

	try
	{
		// mVertices = new md3RealVertex[i];
		mVertices = (md3RealVertex*) memalign(32, sizeof(md3RealVertex) * i);
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate surface vertices.");
		return false;
	}

	return true;
}

bool md3Surface::allocateUVs(unsigned int i)
{
	kAssert(i);
	
	// delete [] mUVs;
	if (mUVs)
		free(mUVs);

	mUVCount = i;

	try
	{
		// mUVs = new md3TexCoord_t[i];
		mUVs = (md3TexCoord_t*) memalign(32, sizeof(md3TexCoord_t) * i);
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate surface uvw's.");
		return false;
	}

	return true;
}
		
void md3Surface::setIndex(unsigned int index, const md3Triangle_t& tri)
{
	kAssert(index < mIndicesCount);
	mIndices[index] = tri;
}

void md3Surface::setVertex(unsigned int index, const md3Vertex_t& v)
{
	kAssert(index < mVerticesCount);
	mVertices[index] = v;

	float y = readLEShort(v.coord[2]) * 0.015625f;
	if (index == 0)
		mLowerY = y;
	
	if (y < mLowerY)
		mLowerY = y;
}

void md3Surface::setUV(unsigned int index, const md3TexCoord_t& uv)
{
	kAssert(index < mUVCount);
	mUVs[index] = uv;
}
		
void md3Surface::adjustVertices()
{
	for (unsigned int i = 0; i < mVerticesCount; i++)
		mVertices[i].pos.y -= mLowerY;
}

md3model::~md3model()
{
	delete [] mFrames;
	delete [] mTags;
	delete [] mSurfaces;

	while (!mAttach.empty())
		mAttach.erase(mAttach.begin());

	std::map<int, md3Animation_t*>::iterator it;
	while (!mAnimations.empty())
	{
		it = mAnimations.begin();
		delete it->second;
		mAnimations.erase(it);
	}

	mAnimations.clear();
}

md3model::md3model(const std::string& filename, bool adjustVertices)
{
	// Get file full path if resource manager is available.
	std::string fullPath = filename;
	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc) fullPath = rsc->getRoot() + filename;

	// Before reading, reset all the pointers
	mFrames = NULL;
	mTags = NULL;
	mSurfaces = NULL;
	mCurrentAnimFrame = 0;

	mAutoFeedAnims = true;
	mActiveAnimation = NULL;
	mAnimations.clear();
	
	// Tags
	mDrawTags = false;
	mAttachParent = NULL;
	mAttach.clear();

	// Ok, start parsing file.
	FILE* md3File = fopen(fullPath.c_str(), "rb");
	if (!md3File)
	{
		S_LOG_INFO("Failed to read md3 file " + fullPath);
		return;
	}

	// Get md3 header
	md3Header_t modelHeader;
	if (fread(&modelHeader, sizeof(md3Header_t), 1, md3File) <= 0)
	{
		S_LOG_INFO("Failed to read md3 header.");
		return;
	}

	if (strncmp(modelHeader.ident, "IDP3", 4))
	{
		S_LOG_INFO("IDENT (" + std::string(modelHeader.ident) + ") on md3 model doesnt match IDP3.");
		return;
	}

	// Version
	if (readLEInt(modelHeader.version) != 15)
	{
		S_LOG_INFO("Invalid md3 version, expected 15.");
		return;
	}

	// Allocate the frames
	mFramesCount = readLEInt(modelHeader.numFrames);

	try
	{
		mFrames = new md3Frame_t[mFramesCount];
	}

	catch (...)
	{
		S_LOG_INFO("Failed to read md3 frames.");
		return;
	}

	// Read Frames
	fseek(md3File, readLEInt(modelHeader.offsetFrames), SEEK_SET);
	for (unsigned int i = 0; i < mFramesCount; i++)
	{
		md3Frame_t tempFrame;
		if (fread(&tempFrame, sizeof(md3Frame_t), 1, md3File) <= 0)
		{
			S_LOG_INFO("Failed to read md3 frame.");
			delete [] mFrames;
			fclose(md3File);

			return;
		}

		float tmp = readLEFloat(tempFrame.mins[1]);
		mFrames[i].mins[0] = readLEFloat(tempFrame.mins[0]);
		mFrames[i].mins[1] = readLEFloat(tempFrame.mins[2]);
		mFrames[i].mins[2] = -tmp;

		tmp = readLEFloat(tempFrame.maxs[1]);
		mFrames[i].maxs[0] = readLEFloat(tempFrame.maxs[0]);
		mFrames[i].maxs[1] = readLEFloat(tempFrame.maxs[2]);
		mFrames[i].maxs[2] = -tmp;

		tmp = readLEFloat(tempFrame.origin[1]);
		mFrames[i].origin[0] = readLEFloat(tempFrame.origin[0]);
		mFrames[i].origin[1] = readLEFloat(tempFrame.origin[2]);
		mFrames[i].origin[2] = -tmp;

		mFrames[i].radius = readLEFloat(tempFrame.radius);

		strncpy(mFrames[i].name, tempFrame.name, 16);
	}

	// Allocate the tags
	mTagsCount = readLEInt(modelHeader.numTags);

	try
	{
		mTags = new md3Tag[mTagsCount * mFramesCount];
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate md3 tags.");
		delete [] mFrames;
		fclose(md3File);

		return;
	}

	// Read Tags
	fseek(md3File, readLEInt(modelHeader.offsetTags), SEEK_SET);
	for (unsigned int i = 0; i < mTagsCount * mFramesCount; i++)
	{
		md3Tag_t tempTag;
		if (fread(&tempTag, sizeof(md3Tag_t), 1, md3File) <= 0)
		{
			S_LOG_INFO("Failed to read tag from md3 file.");
			fclose(md3File);
			delete [] mFrames;
			delete [] mTags;

			return;
		}

		mTags[i] = tempTag;
	}

	// Surfaces
	mSurfacesCount = readLEInt(modelHeader.numSurfaces);

	try 
	{
		mSurfaces = new md3Surface[mSurfacesCount];
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate md3 surfaces.");
		fclose(md3File);
		delete [] mFrames;
		delete [] mTags;

		return;
	}

	// Read Surfaces
	fseek(md3File, readLEInt(modelHeader.offsetSurfaces), SEEK_SET);
	for (unsigned int i = 0; i < mSurfacesCount; i++)
	{
		long int surfaceStartOffset = ftell(md3File);

		md3Surface_t tempSurface;
		if (fread(&tempSurface, sizeof(md3Surface_t), 1, md3File) <= 0)
		{
			S_LOG_INFO("Failed to read md3 surface.");
			fclose(md3File);
			delete [] mFrames;
			delete [] mTags;
			delete [] mSurfaces;

			return;
		}
	
		// Read Shaders
		int numShaders = readLEInt(tempSurface.numShaders);
		md3Shader_t* mShaders;
		try
		{
			mShaders = new md3Shader_t[numShaders];
		}

		catch (...)
		{
			S_LOG_INFO("Failed to allocate surface shaders.");
			fclose(md3File);
			delete [] mFrames;
			delete [] mTags;
			delete [] mSurfaces;

			return;
		}
	
		fseek(md3File, surfaceStartOffset + readLEInt(tempSurface.offsetShaders), SEEK_SET);
		if (fread(mShaders, sizeof(md3Shader_t) * numShaders, 1, md3File) <= 0)
		{
			S_LOG_INFO("Failed to read surface shaders.");
			fclose(md3File);
			delete [] mFrames;
			delete [] mTags;
			delete [] mSurfaces;
			delete [] mShaders;

			return;
		}

		// For each shader, search a material containing the texture.
		for (int shader = 0; shader < numShaders; shader++)
		{
			// Try a material with this name
			material* mat = materialManager::getSingleton().getMaterial(mShaders[shader].name);

			if (!mat)
				mat = materialManager::getSingleton().getMaterialWithFilename(mShaders[shader].name);

			// Material not found, we are going to create
			// a simple material with this texture on it only.
			if (!mat)
			{
				texture* newTexture = textureManager::getSingleton().getTexture(std::string(mShaders[shader].name));
				if (newTexture)
				{
					mat = materialManager::getSingleton().createMaterial(mShaders[shader].name, newTexture);
					mSurfaces[i].setMaterial(mat);
				}
				else
				{
					S_LOG_INFO("Failed to find and create a material (" + std::string(mShaders[shader].name) + ") for md3mesh.");
				}
			}
			else
			{
				mSurfaces[i].setMaterial(mat);
			}
		}

		// we are done with shaders, free it
		delete [] mShaders;

		// Read Triangles
		if (!mSurfaces[i].allocateIndices(readLEInt(tempSurface.numTris)))
		{
			fclose(md3File);
			delete [] mFrames;
			delete [] mTags;
			delete [] mSurfaces;

			return;
		}

		fseek(md3File, surfaceStartOffset + readLEInt(tempSurface.offsetTriangles), SEEK_SET);
		for (unsigned int surf = 0; surf < mSurfaces[i].getIndicesCount(); surf++)
		{
			md3Triangle_t tempTriangle;
			if (fread(&tempTriangle, sizeof(md3Triangle_t), 1, md3File) <= 0)
			{
				S_LOG_INFO("Failed to read surface triangle.");
			
				fclose(md3File);
				delete [] mFrames;
				delete [] mTags;
				delete [] mSurfaces;

				return;
			}

			mSurfaces[i].setIndex(surf, tempTriangle);
		}

		// The right number of indices gets multiplied by 3 (we have the number of tris)
		mSurfaces[i].setIndexCount(readLEInt(tempSurface.numTris) * 3);

		// allocate uv's
		if (!mSurfaces[i].allocateUVs(readLEInt(tempSurface.numVerts)))
		{
			fclose(md3File);
			delete [] mFrames;
			delete [] mTags;
			delete [] mSurfaces;
			return;
		}

		// read uvs
		fseek(md3File, surfaceStartOffset + readLEInt(tempSurface.offsetUV), SEEK_SET);
		for (unsigned int uv = 0; uv < mSurfaces[i].getUVCount(); uv++)
		{
			md3TexCoord_t tempUV;
			if (fread(&tempUV, sizeof(md3TexCoord_t), 1, md3File) <= 0)
			{
				S_LOG_INFO("Failed to read surface uv.");
			
				fclose(md3File);
				delete [] mFrames;
				delete [] mTags;
				delete [] mSurfaces;

				return;
			}

			mSurfaces[i].setUV(uv, tempUV);
		}

		// allocate vertices
		unsigned int tempVertexCount = readLEInt(tempSurface.numVerts) * readLEInt(tempSurface.numFrames);
		if (!mSurfaces[i].allocateVertices(tempVertexCount))
		{
			fclose(md3File);
			delete [] mFrames;
			delete [] mTags;
			delete [] mSurfaces;
			return;
		}

		// allocate temp vertices
		md3Vertex_t* tempVertices;
		try
		{
			tempVertices = new md3Vertex_t[tempVertexCount];
		}

		catch (...)
		{
			S_LOG_INFO("Failed to allocate surface temporary vertices.");
			fclose(md3File);
			delete [] mFrames;
			delete [] mTags;
			delete [] mSurfaces;

			return;
		}

		// read temp vertices
		fseek(md3File, surfaceStartOffset + readLEInt(tempSurface.offsetVertex), SEEK_SET);
		if (fread(tempVertices, sizeof(md3Vertex_t), tempVertexCount, md3File) <= 0)
		{
			S_LOG_INFO("Failed to read surface temporary vertices.");

			fclose(md3File);
			delete [] mFrames;
			delete [] mTags;
			delete [] mSurfaces;
			delete [] tempVertices;

			return;
		}

		// Convert vertices
		for (unsigned int vertex = 0; vertex < mSurfaces[i].getVertexCount(); vertex++)
			mSurfaces[i].setVertex(vertex, tempVertices[vertex]);

		// Adjust Vertices, because we need the model center on lower part of
		// surface (lower Y)
		if (adjustVertices)
			mSurfaces[i].adjustVertices();

		// Set Vertex Count
		mSurfaces[i].setVertexCount(readLEInt(tempSurface.numVerts));

		// We dont need the temporary vertices anymore, free it
		delete [] tempVertices;

		// Copy name
		mSurfaces[i].setName(tempSurface.name);

		// FrameCount
		mSurfaces[i].setFrameCount(readLEInt(tempSurface.numFrames));
	}

	S_LOG_INFO("MD3 Model " + filename + " loaded.");
}
		
md3Surface* md3model::getSurface(unsigned int index)
{
	return &mSurfaces[index];
}
		
const unsigned int md3model::getSurfaceCount() const
{
	return mSurfacesCount;
}

void md3model::setFrame(unsigned short i)
{
	if (i < mFramesCount)
		mCurrentAnimFrame = i;
	else
	{
		std::stringstream msg;
		msg << "Frame desired(" << i << ") is greater than maximum number of frames(" 
		<< mFramesCount << ")";

		S_LOG_INFO(msg.str());
	}
}

int md3model::getFramesCount() const
{
	return mFramesCount;
}

bool md3model::isOpaque() const
{
	for (unsigned int i = 0; i < mSurfacesCount; i++)
	{
		if (!mSurfaces[i].isOpaque())
			return false;
	}

	return true;
}

void md3model::feedAnims()
{
	if (!mAutoFeedAnims || !mAnimations.size() || !mActiveAnimation)
		return;

	// Global Time
	long timeNow = root::getSingleton().getGlobalTime();

	// Time elapsed
	mCurrentAnimFrame += (mActiveAnimation->framesPerSecond * (timeNow - mLastFeedTime)) / 1000.0f;
	mLastFeedTime = timeNow;

	while ((uint32_t)mCurrentAnimFrame >= (mActiveAnimation->firstFrame + mActiveAnimation->numFrames))
		mCurrentAnimFrame -= mActiveAnimation->numFrames;
}
		
void md3model::draw()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();

	// Feed animations =]
	feedAnims();

	// Rotate and Translate
	vector3 finalPos = getAbsolutePosition();

	vec_t angle;
	vector3 axis;
	getAbsoluteOrientation().toAxisAngle(angle, axis);
			
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

	rs->translateScene(finalPos.x, finalPos.y, finalPos.z);
	rs->rotateScene(angle, axis.x, axis.y, axis.z);
	rs->scaleScene(mScale.x, mScale.y, mScale.z);

	for (unsigned int i = 0; i < mSurfacesCount; i++)
		mSurfaces[i].draw((uint32_t)mCurrentAnimFrame);

	if (getDrawBoundingBox())
		getAABoundingBox().draw();
		
	// Attached
	for (std::vector<md3model*>::iterator it = mAttach.begin(); it != mAttach.end(); it++)
	{
		// Each attach changes the transformation matrix
		if (it != mAttach.begin())
		{
			if (!haveCamera)
			{
				rs->setMatrixMode(MATRIXMODE_MODELVIEW);
				rs->identityMatrix();
			}
			else
			{
				haveCamera->copyView();
			}

			rs->translateScene(finalPos.x, finalPos.y, finalPos.z);
			rs->rotateScene(angle, axis.x, axis.y, axis.z);
			rs->scaleScene(mScale.x, mScale.y, mScale.z);
		}

		(*it)->attachDraw();
	}
}
		
void md3model::attachDraw()
{
	if (!mAttachParent)
		return;
	
	renderSystem* rs = root::getSingleton().getRenderSystem();

	// Feed animations =]
	feedAnims();

	// Get Our Tags
	md3Tag* mAttachedTo = mAttachParent->getTag(mAttachTag);
	md3Tag* mAttachConnection = getTag(mAttachTag);

	if (!mAttachedTo || !mAttachConnection)
		return;

	matrix3 rotationMatrix = mAttachedTo->mRotation * mAttachConnection->mRotation;

	vec_t angle = 0;
	vector3 axis;
	rotationMatrix.toAxisAngle(angle, axis);

	vector3 mTranslation = mAttachedTo->mOrigin - mAttachConnection->mOrigin;
	mPosition = mTranslation;
		
	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->translateScene(mTranslation.x, mTranslation.y, mTranslation.z);
	rs->rotateScene(angle, axis.x, axis.y, axis.z);

	for (unsigned int i = 0; i < mSurfacesCount; i++)
		getSurface(i)->draw((uint32_t)mCurrentAnimFrame);

	if (getDrawBoundingBox())
		getAABoundingBox().draw();

	for (std::vector<md3model*>::iterator it = mAttach.begin(); it != mAttach.end(); it++)
		(*it)->attachDraw();
}

boundingBox md3model::getAABoundingBox() const
{
	vector3 mins = vector3(mFrames[(uint32_t)mCurrentAnimFrame].mins[0],
			mFrames[(uint32_t)mCurrentAnimFrame].mins[1],
			mFrames[(uint32_t)mCurrentAnimFrame].mins[2]);
	vector3 maxs = vector3(mFrames[(uint32_t)mCurrentAnimFrame].maxs[0],
			mFrames[(uint32_t)mCurrentAnimFrame].maxs[1],
			mFrames[(uint32_t)mCurrentAnimFrame].maxs[2]);

	return boundingBox(mins, maxs);
}

boundingBox md3model::getBoundingBox() const
{
	vector3 mins = vector3(mFrames[(uint32_t)mCurrentAnimFrame].mins[0],
			mFrames[(uint32_t)mCurrentAnimFrame].mins[1],
			mFrames[(uint32_t)mCurrentAnimFrame].mins[2]);
	vector3 maxs = vector3(mFrames[(uint32_t)mCurrentAnimFrame].maxs[0],
			mFrames[(uint32_t)mCurrentAnimFrame].maxs[1],
			mFrames[(uint32_t)mCurrentAnimFrame].maxs[2]);

	return boundingBox(mOrientation.rotateVector(mins), 
			mOrientation.rotateVector(maxs));
}
		
void md3model::attach(md3model* model, const std::string& tag)
{
	if (!model->getTag(tag))
	{
		S_LOG_INFO("Tag " + tag + " not found on model to attach.");
		return;
	}

	for (unsigned int i = 0; i < mTagsCount; i++)
	{
		if (mTags[i].mName == tag)
		{
			mAttach.push_back(model);
			model->setAttachTag(tag, this);

			return;
		}
	}

	S_LOG_INFO("Tag " + tag + " not found, model not attached.");
}
		
md3Tag* md3model::getTag(const std::string& name)
{
	unsigned int tagIndex;
	for (tagIndex = 0; tagIndex < mTagsCount * mFramesCount; tagIndex++)
	{
		if (mTags[tagIndex].mName == name)
			break;
	}
	
	if (tagIndex == (mTagsCount * mFramesCount))
		return NULL;

	return &mTags[tagIndex + ((uint32_t)mCurrentAnimFrame * mTagsCount)];
}
		
md3Animation_t* md3model::createAnimation(const std::string& name)
{
	// Check if animation exists
	unsigned int nameHash = getHashKey(name);
	std::map<int, md3Animation_t*>::iterator it = mAnimations.find(nameHash);
	if (it != mAnimations.end())
	{
		return it->second;
	}

	// Create animation
	md3Animation_t* newAnim = NULL;
	try
	{
		newAnim = new md3Animation_t;
		newAnim->name = name;

		mAnimations[nameHash] = newAnim;
	
		S_LOG_INFO("Attached animation " + name + " to model.");

		return newAnim;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate md3Animation.");
	}
		
	return NULL;
}
		
bool md3model::insertAnimation(md3Animation_t* anim)
{
	kAssert(anim);

	unsigned int nameHash = getHashKey(anim->name);
	std::map<int, md3Animation_t*>::iterator it = mAnimations.find(nameHash);
	if (it != mAnimations.end())
	{
		S_LOG_INFO("Failed to insert animation " + anim->name + ", this name already exist in animation list.");
		return false;
	}

	mAnimations[nameHash] = anim;
	S_LOG_INFO("Attached animation " + anim->name + " to model.");

	return true;
}
		
void md3model::setAnimation(const std::string& name)
{
	unsigned int nameHash = getHashKey(name);
	std::map<int, md3Animation_t*>::iterator it = mAnimations.find(nameHash);
	if (it != mAnimations.end())
	{
		mActiveAnimation = it->second;
		mCurrentAnimFrame = mActiveAnimation->firstFrame;
		mLastFeedTime = root::getSingleton().getGlobalTime();
		return;
	}

	S_LOG_INFO("Animation " + name + " not found in model.");
}

}

