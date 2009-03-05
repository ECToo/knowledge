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
#include "textureManager.h"
#include "root.h"
#include "resourceManager.h"

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
			
static inline texture* getNewTexture(const std::string& filename)
{
	resourceManager* rscMgr = &resourceManager::getSingleton();
	textureManager* texMgr = &textureManager::getSingleton();
	kAssert(texMgr);
	kAssert(rscMgr);

	bool isJpeg = true;

	std::string fullPath;
	if (rscMgr)
	{
		fullPath = rscMgr->getRoot() + filename;
	}
	else
	{
		fullPath = filename;
	}

	std::string tmp;
	tmp = fullPath + ".jpg";

	FILE* texFile = fopen(std::string(fullPath + ".jpg").c_str(), "rb");
	if (!texFile)
	{
		texFile = fopen(std::string(fullPath + ".tga").c_str(), "rb");
		isJpeg = false;
	}

	if (texFile)
	{
		fclose(texFile);
		texture* newTexture = NULL;

		if (isJpeg)
		{
			texMgr->allocateTextureData(filename + ".jpg");
			newTexture = texMgr->getTexture(filename + ".jpg");
		}
		else
		{
			texMgr->allocateTextureData(filename + ".tga");
			newTexture = texMgr->getTexture(filename + ".tga");
		}

		return newTexture;
	}
		
	S_LOG_INFO("Texture " + filename + " not found.");
	return NULL;
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
	freadBytes = fread(bspLumps, sizeof(q3BspLump), LUMP_MAX_LUMPS, mBspFile);
	if (freadBytes <= 0)
	{
		S_LOG_INFO("Failed reading from the bsp file.");
		fclose(mBspFile);

		return;
	}

	// Allocate and read Vertex 
	mVertexCount = readLEInt(bspLumps[LUMP_VERTICES].length) / sizeof(q3BspVertex);
	mVertices = (q3BspVertex*) memalign(32, mVertexCount * sizeof(q3BspVertex));
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
	mFaces = (q3BspFace*) memalign(32, mFacesCount * sizeof(q3BspFace));
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

		mFaces[i].startVertIndex = readLEInt(mFaces[i].startVertIndex);
		mFaces[i].numVertices = readLEInt(mFaces[i].numVertices);

		mFaces[i].lmId = readLEInt(mFaces[i].lmId);

		mFaces[i].lmCorner[0] = readLEInt(mFaces[i].lmCorner[0]);
		mFaces[i].lmCorner[1] = readLEInt(mFaces[i].lmCorner[1]);
			
		mFaces[i].lmSize[0] = readLEInt(mFaces[i].lmSize[0]);
		mFaces[i].lmSize[1] = readLEInt(mFaces[i].lmSize[1]);

		mFaces[i].lmPos[0] = readLEFloat(mFaces[i].lmPos[0]);
		mFaces[i].lmPos[1] = readLEFloat(mFaces[i].lmPos[1]);
		mFaces[i].lmPos[2] = readLEFloat(mFaces[i].lmPos[2]);

		mFaces[i].lmUv[0][0] = readLEFloat(mFaces[i].lmUv[0][0]);
		mFaces[i].lmUv[0][1] = readLEFloat(mFaces[i].lmUv[0][1]);
		mFaces[i].lmUv[0][2] = readLEFloat(mFaces[i].lmUv[0][2]);

		mFaces[i].lmUv[1][0] = readLEFloat(mFaces[i].lmUv[1][0]);
		mFaces[i].lmUv[1][1] = readLEFloat(mFaces[i].lmUv[1][1]);
		mFaces[i].lmUv[1][2] = readLEFloat(mFaces[i].lmUv[1][2]);

		mFaces[i].normal[0] = readLEFloat(mFaces[i].normal[0]);
		mFaces[i].normal[1] = readLEFloat(mFaces[i].normal[1]);
		mFaces[i].normal[2] = readLEFloat(mFaces[i].normal[2]);

		mFaces[i].patchSize[0] = readLEInt(mFaces[i].patchSize[0]);
		mFaces[i].patchSize[1] = readLEInt(mFaces[i].patchSize[1]);
	}

	// Allocate Indices 
	mIndicesCount = readLEInt(bspLumps[LUMP_INDICES].length) / sizeof(int);
	mIndices = (index_t*) memalign(32, mIndicesCount * sizeof(index_t));
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
	mTexturesCount = readLEInt(bspLumps[LUMP_TEXTURES].length) / sizeof(q3BspTexture);
	bspTextures = (q3BspTexture*) memalign(32, mTexturesCount * sizeof(q3BspTexture));
	if (!bspTextures)
	{
		S_LOG_INFO("Failed to allocate texture array.");
		fclose(mBspFile);

		return;
	}

	fseek(mBspFile, readLEInt(bspLumps[LUMP_TEXTURES].offset), SEEK_SET);
	if (fread(bspTextures, sizeof(q3BspTexture), mTexturesCount, mBspFile) <= 0)
	{
		S_LOG_INFO("Failed to read bsp textures from file.");
		free(mVertices);
		free(mFaces);
		free(mIndices);
		fclose(mBspFile);

		return;
	}

	mMaterials = (material**) memalign(32, sizeof(material*) * mTexturesCount);
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
	memset(mMaterials, 0, sizeof(material*) * mTexturesCount);

	// Get Manager
	materialManager* matMgr = &materialManager::getSingleton();
	kAssert(matMgr);

	// Set
	for (int i = 0; i < mTexturesCount; i++)
	{
		mMaterials[i] = matMgr->getMaterialWithFilename(bspTextures[i].name);

		// Material not found, we are going to create
		// a simple material with this texture on it only.
		if (!mMaterials[i])
		{
			texture* newTexture = getNewTexture(std::string(bspTextures[i].name));
			if (newTexture)
			{
				mMaterials[i] = matMgr->createMaterial(bspTextures[i].name);
				mMaterials[i]->setSingleTexture(newTexture);
			}
		}
	}

	// Free Textures
	free(bspTextures);

	// Allocate Lightmaps
	q3BspLightmap128* bspLightmaps = NULL;
	mLightmapCount = readLEInt(bspLumps[LUMP_LIGHTMAPS].length) / sizeof(q3BspLightmap128);
	bspLightmaps = (q3BspLightmap128*) memalign(32, mLightmapCount * sizeof(q3BspLightmap128));
	if (!bspLightmaps)
	{
		S_LOG_INFO("Failed to allocate lightmaps array.");
		free(mVertices);
		free(mFaces);
		free(mIndices);
		free(mMaterials);
		fclose(mBspFile);

		return;
	}

	fseek(mBspFile, readLEInt(bspLumps[LUMP_LIGHTMAPS].offset), SEEK_SET);
	if (fread(bspLightmaps, sizeof(q3BspLightmap128), mLightmapCount, mBspFile) <= 0)
	{
		S_LOG_INFO("Failed to read bsp lightmaps from file.");
		free(mVertices);
		free(mFaces);
		free(mIndices);
		free(bspLightmaps);
		free(mMaterials);
		fclose(mBspFile);

		return;
	}

	mLightmaps = (texture**) memalign(32, sizeof(texture*) * mLightmapCount);
	if (!mLightmaps)
	{
		S_LOG_INFO("Failed to allocate texture array for bsp lightmaps.");
		free(mVertices);
		free(mFaces);
		free(mIndices);
		free(mMaterials);
		free(bspLightmaps);
		fclose(mBspFile);

		return;
	}
	
	// Clean it
	memset(mLightmaps, 0, sizeof(texture*) * mLightmapCount);
	for (int i = 0; i < mLightmapCount; i++)
	{
		int flags = FLAG_RGB | FLAG_CLAMP_S | FLAG_CLAMP_T | FLAG_CLAMP_R;
		mLightmaps[i] = createRawTexture(bspLightmaps[i].bits, 128, 128, flags);
	}

	// Free read lightmaps
	free(bspLightmaps);

	// Nodes
	mNodesCount = readLEInt(bspLumps[LUMP_NODES].length) / sizeof(q3BspNode);
	mNodes = (q3BspNode*) memalign(32, mNodesCount * sizeof(q3BspNode));
	if (!mNodes)
	{
		S_LOG_INFO("Failed to allocate nodes array for bsp.");
		free(mVertices);
		free(mFaces);
		free(mIndices);
		free(mMaterials);
		free(bspLightmaps);
		fclose(mBspFile);

		return;
	}
	memset(mNodes, 0, sizeof(q3BspNode) * mNodesCount);

	fseek(mBspFile, readLEInt(bspLumps[LUMP_NODES].offset), SEEK_SET);
	for (int i = 0; i < mNodesCount; i++)
	{
		if (fread(&mNodes[i], sizeof(q3BspNode), 1, mBspFile) <= 0)
		{
			S_LOG_INFO("Failed to read node from bsp file.");

			free(mVertices);
			free(mFaces);
			free(mIndices);
			free(mMaterials);
			free(mNodes);
			free(bspLightmaps);
			fclose(mBspFile);

			return;
		}

		mNodes[i].plane = readLEInt(mNodes[i].plane);

		mNodes[i].children[0] = readLEInt(mNodes[i].children[0]);
		mNodes[i].children[1] = readLEInt(mNodes[i].children[1]);

		mNodes[i].mins[0] = readLEFloat(mNodes[i].mins[0]);
		mNodes[i].mins[1] = readLEFloat(mNodes[i].mins[1]);
		mNodes[i].mins[2] = readLEFloat(mNodes[i].mins[2]);

		mNodes[i].maxs[0] = readLEFloat(mNodes[i].maxs[0]);
		mNodes[i].maxs[1] = readLEFloat(mNodes[i].maxs[1]);
		mNodes[i].maxs[2] = readLEFloat(mNodes[i].maxs[2]);
	}

	// Leafs
	mLeafsCount = readLEInt(bspLumps[LUMP_LEAFS].length) / sizeof(q3BspLeaf);
	mLeafs = (q3BspLeaf*) memalign(32, mLeafsCount * sizeof(q3BspLeaf));
	if (!mLeafs)
	{
		S_LOG_INFO("Failed to allocate leafs array for bsp.");
		free(mVertices);
		free(mFaces);
		free(mIndices);
		free(mMaterials);
		free(bspLightmaps);
		fclose(mBspFile);

		return;
	}
	memset(mLeafs, 0, sizeof(q3BspLeaf) * mLeafsCount);

	fseek(mBspFile, readLEInt(bspLumps[LUMP_LEAFS].offset), SEEK_SET);
	for (int i = 0; i < mLeafsCount; i++)
	{
		if (fread(&mLeafs[i], sizeof(q3BspLeaf), 1, mBspFile) <= 0)
		{
			S_LOG_INFO("Failed to read leaf from bsp file.");

			free(mVertices);
			free(mFaces);
			free(mIndices);
			free(mMaterials);
			free(mNodes);
			free(mLeafs);
			free(bspLightmaps);
			fclose(mBspFile);

			return;
		}

		mLeafs[i].cluster = readLEInt(mLeafs[i].cluster);
		mLeafs[i].area = readLEInt(mLeafs[i].area);

		mLeafs[i].firstLeafSurf = readLEInt(mLeafs[i].firstLeafSurf);
		mLeafs[i].numLeafSurf = readLEInt(mLeafs[i].numLeafSurf);

		mLeafs[i].firstLeafBrush = readLEInt(mLeafs[i].firstLeafBrush);
		mLeafs[i].numLeafBrush = readLEInt(mLeafs[i].numLeafBrush);
		
		int temp = readLEInt(mLeafs[i].mins[1]);
		mLeafs[i].mins[0] = readLEInt(mLeafs[i].mins[0]);
		mLeafs[i].mins[1] = readLEInt(mLeafs[i].mins[2]);
		mLeafs[i].mins[2] = -temp;

		temp = readLEInt(mLeafs[i].maxs[1]);
		mLeafs[i].maxs[0] = readLEInt(mLeafs[i].maxs[0]);
		mLeafs[i].maxs[1] = readLEInt(mLeafs[i].maxs[2]);
		mLeafs[i].maxs[2] = -temp;
	}
			
	// Leaf Faces
	mLeafFacesCount = readLEInt(bspLumps[LUMP_LEAF_FACES].length) / sizeof(int);
	mLeafFaces = (int*) memalign(32, sizeof(int) * mLeafFacesCount);
	if (!mLeafFaces)
	{
		S_LOG_INFO("Failed to allocate leafs faces array for bsp.");
		free(mVertices);
		free(mFaces);
		free(mIndices);
		free(mMaterials);
		free(mLeafs);
		free(bspLightmaps);
		fclose(mBspFile);

		return;
	}
	memset(mLeafFaces, 0, sizeof(int) * mLeafFacesCount);

	fseek(mBspFile, readLEInt(bspLumps[LUMP_LEAF_FACES].offset), SEEK_SET);
	for (int i = 0; i < mLeafFacesCount; i++)
	{
		if (fread(&mLeafFaces[i], sizeof(int), 1, mBspFile) <= 0)
		{
			S_LOG_INFO("Failed to allocate leaf face for bsp.");
			free(mVertices);
			free(mFaces);
			free(mIndices);
			free(mMaterials);
			free(mLeafs);
			free(mLeafFaces);
			free(bspLightmaps);
			fclose(mBspFile);

			return;
		}

		mLeafFaces[i] = readLEInt(mLeafFaces[i]);
	}

	// Planes
	mPlanesCount = readLEInt(bspLumps[LUMP_PLANES].length) / sizeof(q3BspPlane);
	mPlanes = (q3BspPlane*) memalign(32, sizeof(q3BspPlane) * mPlanesCount);
	if (!mPlanes)
	{
		S_LOG_INFO("Failed to allocate plane array for bsp.");
		free(mVertices);
		free(mFaces);
		free(mIndices);
		free(mMaterials);
		free(mLeafs);
		free(bspLightmaps);
		fclose(mBspFile);

		return;
	}
	memset(mPlanes, 0, sizeof(q3BspPlane) * mPlanesCount);

	fseek(mBspFile, readLEInt(bspLumps[LUMP_PLANES].offset), SEEK_SET);
	for (int i = 0; i < mPlanesCount; i++)
	{
		if (fread(&mPlanes[i], sizeof(q3BspPlane), 1, mBspFile) <= 0)
		{
			S_LOG_INFO("Failed to allocate plane for bsp.");
			free(mVertices);
			free(mFaces);
			free(mIndices);
			free(mMaterials);
			free(mLeafs);
			free(mLeafFaces);
			free(mPlanes);
			free(bspLightmaps);
			fclose(mBspFile);

			return;
		}

		mPlanes[i].dist = readLEFloat(mPlanes[i].dist);

		float temp = readLEFloat(mPlanes[i].normal[1]);
		mPlanes[i].normal[0] = readLEFloat(mPlanes[i].normal[0]);
		mPlanes[i].normal[1] = readLEFloat(mPlanes[i].normal[2]);
		mPlanes[i].normal[2] = -temp;
	}

	// Visibility Data
	fseek(mBspFile, readLEInt(bspLumps[LUMP_VISDATA].offset), SEEK_SET);
	if (readLEInt(bspLumps[LUMP_VISDATA].length))
	{
		if (fread(&mBspVisData.numOfVis, sizeof(int), 1, mBspFile) <= 0)
		{
			S_LOG_INFO("Failed to read number of vis in bsp file.");
			free(mVertices);
			free(mFaces);
			free(mIndices);
			free(mMaterials);
			free(mLeafs);
			free(mLeafFaces);
			free(mPlanes);
			free(bspLightmaps);
			fclose(mBspFile);

			return;
		}

		if (fread(&mBspVisData.bytesPerVis, sizeof(int), 1, mBspFile) <= 0)
		{
			S_LOG_INFO("Failed to read number bytes per vis in bsp file.");
			free(mVertices);
			free(mFaces);
			free(mIndices);
			free(mMaterials);
			free(mLeafs);
			free(mLeafFaces);
			free(mPlanes);
			free(bspLightmaps);
			fclose(mBspFile);

			return;
		}

		int size = mBspVisData.bytesPerVis * mBspVisData.numOfVis;
		mBspVisData.bitSet = (unsigned char*) memalign(32, size);
		if (!mBspVisData.bitSet)
		{
			S_LOG_INFO("Failed to allocate array of vis bitsets in bsp file.");
			free(mVertices);
			free(mFaces);
			free(mIndices);
			free(mMaterials);
			free(mLeafs);
			free(mLeafFaces);
			free(mPlanes);
			free(bspLightmaps);
			fclose(mBspFile);

			return;
		}

		if (fread(mBspVisData.bitSet, size, 1, mBspFile) <= 0)
		{
			S_LOG_INFO("Failed to read array of vis bitsets in bsp file.");
			free(mVertices);
			free(mFaces);
			free(mIndices);
			free(mMaterials);
			free(mLeafs);
			free(mLeafFaces);
			free(mPlanes);
			free(bspLightmaps);
			free(mBspVisData.bitSet);
			fclose(mBspFile);

			return;
		}
	}

	// We are done here
	fclose(mBspFile);
}
			
void q3Bsp::renderFace(int i)
{
	const q3BspFace* faceToRender = &mFaces[i];
	if (!faceToRender)
	{
		S_LOG_INFO("Failed to fetch face from array.");
		return;
	}

	renderSystem* rs = root::getSingleton().getRenderSystem();
	kAssert(rs);

	material* materialOfFace = mMaterials[faceToRender->textureId];
	if (materialOfFace)
		materialOfFace->prepare();

	if (faceToRender->lmId < 0 && !materialOfFace)
		return;

	rs->clearArrayDesc();
	rs->setVertexArray(mVertices[faceToRender->startVertIndex].pos, sizeof(q3BspVertex));
	rs->setNormalArray(mVertices[faceToRender->startVertIndex].normal, sizeof(q3BspVertex));

	if (materialOfFace)
	{
		rs->setTexCoordArray(mVertices[faceToRender->startVertIndex].uv, sizeof(q3BspVertex));

		if (mDrawLightmaps && faceToRender->lmId >= 0)
		{
			// Send Lightmap
			const int stages = materialOfFace->getNumberOfTextureStages();
			rs->bindTexture(mLightmaps[faceToRender->lmId]->getId(0), stages);
			rs->setTexEnv("modulate", stages);
			rs->setTexCoordArray(mVertices[faceToRender->startVertIndex].lmUv, 
					sizeof(q3BspVertex), stages);
		}
	}

	rs->setVertexCount(faceToRender->numVertices);
	rs->setIndexCount(faceToRender->numIndices);
	rs->setVertexIndex(&mIndices[faceToRender->startIndex]);

	rs->drawArrays();

	if (materialOfFace)
		materialOfFace->finish();
}

void q3Bsp::draw(const camera* viewer)
{
	mBitSet.clear();

	const int leafIndex = findLeaf(viewer->getPosition());
	const int cluster = mLeafs[leafIndex].cluster;

	int leafCount = mLeafsCount;
	while (leafCount--)
	{
		const q3BspLeaf* currLeaf = &mLeafs[leafCount];
		if (!currLeaf)
			break;

		if (!isClusterVisible(cluster, currLeaf->cluster))
			continue;

		const vector3 mins = vector3(currLeaf->mins[0], currLeaf->mins[1], currLeaf->mins[2]);
		const vector3 maxs = vector3(currLeaf->maxs[0], currLeaf->maxs[1], currLeaf->maxs[2]);
		if (!viewer->isBoxInsideFrustum(mins, maxs))
			continue;

		int faceCount = currLeaf->numLeafSurf;
		while (faceCount--)
		{
			const int index = mLeafFaces[currLeaf->firstLeafSurf + faceCount];
			if (mFaces[index].type != FACETYPE_POLYGON) 
				continue;

			if(!mBitSet.isSet(index)) 
			{
				mBitSet.set(index);
				renderFace(index);
			}
		}
	}
}
			
bool q3Bsp::isClusterVisible(int curr, int targ)
{
	if (!mBspVisData.bitSet || curr < 0)
		return true;

	const unsigned char visSet = mBspVisData.bitSet[(curr * mBspVisData.bytesPerVis)+(targ/8)];
	return (visSet & (1 << (targ & 7)));
}

int q3Bsp::findLeaf(const vector3& viewerPos)
{
	int i = 0;
	while (i >= 0)
	{
		const q3BspNode* node = &mNodes[i];
		const q3BspPlane* plane = &mPlanes[node->plane];

		const vector3 planeNormal(plane->normal[0], plane->normal[1], plane->normal[2]);
		const vec_t distance = viewerPos.dotProduct(planeNormal) - plane->dist;

		// Front
		if (distance >= 0)
		{
			i = node->children[0];
		}
		// Back
		else
		{
			i = node->children[1];
		}
	}

	return ~i;
}

}

