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

#ifndef _MD5_H_
#define _MD5_H_

#include "prerequisites.h"
#include "drawable.h"
#include "material.h"
#include "fileParser.h"
#include "timer.h"

// Bone flags on .md5anim
#define BONE_POS_X (1 << 0)
#define BONE_POS_Y (1 << 1)
#define BONE_POS_Z (1 << 2)
#define BONE_ORI_X (1 << 3)
#define BONE_ORI_Y (1 << 4)
#define BONE_ORI_Z (1 << 5)

namespace k {

typedef struct
{
	int index;
	int parentIndex;
	int mask;
	unsigned int startIndex;
} boneFrame_t;

typedef struct
{
	vector3 pos;
	quaternion orientation;
} baseBone_t;

typedef struct
{
	vector3 mins;
	vector3 maxs;
} bound_t;

typedef struct
{
	unsigned int numFrames;
	unsigned int numBones;
	unsigned int frameRate;
	unsigned int animatedComponents;

	// md5anim hierarchy
	boneFrame_t* hierarchy;

	// bounding boxes on each frame
	bound_t* bounds;

	/**
	 * Base (initial) frame from the model.
	 * Each anim identifier will modify
	 * this baseFrame.
	 */
	baseBone_t* baseFrame;

	/**
	 * This double sized array should contain the number
	 * of frames from the animation and inside each member
	 * a new array of the frame modifiers.
	 */
	vec_t** frames;

	/**
	 * Current frame the animation is.
	 * In case of asking the same frame
	 * for the anim system it prevents
	 * the engine from recalculating it.
	 */
	vec_t currentFrame;

	/**
	 * Last time this anim was feeded.
	 */
	long lastFeedTime;
} anim_t;

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

	// Animated Skeleton
	anim_t* currentAnim;
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

class DLL_EXPORT md5mesh
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
		vec_t* mVertexList;
		vec_t* mUvList;
		vec_t* mNormalList;

		unsigned int mIndexListSize;
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

		void setMaterial(material* mat);
		void setMaterial(const std::string& matName);

		bool isOpaque() const;

		/**
		 * Compile Base Positions
		 */
		void compileBase(std::vector<bone_t*>* boneList);

		/**
		 * Compile Vertices for animations
		 */
		void compileVertices(std::vector<bone_t*>* boneList);

		void draw();
};

class DLL_EXPORT md5model : public drawable3D
{
	private:
		std::map<std::string, anim_t*> mAnimations;
		std::list<md5mesh*> mMeshes;
		std::vector<bone_t*> mBones;

		/**
		 * Used to prepare the base model (static default position)
		 */
		void compileBase();

		/**
		 * Used to prepare the model when animated
		 */
		void compileVertices();

		/**
		 * Retrieve an model animation by its name
		 */
		anim_t* getAnimation(const std::string& name);

		/**
		 * Auto set the animation frames based on frame
		 * time. The default is true.
		 */
		bool mAutoFeedAnims;

	public:
		md5model(const std::string& filename);
		~md5model();

		/**
		 * Clone this model, allocating a new one exactly like this.
		 * Keep in mind that when you clone it, it will receive the same animations,
		 * same bones and same vertices/materials.
		 */ 
		void clone(md5model* cloned);

		/**
		 * Loads an md5anim file and attach it to the list of model animations.
		 * @name The animation name (how you call it from functions).
		 * @filename The md5anim file with FULL path.
		 */
		void attachAnimation(const std::string& filename, const std::string& name);

		/**
		 * Set the model animation.
		 * @name The previously attached animation name.
		 */
		void setAnimation(const std::string& name);

		/**
		 * Define if the model animations
		 * are set automatically or not.
		 * The default setting is true.
		 */
		void setAutoFeed(bool feed);

		/**
		 * Tells if the model auto update
		 * its anims.
		 */
		bool getAutoFeed();

		/**
		 * Feed animations ;)
		 */
		void feedAnims();

		/**
		 * Set the model desired frame.
		 * If the specified frame is greater than the number
		 * of the frames in the animation it will result in 
		 * frame = numberOfFrames % desiredFrame
		 */
		void setAnimationFrame(unsigned int frameNum);

		/**
		 * Get one md5mesh from an index.
		 */
		md5mesh* getMesh(unsigned int index);

		/**
		 * Overloaded
		 */
		void draw();

		/**
		 * Render sorting
		 */
		bool isOpaque() const;

		/**
		 * Return model Bounding box, rotated
		 * like model orientation.
		 */
		boundingBox getAABoundingBox() const;

		/**
		 * Return model bounding box, untransformed
		 */
		boundingBox getBoundingBox() const;
};

}

#endif

