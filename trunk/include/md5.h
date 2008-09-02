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
	vector2 uv;
	vector2 weight;

	vector3 renderPos;
	vector3 renderNormal;

	// normal unasigned to a bone
	vector3 normal;

	// For the bind position, prevent recalculation
	vector3 basePos;
	vector3 baseNormal;
} vert_t;

typedef struct
{
	int index[3];
} triangle_t;

class md5mesh
{
	private:
		// Vertex Data
		std::vector<vert_t*> mVertexes;

		// Triangles
		std::vector<triangle_t*> mTriangles;

		// Weight
		std::vector<weight_t*> mWeights;

		// knowledge material
		material* mMaterial;

		// Used to glDrawelements
		vec_t* mNormalList;
		unsigned int* mIndexList;

	public:
		md5mesh();
		~md5mesh();

		void pushVertex(const vector2& uv, const vector2& weight);
		void pushTriangle(const vector3& tri);
		void pushWeight(const vector2& joint, const vector3& pos);

		/**
		 * Compile Base Positions
		 */
		void compileBase(std::vector<bone_t*>* boneList);
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

