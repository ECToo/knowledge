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

	/**
	 * Describes a 'Lump' (member of map)
	 */
	typedef struct
	{
		int offset;
		int length;
	} q3BspLump;

	typedef struct
	{
		vec_t pos[3]; // vertex pos (x,y,z)
		vec_t uv[2]; // s,t uv
		vec_t lmUv[2]; // lightmap uv
		vec_t normal[3]; // normal (x,y,z)
		char color[4]; // rgba
	} q3BspVertex;

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

			bool isSet(int i);
			void clear();
	};

	class DLL_EXPORT q3Bsp : public world
	{
		private:
			int mLightmapCount;
			int mTexturesCount;

			int mIndicesCount;
			int mVertexCount;
			int mFacesCount;

			index_t* mIndices;
			q3BspVertex* mVertices;
			q3BspFace* mFaces;

			// Only 128x128 lightmaps
			// are supported on vanilla q3.
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
			q3BitSet mBitSet;

			/**
			 * Are we drawing lightmaps?
			 */
			bool mDrawLightmaps;

		public:
			q3Bsp()
			{
				mLightmapCount = 0;
				mIndicesCount = 0;
				mVertexCount = 0;
				mFacesCount = 0;

				mMaterials = NULL;

				mIndices = NULL;
				mVertices = NULL;
				mFaces = NULL;
				mLightmaps = NULL;

				mDrawLightmaps = true;
			}

			~q3Bsp()
			{
				if (mIndices)
					free(mIndices);

				if (mVertices)
					free(mVertices);

				if (mFaces)
					free(mFaces);
			}

			void setLightmapsDrawing(bool lm)
			{
				mDrawLightmaps = lm;
			}

			bool isDrawingLightmaps()
			{
				return mDrawLightmaps;
			}

			void loadQ3Bsp(const std::string& filename);
			void renderFace(int i);
			void draw(const vector3& viewerPos);
	};
}

#endif

