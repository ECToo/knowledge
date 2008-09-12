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
	delete [] mNormalList;
	delete [] mIndexList;

	delete [] mVertices;
	delete [] mWeights;
	delete [] mTriangles;
}

void md5mesh::prepareVertices(unsigned int size)
{
	#ifdef __WII__
	mVertices = (vert_t*) memalign(32, size * sizeof(vert_t));
	mVertexList = (vec_t*) memalign(32, size * sizeof(vec_t) * 3);
	mUvList = (vec_t*) memalign(32, size * sizeof(vec_t) * 2);
	#else
	mVertices = (vert_t*) malloc(size * sizeof(vert_t));
	mVertexList = (vec_t*) malloc(size * sizeof(vec_t) * 3);
	mUvList = (vec_t*) malloc(size * sizeof(vec_t) * 2);
	#endif

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
	#ifdef __WII__
	mTriangles = (triangle_t*) memalign(32, size * sizeof(triangle_t));
	#else
	mTriangles = (triangle_t*) malloc(size*sizeof(triangle_t));
	#endif

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
	#ifdef __WII__
	mWeights = (weight_t*) memalign(32, size * sizeof(weight_t));
	#else
	mWeights = (weight_t*) malloc(size*sizeof(weight_t));
	#endif

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
	assert(newVertex != NULL);

	newVertex->uv[0] = uv.x;
	newVertex->uv[1] = uv.y;
	newVertex->weight = weight;
}

void md5mesh::pushTriangle(const vector3& triangle)
{
	triangle_t* newTri = &mTriangles[mTIndex++];
	assert(newTri != NULL);
		
	newTri->index[0] = triangle.x;
	newTri->index[1] = triangle.y;
	newTri->index[2] = triangle.z;
}

void md5mesh::pushWeight(const vector2& joint, const vector3& pos)
{
	weight_t* newWeight = &mWeights[mWIndex++];
	assert(newWeight != NULL);

	newWeight->jointIndex = joint.x;
	newWeight->value = joint.y;
	newWeight->pos = pos;
}
		
void md5mesh::setMaterial(material* mat)
{
	assert(mat != NULL);
	mMaterial = mat;
}

void md5mesh::compileBase(std::vector<bone_t*>* boneList)
{
	for (unsigned int vIt = 0; vIt < mVCount; vIt++)
	{
		vert_t* vertex = &mVertices[vIt];
		assert(vertex != NULL);

		vertex->basePos = vector3(0, 0, 0);
		for (int w = vertex->weight.x; w < vertex->weight.x+vertex->weight.y; w++)
		{
			vector3 tempPos;

			weight_t* weight = &mWeights[w];
			assert(weight != NULL);
				
			bone_t* bone = (*boneList)[weight->jointIndex];
			assert(bone != NULL);

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
		assert(tri != NULL);

		vector3 v1, v2, v3;
		vector3 edge1, edge2;
		vector3 normal;

		v1 = mVertices[tri->index[0]].basePos;
		v2 = mVertices[tri->index[1]].basePos;
		v3 = mVertices[tri->index[2]].basePos;

		edge1 = v2 - v1;
		edge2 = v3 - v1;

		normal = edge2.crossProduct(edge1);
		normal.normalise();

		mVertices[tri->index[0]].baseNormal += normal;
		mVertices[tri->index[1]].baseNormal += normal;
		mVertices[tri->index[2]].baseNormal += normal;
	}

	// Normalize Normals
	for (unsigned int vIt = 0; vIt < mVCount; vIt++)
	{
		vert_t* vertex = &mVertices[vIt];
		assert(vertex != NULL);

		vertex->baseNormal.normalise();
		vertex->renderNormal = vertex->baseNormal;
	}

	// Rotate normals by the inverse weight
	// joint orientation in order to get
	// normals for each frame
	for (unsigned int vIt = 0; vIt < mVCount; vIt++)
	{
		vert_t* vertex = &mVertices[vIt];
		assert(vertex != NULL);

		vector3 normal;
		for (int w = 0; w < vertex->weight.x + vertex->weight.y; w++)
		{
			vector3 tempPos;

			weight_t* weight = &mWeights[w];
			assert(weight != NULL);

			bone_t* bone = (*boneList)[weight->jointIndex];
			assert(bone != NULL);

			tempPos = bone->orientation.inverseVector(vertex->baseNormal);
			normal += tempPos;
		}

		normal.normalise();
		vertex->normal = normal;
	}

	// Compute Final Static Positions
	if (!mNormalList)
	{
		#ifdef __WII__
		mNormalList = (vec_t*) memalign(32, sizeof(vec_t) * 3 * mVCount);
		#else
		mNormalList = (vec_t*) malloc(sizeof(vec_t) * 3 * mVCount);
		#endif
	}

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
	unsigned int finalSize = 3*mTCount;
	if (!mIndexList)
	{
		#ifdef __WII__
		mIndexList = (u16*) memalign(32, sizeof(u16) * finalSize);
		#else
		mIndexList = (unsigned int*) malloc(sizeof(unsigned int) * finalSize);
		#endif

		mIndexListSize = finalSize;
	}

	// Double Check
	if (!mIndexList)
	{
		S_LOG_INFO("Failed to allocate array of indices.");
		return;
	}

	#ifdef __WII__
	memset(mIndexList, 0, sizeof(u16) * finalSize);
	#else
	memset(mIndexList, 0, sizeof(unsigned int) * finalSize);
	#endif

	for (unsigned int i = 0; i < mTCount; i++)
	{
		mIndexList[i+2*i] = mTriangles[i].index[0];
		mIndexList[i+2*i+1] = mTriangles[i].index[1];
		mIndexList[i+2*i+2] = mTriangles[i].index[2];
	}
}

void md5mesh::draw()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	// TODO: material parsing and allocation
	mMaterial->prepare();
	
	/* OLD
	rs->setVertexArray(mVertices[0].renderPos, sizeof(vert_t), mTCount * 3);
	rs->setTexCoordArray(mVertices[0].uv, sizeof(vert_t));
	*/

	rs->setVertexArray(mVertexList, 0, mTCount * 3);
	rs->setTexCoordArray(mUvList, 0);
	rs->setNormalArray(mNormalList);

	rs->setVertexIndex(mIndexList, mIndexListSize);
	rs->drawArrays();
}

md5model::md5model(const std::string& filename)
{
	parsingFile file(filename);
	if (!file.isReady())
	{
		S_LOG_INFO("Failed to load model filename (" + filename + ")");
		return;
	}

	unsigned numberOfJointsToParse = 0;

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
				assert(newBone != NULL);
					
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
}

md5model::md5model(const md5model& source)
{
	//TODO
}

md5model::md5model()
{}

md5model::~md5model()
{
	//TODO
}

void md5model::compileBase()
{
	std::list<md5mesh*>::iterator it;
	for (it = mMeshes.begin(); it != mMeshes.end(); it++)
	{
		md5mesh* mesh = (*it);
		assert(mesh != NULL);
	
		mesh->compileBase(&mBones);
	}
}

void md5model::draw()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	vec_t angle;
	vector3 axis;
	mOrientation.toAxisAngle(angle, axis);

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->identityMatrix();
	rs->translateScene(mPosition.x, mPosition.y, mPosition.z);
	rs->rotateScene(angle, axis.x, axis.y, axis.z);

	std::list<md5mesh*>::iterator it;
	for (it = mMeshes.begin(); it != mMeshes.end(); it++)
	{
		md5mesh* mesh = (*it);
		assert(mesh != NULL);
	
		mesh->draw();
	}
}

}

