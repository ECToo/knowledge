#include "md5.h"

namespace k {

md5mesh::md5mesh()
{
	mNormalList = NULL;
	mIndexList = NULL;

	mVertexes.clear();
	mWeights.clear();
	mTriangles.clear();
}

md5mesh::~md5mesh()
{
	// TODO
}
	
void md5mesh::pushVertex(const vector2& uv, const vector2& weight)
{
	vert_t* newVertex = new vert_t;
	if (newVertex)
	{
		newVertex->uv = uv;
		newVertex->weight = weight;
		
		mVertexes.push_back(newVertex);
	}
}

void md5mesh::pushTriangle(const vector3& triangle)
{
	triangle_t* newTri = new triangle_t;
	if (newTri)
	{
		newTri->index[0] = triangle.x;
		newTri->index[1] = triangle.y;
		newTri->index[2] = triangle.z;

		mTriangles.push_back(newTri);
	}
}

void md5mesh::pushWeight(const vector2& joint, const vector3& pos)
{
	weight_t* newWeight = new weight_t;
	if (newWeight)
	{
		newWeight->jointIndex = joint.x;
		newWeight->value = joint.y;
		newWeight->pos = pos;

		mWeights.push_back(newWeight);
	}
}

void md5mesh::compileBase(std::vector<bone_t*>* boneList)
{
	std::vector<vert_t*>::iterator vIt;
	for (vIt = mVertexes.begin(); vIt != mVertexes.end(); vIt++)
	{
		vert_t* vertex = (*vIt);
		if (!vertex)
			continue;

		for (int w = vertex->weight.x; w < vertex->weight.x+vertex->weight.y; w++)
		{
			vector3 tempPos;

			weight_t* weight = mWeights[w];
			if (!weight)
				return;
				
			bone_t* bone = (*boneList)[weight->jointIndex];
			if (!bone)
				return;

			tempPos = bone->orientation.rotateVector(weight->pos);
			vertex->basePos.x += (tempPos.x + bone->pos.x)*weight->value;
			vertex->basePos.y += (tempPos.y + bone->pos.y)*weight->value;
			vertex->basePos.z += (tempPos.z + bone->pos.z)*weight->value;
		}

		// Copy final position
		vertex->renderPos = vertex->basePos;
		vertex->baseNormal = vector3(0, 0, 0);
	}

	// Triangles
	std::vector<triangle_t*>::iterator tIt;
	for (tIt = mTriangles.begin(); tIt != mTriangles.end(); tIt++)
	{
		triangle_t* tri = (*tIt);
		if (!tri)
			continue;

		vector3 v1, v2, v3;
		vector3 edge1, edge2;
		vector3 normal;

		v1 = mVertexes[tri->index[0]]->basePos;
		v2 = mVertexes[tri->index[1]]->basePos;
		v3 = mVertexes[tri->index[2]]->basePos;

		edge1 = v2 - v1;
		edge2 = v3 - v1;

		normal = edge1.crossProduct(edge2);
		normal.normalise();

		mVertexes[tri->index[0]]->baseNormal += normal;
		mVertexes[tri->index[1]]->baseNormal += normal;
		mVertexes[tri->index[2]]->baseNormal += normal;
	}

	// Normalize Normals
	for (vIt = mVertexes.begin(); vIt != mVertexes.end(); vIt++)
	{
		vert_t* vertex = (*vIt);
		if (!vertex)
			continue;

		vertex->baseNormal.normalise();
		vertex->renderNormal = vertex->baseNormal;
	}

	// Rotate normals by the inverse weight
	// joint orientation in order to get
	// normals for each frame
	for (vIt = mVertexes.begin(); vIt != mVertexes.end(); vIt++)
	{
		vert_t* vertex = (*vIt);
		if (!vertex)
			continue;

		vector3 normal;
		for (int w = 0; w < vertex->weight.x + vertex->weight.y; w++)
		{
			vector3 tempPos;

			weight_t* weight = mWeights[w];
			if (!weight)
				return;

			bone_t* bone = (*boneList)[weight->jointIndex];
			if (!bone)
				return;

			tempPos = bone->orientation.inverseVector(vertex->baseNormal);
			normal += tempPos;
		}

		normal.normalise();
		vertex->normal = normal;
	}

	// TODO: Make Vertex list
}

md5model::md5model(const std::string& filename)
{
	parsingFile file(filename);
	if (!file.isReady())
	{
		return;
	}

	unsigned numberOfJointsToParse = 0;

	std::string token(file.getNextToken());
	while (!token.length())
	{
		if (token == "mesh")
		{
			md5mesh* thisMesh = new md5mesh();
			if (!thisMesh)
				return;

			file.skipNextToken(); // {

			// Shader Comes here with the name
			file.skipNextToken(); // "shader"
			file.skipNextToken(); // "shader" param

			file.skipNextToken(); // numverts
			token = file.getNextToken(); // numverts param

			unsigned int numberOfVertices = atoi(token.c_str());
			for (unsigned int i = 0; i < numberOfVertices; i++)
			{
				// vertices are like: vert 0 ( 0.539093 0.217694 ) 0 3
				token = file.getNextToken(); // vert
				if (token != "vert")
					break;

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
				delete thisMesh;
				return;
			}

			unsigned int numberOfTris = atoi(file.getNextToken());
			for (unsigned int i = 0; i < numberOfTris; i++)
			{
				// Tris are described like: tri 0 0 2 1
				token = file.getNextToken();
				if (token != "tri")
					break;

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
				delete thisMesh;
				return;
			}

			unsigned int numberOfWeights = atoi(file.getNextToken());
			for (unsigned int i = 0; i < numberOfWeights; i++)
			{
				// weights are described like that: weight 0 4 0.000000 ( 13.718612 -2.371034 0.006163 )
				token = file.getNextToken();
				if (token != "weight")
					break;

				file.skipNextToken(); // index

				vector2 joint;
				vector3 pos;

				token = file.getNextToken(); // join index
				joint.x = atoi(token.c_str());

				token = file.getNextToken(); // join value
				joint.y = atoi(token.c_str());

				file.skipNextToken(); // (

				// Weight positions
				token = file.getNextToken();
				pos.x = atof(token.c_str());
				token = file.getNextToken();
				pos.y = atof(token.c_str());
				token = file.getNextToken();
				pos.z = atof(token.c_str());

				file.skipNextToken(); // )
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
				if (newBone)
				{
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
		} 
		// if (token == "joints")

		compileBase();
	} 
	// while (!token.is_empty())
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
		if (!mesh)
			continue;
	
		mesh->compileBase(&mBones);
	}
}

void md5model::draw()
{
}

}

