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

#include "bsp46.h"
#include "logger.h"
#include "materialManager.h"

namespace k {
			
void q3BitSet::configure(int i)
{
	mBitsCount = i / 32 + 1;

	if (mBits)
		delete [] mBits;

	mBits = new int[mBitsCount];
	if (!mBits)
	{
		S_LOG_INFO("Failed to allocate bitset array.");
		return;
	}

	memset(mBits, 0, sizeof(int) * mBitsCount);
}

void q3BitSet::set(int i)
{
	if (mBits)
		mBits[i >> 5] |= (1 << (i & 31));
	else
		S_LOG_INFO("Bitset array not allocated.");
}

void q3BitSet::unSet(int i)
{
	if (mBits)
		mBits[i >> 5] &= ~(1 << (i & 31));
	else
		S_LOG_INFO("Bitset array not allocated.");
}

bool q3BitSet::isSet(int i)
{
	if (mBits)
		return (mBits[i >> 5] & (1 << (i & 31)));
	else
	{
		S_LOG_INFO("Bitset array not allocated.");
		return false;
	}
}

void q3BitSet::clear()
{
	if (mBits)
		memset(mBits, 0, sizeof(unsigned int) * mBitsCount);
	else
		S_LOG_INFO("Bitset array not allocated.");
}
			
void q3Bsp::loadQ3Bsp(const std::string& filename)
{
	FILE* mBspFile = fopen(filename.c_str(), "rb");
	if (!mBspFile)
	{
		S_LOG_INFO("Failed to open BSP file " + filename);
		return;
	}

	int freadBytes = 0;

	q3BspHeader bspHeader;
	freadBytes = fread(&bspHeader, sizeof(q3BspHeader), 1, mBspFile);
	bspHeader.version = readLEInt(bspHeader.version);

	if (bspHeader.version != 0x2e || freadBytes <= 0)
	{
		std::stringstream erro;
		erro << "Invalid BSP version(" << bspHeader.version;
		erro << ") expected 46.";

		S_LOG_INFO(erro.str());
		fclose(mBspFile);

		return;
	}

	q3BspLump bspLumps[LUMP_MAX_LUMPS];
	freadBytes = fread(bspLumps, sizeof(q3BspLump) * LUMP_MAX_LUMPS, 1, mBspFile);
	if (freadBytes <= 0)
	{
		S_LOG_INFO("Failed reading from the bsp file.");
		fclose(mBspFile);

		return;
	}

	// Allocate and read Vertex 
	mVertexCount = readLEInt(bspLumps[LUMP_VERTICES].length) / sizeof(q3BspVertex);
	mVertices = (q3BspVertex*) malloc(mVertexCount * sizeof(q3BspVertex));
	if (!mVertices)
	{
		S_LOG_INFO("Failed to allocate vertex array.");
		fclose(mBspFile);

		return;
	}

	fseek(mBspFile, readLEInt(bspLumps[LUMP_VERTICES].offset), SEEK_SET);
	for (int i = 0; i < mVertexCount; i++)
	{
		if (fread(&mVertices[i], sizeof(q3BspVertex), 1, mBspFile) <= 0)
		{
			S_LOG_INFO("Failed to read bsp vertices from file.");
			free(mVertices);
			fclose(mBspFile);

			return;
		}

		float tmp = readLEFloat(mVertices[i].pos[1]);
		mVertices[i].pos[0] = readLEFloat(mVertices[i].pos[0]);
		mVertices[i].pos[1] = readLEFloat(mVertices[i].pos[2]);
		mVertices[i].pos[2] = -tmp;

		mVertices[i].uv[0] = readLEFloat(mVertices[i].uv[0]);
		mVertices[i].uv[1] = readLEFloat(mVertices[i].uv[1]);

		mVertices[i].lmUv[0] = readLEFloat(mVertices[i].lmUv[0]);
		mVertices[i].lmUv[1] = readLEFloat(mVertices[i].lmUv[1]);

		mVertices[i].normal[0] = readLEFloat(mVertices[i].normal[0]);
		mVertices[i].normal[1] = readLEFloat(mVertices[i].normal[1]);
		mVertices[i].normal[2] = readLEFloat(mVertices[i].normal[2]);
	}

	// Allocate Faces
	mFacesCount = readLEInt(bspLumps[LUMP_FACES].length) / sizeof(q3BspFace);
	mFaces = (q3BspFace*) malloc(mFacesCount * sizeof(q3BspFace));
	if (!mFaces)
	{
		S_LOG_INFO("Failed to allocate face array.");
		fclose(mBspFile);

		return;
	}

	// Configure Bitset
	mBitSet.configure(mFacesCount);

	fseek(mBspFile, readLEInt(bspLumps[LUMP_FACES].offset), SEEK_SET);
	for (int i = 0; i < mFacesCount; i++)
	{
		if (fread(&mFaces[i], sizeof(q3BspFace), 1, mBspFile) <= 0)
		{
			S_LOG_INFO("Failed to read bsp faces from file.");
			free(mVertices);
			free(mFaces);
			fclose(mBspFile);

			return;
		}

		mFaces[i].textureId = readLEInt(mFaces[i].textureId);
		mFaces[i].effect = readLEInt(mFaces[i].effect);
		mFaces[i].type = readLEInt(mFaces[i].type);
		mFaces[i].startIndex = readLEInt(mFaces[i].startIndex);
		mFaces[i].numIndices = readLEInt(mFaces[i].numIndices);
		mFaces[i].lmId = readLEInt(mFaces[i].lmId);

		mFaces[i].lmCorner[0] = readLEInt(mFaces[i].lmCorner[0]);
		mFaces[i].lmCorner[1] = readLEInt(mFaces[i].lmCorner[1]);
			
		mFaces[i].lmSize[0] = readLEInt(mFaces[i].lmSize[0]);
		mFaces[i].lmSize[1] = readLEInt(mFaces[i].lmSize[1]);

		mFaces[i].lmPos[0] = readLEFloat(mFaces[i].lmPos[0]);
		mFaces[i].lmPos[1] = readLEFloat(mFaces[i].lmPos[1]);
		mFaces[i].lmPos[2] = readLEFloat(mFaces[i].lmPos[2]);

		mFaces[i].lmUv[0] = readLEFloat(mFaces[i].lmUv[0]);
		mFaces[i].lmUv[1] = readLEFloat(mFaces[i].lmUv[1]);

		mFaces[i].normal[0] = readLEFloat(mFaces[i].normal[0]);
		mFaces[i].normal[1] = readLEFloat(mFaces[i].normal[1]);
		mFaces[i].normal[2] = readLEFloat(mFaces[i].normal[2]);

		mFaces[i].patchSize[0] = readLEInt(mFaces[i].patchSize[0]);
		mFaces[i].patchSize[1] = readLEInt(mFaces[i].patchSize[1]);
	}

	// Allocate Indices 
	mIndicesCount = readLEInt(bspLumps[LUMP_INDICES].length) / 4;
	mIndices = (index_t*) malloc(mIndicesCount * sizeof(index_t));
	if (!mIndices)
	{
		S_LOG_INFO("Failed to allocate index array.");
		fclose(mBspFile);

		return;
	}

	fseek(mBspFile, readLEInt(bspLumps[LUMP_INDICES].offset), SEEK_SET);
	for (int i = 0; i < mIndicesCount; i++)
	{
		int tempIndex;
		if (fread(&tempIndex, sizeof(int), 1, mBspFile) <= 0)
		{
			S_LOG_INFO("Failed to read bsp indices from file.");
			free(mVertices);
			free(mFaces);
			free(mIndices);
			fclose(mBspFile);

			return;
		}

		mIndices[i] = readLEInt(tempIndex);
	}

	// Allocate Textures
	q3BspTexture* bspTextures = NULL;
	int bspTexturesCount = readLEInt(bspLumps[LUMP_TEXTURES].length) / sizeof(q3BspTexture);
	bspTextures = (q3BspTexture*) malloc(bspTexturesCount * sizeof(q3BspTexture));
	if (!bspTextures)
	{
		S_LOG_INFO("Failed to allocate texture array.");
		fclose(mBspFile);

		return;
	}

	fseek(mBspFile, readLEInt(bspLumps[LUMP_TEXTURES].offset), SEEK_SET);
	if (fread(bspTextures, sizeof(q3BspTexture), bspTexturesCount, mBspFile) <= 0)
	{
		S_LOG_INFO("Failed to read bsp textures from file.");
		free(mVertices);
		free(mFaces);
		free(mIndices);
		fclose(mBspFile);

		return;
	}

	mMaterials = (material**) malloc(sizeof(material*) * bspTexturesCount);
	if (!mMaterials)
	{
		S_LOG_INFO("Failed to allocate materials array for bsp textures.");
		free(mVertices);
		free(mFaces);
		free(mIndices);
		fclose(mBspFile);

		return;
	}
	
	// Clean it
	memset(mMaterials, 0, sizeof(material*) * bspTexturesCount);

	// Get Manager
	materialManager* matMgr = &materialManager::getSingleton();
	kAssert(matMgr);

	// Set
	for (int i = 0; i < bspTexturesCount; i++)
		mMaterials[i] = matMgr->getMaterialWithFilename(bspTextures[i].name);

	// Free Textures
	free(bspTextures);

	// Allocate Lightmaps
	mLightmapCount = readLEInt(bspLumps[LUMP_LIGHTMAPS].length) / sizeof(q3BspLightmap128);
	mLightmaps = (q3BspLightmap128*) malloc(mLightmapCount * sizeof(q3BspLightmap128));
	if (!mLightmaps)
	{
		S_LOG_INFO("Failed to allocate lightmaps array.");
		fclose(mBspFile);

		return;
	}

	fseek(mBspFile, readLEInt(bspLumps[LUMP_LIGHTMAPS].offset), SEEK_SET);
	if (fread(mLightmaps, sizeof(q3BspLightmap128), mLightmapCount, mBspFile) <= 0)
	{
		S_LOG_INFO("Failed to read bsp lightmaps from file.");
		free(mVertices);
		free(mFaces);
		free(mIndices);
		free(mLightmaps);
		fclose(mBspFile);

		return;
	}
}

}

