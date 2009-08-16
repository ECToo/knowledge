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

	 Lots of information about the md3 format came from the wikipedia site:
	 http://en.wikipedia.org/wiki/MD3
*/

#ifndef _MD3_H_
#define _MD3_H_

#include "prerequisites.h"
#include "drawable.h"
#include "material.h"
#include "timer.h"
#include "ray.h"

namespace k {

typedef struct
{
	char ident[4]; // must be IDP3
	int version; // should be 15
	char name[64];
	int flags;
	int numFrames;
	int numTags;
	int numSurfaces;
	int numSkins;
	int offsetFrames;
	int offsetTags;
	int offsetSurfaces;
	int offsetEOF;
} md3Header_t;

typedef struct
{
	float mins[3];
	float maxs[3];
	float origin[3];
	float radius;
	char name[16];
} md3Frame_t;

typedef struct
{
	char name[64];
	float origin[3];
	float axis[3][3];
} md3Tag_t;

class md3Tag
{
	public:
		std::string mName;
		vector3 mOrigin;
		matrix3 mRotation;

		// Axis Angle Notation
		vec_t mAA_Angle;
		vector3 mAA_Axis;

		inline md3Tag operator = (const md3Tag_t& tag)
		{
			mName = tag.name;

			mOrigin.x = readLEFloat(tag.origin[0]);
			mOrigin.y = readLEFloat(tag.origin[2]);
			mOrigin.z = -readLEFloat(tag.origin[1]);

			mRotation.m[0][0] = readLEFloat(tag.axis[0][0]);
			mRotation.m[0][1] = readLEFloat(tag.axis[0][1]);
			mRotation.m[0][2] = readLEFloat(tag.axis[0][2]);

			mRotation.m[1][0] = readLEFloat(tag.axis[1][0]);
			mRotation.m[1][1] = readLEFloat(tag.axis[1][1]);
			mRotation.m[1][2] = readLEFloat(tag.axis[1][2]);

			mRotation.m[2][0] = readLEFloat(tag.axis[2][0]);
			mRotation.m[2][1] = readLEFloat(tag.axis[2][1]);
			mRotation.m[2][2] = readLEFloat(tag.axis[2][2]);

			mRotation.toAxisAngle(mAA_Angle, mAA_Axis);

			return *this;
		}
};

typedef struct
{
	char ident[4]; // should be IDP3
	char name[64];
	int flags;
	int numFrames;
	int numShaders;
	int numVerts;
	int numTris;
	int offsetTriangles;
	int offsetShaders;
	int offsetUV;
	int offsetVertex;
	int offsetEnd;
} md3Surface_t;

typedef struct
{
	char name[64];
	int index;
} md3Shader_t;

// Indices on wii are u16 so better use md3Triangle_t with unsigned int and then convert;)
typedef struct 
{
	int indices[3];
} md3Triangle_t;

class md3Triangle
{
	public:
		index_t indices[3];

		inline md3Triangle operator = (const md3Triangle_t& tri)
		{
			int nindices[3];
			nindices[0] = readLEInt(tri.indices[0]);
			nindices[1] = readLEInt(tri.indices[1]);
			nindices[2] = readLEInt(tri.indices[2]);

			indices[0] = (index_t)nindices[0];
			indices[1] = (index_t)nindices[1];
			indices[2] = (index_t)nindices[2];

			return *this;
		}
};

class md3TexCoord_t
{
	public:
		vector2 uv;

		inline md3TexCoord_t operator = (const md3TexCoord_t& u)
		{
			uv.x = readLEFloat(u.uv.x);
			uv.y = readLEFloat(u.uv.y);

			return *this;
		}
};

typedef struct
{
	short coord[3]; // we need to multiply to 1.0/64 to get the original values

	/** 
	 * To decode md3 normals we do:
	 *
	 * lat <- zenith * (2 * pi ) / 255
	 * lng <- azimuth * (2 * pi) / 255
	 * x <- cos ( lat ) * sin ( lng )
	 * y <- sin ( lat ) * sin ( lng )
	 * z <- cos ( lng )
	 */
	char normal[2];

} md3Vertex_t;

/**
 * This should be the real vertex 
 * used by the engine, from md3Vertex
 * and md3TexCoord.
 */
class md3RealVertex
{
	/*
	float pos[3];
	float normal[3];
	*/

	public:
		vector3 pos;
		vector3 normal;

		inline md3RealVertex operator = (const md3Vertex_t& t)
		{
			const float vertexMultiplier = 0.015625f;
	 		const float lat = t.normal[1] * (2.0f * M_PI) / 255.0f;
	 		const float lng = t.normal[0] * (2.0f * M_PI) / 255.0f;

			pos.x = readLEShort(t.coord[0]) * vertexMultiplier;
			pos.y = readLEShort(t.coord[2]) * vertexMultiplier;
			pos.z = -readLEShort(t.coord[1]) * vertexMultiplier;

			normal.x = cos(lat) * sin(lng);
			normal.y = cos(lng);
			normal.z = -sin(lat) * sin(lng);

			return *this;
		}
};

/**
 * This structure hold an "animation"
 * definition for the md3 model. 
 */
typedef struct
{
	std::string name;
	unsigned int firstFrame;
	unsigned int numFrames;
	unsigned int loopingFrames;
	unsigned int framesPerSecond;
} md3Animation_t;

class DLL_EXPORT md3Surface
{
	protected:
		/**
		 * Surface name
		 */
		std::string mName;

		//
		material* mMaterial;

		// Number of Frames
		unsigned int mFrameCount;

		// Number of Vertices
		unsigned int mVerticesCount;

		// Vertex Arrays
		md3RealVertex* mVertices;

		// Number of Indices
		unsigned int mIndicesCount;

		// Indices array
		md3Triangle* mIndices;

		// Number of Uv's
		unsigned int mUVCount;

		// UV's array
		md3TexCoord_t* mUVs;

		// Lower Y, for ajudsting
		float mLowerY;

		// Are We drawing normals?
		bool mDrawNormals;

	public:
		md3Surface();
		~md3Surface();

		/**
		 * Set Surface name
		 */
		void setName(const std::string& name)
		{
			mName = name;
		}

		/**
		 * Return surface name.
		 */
		const std::string& getName() const
		{
			return mName;
		}

		/**
		 * Adjust vertices, because we want the model
		 * to have its bottom part on its lower (Y) vertex.
		 */
		void adjustVertices();

		/**
		 * Set if normals are drawn or not.
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
		 * Allocate indices
		 */
		bool allocateIndices(unsigned int i);

		/**
		 * Allocate vertices
		 */
		bool allocateVertices(unsigned int i);

		/**
		 * Allocate uvs 
		 */
		bool allocateUVs(unsigned int i);

		/**
		 * In case we want to replace index count.
		 */
		void setIndexCount(unsigned int c)
		{
			mIndicesCount = c;
		}

		/**
		 * In case we want to replace vertex count.
		 */
		void setVertexCount(unsigned int c)
		{
			mVerticesCount = c;
		}

		/**
		 * In case we want to replace uv count.
		 */
		void setUVCount(unsigned int c)
		{
			mUVCount = c;
		}

		/**
		 * In case we want to replace frame count.
		 */
		void setFrameCount(unsigned int c)
		{
			mFrameCount = c;
		}

		/**
		 * Set index. Set the surface index[index]
		 * to tri values.
		 *
		 * @index Index to the destination index.
		 * @tri Values to copy from.
		 */
		void setIndex(unsigned int index, const md3Triangle_t& tri);

		/**
		 * Set Vertex. Set the surface vertex[index]
		 * to v values.
		 *
		 * @index Index to the destination vertex.
		 * @v Values to copy from.
		 */
		void setVertex(unsigned int index, const md3Vertex_t& v);

		/**
		 * Set UV. Set the surface uv[index]
		 * to uv values.
		 *
		 * @index Index to the destination uv.
		 * @uv Values to copy from.
		 */
		void setUV(unsigned int index, const md3TexCoord_t& uv);

		/**
		 * Get number of indices
		 */
		unsigned int getIndicesCount() const
		{
			return mIndicesCount;
		}

		/**
		 * Get number of frames 
		 */
		unsigned int getFrameCount() const
		{
			return mFrameCount;
		}

		/**
		 * Get number of Vertices 
		 */
		unsigned int getVertexCount() const
		{
			return mVerticesCount;
		}

		/**
		 * Get number of uv's 
		 */
		unsigned int getUVCount() const
		{
			return mUVCount;
		}

		/**
		 * Is this model opaque? We need this
		 * so we can render it right (putting opaque on front) in renderer.
		 */
		bool isOpaque() const;

		/**
		 * Draw the model.
		 * @frameNum The number of frame to draw.
		 */
		void draw(short frameNum);

		/**
		 * Set surface material.
		 */
		void setMaterial(material* mat);

		/**
		 * Set surface material.
		 *	This function will call material manager to retrieve the material pointer.
		 */
		void setMaterial(const std::string& matName);

		/**
		 * Trace against this meshe triangles.
		 */
		void trace(ray& traceRay, short frameNum) const;
};

class DLL_EXPORT md3model : public drawable3D
{
	protected:
		unsigned int mFramesCount;
		md3Frame_t* mFrames;

		unsigned int mTagsCount;
		md3Tag* mTags;

		unsigned int mSurfacesCount;
		md3Surface* mSurfaces;

		/**
		 * Hash map of md3 "animations"
		 */
		std::map<int, md3Animation_t*> mAnimations;

		/**
		 * Active md3 animation.
		 */
		md3Animation_t* mActiveAnimation;

		/**
		 * Attached models to tags.
		 * The key of the map is the tag name.
		 * If the tag is not found, model will not be attached.
		 */
		std::vector<md3model*> mAttach;
		
		/**
		 * Attach Parent
		 */
		md3model* mAttachParent;

		/**
		 * The tag we are attached to.
		 */
		std::string mAttachTag;

		/**
		 * Are we drawing the tags?
		 */
		bool mDrawTags;

		/**
		 * Auto set the animation frames based on frame
		 * time. The default is true.
		 */
		bool mAutoFeedAnims;

		/**
		 * Current animation frame.
		 */
		vec_t mCurrentAnimFrame;

		/**
		 * Last time animation was updated
		 */
		long mLastFeedTime;

	public:
		md3model(const std::string& filename, bool adjustVertices = false);

		/**
		 * Clean allocated structures
		 */
		~md3model();

		/**
		 * Set to wich tag this model is attached to.
		 */
		void setAttachTag(const std::string& tag, md3model* parent)
		{
			mAttachParent = parent;
			mAttachTag = tag;
		}

		/**
		 * Set if submeshes normals are drawn or not.
		 */
		void setDrawNormals(bool draw);

		/**
		 * Get surface (mesh) index
		 */
		md3Surface* getSurface(unsigned int index);

		/**
		 * Get the number of surfaces (meshes)
		 */
		const unsigned int getSurfaceCount() const;

		/**
		 * Define if the model animations
		 * are set automatically or not.
		 * The default setting is true.
		 */
		void setAutoFeed(bool feed)
		{
			mAutoFeedAnims = feed;
		}

		/**
		 * Tells if the model auto update
		 * its anims.
		 */
		bool getAutoFeed() const
		{
			return mAutoFeedAnims;
		}

		/**
		 * Feed animations ;)
		 */
		void feedAnims();

		/**
		 * Get Model tag.
		 * @name The Tag name.
		 * @return A pair with the tag index and a pointer to the tag
		 */
		md3Tag* getTag(const std::string& name);

		/**
		 * Set Drawing frame.
		 */
		void setFrame(unsigned short i);

		/**
		 * Return the number of frames
		 */
		int getFramesCount() const;

		/**
		 * Attach another md3model to a tag of this one.
		 */
		void attach(md3model* model, const std::string& tag);

		/**
		 * Draw frame when this is attached to another model tag.
		 */
		void attachDraw();

		/**
		 * Create an md3Animation_t. If the animation already
		 * exists, it returns the current animation.
		 *
		 * @name The name of the new animation.
		 * @return The animation with name @name or NULL on failure.
		 */
		md3Animation_t* createAnimation(const std::string& name);

		/**
		 * Insert a md3Animation into model animation list.
		 * You wont need to free your pointer after inserting
		 * it, the md3model class will take care of it.
		 *
		 * @anim Pointer to md3Animation.
		 * @return True on success, false on failure.
		 */
		bool insertAnimation(md3Animation_t* anim);

		/**
		 * Set model active animation. The animation
		 * must be pre-registered with insertAnimation()
		 */
		void setAnimation(const std::string& name);

		/**
		 * Trace against this model. If this ray collides
		 * with some triangle the ray mFraction (getFraction()) 
		 * will be different than 0
		 */
		void trace(ray& traceRay);

		bool isOpaque() const;
		void draw();
		boundingBox getAABoundingBox() const;
		boundingBox getBoundingBox() const;
};

}

#endif

