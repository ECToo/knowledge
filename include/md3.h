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

	 Lots of information about the md3 format came from the wikipedia site:
	 http://en.wikipedia.org/wiki/MD3
*/

#ifndef _MD3_H_
#define _MD3_H_

#include "prerequisites.h"
#include "drawable.h"
#include "material.h"
#include "timer.h"

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

typedef struct
{
	index_t indices[3];
} md3Triangle_t;

typedef struct
{
	float uv[2];
} md3TexCoord_t;

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
typedef struct
{
	float pos[3];
	float normal[3];
} md3RealVertex_t;

class DLL_EXPORT md3Surface
{
	public:
		char name[64];

		material* mMaterial;

		// Number of Frames
		int mFrameCount;

		int mVerticesCount;
		md3RealVertex_t* mVertices;

		int mIndicesCount;
		md3Triangle_t* mIndices;

		int mUVCount;
		md3TexCoord_t* mUVs;

		void _clean();
		void draw(short frameNum);
};

class DLL_EXPORT md3model : public drawable3D
{
	protected:
		int mFramesCount;
		md3Frame_t* mFrames;

		int mTagsCount;
		md3Tag_t* mTags;

		int mSurfacesCount;
		md3Surface* mSurfaces;

		int mCurrentFrame;

		void _clean();

	public:
		md3model(const std::string& filename);

		// Clean allocated structures
		~md3model() { _clean(); }

		void setFrame(short i);
		int getFramesCount() const;

		void draw();
		boundingBox getAABoundingBox();
		boundingBox getBoundingBox();
};

}

#endif

