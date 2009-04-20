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

#include "md5.h"
#include "logger.h"
#include "root.h"
#include "materialManager.h"

namespace k {

md5mesh::md5mesh()
{
	mNormalList = NULL;
	mIndexList = NULL;
	mIndexListSize = 0;

	mVIndex = 0;
	mVCount = 0;
	mVertices = NULL;

	mWIndex = 0;
	mWCount = 0;
	mWeights = NULL;

	mTIndex = 0;
	mTCount = 0;
	mTriangles = NULL;
}

md5mesh::~md5mesh()
{
	free(mNormalList);
	free(mIndexList);
	free(mVertices);
	free(mVertexList);
	free(mUvList);
	free(mWeights);
	free(mTriangles);
}

void md5mesh::prepareVertices(unsigned int size)
{
	mVertices = (vert_t*) memalign(32, size * sizeof(vert_t));
	mVertexList = (vec_t*) memalign(32, size * sizeof(vec_t) * 3);
	mUvList = (vec_t*) memalign(32, size * sizeof(vec_t) * 2);

	if (!mVertices || !mVertexList || !mUvList)
	{
		S_LOG_INFO("Failed to prepare vertices array on md5 model.");
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
	kAssert(newVertex != NULL);

	newVertex->uv[0] = uv.x;
	newVertex->uv[1] = uv.y;
	newVertex->weight = weight;
}

void md5mesh::pushTriangle(const vector3& triangle)
{
	triangle_t* newTri = &mTriangles[mTIndex++];
	kAssert(newTri != NULL);
		
	newTri->index[0] = triangle.x;
	newTri->index[1] = triangle.y;
	newTri->index[2] = triangle.z;
}

void md5mesh::pushWeight(const vector2& joint, const vector3& pos)
{
	weight_t* newWeight = &mWeights[mWIndex++];
	kAssert(newWeight != NULL);

	newWeight->jointIndex = joint.x;
	newWeight->value = joint.y;
	newWeight->pos = pos;
}
		
void md5mesh::setMaterial(material* mat)
{
	kAssert(mat != NULL);
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
		kAssert(vertex != NULL);

		vertex->renderPos[0] = vertex->renderPos[1] = vertex->renderPos[2] = 0;
		vertex->renderNormal = vector3(0, 0, 0);
		for (int w = vertex->weight.x; w < vertex->weight.x+vertex->weight.y; w++)
		{
			vector3 tempPos;

			weight_t* weight = &mWeights[w];
			kAssert(weight != NULL);
				
			bone_t* bone = (*boneList)[weight->jointIndex];
			kAssert(bone != NULL);

			tempPos = bone->orientation.rotateVector(weight->pos);
			vertex->renderPos[0] += (tempPos.x + bone->pos.x) * weight->value;
			vertex->renderPos[1] += (tempPos.y + bone->pos.y) * weight->value;
			vertex->renderPos[2] += (tempPos.z + bone->pos.z) * weight->value;

			vertex->renderNormal += bone->orientation.rotateVector(vertex->baseNormal);
		}

		// Put on vertex list
		mVertexList[vIt*3] = vertex->renderPos[0];
		mVertexList[vIt*3 + 1] = vertex->renderPos[1];
		mVertexList[vIt*3 + 2] = vertex->renderPos[2];
		mUvList[vIt*2] = vertex->uv[0];
		mUvList[vIt*2 + 1] = vertex->uv[1];
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
		kAssert(vertex != NULL);

		vertex->basePos = vector3(0, 0, 0);
		for (int w = vertex->weight.x; w < vertex->weight.x+vertex->weight.y; w++)
		{
			vector3 tempPos;

			weight_t* weight = &mWeights[w];
			kAssert(weight != NULL);
				
			bone_t* bone = (*boneList)[weight->jointIndex];
			kAssert(bone != NULL);

			tempPos = bone->orientation.rotateVector(weight->pos);
			vertex->basePos += (tempPos + bone->pos) * weight->value;
		}

		// Copy final position
		vertex->renderPos[0] = vertex->basePos.x;
		vertex->renderPos[1] = vertex->basePos.y;
		vertex->renderPos[2] = vertex->basePos.z;

		// Put on vertex list
		mVertexList[vIt*3] = vertex->basePos.x;
		mVertexList[vIt*3 + 1] = vertex->basePos.y;
		mVertexList[vIt*3 + 2] = vertex->basePos.z;
		mUvList[vIt*2] = vertex->uv[0];
		mUvList[vIt*2 + 1] = vertex->uv[1];

		vertex->baseNormal = vector3(0, 0, 0);
	}

	// Triangles
	for (unsigned int tIt = 0; tIt < mTCount; tIt++)
	{
		triangle_t* tri = &mTriangles[tIt];
		kAssert(tri != NULL);

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
		kAssert(vertex != NULL);

		vertex->baseNormal.normalize();
		vertex->renderNormal = vertex->baseNormal;
	}

	// Rotate normals by the inverse weight
	// joint orientation in order to get
	// normals for each frame
	for (unsigned int vIt = 0; vIt < mVCount; vIt++)
	{
		vert_t* vertex = &mVertices[vIt];
		kAssert(vertex != NULL);

		vector3 normal;
		for (int w = 0; w < vertex->weight.x + vertex->weight.y; w++)
		{
			vector3 tempPos;

			weight_t* weight = &mWeights[w];
			kAssert(weight != NULL);

			bone_t* bone = (*boneList)[weight->jointIndex];
			kAssert(bone != NULL);

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
	mMaterial->prepare();

	rs->clearArrayDesc();
	rs->setVertexArray(mVertexList);
	rs->setVertexCount(mVCount);

	rs->setTexCoordArray(mUvList);
	rs->setNormalArray(mNormalList);

	rs->setVertexIndex(mIndexList);
	rs->setIndexCount(mIndexListSize);

	rs->drawArrays();

	mMaterial->finish();
}

md5model::md5model(const std::string& filename)
{
	// Clear Lists/Maps/Vectors
	mAnimations.clear();
	mMeshes.clear();
	mBones.clear();

	// Auto feed is on by default
	mAutoFeedAnims = true;

	parsingFile file(filename);
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
			md5mesh* thisMesh = new md5mesh();
			if (!thisMesh)
				return;

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

				bone_t* newBone = new bone_t;
				kAssert(newBone != NULL);
					
				newBone->parentIndex = parentIndex;
				newBone->index = i;
				newBone->pos = pos;
				newBone->orientation = quaternion(orientation);
				newBone->orientation.computeW();

				newBone->basePos = newBone->pos;
				newBone->baseOrientation = newBone->orientation;

				mBones.push_back(newBone);
			}
		} 
		// if (token == "joints")

		token = file.getNextToken();
	} 
	// while (!token.is_empty())

	compileBase();
	S_LOG_INFO("MD5 Model " + filename + " loaded.");
}

md5model::md5model(const md5model& source)
{
	//TODO
}

md5model::~md5model()
{
	//TODO
}

void md5model::compileVertices()
{
	std::list<md5mesh*>::iterator it;
	for (it = mMeshes.begin(); it != mMeshes.end(); it++)
	{
		md5mesh* mesh = (*it);
		kAssert(mesh != NULL);
	
		mesh->compileVertices(&mBones);
	}
}

void md5model::compileBase()
{
	std::list<md5mesh*>::iterator it;
	for (it = mMeshes.begin(); it != mMeshes.end(); it++)
	{
		md5mesh* mesh = (*it);
		kAssert(mesh != NULL);
	
		mesh->compileBase(&mBones);
	}
}

void md5model::draw()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();

	// Feed animations =]
	feedAnims();

	vector3 finalPos = getPosition();
	quaternion finalOrientation = getOrientation();

	// Rotate and Translate
	vec_t angle;
	vector3 axis;
	finalOrientation.toAxisAngle(angle, axis);

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->translateScene(finalPos.x, finalPos.y, finalPos.z);
	rs->rotateScene(angle, axis.x, axis.y, axis.z);
	rs->scaleScene(mScale.x, mScale.y, mScale.z);

	// Draw Meshes
	std::list<md5mesh*>::iterator it;
	for (it = mMeshes.begin(); it != mMeshes.end(); it++)
	{
		md5mesh* mesh = (*it);
		kAssert(mesh != NULL);
	
		mesh->draw();
	}
}
		
void md5model::attachAnimation(const std::string& filename, const std::string& name)
{
	parsingFile file(filename);
	if (!file.isReady())
	{
		S_LOG_INFO("Failed to attach model animation (" + filename + ")");
		return;
	}

	// Great, the file is found, allocate an animation slot
	anim_t* newAnimation = new anim_t;
	if (!newAnimation)
	{
		S_LOG_INFO("Failed to allocate memory for a new animation.");
		return;
	}
	memset(newAnimation, 0, sizeof(anim_t));

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
			newAnimation->bounds = new bound_t[newAnimation->numFrames];
			if (!newAnimation->bounds)
			{
				S_LOG_INFO("Failed to allocate bounding boxes array.");
				delete newAnimation;

				return;
			}
			memset(newAnimation->bounds, 0, sizeof(bound_t)*newAnimation->numFrames);

			newAnimation->frames = new vec_t*[newAnimation->numFrames];
			if (!newAnimation->frames)
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
			newAnimation->baseFrame = new baseBone_t[newAnimation->numBones];
			if (!newAnimation->baseFrame)
			{
				S_LOG_INFO("Failed to allocate base bone positions for each frame.");
				delete newAnimation;

				return;
			}
			memset(newAnimation->baseFrame, 0, sizeof(baseBone_t)*newAnimation->numBones);

			// The Complete hierarchy for animation
			newAnimation->hierarchy = new boneFrame_t[newAnimation->numBones];
			if (!newAnimation->hierarchy)
			{
				S_LOG_INFO("Failed to allocate animation hierarchy.");
				delete newAnimation;

				return;
			}
			memset(newAnimation->hierarchy, 0, sizeof(boneFrame_t)*newAnimation->numBones);
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
				newAnimation->frames[i] = new vec_t[newAnimation->animatedComponents];
				if (!newAnimation->frames[i])
				{
					S_LOG_INFO("Failed to allocate frame animated components.");
					delete newAnimation;
					
					return;
				}
				memset(newAnimation->frames[i], 0, sizeof(vec_t)*newAnimation->animatedComponents);
			}
		} // numAnimatedComponents
		else
		if (token == "hierarchy")
		{
			file.skipNextToken(); // {
			for (unsigned int i = 0; i < newAnimation->numBones; i++)
			{
				boneFrame_t* thisBone = &newAnimation->hierarchy[i];
				kAssert(thisBone != NULL);

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
				kAssert(thisBound != NULL);

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
				kAssert(thisBone != NULL);

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
			kAssert(activeFrame != NULL);

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
		kAssert(thisBone != NULL);

		thisBone->currentAnim = destAnimation;
	}
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
		kAssert(thisBone != NULL);

		if (!thisBone->currentAnim)
			continue;

		anim_t* currentAnim = thisBone->currentAnim;

		// Time elapsed
		currentAnim->currentFrame += (currentAnim->frameRate * (timeNow - currentAnim->lastFeedTime)) / 1000.0f;
		currentAnim->lastFeedTime = timeNow;

		if ((uint32_t)currentAnim->currentFrame >= currentAnim->numFrames)
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
			kAssert(parentBone != NULL);

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
		kAssert(thisBone != NULL);

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
			kAssert(parentBone != NULL);

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
		
boundingBox md5model::getBoundingBox()
{
	return boundingBox(vector3::zero, vector3::zero);
}

boundingBox md5model::getAABoundingBox()
{
	return boundingBox(vector3::zero, vector3::zero);
}

}

