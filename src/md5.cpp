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

#include "md5.h"
#include "logger.h"
#include "root.h"
#include "resourceManager.h"
#include "materialManager.h"
#include "camera.h"

namespace k {

md5mesh::md5mesh()
{
	mNormalList = NULL;
	mIndexList = NULL;
	mIndexListSize = 0;
	mDrawNormals = false;

	mVIndex = 0;
	mVCount = 0;
	mVertices = NULL;
	mDrawingNormals = NULL;

	mWIndex = 0;
	mWCount = 0;
	mWeights = NULL;

	mTIndex = 0;
	mTCount = 0;
	mTriangles = NULL;
}

md5mesh::~md5mesh()
{
	if (mNormalList)
		free(mNormalList);

	if (mIndexList)
		free(mIndexList);
	
	if (mVertices)
		free(mVertices);

	if (mVertexList)
		free(mVertexList);

	if (mUvList)
		free(mUvList);

	if (mWeights)
		free(mWeights);
	
	if (mTriangles)
		free(mTriangles);
}
		
void md5mesh::setDrawNormals(bool draw)
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
		mDrawingNormals = (vector3*) memalign(32, mVCount * 2 * sizeof(vector3));
		if (!mDrawingNormals)
		{
			S_LOG_INFO("Failed to allocate normal array for drawing normals.");
		}
	}

	mDrawNormals = draw;
}
		
bool md5mesh::isOpaque() const
{
	if (mMaterial)
		return mMaterial->isOpaque();
	else
		return true;
}

const boundingBox& md5mesh::getAABoundingBox()
{
	return mAABB;
}

void md5mesh::prepareVertices(unsigned int size)
{
	mVertices = (vert_t*) memalign(32, size * sizeof(vert_t));
	mVertexList = (vec_t*) memalign(32, size * sizeof(vec_t) * 3);
	mUvList = (vec_t*) memalign(32, size * sizeof(vec_t) * 2);

	if (!mVertices || !mVertexList || !mUvList)
	{
		S_LOG_INFO("Failed to prepare vertices array on md5 model.");

		if (!mVertices)
			free(mVertices);

		if (!mVertexList)
			free(mVertexList);

		if (!mUvList)
			free(mUvList);
	}
	else
	{
		memset(mUvList, 0, sizeof(vec_t) * 2 * size);
		memset(mVertexList, 0, sizeof(vec_t) * 3 * size);
		memset(mVertices, 0, sizeof(vert_t) * size);
		mVCount = size;
	}
}

void md5mesh::prepareTriangles(unsigned int size)
{
	mTriangles = (triangle_t*) memalign(32, size * sizeof(triangle_t));

	if (!mTriangles)
	{
		S_LOG_INFO("Failed to prepare triangle array on md5 model.");
	}
	else
	{
		memset(mTriangles, 0, sizeof(triangle_t) * size);
		mTCount = size;
	}
}

void md5mesh::prepareWeights(unsigned int size)
{
	mWeights = (weight_t*) memalign(32, size * sizeof(weight_t));

	if (!mWeights)
	{
		S_LOG_INFO("Failed to prepare weights array on md5 model.");
	}
	else
	{
		memset(mWeights, 0, sizeof(weight_t) * size);
		mWCount = size;
	}
}
	
void md5mesh::pushVertex(const vector2& uv, const vector2& weight)
{
	vert_t* newVertex = &mVertices[mVIndex++];
	kAssert(newVertex);

	newVertex->uv = uv;
	newVertex->weight = weight;
}

void md5mesh::pushTriangle(const vector3& triangle)
{
	triangle_t* newTri = &mTriangles[mTIndex++];
	kAssert(newTri);
		
	newTri->index[0] = (index_t) triangle.x;
	newTri->index[1] = (index_t) triangle.y;
	newTri->index[2] = (index_t) triangle.z;
}

void md5mesh::pushWeight(const vector2& joint, const vector3& pos)
{
	weight_t* newWeight = &mWeights[mWIndex++];
	kAssert(newWeight);

	newWeight->jointIndex = (index_t) joint.x;
	newWeight->value = joint.y;
	newWeight->pos = pos;
}
		
void md5mesh::setMaterial(material* mat)
{
	kAssert(mat);
	mMaterial = mat;
}

void md5mesh::setMaterial(const std::string& matName)
{
	materialManager* matMgr = &materialManager::getSingleton();
	material* mat = matMgr->getMaterial(matName);

	if (!mat)
	{
		S_LOG_INFO("Invalid pointer to material in md5mesh.");
		return;
	}

	mMaterial = mat;
}
		
void md5mesh::compileVertices(std::vector<bone_t*>* boneList)
{
	for (unsigned int vIt = 0; vIt < mVCount; vIt++)
	{
		vert_t* vertex = &mVertices[vIt];
		kAssert(vertex);

		vertex->renderPos = vector3::zero;
		vertex->renderNormal = vertex->baseNormal;

		for (int w = vertex->weight.x; w < vertex->weight.x+vertex->weight.y; w++)
		{
			vector3 tempPos;
			vector3 tempNormal;

			weight_t* weight = &mWeights[w];
			kAssert(weight);
				
			bone_t* bone = (*boneList)[weight->jointIndex];
			kAssert(bone);

			tempPos = bone->orientation.rotateVector(weight->pos);
			vertex->renderPos += (tempPos + bone->pos) * weight->value;

			// This doesnt work, was supposed to?
			// vertex->renderNormal += bone->orientation.rotateVector(vertex->baseNormal);
		}

		// Put on vertex list
		mVertexList[vIt*3] = vertex->renderPos.x;
		mVertexList[vIt*3 + 1] = vertex->renderPos.y;
		mVertexList[vIt*3 + 2] = vertex->renderPos.z;
		mUvList[vIt*2] = vertex->uv.x;
		mUvList[vIt*2 + 1] = vertex->uv.y;
	}

	// Copy the normals
	memset(mNormalList, 0, sizeof(vec_t) * 3 * mVCount);
	for (unsigned int i = 0; i < mVCount; i++)
	{
		mNormalList[i+2*i] = mVertices[i].renderNormal.x;
		mNormalList[i+2*i+1] = mVertices[i].renderNormal.y;
		mNormalList[i+2*i+2] = mVertices[i].renderNormal.z;
	}
}

void md5mesh::compileBase(std::vector<bone_t*>* boneList)
{
	for (unsigned int vIt = 0; vIt < mVCount; vIt++)
	{
		vert_t* vertex = &mVertices[vIt];
		kAssert(vertex);

		vertex->basePos = vector3::zero;
		for (int w = vertex->weight.x; w < vertex->weight.x+vertex->weight.y; w++)
		{
			vector3 tempPos;

			weight_t* weight = &mWeights[w];
			kAssert(weight);
				
			bone_t* bone = (*boneList)[weight->jointIndex];
			kAssert(bone);

			tempPos = bone->orientation.rotateVector(weight->pos);
			vertex->basePos += (tempPos + bone->pos) * weight->value;
		}

		// BoundingBoxData
		mAABB.setTest(vertex->basePos);

		// Copy final position
		vertex->renderPos = vertex->basePos;

		// Put on vertex list
		mVertexList[vIt*3] = vertex->basePos.x;
		mVertexList[vIt*3 + 1] = vertex->basePos.y;
		mVertexList[vIt*3 + 2] = vertex->basePos.z;
		mUvList[vIt*2] = vertex->uv.x;
		mUvList[vIt*2 + 1] = vertex->uv.y;

		vertex->baseNormal = vector3::zero;
	}

	// Triangles
	for (unsigned int tIt = 0; tIt < mTCount; tIt++)
	{
		triangle_t* tri = &mTriangles[tIt];
		kAssert(tri);

		vector3 v1, v2, v3;
		vector3 edge1, edge2;
		vector3 normal;

		v1 = mVertices[tri->index[0]].basePos;
		v2 = mVertices[tri->index[1]].basePos;
		v3 = mVertices[tri->index[2]].basePos;

		edge1 = v2 - v1;
		edge2 = v3 - v1;

		normal = edge2.crossProduct(edge1);
		normal.normalize();

		mVertices[tri->index[0]].baseNormal += normal;
		mVertices[tri->index[1]].baseNormal += normal;
		mVertices[tri->index[2]].baseNormal += normal;
	}

	// Normalize Normals
	for (unsigned int vIt = 0; vIt < mVCount; vIt++)
	{
		vert_t* vertex = &mVertices[vIt];
		kAssert(vertex);

		vertex->baseNormal.normalize();
		vertex->renderNormal = vertex->baseNormal;
	}

	// Rotate normals by the inverse weight
	// joint orientation in order to get
	// normals for each frame
	for (unsigned int vIt = 0; vIt < mVCount; vIt++)
	{
		vert_t* vertex = &mVertices[vIt];
		kAssert(vertex);

		vector3 normal;
		for (int w = 0; w < vertex->weight.x + vertex->weight.y; w++)
		{
			vector3 tempPos;

			weight_t* weight = &mWeights[w];
			kAssert(weight);

			bone_t* bone = (*boneList)[weight->jointIndex];
			kAssert(bone);

			tempPos = bone->orientation.inverseVector(vertex->baseNormal);
			normal += tempPos;
		}

		normal.normalize();
		vertex->normal = normal;
	}

	// Compute Final Static Positions
	if (!mNormalList)
		mNormalList = (vec_t*) memalign(32, sizeof(vec_t) * 3 * mVCount);

	// Double Check
	if (!mNormalList)
	{
		S_LOG_INFO("Failed to allocate array of normals");
		return;
	}

	// Copy the normals
	memset(mNormalList, 0, sizeof(vec_t) * 3 * mVCount);
	for (unsigned int i = 0; i < mVCount; i++)
	{
		mNormalList[i+2*i] = mVertices[i].renderNormal.x;
		mNormalList[i+2*i+1] = mVertices[i].renderNormal.y;
		mNormalList[i+2*i+2] = mVertices[i].renderNormal.z;
	}

	// Triangles Indexes
	unsigned int finalSize = 3 * mTCount;
	if (!mIndexList)
	{
		mIndexList = (index_t*) memalign(32, sizeof(index_t) * finalSize);
		mIndexListSize = finalSize;
	}

	// Double Check
	if (!mIndexList)
	{
		S_LOG_INFO("Failed to allocate array of indices.");
		return;
	}

	memset(mIndexList, 0, sizeof(index_t) * finalSize);
	for (unsigned int i = 0; i < mTCount; i++)
	{
		mIndexList[i*3] = mTriangles[i].index[0];
		mIndexList[i*3 + 1] = mTriangles[i].index[1];
		mIndexList[i*3 + 2] = mTriangles[i].index[2];
	}
}

void md5mesh::draw()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	mMaterial->start();

	rs->clearArrayDesc();
	rs->setVertexArray(mVertexList);
	rs->setVertexCount(mVCount);

	rs->setTexCoordArray(mUvList);
	rs->setNormalArray(mNormalList);

	rs->setVertexIndex(mIndexList);
	rs->setIndexCount(mIndexListSize);

	rs->drawArrays();

	mMaterial->finish();

	if (mDrawNormals)
	{
		// Copy Normals
		for (unsigned int i = 0; i < mVCount; i++)
		{
			mDrawingNormals[i * 2] = mVertices[i].renderPos;
			mDrawingNormals[(i * 2) + 1] = (mVertices[i].renderPos + (mVertices[i].renderNormal * 2));
		}

		material* normalMaterial =  materialManager::getSingleton().getMaterial("k_base_white");
		kAssert(normalMaterial);

		normalMaterial->start();

		rs->clearArrayDesc(VERTEXMODE_LINE);
		rs->setVertexArray(mDrawingNormals[0].vec);
		rs->setVertexCount(mVCount * 2);

		rs->drawArrays(true);

		normalMaterial->finish();
	}
}

md5model::md5model(const std::string& filename) : drawable3D()
{
	// Clear Lists/Maps/Vectors
	mAnimations.clear();
	mMeshes.clear();
	mBones.clear();

	// Auto feed is on by default
	mDrawableAttach = NULL;
	mAutoFeedAnims = true;

	// Get Path from resource manager (if any)
	std::string fullPath = filename;
	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc) fullPath = rsc->getRoot() + filename;

	parsingFile file(fullPath);
	if (!file.isReady())
	{
		S_LOG_INFO("Failed to load model filename (" + filename + ")");
		return;
	}

	unsigned int numberOfJointsToParse = 0;

	std::string token = file.getNextToken();
	while (!file.eof())
	{
		if (token == "mesh")
		{
			try 
			{
				md5mesh* thisMesh = new md5mesh();
				file.skipNextToken(); // {
	
				// Shader Comes here with the name
				file.skipNextToken(); // "shader"
				token = file.getNextToken(); // "shader" param
				thisMesh->setMaterial(materialManager::getSingleton().getMaterial(token));
	
				file.skipNextToken(); // numverts
				token = file.getNextToken(); // numverts param
	
				unsigned int numberOfVertices = atoi(token.c_str());
				thisMesh->prepareVertices(numberOfVertices);
	
				for (unsigned int i = 0; i < numberOfVertices; i++)
				{
					// vertices are like: vert 0 ( 0.539093 0.217694 ) 0 3
					token = file.getNextToken(); // vert
					if (token != "vert")
					{
						S_LOG_INFO("Unexpected input received (" + token + ") expected vert");
						break;
					}
	
					vector2 uv;
					vector2 weight;
	
					// Should Get Index
					file.skipNextToken();
	
					// UV DATA
					file.skipNextToken(); // (
	
					token = file.getNextToken(); // First uv value
					uv.x = atof(token.c_str());
	
					token = file.getNextToken(); // Second uv value
					uv.y = atof(token.c_str());
	
					file.skipNextToken(); // )
	
					// Weight Data
					token = file.getNextToken(); // First weight
					weight.x = atoi(token.c_str());
	
					token = file.getNextToken(); // Second weight
					weight.y = atoi(token.c_str());

					// Check for 0 weighted vertices
					if ((int)weight.y == 0)
					{
						S_LOG_INFO("Model cant have 0 weighted vertices, aborting.");
						delete thisMesh;
						return;
					}
	
					// Push data
					thisMesh->pushVertex(uv, weight);
				}
	
				token = file.getNextToken(); // numtris
				if (token != "numtris")
				{
					S_LOG_INFO("Unexpected input received (" + token + ") expected numtris");
					delete thisMesh;
					return;
				}
	
				unsigned int numberOfTris = atoi(file.getNextToken().c_str());
				thisMesh->prepareTriangles(numberOfTris);
	
				for (unsigned int i = 0; i < numberOfTris; i++)
				{
					// Tris are described like: tri 0 0 2 1
					token = file.getNextToken();
					if (token != "tri")
					{
						S_LOG_INFO("Unexpected input received (" + token + ") expected tri");
						break;
					}
	
					vector3 triangle;
	
					file.skipNextToken(); // index
	
					token = file.getNextToken();
					triangle.x = atoi(token.c_str());
	
					token = file.getNextToken();
					triangle.y = atoi(token.c_str());
	
					token = file.getNextToken();
					triangle.z = atoi(token.c_str());
	
					thisMesh->pushTriangle(triangle);
				}
	
				// Read Weight Data
				token = file.getNextToken(); // numweights
				if (token != "numweights")
				{
					S_LOG_INFO("Unexpected input received (" + token + ") expected numweights");
					delete thisMesh;
					return;
				}
	
				unsigned int numberOfWeights = atoi(file.getNextToken().c_str());
				thisMesh->prepareWeights(numberOfWeights);
	
				for (unsigned int i = 0; i < numberOfWeights; i++)
				{
					// weights are described like that: weight 0 4 0.000000 ( 13.718612 -2.371034 0.006163 )
					token = file.getNextToken();
					if (token != "weight")
					{
						S_LOG_INFO("Unexpected input received (" + token + ") expected weight");
						break;
					}
	
					file.skipNextToken(); // index
	
					vector2 joint;
					vector3 pos;
	
					token = file.getNextToken(); // join index
					joint.x = atoi(token.c_str());
	
					token = file.getNextToken(); // join value
					joint.y = atof(token.c_str());
	
					file.skipNextToken(); // (

					// Weight positions
					token = file.getNextToken();
					pos.x = atof(token.c_str());
					token = file.getNextToken();
					pos.y = atof(token.c_str());
					token = file.getNextToken();
					pos.z = atof(token.c_str());
	
					file.skipNextToken(); // )

					thisMesh->pushWeight(joint, pos);
				}
				mMeshes.push_back(thisMesh);
			} 
			catch (...)
			{
				S_LOG_INFO("Failed to allocate md5mesh.");
				return;
			}
		} 
		// if (token == "mesh")
		else
		if (token == "numJoints")
		{
			token = file.getNextToken(); // number of Joints
			numberOfJointsToParse = atoi(token.c_str());
		} 
		// if (token == "numJoints")
		else
		if (token == "joints")
		{
			// Joints are described like that: "Bip01" -1 ( 0.000681 -0.000434 37.323601 ) ( 0.000000 0.000000 0.707106 )
			file.skipNextToken(); // {

			for (unsigned int i = 0; i < numberOfJointsToParse; i++)
			{
				int parentIndex;
				vector3 pos;
				vector3 orientation;

				file.skipNextToken(); // name

				token = file.getNextToken(); // parent index
				parentIndex = atoi(token.c_str());

				file.skipNextToken(); // (
				token = file.getNextToken();
				pos.x = atof(token.c_str());
				token = file.getNextToken();
				pos.y = atof(token.c_str());
				token = file.getNextToken();
				pos.z = atof(token.c_str());
				file.skipNextToken(); // )

				file.skipNextToken(); // (
				token = file.getNextToken();
				orientation.x = atof(token.c_str());
				token = file.getNextToken();
				orientation.y = atof(token.c_str());
				token = file.getNextToken();
				orientation.z = atof(token.c_str());
				file.skipNextToken(); // )

				try
				{
					bone_t* newBone = new bone_t;
					
					newBone->parentIndex = parentIndex;
					newBone->index = i;
					newBone->pos = pos;
					newBone->orientation = quaternion(orientation);
					newBone->orientation.computeW();
					newBone->currentAnim = NULL;

					newBone->basePos = newBone->pos;
					newBone->baseOrientation = newBone->orientation;

					mBones.push_back(newBone);
				}
				catch (...)
				{
					S_LOG_INFO("Failed to allocate new bone.");
					return;
				}
			}
		} 
		// if (token == "joints")

		token = file.getNextToken();
	} 
	// while (!token.is_empty())

	compileBase();
	S_LOG_INFO("MD5 Model " + filename + " loaded.");
}

void md5model::clone(md5model* cloned)
{
	kAssert(cloned);
}

md5model::~md5model()
{
	std::map<std::string, anim_t*>::iterator animIt;
	for (animIt = mAnimations.begin(); animIt != mAnimations.end(); animIt++)
	{
		anim_t* tmpAnim = animIt->second;

		delete [] tmpAnim->hierarchy;
		delete [] tmpAnim->baseFrame;
		delete [] tmpAnim->bounds;

		for (unsigned int i = 0; i < tmpAnim->numFrames; i++)
			delete [] tmpAnim->frames[i];

		delete [] tmpAnim->frames;
		delete tmpAnim;
	}

	std::list<md5mesh*>::iterator meshIt;
	for (meshIt = mMeshes.begin(); meshIt != mMeshes.end(); meshIt++)
		delete (*meshIt);


	std::vector<bone_t*>::iterator boneIt;
	for (boneIt = mBones.begin(); boneIt != mBones.end(); boneIt++)
		delete (*boneIt);

	mAnimations.clear();
	mMeshes.clear();
	mBones.clear();
}

void md5model::compileVertices()
{
	std::list<md5mesh*>::iterator it;
	for (it = mMeshes.begin(); it != mMeshes.end(); it++)
	{
		md5mesh* mesh = (*it);
		kAssert(mesh);
	
		mesh->compileVertices(&mBones);
	}
}

void md5model::compileBase()
{
	std::list<md5mesh*>::iterator it;
	for (it = mMeshes.begin(); it != mMeshes.end(); it++)
	{
		md5mesh* mesh = (*it);
		kAssert(mesh);
	
		mesh->compileBase(&mBones);
	}
}

void md5model::draw()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();

	// Feed animations =]
	feedAnims();

	vector3 finalPos = getAbsolutePosition();
	quaternion finalOrientation = getAbsoluteOrientation();

	// Rotate and Translate
	vec_t angle;
	vector3 axis;
	finalOrientation.toAxisAngle(angle, axis);

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

	// Draw Meshes
	std::list<md5mesh*>::iterator it;
	for (it = mMeshes.begin(); it != mMeshes.end(); it++)
	{
		md5mesh* mesh = (*it);
		kAssert(mesh);
	
		mesh->draw();
	}
	
	if (getDrawBoundingBox())
		getAABoundingBox().draw();
}

bool md5model::isOpaque() const
{
	std::list<md5mesh*>::const_iterator it;
	for (it = mMeshes.begin(); it != mMeshes.end(); it++)
	{
		if (!(*it)->isOpaque())
			return false;
	}

	return true;
}

void md5model::attachAnimation(const std::string& filename, const std::string& name)
{
	std::string fullPath = filename;

	// Get Path from resource manager (if any)
	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc) fullPath = rsc->getRoot() + filename;

	parsingFile file(fullPath);
	if (!file.isReady())
	{
		S_LOG_INFO("Failed to attach model animation (" + filename + ")");
		return;
	}

	// Great, the file is found, allocate an animation slot
	anim_t* newAnimation;
	try
	{
		newAnimation = new anim_t;
		memset(newAnimation, 0, sizeof(anim_t));
	}
	
	catch (...)
	{
		S_LOG_INFO("Failed to allocate memory for a new animation.");
		return;
	}

	// Parse it
	std::string token = file.getNextToken();
	while (!file.eof())
	{
		if (token == "MD5Version")
		{
			token = file.getNextToken();
			if (token != "10")
			{
				S_LOG_INFO("Incorrect version of md5anim file, expected 10, found " + token);
				delete newAnimation;

				return;
			}
		} // MD5Version
		else
		if (token == "commandline")
		{
			file.skipNextToken();
		} // commandline
		else
		if (token == "numFrames")
		{
			token = file.getNextToken(); // number of frames
			newAnimation->numFrames = atoi(token.c_str());
			try
			{
				newAnimation->bounds = new bound_t[newAnimation->numFrames];
				memset(newAnimation->bounds, 0, sizeof(bound_t)*newAnimation->numFrames);
			}

			catch (...)
			{
				S_LOG_INFO("Failed to allocate bounding boxes array.");
				delete newAnimation;

				return;
			}

			try
			{
				newAnimation->frames = new vec_t*[newAnimation->numFrames];
			}

			catch (...)
			{
				S_LOG_INFO("Failed to allocate array of frames.");
				delete newAnimation;

				return;
			}
		}
		else
		if (token == "numJoints")
		{
			token = file.getNextToken(); // number of joints
			newAnimation->numBones = atoi(token.c_str());

			// A base position for each frame
			try
			{
				newAnimation->baseFrame = new baseBone_t[newAnimation->numBones];
				memset(newAnimation->baseFrame, 0, sizeof(baseBone_t)*newAnimation->numBones);
			}

			catch (...)
			{
				S_LOG_INFO("Failed to allocate base bone positions for each frame.");
				delete newAnimation;

				return;
			}

			// The Complete hierarchy for animation
			try 
			{
				newAnimation->hierarchy = new boneFrame_t[newAnimation->numBones];
				memset(newAnimation->hierarchy, 0, sizeof(boneFrame_t)*newAnimation->numBones);
			}

			catch (...)
			{
				S_LOG_INFO("Failed to allocate animation hierarchy.");
				delete newAnimation;

				return;
			}
		} // numJoints
		else
		if (token == "frameRate")
		{
			token = file.getNextToken(); // frameRate param
			newAnimation->frameRate = atoi(token.c_str());
		} // framerate
		else
		if (token == "numAnimatedComponents")
		{
			token = file.getNextToken(); // animated components number
			newAnimation->animatedComponents = atoi(token.c_str()); 
			for (unsigned int i = 0; i < newAnimation->numFrames; i++)
			{
				try
				{
					newAnimation->frames[i] = new vec_t[newAnimation->animatedComponents];
					memset(newAnimation->frames[i], 0, sizeof(vec_t)*newAnimation->animatedComponents);
				}

				catch (...)
				{
					S_LOG_INFO("Failed to allocate frame animated components.");
					delete newAnimation;
					
					return;
				}
			}
		} // numAnimatedComponents
		else
		if (token == "hierarchy")
		{
			file.skipNextToken(); // {
			for (unsigned int i = 0; i < newAnimation->numBones; i++)
			{
				boneFrame_t* thisBone = &newAnimation->hierarchy[i];
				kAssert(thisBone);

				// Set Index
				thisBone->index = i;

				// Bone name
				file.skipNextToken();

				// parent
				token = file.getNextToken();
				thisBone->parentIndex = atoi(token.c_str());

				// mask 
				token = file.getNextToken();
				thisBone->mask = atoi(token.c_str());

				// start index
				token = file.getNextToken();
				thisBone->startIndex = atoi(token.c_str());
			}
			file.skipNextToken(); // }
		} // hierarchy
		else
		if (token == "bounds")
		{
			file.skipNextToken(); // {
			for (unsigned int i = 0; i < newAnimation->numFrames; i++)
			{
				bound_t* thisBound = &newAnimation->bounds[i];
				kAssert(thisBound);

				file.skipNextToken(); // (
				token = file.getNextToken(); // X
				thisBound->mins.x = atof(token.c_str());
				token = file.getNextToken(); // Y
				thisBound->mins.y = atof(token.c_str());
				token = file.getNextToken(); // Z
				thisBound->mins.z = atof(token.c_str());
				file.skipNextToken(); // )

				file.skipNextToken(); // (
				token = file.getNextToken(); // X
				thisBound->maxs.x = atof(token.c_str());
				token = file.getNextToken(); // Y
				thisBound->maxs.y = atof(token.c_str());
				token = file.getNextToken(); // Z
				thisBound->maxs.z = atof(token.c_str());
				file.skipNextToken(); // )
			}
			file.skipNextToken(); // }
		} // bounds
		else
		if (token == "baseframe")
		{
			file.skipNextToken(); // {
			for (unsigned int i = 0; i < newAnimation->numBones; i++)
			{
				// For Quaternion
				quaternion q;

				baseBone_t* thisBone = &newAnimation->baseFrame[i];
				kAssert(thisBone);

				file.skipNextToken(); // (
				token = file.getNextToken(); // X
				thisBone->pos.x = atof(token.c_str());
				token = file.getNextToken(); // Y
				thisBone->pos.y = atof(token.c_str());
				token = file.getNextToken(); // Z
				thisBone->pos.z = atof(token.c_str());
				file.skipNextToken(); // )

				file.skipNextToken(); // (
				token = file.getNextToken(); // X
				q.x = atof(token.c_str());
				token = file.getNextToken(); // Y
				q.y = atof(token.c_str());
				token = file.getNextToken(); // Z
				q.z = atof(token.c_str());
				file.skipNextToken(); // )

				q.computeW();
				thisBone->orientation = q;
			}
			file.skipNextToken(); // }
		} // baseframe
		else
		if (token == "frame")
		{
			vec_t* activeFrame;	

			token = file.getNextToken(); // frame index
			activeFrame = newAnimation->frames[atoi(token.c_str())];
			kAssert(activeFrame);

			file.skipNextToken(); // {
			for (unsigned int i = 0; i < newAnimation->animatedComponents; i++)
			{
				token = file.getNextToken();
				activeFrame[i] = atof(token.c_str());
			}
			file.skipNextToken(); // }
		} // frame
			
		token = file.getNextToken();
	}

	// Successfully parsed file, save it on the model
	mAnimations[name] = newAnimation;
	S_LOG_INFO("Attached animation " + name + " to model.");
}
		
anim_t* md5model::getAnimation(const std::string& name)
{
	std::map<std::string, anim_t*>::iterator it = mAnimations.find(name);
	if (it != mAnimations.end())
	{
		return it->second;
	}
	
	return NULL;
}

void md5model::setAnimation(const std::string& name)
{
	anim_t* destAnimation = getAnimation(name);
	if (!destAnimation)
	{
		S_LOG_INFO("Animation " + name + " not found, did you attached it?");
		return;
	}

	destAnimation->currentFrame = 0;
	destAnimation->lastFeedTime = root::getSingleton().getGlobalTime();

	for (unsigned int i = 0; i < mBones.size(); i++)
	{
		bone_t* thisBone = mBones[i];
		kAssert(thisBone);

		thisBone->currentAnim = destAnimation;
	}
}
		
void md5model::setDrawNormals(bool draw)
{
	std::list<md5mesh*>::iterator it;
	for (it = mMeshes.begin(); it != mMeshes.end(); it++)
		(*it)->setDrawNormals(draw);
}

void md5model::feedAnims()
{
	if (!mAutoFeedAnims || !mAnimations.size())
		return;

	// Global Time
	long timeNow = root::getSingleton().getGlobalTime();

	// Update anims
	for (unsigned int i = 0; i < mBones.size(); i++)
	{
		bone_t* thisBone = mBones[i];
		kAssert(thisBone);

		if (!thisBone->currentAnim)
			continue;

		anim_t* currentAnim = thisBone->currentAnim;

		// Time elapsed
		currentAnim->currentFrame += (currentAnim->frameRate * (timeNow - currentAnim->lastFeedTime)) / 1000.0f;
		currentAnim->lastFeedTime = timeNow;

		while ((uint32_t)currentAnim->currentFrame >= currentAnim->numFrames)
			currentAnim->currentFrame -= currentAnim->numFrames;

		// Real frame number as int
		unsigned int realFrameNum = (uint32_t)currentAnim->currentFrame;

		// Copy Bone frame positions to modify 'em
		thisBone->pos = currentAnim->baseFrame[thisBone->index].pos;
		thisBone->orientation = currentAnim->baseFrame[thisBone->index].orientation;

		// Check modifiers per frame
		boneFrame_t* boneOnFrame = &currentAnim->hierarchy[thisBone->index];
		if (boneOnFrame->mask)
		{
			int j = 0;
			bool quatChanged = false;

			if (boneOnFrame->mask & BONE_POS_X)
				thisBone->pos.x = currentAnim->frames[realFrameNum][boneOnFrame->startIndex + j++];
			if (boneOnFrame->mask & BONE_POS_Y)
				thisBone->pos.y = currentAnim->frames[realFrameNum][boneOnFrame->startIndex + j++];
			if (boneOnFrame->mask & BONE_POS_Z)
				thisBone->pos.z = currentAnim->frames[realFrameNum][boneOnFrame->startIndex + j++];

			if (boneOnFrame->mask & BONE_ORI_X)
			{
				thisBone->orientation.x = currentAnim->frames[realFrameNum][boneOnFrame->startIndex + j++];
				quatChanged = true;
			}
			if (boneOnFrame->mask & BONE_ORI_Y)
			{
				thisBone->orientation.y = currentAnim->frames[realFrameNum][boneOnFrame->startIndex + j++];
				quatChanged = true;
			}
			if (boneOnFrame->mask & BONE_ORI_Z)
			{
				thisBone->orientation.z = currentAnim->frames[realFrameNum][boneOnFrame->startIndex + j++];
				quatChanged = true;
			}

			if (quatChanged)
				thisBone->orientation.computeW();
		} // masks

		// This bone has a parent
		if (boneOnFrame->parentIndex > -1)
		{
			bone_t* parentBone = mBones[thisBone->parentIndex];
			kAssert(parentBone);

			quaternion oriQuat = thisBone->orientation;
			vector3 oriPos = thisBone->pos;

			vector3 tempPos = parentBone->orientation.rotateVector(oriPos);
			thisBone->pos = parentBone->pos + tempPos;

			thisBone->orientation = parentBone->orientation * oriQuat;
			thisBone->orientation.normalize();
		}
	} // for Bones

	compileVertices();
}

void md5model::setAnimationFrame(unsigned int frameNum)
{
	for (unsigned int i = 0; i < mBones.size(); i++)
	{
		bone_t* thisBone = mBones[i];
		kAssert(thisBone);

		if (!thisBone->currentAnim)
			continue;

		anim_t* currentAnim = thisBone->currentAnim;

		unsigned int realFrameNum;
		if (frameNum >= currentAnim->numFrames && currentAnim->numFrames)
			realFrameNum = frameNum % currentAnim->numFrames;
		else
			realFrameNum = frameNum;

		if (currentAnim->currentFrame == frameNum)
			continue;

		// Copy Bone frame positions to modify 'em
		thisBone->pos = currentAnim->baseFrame[thisBone->index].pos;
		thisBone->orientation = currentAnim->baseFrame[thisBone->index].orientation;

		// Check modifiers per frame
		boneFrame_t* boneOnFrame = &currentAnim->hierarchy[thisBone->index];
		if (boneOnFrame->mask)
		{
			int j = 0;
			bool quatChanged = false;

			if (boneOnFrame->mask & BONE_POS_X)
				thisBone->pos.x = currentAnim->frames[realFrameNum][boneOnFrame->startIndex + j++];
			if (boneOnFrame->mask & BONE_POS_Y)
				thisBone->pos.y = currentAnim->frames[realFrameNum][boneOnFrame->startIndex + j++];
			if (boneOnFrame->mask & BONE_POS_Z)
				thisBone->pos.z = currentAnim->frames[realFrameNum][boneOnFrame->startIndex + j++];

			if (boneOnFrame->mask & BONE_ORI_X)
			{
				thisBone->orientation.x = currentAnim->frames[realFrameNum][boneOnFrame->startIndex + j++];
				quatChanged = true;
			}
			if (boneOnFrame->mask & BONE_ORI_Y)
			{
				thisBone->orientation.y = currentAnim->frames[realFrameNum][boneOnFrame->startIndex + j++];
				quatChanged = true;
			}
			if (boneOnFrame->mask & BONE_ORI_Z)
			{
				thisBone->orientation.z = currentAnim->frames[realFrameNum][boneOnFrame->startIndex + j++];
				quatChanged = true;
			}

			if (quatChanged)
				thisBone->orientation.computeW();
		} // masks

		// This bone has a parent
		if (boneOnFrame->parentIndex > -1)
		{
			bone_t* parentBone = mBones[thisBone->parentIndex];
			kAssert(parentBone);

			quaternion oriQuat = thisBone->orientation;
			vector3 oriPos = thisBone->pos;

			vector3 tempPos = parentBone->orientation.rotateVector(oriPos);
			thisBone->pos = parentBone->pos + tempPos;

			thisBone->orientation = parentBone->orientation * oriQuat;
			thisBone->orientation.normalize();
		}
	} // for Bones

	compileVertices();
}
		
md5mesh* md5model::getMesh(unsigned int index)
{
	unsigned int i = 0;
	std::list<md5mesh*>::iterator it;
	for (it = mMeshes.begin(); it != mMeshes.end(); it++)
	{
		if (i == index)
			return (*it);

		i++;
	}

	return NULL;
}
		
void md5model::setAutoFeed(bool feed)
{
	mAutoFeedAnims = feed;
}

bool md5model::getAutoFeed()
{
	return mAutoFeedAnims;
}
		
boundingBox md5model::getBoundingBox() const
{
	return boundingBox(vector3::zero, vector3::zero);
}

boundingBox md5model::getAABoundingBox() const
{
	boundingBox AABB;

	for (unsigned int i = 0; i < mBones.size(); i++)
	{
		bone_t* thisBone = mBones[i];
		if (!thisBone)
			continue;

		anim_t* thisAnim = thisBone->currentAnim;
		if (!thisAnim)
			continue;

		if (thisAnim)
		{
			// Get Bounds for current Frame
			bound_t* animBound = &thisAnim->bounds[(int)thisAnim->currentFrame];
			if (!animBound)
				continue;

			AABB.setTestMins(animBound->mins);
			AABB.setTestMaxs(animBound->maxs);
		}
	}

	// In case we didnt find any anim, use model ones
	if (AABB.getMins() == vector3::zero && AABB.getMaxs() == vector3::zero)
	{
		std::list<md5mesh*>::const_iterator it;
		for (it = mMeshes.begin(); it != mMeshes.end(); it++)
		{
			boundingBox meshAABB = (*it)->getAABoundingBox();
			AABB.setTestMins(meshAABB.getMins());
			AABB.setTestMaxs(meshAABB.getMaxs());
		}
	}

	return AABB;
}

}

