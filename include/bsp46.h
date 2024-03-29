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


/**
 * Version 46 (0x2e) of BSP
 * is the version of quake 3 arena
 * by iD Software.
 */

#ifndef _QUAKE3_BSP_H
#define _QUAKE3_BSP_H

#include "prerequisites.h"
#include "vector2.h"
#include "vector3.h"
#include "material.h"
#include "world.h"
#include "camera.h"
#include "logger.h"
#include "fileParser.h"

namespace k
{
	enum Q3_BSP_FACE_TYPE
	{
		FACETYPE_NONE = 0,
		FACETYPE_POLYGON,
		FACETYPE_PATCH,
		FACETYPE_MESH,
		FACETYPE_BILLBOARD
	};

	enum Q3_BSP_CONTENTS
	{
		CONTENTS_SOLID = 1,
		CONTENTS_LAVA = 8,
		CONTENTS_SLIME = 16,
		CONTENTS_WATER = 32,
		CONTENTS_FOG = 64
	};

	enum Q3_TRACE_TYPE
	{
		TRACE_TYPE_RAY,
		TRACE_TYPE_SPHERE,
		TRACE_TYPE_BOX
	};

	enum Q3_BSP_LUMPS
	{
		LUMP_ENTITIES = 0,
		LUMP_TEXTURES,
		LUMP_PLANES,
		LUMP_NODES,
		LUMP_LEAFS,
		LUMP_LEAF_FACES,
		LUMP_LEAF_BRUSHES,
		LUMP_MODELS,
		LUMP_BRUSHES,
		LUMP_BRUSHES_SIDES,
		LUMP_VERTICES,
		LUMP_INDICES,
		LUMP_SHADERS,
		LUMP_FACES,
		LUMP_LIGHTMAPS,
		LUMP_LIGHTVOLUMES,
		LUMP_VISDATA,
		LUMP_MAX_LUMPS
	};

	typedef struct
	{
		char id[4];
		int version; // should read 0x2e in LE
	} q3BspHeader;

	// Describes a 'Lump' (member of map)
	typedef struct
	{
		int offset;
		int length;
	} q3BspLump;

	#pragma pack(push, 1)
	typedef struct
	{
		vec_t pos[3]; // vertex pos (x,y,z)
		vec_t uv[2]; // s,t uv
		vec_t lmUv[2]; // lightmap uv
		vec_t normal[3]; // normal (x,y,z)
		char color[4]; // rgba
	} q3BspVertex;
	#pragma pack(pop)

	typedef struct
	{
		int textureId;
		int effect;
		int type;

		int startVertIndex;
		int numVertices;

		int startIndex;
		int numIndices;

		int lmId; // lightmap id
		int lmCorner[2];
		int lmSize[2];

		vec_t lmPos[3]; // origin of lightmap
		vec_t lmUv[2][3]; // s,t in space
		vec_t normal[3];

		int patchSize[2]; // bezier patch
	} q3BspFace;

	typedef struct
	{
		char name[64]; 
		int flags;
		int contents;
	} q3BspTexture;

	typedef struct
	{
		unsigned char bits[128 * 128 * 3];
	} q3BspLightmap128;

	typedef struct
	{
		unsigned char bits[256 * 256 * 3];
	} q3BspLightmap256;

	typedef struct
	{
		int plane;
		int children[2];
		vec_t mins[3];
		vec_t maxs[3];
	} q3BspNode;

	typedef struct
	{
		int cluster;
		int area;
		
		int mins[3];
		int maxs[3];

		int firstLeafSurf;
		int numLeafSurf;

		int firstLeafBrush;
		int numLeafBrush;
	} q3BspLeaf;

	typedef struct
	{
		int firstSide;
		int numSides;
		int shaderNum; // for content flags
	} q3BspBrush;

	typedef struct
	{
		int planeIndex;
		int textureIndex;
	} q3BspBrushSide;

	typedef struct
	{
		vec_t normal[3];
		float dist;
	} q3BspPlane;

	typedef struct
	{
		int numOfVis;
		int bytesPerVis;
		unsigned char* bitSet;
	} q3BspVis;

	typedef struct
	{
		// How far along the line
		// we got before collision.
		float fraction;

		// Point of collision
		vector3 end;

		// Normal of the collision plane
		vector3 planeNormal;

		// if the segment starts
		// outside a surface.
		bool startsOut;

		// true if line segment
		// is completely enclosed
		// in a solid volume.
		bool enclosedInSolid;
	} q3BspTrace;

	class DLL_EXPORT bezierPatch
	{
		protected:
			/**
			 * Final Vertices for bezier patch.
			 */
			q3BspVertex* mVertices;
			
			/**
			 * Control points that defines the patch.
			 */
			q3BspVertex mControlPoints[9];

			/**
			 * Indices for drawing.
			 */
			index_t* mIndices;
			index_t* mTrianglesPerRow;
			index_t** mRowIndices;

			unsigned int mSteps; // precision of the patch subdivisions
			unsigned int mNumOfVertices;
			unsigned int mNumOfIndices;
			unsigned int mCurrCP; // the control point we are on the array

		public:
			bezierPatch();
			~bezierPatch();

			/**
			 * Set the number of steps wich is
			 * exactly the number of vertices
			 * in each direction of the patch plane.
			 */
			void configure(unsigned int steps);
			void pushCP(q3BspVertex* cp);

			/**
			 * Compile final vertices, from control points.
			 */
			void compile();

			/**
			 * Return number of vertices on this patch. This
			 * number is equal of the number of control patches - 1 / 2 * steps
			 */
			const unsigned int getVertexCount() const;

			/**
			 * Return the number of indices in the
			 * row.
			 */
			const unsigned int getRowIndicesCount(const short i) const;

			/**
			 * Return a row of indices.
			 */
			const index_t* getIndices(const short i) const;

			/**
			 * Return patch vertices
			 */
			const q3BspVertex* getVertices() const;

			/**
			 * Get patch level
			 */
			const unsigned int getLevel() const
			{ return mSteps; }
	};

	class DLL_EXPORT bezierPatchSet
	{
		protected:
			bezierPatch** mPatches;
			unsigned int mPatchNum;
			unsigned int mPatchIndex;

		public:
			bezierPatchSet()
			{
				mPatches = NULL;
				mPatchIndex = mPatchNum = 0;
			}

			~bezierPatchSet()
			{
				for (unsigned int i = 0; i < mPatchNum; i++)
				{
					if (mPatches[i])
						delete [] mPatches[i];
				}

				delete [] mPatches;
			}

			const bezierPatch* getPatch(const short i) const;
			const unsigned int getPatchesCount() const;

			void configure(const short i);
			void pushPatch(bezierPatch* patch);
	};

	class DLL_EXPORT q3BitSet
	{
		protected:
			int* mBits;
			int mBitsCount;

		public:
			q3BitSet()
			{
				mBits = NULL;
				mBitsCount = 0;
			}

			~q3BitSet()
			{
				if (mBits)
					delete [] mBits;
			}

			void configure(int i);

			void set(int i);
			void unSet(int i);

			bool isSet(int i) const;
			void clear();
	};

	class DLL_EXPORT q3Entity
	{
		protected:
			std::map<std::string, std::string> mKeys;

		public:

			void printKeys() const
			{
				std::map<std::string, std::string>::const_iterator it;
				for (it = mKeys.begin(); it != mKeys.end(); it++)
					S_LOG_INFO("Key: " + it->first + " -> " + it->second);
			}

			void pushKey(std::string name, std::string value)
			{
				mKeys[name] = value;
			}

			std::string getValue(std::string name) const
			{
				std::map<std::string, std::string>::const_iterator it = mKeys.find(name);
				if (it != mKeys.end())
					return it->second;
				else
					return std::string("null");
			}
	};

	class DLL_EXPORT q3Bsp : public world
	{
		protected:
			int mLightmapCount;
			int mTexturesCount;

			int mIndicesCount;
			int mVertexCount;
			int mFacesCount;
			int mNodesCount;
			int mLeafsCount;
			int mLeafFacesCount;
			int mLeafBrushesCount;
			int mPlanesCount;
			int mPatchesCount;
			int mMaterialsCount;

			int mBrushesCount;
			int mBrushSidesCount;

			index_t* mIndices;
			q3BspVertex* mVertices;
			q3BspFace* mFaces;
			q3BspNode* mNodes;
			q3BspLeaf* mLeafs;
			q3BspPlane* mPlanes;
			q3BspBrush* mBrushes;
			q3BspBrushSide* mBrushSides;
			int* mLeafFaces;
			int* mLeafBrushes;

			/**
			 * For collision checking
			 */
			q3BspTrace mTempTrace;
			vector3 mTraceStart, mTraceEnd;
			float mTraceRadius;
			int mTraceType, mTraceFlags;

			/**
			 * Game entities
			 */
			std::list<q3Entity> mEntities;

			/**
			 * Array of bezier patches
			 */
			bezierPatchSet* mPatches;

			/** 
			 * Vis data
			 */
			q3BspVis mBspVisData;

			/*
			 * Only 128x128 lightmaps
			 * are supported on vanilla q3.
			 */
			texture** mLightmaps;

			/**
			 * Instead of saving the textures like quake 3 does
			 * we are going to save the materials in wich
			 * the textures are.
			 */
			material** mMaterials;

			/**
			 * See wich faces we already rendered.
			 */
			q3BitSet mFaceSet;

			/**
			 * Are we drawing lightmaps?
			 */
			bool mDrawLightmaps;

			/**
			 * Vertex Buffer Objects
			 */
			platformVBO mVBOVertex;
			platformVBO mVBOIndex;
			
			/**
			 * Free allocated memory
			 */
			void _clean();

			/**
			 * Parse a single entity
			 */
			void _parseEntity(parsingFile& file);

			/**
			 * Parse entities string
			 */
			void _parseEntities(char* str);

			/**
			 * Correctly loaded.
			 */
			bool mSuccessfullyLoaded;

			/**
			 * Default steps for bezier curves
			 */
			unsigned int mBezierSteps;

		public:
			q3Bsp(unsigned int drawSteps = 6)
			{
				mBezierSteps = drawSteps;

				mLightmapCount = 0;
				mIndicesCount = 0;
				mVertexCount = 0;
				mFacesCount = 0;
				mNodesCount = 0;
				mLeafsCount = 0;
				mLeafFacesCount = 0;
				mPlanesCount = 0;

				mPatchesCount = 0;
				mMaterials = NULL;

				mIndices = NULL;
				mVertices = NULL;
				mFaces = NULL;
				mLightmaps = NULL;
				mNodes = NULL;
				mLeafs = NULL;
				mLeafFaces = NULL;
				mPlanes = NULL;
				mPatches = NULL;

				mBspVisData.numOfVis = 0;
				mBspVisData.bytesPerVis = 0;
				mBspVisData.bitSet = NULL;

				mDrawLightmaps = true;

				// VBOS
			 	mVBOVertex = 0;
				mVBOIndex = 0;

				// Loading
				mSuccessfullyLoaded = false;
			}

			~q3Bsp()
			{
				_clean();
			}

			void setLightmapsDrawing(bool lm)
			{
				mDrawLightmaps = lm;
			}

			bool isDrawingLightmaps() const
			{
				return mDrawLightmaps;
			}
			
			const std::list<q3Entity>& getEntities() const
			{
				return mEntities;
			}

			/**
			 * Collision check. Based on devmaster
			 * article of Nathan Ostgard, iD quake 3
			 * and my own toughs.
			 */
			void checkBrush(const q3BspBrush* brush);
			void checkNode(int index, const float startFraction, const float endFraction,
					const vector3& start, const vector3& end);

			q3BspTrace trace(const vector3& start, const vector3& end, int flags = 0);
			q3BspTrace traceSphere(const vector3& start, const vector3& end, float radius, int flags = 0);

			/**
			 * Rendering awesomeness
			 */

			bool isClusterVisible(int curr, int targ) const;
			int findLeaf(const vector3& viewerPos) const;

			/**
			 * Load a bsp file
			 */
			void loadQ3Bsp(const std::string& filename);

			/**
			 * Did the bsp loaded ok?
			 */
			bool getLoadSuccess();

			/**
			 * Render a single BSP face
			 */
			void renderFace(int i);

			/**
			 * Render a bsp patch
			 */
			void renderPatch(int i);

			/**
			 * Draw world.
			 */
			void draw(const camera* viewer);
	};
}

#endif

