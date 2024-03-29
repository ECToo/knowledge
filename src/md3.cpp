/*
Copyright (c) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
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
	mDrawingNormals = NULL;
	mIndices = NULL;
	mUVs = NULL;

	mDrawNormals = false;
}

md3Surface::~md3Surface()
{
	if (mVertices) 
		free(mVertices);

	if (mIndices) 
		free(mIndices);

	if (mUVs) 
		free(mUVs);

	if (mDrawingNormals)
		free(mDrawingNormals);
}
		
void md3Surface::setDrawNormals(bool draw)
{
	if (!draw && mDrawNormals)
	{
		// Remove our array
		if (mDrawingNormals)
			free(mDrawingNormals);
	}
	else
	if (draw && !mDrawNormals)
	{
		// Construct the array
		mDrawingNormals = (vector3*) memalign(32, mVerticesCount * 2 * sizeof(vector3));
		if (!mDrawingNormals)
		{
			S_LOG_INFO("Failed to allocate normal array for drawing normals.");
		}
	}

	mDrawNormals = draw;
}

void md3Surface::draw(short frameNum)
{
	renderSystem* rs = root::getSingleton().getRenderSystem();

	if (mMaterial)
		mMaterial->start();

	rs->clearArrayDesc();
	rs->setVertexArray(mVertices[frameNum * mVerticesCount].pos.vec, sizeof(md3RealVertex));
	rs->setVertexCount(mVerticesCount);

	rs->setTexCoordArray(mUVs[0].uv.vec);
	rs->setNormalArray(mVertices[frameNum * mVerticesCount].normal.vec, sizeof(md3RealVertex));
		
	rs->setVertexIndex(mIndices[0].indices);
	rs->setIndexCount(mIndicesCount);

	rs->drawArrays();

	if (mMaterial)
		mMaterial->finish();
	
	if (mDrawNormals)
	{
		// Copy Normals
		for (unsigned int i = 0; i < mVerticesCount; i++)
		{
			mDrawingNormals[i * 2] = mVertices[i + (frameNum * mVerticesCount)].pos;
			mDrawingNormals[(i * 2) + 1] = (mVertices[i + (frameNum * mVerticesCount)].pos + (mVertices[i + (frameNum * mVerticesCount)].normal * 2));
		}

		material* normalMaterial =  materialManager::getSingleton().getMaterial("k_base_white");
		kAssert(normalMaterial);

		normalMaterial->start();

		rs->clearArrayDesc(VERTEXMODE_LINE);
		rs->setVertexArray(mDrawingNormals[0].vec);
		rs->setVertexCount(mVerticesCount * 2);

		rs->drawArrays(true);

		normalMaterial->finish();
	}
}
		
bool md3Surface::trace(ray& traceRay, short frameNum) const
{
	// TODO: Test against surface bounding box.

	for (unsigned int i = 0; i < mIndicesCount; i += 3)
	{
		if (traceRay.intersect(mVertices[i + (frameNum * mVerticesCount)].pos, 
					mVertices[i + 1 + (frameNum * mVerticesCount)].pos, 
					mVertices[i + 2 + (frameNum * mVerticesCount)].pos))
		{
			return true;
		}
	}

	return false;
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
	mIndices = (md3Triangle*) memalign(32, sizeof(md3Triangle) * i);
	if (!mIndices)
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
	mVertices = (md3RealVertex*) memalign(32, sizeof(md3RealVertex) * i);
	if (!mVertices)
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
	mUVs = (md3TexCoord_t*) memalign(32, sizeof(md3TexCoord_t) * i);
	if (!mUVs)
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

	const float y = mVertices[index].pos.y;
	if (index == 0 || y < mLowerY)
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

	std::map<int, md3Animation_t*>::iterator it;
	while (!mAnimations.empty())
	{
		it = mAnimations.begin();

		md3Animation_t* anim = it->second;
		delete anim;

		mAnimations.erase(it);
	}

	mAttach.clear();
	mAnimations.clear();
}

md3model::md3model(const md3model* shared) : drawable3D()
{
	kAssert(shared);
	mShared = shared;

	mDrawableAttach = NULL;
	mFrames = NULL;
	mFramesCount = 0;

	mSurfaces = NULL;
	mSurfacesCount = 0;

	mCurrentAnimFrame = 0;

	mAutoFeedAnims = true;
	mActiveAnimation = NULL;
	mAnimations.clear();
	mLastFeedTime = 0;
	
	// Tags
	mDrawTags = false;
	mAttachParent = NULL;
	mAttach.clear();

	// Copy Tags from shared
	mTagsCount = mShared->getTagsCount();
	mTags = NULL;

	if (!mTagsCount)
		return;

	try
	{
		mTags = new md3Tag[mTagsCount * mShared->getFramesCount()];

		const md3Tag* sharedTags = mShared->getTags();
		for (unsigned int i = 0; i < (mTagsCount * mShared->getFramesCount()); i++)
		{
			mTags[i].mName = sharedTags[i].mName;
			mTags[i].mOrigin = sharedTags[i].mOrigin;
	
			mTags[i].mRotation = sharedTags[i].mRotation;
			mTags[i].mRotation.toAxisAngle(mTags[i].mAA_Angle, mTags[i].mAA_Axis);
		}
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate md3 tags.");
		return;
	}
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
	mShared = NULL;

	mAutoFeedAnims = true;
	mActiveAnimation = NULL;
	mAnimations.clear();
	
	// Tags
	mDrawTags = false;
	mAttachParent = NULL;
	mAttach.clear();

	// Ok, start parsing file.
	unsigned long md3Index = 0;
	char* md3String = NULL;
	unsigned int md3StringLength = 0;

	std::ifstream input;
	input.open(fullPath.c_str(), std::ifstream::in | std::ifstream::binary);

	if (input.is_open())
	{

		input.seekg(0, std::ios::end);
		md3StringLength = input.tellg();
		input.seekg(0, std::ios::beg);

		try
		{
			md3String = new char[md3StringLength + 1];
			memset(md3String, 0, md3StringLength);
			input.read(md3String, md3StringLength);
			input.close();
		}

		catch (...)
		{
			S_LOG_INFO("Failed to allocate md3 file data.");
			input.close();
			return;
		}
	}
	else
	{
		S_LOG_INFO("Failed to read md3 file.");
		return;
	}

	// Get md3 header
	md3Header_t modelHeader;
	memcpy(&modelHeader, md3String, sizeof(md3Header_t));
	md3Index = sizeof(md3Header_t);

	if (strncmp(modelHeader.ident, "IDP3", 4))
	{
		S_LOG_INFO("IDENT (" + std::string(modelHeader.ident) + ") on md3 model doesnt match IDP3.");
		return;
	}

	// Version
	if (readLEInt(modelHeader.version) != 15)
	{
		std::stringstream msg;
		msg << "Invalid md3 version (" << readLEInt(modelHeader.version) << "), expected 15.";
		S_LOG_INFO(msg.str());

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
	md3Index = readLEInt(modelHeader.offsetFrames);
	for (unsigned int i = 0; i < mFramesCount; i++)
	{
		md3Frame_t tempFrame;
		memcpy(&tempFrame, md3String + md3Index, sizeof(md3Frame_t));
		md3Index += sizeof(md3Frame_t);

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
		// fclose(md3File);

		return;
	}

	// Read Tags
	md3Index = readLEInt(modelHeader.offsetTags);
	for (unsigned int i = 0; i < mTagsCount * mFramesCount; i++)
	{
		md3Tag_t tempTag;
		memcpy(&tempTag, md3String + md3Index, sizeof(md3Tag_t));
		md3Index += sizeof(md3Tag_t);

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
		delete [] mFrames;
		delete [] mTags;

		return;
	}

	// Read Surfaces
	md3Index = readLEInt(modelHeader.offsetSurfaces);
	for (unsigned int i = 0; i < mSurfacesCount; i++)
	{
		// long int surfaceStartOffset = ftell(md3File);
		long int surfaceStartOffset = md3Index;

		md3Surface_t tempSurface;
		memcpy(&tempSurface, md3String + md3Index, sizeof(md3Surface_t));
		md3Index += sizeof(md3Surface_t);
	
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
			// fclose(md3File);

			delete [] mFrames;
			delete [] mTags;
			delete [] mSurfaces;

			return;
		}
	
		md3Index = surfaceStartOffset + readLEInt(tempSurface.offsetShaders);
		memcpy(mShaders, md3String + md3Index, sizeof(md3Shader_t) * numShaders);
		md3Index += sizeof(md3Shader_t) * numShaders;

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
			// fclose(md3File);
			delete [] mFrames;
			delete [] mTags;
			delete [] mSurfaces;

			return;
		}

		// fseek(md3File, surfaceStartOffset + readLEInt(tempSurface.offsetTriangles), SEEK_SET);
		md3Index = surfaceStartOffset + readLEInt(tempSurface.offsetTriangles);
		for (unsigned int surf = 0; surf < mSurfaces[i].getIndicesCount(); surf++)
		{
			md3Triangle_t tempTriangle;
			memcpy(&tempTriangle, md3String + md3Index, sizeof(md3Triangle_t));
			md3Index += sizeof(md3Triangle_t);

			mSurfaces[i].setIndex(surf, tempTriangle);
		}

		// The right number of indices gets multiplied by 3 (we have the number of tris)
		mSurfaces[i].setIndexCount(readLEInt(tempSurface.numTris) * 3);

		// allocate uv's
		if (!mSurfaces[i].allocateUVs(readLEInt(tempSurface.numVerts)))
		{
			// fclose(md3File);
			delete [] mFrames;
			delete [] mTags;
			delete [] mSurfaces;
			return;
		}

		// read uvs
		md3Index = surfaceStartOffset + readLEInt(tempSurface.offsetUV);
		for (unsigned int uv = 0; uv < mSurfaces[i].getUVCount(); uv++)
		{
			md3TexCoord_t tempUV;
			memcpy(&tempUV, md3String + md3Index, sizeof(md3TexCoord_t));
			md3Index += sizeof(md3TexCoord_t);

			mSurfaces[i].setUV(uv, tempUV);
		}

		// allocate vertices
		unsigned int tempVertexCount = readLEInt(tempSurface.numVerts) * readLEInt(tempSurface.numFrames);
		if (!mSurfaces[i].allocateVertices(tempVertexCount))
		{
			// fclose(md3File);
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
			// fclose(md3File);
			delete [] mFrames;
			delete [] mTags;
			delete [] mSurfaces;

			return;
		}

		// read temp vertices
		md3Index = surfaceStartOffset + readLEInt(tempSurface.offsetVertex);
		memcpy(tempVertices, md3String + md3Index, sizeof(md3Vertex_t) * tempVertexCount);
		md3Index += sizeof(md3Vertex_t) * tempVertexCount;

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
	delete [] md3String;
}
		
void md3model::setFrame(unsigned short i)
{
	if (i < getFramesCount())
	{
		mCurrentAnimFrame = i;
	}
	else
	{
		std::stringstream msg;
		msg << "Frame desired(" << i << ") is greater than maximum number of frames(" 
		<< getFramesCount() << ")";

		S_LOG_INFO(msg.str());
	}
}

int md3model::getFramesCount() const
{
	if (mShared)
		return mShared->getFramesCount();
	else	
		return mFramesCount;
}

bool md3model::isOpaque() const
{
	for (unsigned int i = 0; i < getSurfacesCount(); i++)
	{
		if (!getSurface(i)->isOpaque())
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

	for (unsigned int i = 0; i < getSurfacesCount(); i++)
		getSurface(i)->draw((uint32_t)mCurrentAnimFrame);

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

	for (unsigned int i = 0; i < getSurfacesCount(); i++)
		getSurface(i)->draw((uint32_t)mCurrentAnimFrame);

	if (getDrawBoundingBox())
		getAABoundingBox().draw();

	for (std::vector<md3model*>::iterator it = mAttach.begin(); it != mAttach.end(); it++)
		(*it)->attachDraw();
}

boundingBox md3model::getAABoundingBox() const
{
	if (mShared)
		return mShared->getAABoundingBox();

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
	if (mShared)
		return mShared->getBoundingBox();

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
	for (tagIndex = 0; tagIndex < mTagsCount * getFramesCount(); tagIndex++)
	{
		if (mTags[tagIndex].mName == name)
			break;
	}
	
	if (tagIndex == (mTagsCount * getFramesCount()))
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
		
void md3model::setDrawNormals(bool draw)
{
	for (unsigned int i = 0; i < getSurfacesCount(); i++)
		getSurface(i)->setDrawNormals(draw);
}
		
bool md3model::trace(ray& traceRay)
{
	matrix4 mFinal = mOrientation.toMatrix().getInverseTranslation(mPosition);
	traceRay.setTransformation(mFinal);

	for (unsigned int i = 0; i < getSurfacesCount(); i++)
	{
		if (getSurface(i)->trace(traceRay, (uint32_t)mCurrentAnimFrame))
			return true;
	}

	return false;
}

}

