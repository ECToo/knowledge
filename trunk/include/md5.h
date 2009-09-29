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
	index_t index[3];
} triangle_t;

/**
 * \brief Handle submeshes (md5mesh) of md5model.
 * This class is responsible for handling and controlling
 * submeshes of the md5model class.
 */
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

		// Are we drawing the normals?
		bool mDrawNormals;

		// Used to glDrawelements
		vec_t* mVertexList;
		vec_t* mUvList;
		vec_t* mNormalList;

		unsigned int mIndexListSize;
		index_t* mIndexList;

		// Non animated meshes, have a default boundingBox
		boundingBox mAABB;

	public:
		/**
		 * Constructor.
		 */
		md5mesh();

		/**
		 * Destructor, deallocate used data.
		 */
		~md5mesh();

		/**
		 * Allocate vertex data.
		 * @param size The number of vertexes to allocate.
		 */
		void prepareVertices(unsigned int size);

		/**
		 * Allocate triangle data.
		 * @param size The number of triangles to allocate.
		 */
		void prepareTriangles(unsigned int size);

		/**
		 * Allocate weight data.
		 * @param size The number of weights to allocate.
		 */
		void prepareWeights(unsigned int size);

		/**
		 * Push a vertex to vertexes list
		 */
		void pushVertex(const vector2& uv, const vector2& weight);

		/**
		 * Push a triangle to triangles list
		 */
		void pushTriangle(const vector3& tri);

		/**
		 * Push a weight to weights list
		 */
		void pushWeight(const vector2& joint, const vector3& pos);

		/**
		 * Set this surface material.
		 * @param[in] mat A valid pointer to a material.
		 */
		void setMaterial(material* mat);

		/**
		 * Set this surface material.
		 * @param[in] matName The material name.
		 */
		void setMaterial(const std::string& matName);

		/**
		 * Set normal drawing
		 */
		void setDrawNormals(bool draw)
		{
			mDrawNormals = draw;
		}

		/**
		 * Are we drawing normals?
		 */
		bool getDrawNormals() const
		{
			return mDrawNormals;
		}

		/**
		 * Is this object opaque?
		 */
		bool isOpaque() const;

		/**
		 * Return mesh bounding box
		 */
		const boundingBox& getAABoundingBox();

		/**
		 * Compile Base Positions
		 */
		void compileBase(std::vector<bone_t*>* boneList);

		/**
		 * Compile Vertices for animations
		 */
		void compileVertices(std::vector<bone_t*>* boneList);


		/**
		 * Draw this surface
		 */
		void draw();
};

/**
 * \brief Handle md5 models.
 */
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
		/**
		 * Constructor. The model will be allocated from the full path (from the resourceManager root).
		 * @param[in] filename The model full path.
		 */
		md5model(const std::string& filename);

		/**
		 * Destructor, free allocated data.
		 */
		~md5model();

		/**
		 * Ask submeshes to draw (or not) their normals.
		 */
		void setDrawNormals(bool draw);

		/**
		 * Clone this model, allocating a new one exactly like this.
		 * Keep in mind that when you clone it, it will receive the same animations,
		 * same bones and same vertices/materials.
		 */ 
		void clone(md5model* cloned);

		/**
		 * Loads an md5anim file and attach it to the list of model animations.
		 * @mname The animation name (how you call it from functions).
		 * @filename The md5anim file with FULL path.
		 */
		void attachAnimation(const std::string& filename, const std::string& mname);

		/**
		 * Set the model animation.
		 * @aname The previously attached animation name.
		 */
		void setAnimation(const std::string& aname);

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

