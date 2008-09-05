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

#ifndef _MD5_H_
#define _MD5_H_

#include "prerequisites.h"
#include "drawable.h"
#include "material.h"
#include "fileAccess.h"

namespace k {

typedef struct
{
	int index;
	int parentIndex;

	// To Bind
	vector3 pos;
	quaternion orientation;

	// Base
	vector3 basePos;
	quaternion baseOrientation;
} bone_t;

typedef struct
{
	int jointIndex;

	vec_t value;
	vector3 pos;
} weight_t;

typedef struct
{
	vec_t uv[2];
	vector2 weight;

	vec_t renderPos[3];
	vector3 renderNormal;

	// normal unasigned to a bone
	vector3 normal;

	// For the bind position, prevent recalculation
	vector3 basePos;
	vector3 baseNormal;
} vert_t;

typedef struct
{
	index_t index[3];
} triangle_t;

class md5mesh
{
	private:
		// Vertex Data
		unsigned int mVIndex;
		unsigned int mVCount;
		vert_t* mVertices;

		// Triangles
		unsigned int mTIndex;
		unsigned int mTCount;
		triangle_t* mTriangles;

		// Weight
		unsigned int mWIndex;
		unsigned int mWCount;
		weight_t* mWeights;

		// knowledge material
		material* mMaterial;

		// Used to glDrawelements
		vec_t* mNormalList;
		index_t* mIndexList;

	public:
		md5mesh();
		~md5mesh();

		void prepareVertices(unsigned int size);
		void prepareTriangles(unsigned int size);
		void prepareWeights(unsigned int size);
		void pushVertex(const vector2& uv, const vector2& weight);
		void pushTriangle(const vector3& tri);
		void pushWeight(const vector2& joint, const vector3& pos);

		/**
		 * Compile Base Positions
		 */
		void compileBase(std::vector<bone_t*>* boneList);
		void draw();
};

class md5model : public drawable3D
{
	private:
		std::list<md5mesh*> mMeshes;
		std::vector<bone_t*> mBones;

		/**
		 * Used to prepare the base model (static default position)
		 */
		void compileBase();

	public:
		md5model(const std::string& filename);
		md5model(const md5model& source);

		/**
		 * Dont use this, will fail.
		 */
		md5model();
		~md5model();

		void draw();
};

}

#endif

