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
#include "materialManager.h"
#include "textureManager.h"
#include "resourceManager.h"
#include "root.h"

namespace k {

#define Q3_EPSILON 1.0f/8.0f
			
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

bool q3BitSet::isSet(int i) const
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
			
void q3Bsp::_clean()
{
	if (mIndices)
		free(mIndices);

	if (mVertices)
		free(mVertices);

	if (mFaces)
		free(mFaces);

	if (mNodes)
		free(mNodes);

	if (mLeafs)
		free(mLeafs);

	if (mPlanes)
		free(mPlanes);

	if (mLeafFaces)
		free(mLeafFaces);

	if (mLeafBrushes)
		free(mLeafBrushes);

	if (mBspVisData.bitSet)
		free(mBspVisData.bitSet);

	if (mBrushes)
		free(mBrushes);

	if (mBrushSides)
		free(mBrushSides);
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
			_clean();

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

		tmp = readLEFloat(mVertices[i].normal[1]);
		mVertices[i].normal[0] = readLEFloat(mVertices[i].normal[0]);
		mVertices[i].normal[1] = readLEFloat(mVertices[i].normal[2]);
		mVertices[i].normal[2] = -tmp;
	}

	// Allocate Faces
	mFacesCount = readLEInt(bspLumps[LUMP_FACES].length) / sizeof(q3BspFace);
	mFaces = (q3BspFace*) memalign(32, mFacesCount * sizeof(q3BspFace));
	if (!mFaces)
	{
		S_LOG_INFO("Failed to allocate face array.");
		_clean();

		fclose(mBspFile);
		return;
	}

	// Configure Bitset
	mFaceSet.configure(mFacesCount);

	// Count number of patches needed
	unsigned int mPatchesCount = 0;

	fseek(mBspFile, readLEInt(bspLumps[LUMP_FACES].offset), SEEK_SET);
	for (int i = 0; i < mFacesCount; i++)
	{
		if (fread(&mFaces[i], sizeof(q3BspFace), 1, mBspFile) <= 0)
		{
			S_LOG_INFO("Failed to read bsp faces from file.");
			_clean();

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

		float temp = readLEFloat(mFaces[i].lmPos[1]);
		mFaces[i].lmPos[0] = readLEFloat(mFaces[i].lmPos[0]);
		mFaces[i].lmPos[1] = readLEFloat(mFaces[i].lmPos[2]);
		mFaces[i].lmPos[2] = -temp;

		mFaces[i].lmUv[0][0] = readLEFloat(mFaces[i].lmUv[0][0]);
		mFaces[i].lmUv[0][1] = readLEFloat(mFaces[i].lmUv[0][1]);
		mFaces[i].lmUv[0][2] = readLEFloat(mFaces[i].lmUv[0][2]);

		mFaces[i].lmUv[1][0] = readLEFloat(mFaces[i].lmUv[1][0]);
		mFaces[i].lmUv[1][1] = readLEFloat(mFaces[i].lmUv[1][1]);
		mFaces[i].lmUv[1][2] = readLEFloat(mFaces[i].lmUv[1][2]);

		temp = readLEFloat(mFaces[i].normal[1]);
		mFaces[i].normal[0] = readLEFloat(mFaces[i].normal[0]);
		mFaces[i].normal[1] = readLEFloat(mFaces[i].normal[2]);
		mFaces[i].normal[2] = -temp;

		mFaces[i].patchSize[0] = readLEInt(mFaces[i].patchSize[0]);
		mFaces[i].patchSize[1] = readLEInt(mFaces[i].patchSize[1]);

		if (mFaces[i].type == FACETYPE_PATCH && (mFaces[i].patchSize[0] || mFaces[i].patchSize[1]))
			mPatchesCount++;
	}

	// Allocate and configure patches
	mPatches = (bezierPatchSet*) malloc(mPatchesCount * sizeof(bezierPatchSet));
	if (!mPatches)
	{
		S_LOG_INFO("Failed to allocate index array.");
		_clean();
		
		fclose(mBspFile);
		return;
	}

	unsigned int activePatch = 0;
	for (int i = 0; i < mFacesCount; i++)
	{
		q3BspFace* thisFace = &mFaces[i];
		if (!thisFace || !thisFace->type == FACETYPE_PATCH)
			continue;

		if (!thisFace->patchSize[0] && !thisFace->patchSize[1])
		{
			thisFace->effect = -1;
			continue;
		}

		const unsigned int curvesCount = ((thisFace->patchSize[0] - 1) / 2) / ((thisFace->patchSize[1] - 1) / 2);
		mPatches[activePatch].configure(curvesCount);

		for (unsigned int i = 0; i < curvesCount; i++)
		{
			bezierPatch* thisPatch = (bezierPatch*) malloc(sizeof(bezierPatch));
			kAssert(thisPatch);

			// TODO: Make steps variable
			thisPatch->configure(6);

			// we are building lots of 3x3 patches
			for (unsigned int h = 0; h < 3; h++)
			{
				for (unsigned int w = 0; w < 3; w++)
				{
					int column = i * 2 + h * thisFace->patchSize[0];
					thisPatch->pushCP(&mVertices[thisFace->startVertIndex + column + w]);
				}
			}

			thisPatch->compile();
			mPatches[activePatch].pushPatch(thisPatch);
		}
			
		// Increase counter and reference patch set on the face
		thisFace->effect = activePatch++;
	}

	// Allocate Indices 
	mIndicesCount = readLEInt(bspLumps[LUMP_INDICES].length) / sizeof(int);
	mIndices = (index_t*) memalign(32, mIndicesCount * sizeof(index_t));
	if (!mIndices)
	{
		S_LOG_INFO("Failed to allocate index array.");
		_clean();
		
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
			_clean();
		
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
		_clean();

		fclose(mBspFile);
		return;
	}

	fseek(mBspFile, readLEInt(bspLumps[LUMP_TEXTURES].offset), SEEK_SET);
	if (fread(bspTextures, sizeof(q3BspTexture), mTexturesCount, mBspFile) <= 0)
	{
		S_LOG_INFO("Failed to read bsp textures from file.");
		_clean();

		fclose(mBspFile);
		return;
	}

	mMaterialsCount = mTexturesCount;
	mMaterials = (material**) memalign(32, sizeof(material*) * mTexturesCount);
	if (!mMaterials)
	{
		S_LOG_INFO("Failed to allocate materials array for bsp textures.");
		_clean();

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
			mMaterials[i] = matMgr->createMaterial(bspTextures[i].name);
			mMaterials[i]->setContentFlags(readLEInt(bspTextures[i].contents));
			texture* newTexture = getNewTexture(std::string(bspTextures[i].name));
			if (newTexture)
				mMaterials[i]->setSingleTexture(newTexture);
			else
				mMaterials[i]->setNoDraw(true);
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
		_clean();

		fclose(mBspFile);
		return;
	}

	fseek(mBspFile, readLEInt(bspLumps[LUMP_LIGHTMAPS].offset), SEEK_SET);
	if (fread(bspLightmaps, sizeof(q3BspLightmap128), mLightmapCount, mBspFile) <= 0)
	{
		S_LOG_INFO("Failed to read bsp lightmaps from file.");
		_clean();

		fclose(mBspFile);
		return;
	}

	mLightmaps = (texture**) memalign(32, sizeof(texture*) * mLightmapCount);
	if (!mLightmaps)
	{
		S_LOG_INFO("Failed to allocate texture array for bsp lightmaps.");
		_clean();

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
		_clean();

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
			_clean();

			fclose(mBspFile);
			return;
		}

		mNodes[i].plane = readLEInt(mNodes[i].plane);

		mNodes[i].children[0] = readLEInt(mNodes[i].children[0]);
		mNodes[i].children[1] = readLEInt(mNodes[i].children[1]);

		float temp = readLEFloat(mNodes[i].mins[1]);
		mNodes[i].mins[0] = readLEFloat(mNodes[i].mins[0]);
		mNodes[i].mins[1] = readLEFloat(mNodes[i].mins[2]);
		mNodes[i].mins[2] = -temp;

		temp = readLEFloat(mNodes[i].maxs[1]);
		mNodes[i].maxs[0] = readLEFloat(mNodes[i].maxs[0]);
		mNodes[i].maxs[1] = readLEFloat(mNodes[i].maxs[2]);
		mNodes[i].maxs[2] = -temp;
	}

	// Leafs
	mLeafsCount = readLEInt(bspLumps[LUMP_LEAFS].length) / sizeof(q3BspLeaf);
	mLeafs = (q3BspLeaf*) memalign(32, mLeafsCount * sizeof(q3BspLeaf));
	if (!mLeafs)
	{
		S_LOG_INFO("Failed to allocate leafs array for bsp.");
		_clean();

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
			_clean();

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
		_clean();

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
			_clean();

			fclose(mBspFile);
			return;
		}

		mLeafFaces[i] = readLEInt(mLeafFaces[i]);
	}

	// Leaf Brushes 
	mLeafBrushesCount = readLEInt(bspLumps[LUMP_LEAF_BRUSHES].length) / sizeof(int);
	mLeafBrushes = (int*) memalign(32, sizeof(int) * mLeafBrushesCount);
	if (!mLeafBrushes)
	{
		S_LOG_INFO("Failed to allocate leafs brushes array for bsp.");
		_clean();

		fclose(mBspFile);
		return;
	}

	fseek(mBspFile, readLEInt(bspLumps[LUMP_LEAF_BRUSHES].offset), SEEK_SET);
	for (int i = 0; i < mLeafBrushesCount; i++)
	{
		if (fread(&mLeafBrushes[i], sizeof(int), 1, mBspFile) <= 0)
		{
			S_LOG_INFO("Failed to allocate leaf brush for bsp.");
			_clean();

			fclose(mBspFile);
			return;
		}

		mLeafBrushes[i] = readLEInt(mLeafBrushes[i]);
	}

	// Planes
	mPlanesCount = readLEInt(bspLumps[LUMP_PLANES].length) / sizeof(q3BspPlane);
	mPlanes = (q3BspPlane*) memalign(32, sizeof(q3BspPlane) * mPlanesCount);
	if (!mPlanes)
	{
		S_LOG_INFO("Failed to allocate plane array for bsp.");
		_clean();

		fclose(mBspFile);
		return;
	}

	fseek(mBspFile, readLEInt(bspLumps[LUMP_PLANES].offset), SEEK_SET);
	for (int i = 0; i < mPlanesCount; i++)
	{
		if (fread(&mPlanes[i], sizeof(q3BspPlane), 1, mBspFile) <= 0)
		{
			S_LOG_INFO("Failed to allocate plane for bsp.");
			_clean();

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
			_clean();

			fclose(mBspFile);
			return;
		}

		if (fread(&mBspVisData.bytesPerVis, sizeof(int), 1, mBspFile) <= 0)
		{
			S_LOG_INFO("Failed to read number bytes per vis in bsp file.");
			_clean();

			fclose(mBspFile);
			return;
		}

		int size = mBspVisData.bytesPerVis * mBspVisData.numOfVis;
		mBspVisData.bitSet = (unsigned char*) memalign(32, size);
		if (!mBspVisData.bitSet)
		{
			S_LOG_INFO("Failed to allocate array of vis bitsets in bsp file.");
			_clean();

			fclose(mBspFile);
			return;
		}

		if (fread(mBspVisData.bitSet, size, 1, mBspFile) <= 0)
		{
			S_LOG_INFO("Failed to read array of vis bitsets in bsp file.");
			_clean();

			fclose(mBspFile);
			return;
		}
	}

	// Brushes
	mBrushesCount = readLEInt(bspLumps[LUMP_BRUSHES].length);
	mBrushes = (q3BspBrush*) memalign(32, sizeof(q3BspBrush) * mBrushesCount);
	if (!mBrushes)
	{
		S_LOG_INFO("Failed to allocate brushes for bsp.");
		_clean();

		fclose(mBspFile);
		return;
	}

	fseek(mBspFile, readLEInt(bspLumps[LUMP_BRUSHES].offset), SEEK_SET);
	for (int i = 0; i < mBrushesCount; i++)
	{
		q3BspBrush tempBrush;
		if (fread(&tempBrush, sizeof(q3BspBrush), 1, mBspFile) <= 0)
		{
			S_LOG_INFO("Failed to read brush from bsp.");
			_clean();

			fclose(mBspFile);
			return;
		}
		
		mBrushes[i].firstSide = readLEInt(tempBrush.firstSide);
		mBrushes[i].numSides = readLEInt(tempBrush.numSides);
		mBrushes[i].shaderNum = readLEInt(tempBrush.shaderNum);
	}

	// Brush Sides
	mBrushSidesCount = readLEInt(bspLumps[LUMP_BRUSHES_SIDES].length);
	mBrushSides = (q3BspBrushSide*) memalign(32, sizeof(q3BspBrushSide) * mBrushSidesCount);
	if (!mBrushSides)
	{
		S_LOG_INFO("Failed to allocate brush sides for bsp.");
		_clean();

		fclose(mBspFile);
		return;
	}

	fseek(mBspFile, readLEInt(bspLumps[LUMP_BRUSHES_SIDES].offset), SEEK_SET);
	for (int i = 0; i < mBrushSidesCount; i++)
	{
		q3BspBrushSide tempBrush;
		if (fread(&tempBrush, sizeof(q3BspBrushSide), 1, mBspFile) <= 0)
		{
			S_LOG_INFO("Failed to read brush side from bsp.");
			_clean();

			fclose(mBspFile);
			return;
		}
		
		mBrushSides[i].planeIndex = readLEInt(tempBrush.planeIndex);
		mBrushSides[i].textureIndex = readLEInt(tempBrush.textureIndex);
	}

	// Entities
	unsigned int mEntSize = readLEInt(bspLumps[LUMP_ENTITIES].length);
	char* mRawEnt = (char*) memalign(32, mEntSize);
	if (!mRawEnt)
	{
		S_LOG_INFO("Failed to allocate entities memory for bsp.");
		_clean();

		fclose(mBspFile);
		return;
	}
	memset(mRawEnt, 0, mEntSize);

	fseek(mBspFile, readLEInt(bspLumps[LUMP_ENTITIES].offset), SEEK_SET);
	if (fread(mRawEnt, mEntSize, 1, mBspFile) <= 0)
	{
		S_LOG_INFO("Failed to read entities from bsp.");
		_clean();

		fclose(mBspFile);
		return;
	}

	_parseEntities(mRawEnt);
	free(mRawEnt);

	// We are done parsing data.
	fclose(mBspFile);

	/* TODO - remake VBO support
	// Try to generate the Vertex Buffer Objects
	renderSystem* rs = root::getSingleton().getRenderSystem();

	if (rs && rs->getVBOSupport())
	{
		rs->genVBO(&mVBOVertex);
		rs->bindVBO(&mVBOVertex, VBO_ARRAY);
		rs->setVBOData(VBO_ARRAY, mVertexCount * sizeof(q3BspVertex), mVertices, VBO_STATIC_DRAW);
		rs->bindVBO(0, VBO_ARRAY);

		rs->genVBO(&mVBOIndex);
		rs->bindVBO(&mVBOIndex, VBO_ELEMENT_ARRAY);
		rs->setVBOData(VBO_ELEMENT_ARRAY, mIndicesCount * sizeof(index_t), mIndices, VBO_STATIC_DRAW);
		rs->bindVBO(0, VBO_ARRAY);
	}
	*/
}
			
void q3Bsp::_parseEntity(parsingFile& file)
{
	q3Entity newEnt;

	while (!file.eof())
	{
		std::string token = file.getNextToken();
		if (token == "}")
		{
			break;
		}
		else
		{
			std::string key = token;
			token = file.getNextToken();

			newEnt.pushKey(key, token);
		}
	}

	mEntities.push_back(newEnt);
}

void q3Bsp::_parseEntities(char* str)
{
	if (!str)
	{
		S_LOG_INFO("Invalid pointer to entities string.");
		return;
	}

	mEntities.clear();

	// start parsing the list
	parsingFile file(str);
	if (!file.isReady())
		return;

	std::string token = file.getNextToken();
	while (!file.eof())
	{
		// A new entity starts here ;)
		if (token == "{")
		{
			_parseEntity(file);
		}

		token = file.getNextToken();
	}
}
			
void q3Bsp::renderPatch(int i)
{
	const q3BspFace* patchFace = &mFaces[i];
	if (!patchFace)
	{
		S_LOG_INFO("Failed to fetch patch definition face from array.");
		return;
	}

	const bezierPatchSet* patchSet = &mPatches[patchFace->effect];
	if (!patchSet)
	{
		S_LOG_INFO("Failed to fetch patch from array.");
		return;
	}

	material* materialOfFace = mMaterials[patchFace->textureId];
	if (patchFace->lmId < 0 && !materialOfFace)
		return;

	renderSystem* rs = root::getSingleton().getRenderSystem();
	kAssert(rs);

	for (unsigned int i = 0; i < patchSet->getPatchesCount(); i++)
	{
		const bezierPatch* thisPatch = patchSet->getPatch(i);
		kAssert(thisPatch);

		const q3BspVertex* patchVertices = thisPatch->getVertices();
		kAssert(patchVertices);

		const unsigned int patchVertexCount = thisPatch->getVertexCount();

		for (unsigned int j = 0; j < thisPatch->getLevel(); j++)
		{
			if (materialOfFace)
				materialOfFace->prepare();

			rs->clearArrayDesc(VERTEXMODE_TRI_STRIP);
			rs->setVertexArray(patchVertices[0].pos, sizeof(q3BspVertex));
			rs->setNormalArray(patchVertices[0].normal, sizeof(q3BspVertex));

			if (materialOfFace)
			{
				rs->setTexCoordArray(patchVertices[0].uv, sizeof(q3BspVertex));
				if (mDrawLightmaps && patchFace->lmId >= 0)
				{
					// Send Lightmap
					const int stages = materialOfFace->getNumberOfTextureStages();
					rs->bindTexture(mLightmaps[patchFace->lmId]->getId(0), stages);
					rs->setTexEnv(TEX_ENV_MODULATE, stages);
					rs->setTexCoordArray(patchVertices[0].lmUv, sizeof(q3BspVertex), stages);
				}
			}

			// Set of indices for each level
			rs->setVertexCount(patchVertexCount);
			rs->setIndexCount(thisPatch->getRowIndicesCount(j));
			rs->setVertexIndex(thisPatch->getIndices(j));

			rs->drawArrays();
	
			if (materialOfFace)
				materialOfFace->finish();
		}
	}
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

	// TODO: redo VBO support
	// if (rs->getVBOSupport())
	if (0)
	{
		const unsigned int vSize = sizeof(q3BspVertex);
		const unsigned int vStart = faceToRender->startVertIndex * vSize;
				
		rs->clearArrayDesc();

		rs->setVBO(true);
		rs->bindVBO(&mVBOVertex, VBO_ARRAY);
		rs->bindVBO(&mVBOIndex, VBO_ELEMENT_ARRAY);

		rs->setVertexArray(vStart, vSize);
		rs->setNormalArray(vStart + sizeof(vec_t) * 7, vSize);

		if (materialOfFace)
		{
			rs->setTexCoordArray(vStart + sizeof(vec_t) * 3, vSize);
			if (mDrawLightmaps && faceToRender->lmId >= 0)
			{
				// Send Lightmap
				const short stages = materialOfFace->getNumberOfTextureStages();
				rs->bindTexture(mLightmaps[faceToRender->lmId]->getId(0), stages);
				rs->setTexEnv(TEX_ENV_MODULATE, stages);
				rs->setTexCoordArray(vStart + sizeof(vec_t) * 5, vSize, stages);
			}
		}

		rs->setVertexCount(faceToRender->numVertices);
		rs->setIndexCount(faceToRender->numIndices);
		rs->setVertexIndex(faceToRender->startIndex * sizeof(index_t));

		rs->drawArrays();

		rs->bindVBO(0, VBO_ARRAY);
		rs->bindVBO(0, VBO_ELEMENT_ARRAY);
		rs->setVBO(false);
	}
	else
	{
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
				rs->setTexEnv(TEX_ENV_MODULATE, stages);
				rs->setTexCoordArray(mVertices[faceToRender->startVertIndex].lmUv, 
						sizeof(q3BspVertex), stages);
			}
		}

		rs->setVertexCount(faceToRender->numVertices);
		rs->setIndexCount(faceToRender->numIndices);
		rs->setVertexIndex(&mIndices[faceToRender->startIndex]);

		rs->drawArrays();
	}

	if (materialOfFace)
		materialOfFace->finish();
}

void q3Bsp::draw(const camera* viewer)
{
	mFaceSet.clear();

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

			switch (mFaces[index].type)
			{
				case FACETYPE_NONE:
				case FACETYPE_BILLBOARD:
					break;

				case FACETYPE_MESH:
				case FACETYPE_POLYGON:
					if (!mFaceSet.isSet(index)) 
					{
						mFaceSet.set(index);
						renderFace(index);
					}
					break;

				case FACETYPE_PATCH:
					if (!mFaceSet.isSet(index) && mFaces[index].effect != -1) 
					{
						mFaceSet.set(index);
						renderPatch(index);
					}
					break;
			};
		}
	}
}
			
bool q3Bsp::isClusterVisible(int curr, int targ) const
{
	if (!mBspVisData.bitSet || curr < 0)
		return true;

	const unsigned char visSet = mBspVisData.bitSet[(curr * mBspVisData.bytesPerVis)+(targ/8)];
	return (visSet & (1 << (targ & 7)));
}

int q3Bsp::findLeaf(const vector3& viewerPos) const
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

static inline float q3DotProduct(const vector3& a, const vec_t* b)
{
	return (a.x * b[0] + a.y * b[1] + a.z * b[2]);
}

			
void q3Bsp::checkBrush(const q3BspBrush* brush)
{
	float startFraction = -1.0f;
	float endFraction = 1.0f;
	bool startsOut = false;
	bool endsOut = false;

	for (int i = 0; i < brush->numSides; i++)
	{
		q3BspBrushSide* thisSide = &mBrushSides[brush->firstSide + i];
		q3BspPlane* thisPlane = &mPlanes[thisSide->planeIndex];

		float startDist = q3DotProduct(mTraceStart, thisPlane->normal) - thisPlane->dist;	
		float endDist = q3DotProduct(mTraceEnd, thisPlane->normal) - thisPlane->dist;	

		if (startDist > 0)
			startsOut = true;
		if (endDist > 0)
			endsOut = true;

		// Completely in front of plane
		if (startDist > 0 && (endDist >= Q3_EPSILON || endDist >= startDist))
			return;

		// Doesnt cross the plane
		if (startDist <= 0 && endDist <= 0)
			continue;

		// Line is entering the brush
		if (startDist > endDist)
		{
			float fraction = (startDist - Q3_EPSILON) / (startDist - endDist);
			if (fraction > startFraction)
				startFraction = fraction;
		}
		else
		// line is leaving the brush
		{
			float fraction = (startDist + Q3_EPSILON) / (startDist - endDist);
			if (fraction < endFraction)
				endFraction = fraction;
		}
	}

	if (!startsOut)
	{
		mTempTrace.startsOut = false;
		if (!endsOut)
			mTempTrace.enclosedInSolid = true;

		return;
	}

	if (startFraction < endFraction)
	{
		if (startFraction > -1 && startFraction < mTempTrace.fraction)
		{
			if (startFraction < 0)
				startFraction = 0;

			mTempTrace.fraction = startFraction;
		}
	}
}

void q3Bsp::checkNode(int index, const float startFraction, const float endFraction,
					const vector3& start, const vector3& end)
{
	// we hit a leaf, go through it =]
	if (index < 0)
	{
		q3BspLeaf* thisLeaf = &mLeafs[~index];
		for (int i = 0; i < thisLeaf->numLeafBrush; i++)
		{
			unsigned int realIndex = mLeafBrushes[thisLeaf->firstLeafBrush + i];
		
			// Go go brushes!
			q3BspBrush* thisBrush = &mBrushes[realIndex];

			if ((thisBrush->numSides > 0) &&
				(!mTraceFlags || (mMaterials[thisBrush->shaderNum]->getContentFlags() & mTraceFlags)))
			{
				checkBrush(thisBrush);
			}
		}

		return;
	}

	// Lets walk through the tree to find the leaf
	q3BspNode* thisNode = &mNodes[index];
	q3BspPlane* thisPlane = &mPlanes[thisNode->plane];

	float startDist = q3DotProduct(start, thisPlane->normal) - thisPlane->dist;
	float endDist = q3DotProduct(end, thisPlane->normal) - thisPlane->dist;

	// In front of plane
	if (startDist >= 0 && endDist >= 0)
	{
		checkNode(thisNode->children[0], startFraction, endFraction, start, end);
	}
	else
	// Behind the plane
	if (startDist < 0 && endDist < 0)
	{
		checkNode(thisNode->children[1], startFraction, endFraction, start, end);
	}
	else
	{
		// the line is "cutting" the planes
		vector3 endStartDiff = end - start;
		vector3 middle;
		int side;
		float fraction1, fraction2, middleFraction;

		// back
		if (startDist < endDist)
		{
			side = 1;
			float invDist = 1.0f / (startDist - endDist);
	
			fraction1 = (startDist + Q3_EPSILON) * invDist;
			fraction2 = fraction1;
		}
		else
		// front
		if (startDist > endDist)
		{
			side = 0;
			float invDist = 1.0f / (startDist - endDist);
	
			fraction1 = (startDist + Q3_EPSILON) * invDist;
			fraction2 = (startDist - Q3_EPSILON) * invDist;
		}
		// choose front
		else
		{
			side = 0;
			fraction1 = 1.0f;
			fraction2 = 0.0f;
		}
	
		// check out fraction 1 and 2
		if (fraction1 < 0.0f)
			fraction1 = 0.0f;
		else
		if (fraction1 > 1.0f)
			fraction1 = 1.0f;

		if (fraction2 < 0.0f)
			fraction2 = 0.0f;
		else
		if (fraction2 > 1.0f)
			fraction2 = 1.0f;
	
		// middle point for the first side
		middleFraction = startFraction + (endFraction - startFraction) * fraction1;
		middle = start + endStartDiff * fraction1;
		checkNode(thisNode->children[side], startFraction, middleFraction, start, middle);
		
		// middle point for the second side
		middleFraction = startFraction + (endFraction - startFraction) * fraction2;
		middle = start + endStartDiff * fraction2;
		checkNode(thisNode->children[side ^ 1], middleFraction, endFraction, middle, end);
	}
}
				
q3BspTrace q3Bsp::trace(const vector3& start, const vector3& end, int flags)
{
	mTempTrace.startsOut = true;
	mTempTrace.enclosedInSolid = false;
	mTempTrace.fraction = 1.0f;

	mTraceStart = start;
	mTraceEnd = end;
	mTraceFlags = flags;

	checkNode(0, 0, 1.0f, start, end);
	if (mTempTrace.fraction == 1.0f)
	{
		mTempTrace.end = end;
	}
	else
	{
		mTempTrace.end = mTraceStart + (mTraceEnd - mTraceStart) * mTempTrace.fraction;
	}

	return mTempTrace;
}
			
bezierPatch::bezierPatch()
{
	mVertices = NULL;
	mIndices = NULL;

	mNumOfVertices = mNumOfIndices = mSteps = 0;
	mCurrCP = 0;
}

bezierPatch::~bezierPatch()
{
	if (mVertices)
		free(mVertices);

	if (mIndices)
		free(mIndices);

	if (mControlPoints)
		free(mControlPoints);

	if (mTrianglesPerRow)
		free(mTrianglesPerRow);

	if (mRowIndices)
		free(mRowIndices);
}
			
void bezierPatch::configure(unsigned int steps)
{
	mSteps = steps;
	mCurrCP = 0;
	
	mNumOfVertices = pow(mSteps + 1, 2);
	mVertices = (q3BspVertex*) memalign(32, mNumOfVertices * sizeof(q3BspVertex));
	if (!mVertices)
	{
		S_LOG_INFO("Failed to allocate bezier surface vertices.");
		return;
	}

	mNumOfIndices = mSteps * (mSteps + 1) * 2;
	mIndices = (index_t*) memalign(32, mNumOfIndices * sizeof(index_t));
	if (!mIndices)
	{
		S_LOG_INFO("Failed to allocate bezier surface indices.");
		free(mVertices);
		free(mControlPoints);
		return;
	}

	mTrianglesPerRow = (index_t*) memalign(32, mSteps * sizeof(index_t));
	if (!mTrianglesPerRow)
	{	
		S_LOG_INFO("Failed to allocate bezier surface trianglesPerRow.");
		free(mVertices);
		free(mControlPoints);
		free(mIndices);
		return;
	}

	mRowIndices = (index_t**) memalign(32, mSteps * sizeof(index_t*));
	if (!mRowIndices)
	{	
		S_LOG_INFO("Failed to allocate bezier surface row indices.");
		free(mVertices);
		free(mControlPoints);
		free(mIndices);
		free(mTrianglesPerRow);
		return;
	}

	memset(mVertices, 0, sizeof(q3BspVertex) * mNumOfVertices);
	memset(mIndices, 0, sizeof(index_t) * mNumOfIndices);
	memset(mControlPoints, 0, sizeof(q3BspVertex) * 9);
	memset(mTrianglesPerRow, 0, sizeof(index_t) * mSteps);
	memset(mRowIndices, 0, sizeof(index_t*) * mSteps);
}
			
void bezierPatch::pushCP(q3BspVertex* cp)
{
	kAssert(cp);
	memcpy(&mControlPoints[mCurrCP++], cp, sizeof(q3BspVertex));
}

void bezierPatch::compile()
{
	if (!mVertices || !mIndices)
		return;

	// Compute the first column of vertices
	for (unsigned int i = 0; i <= mSteps; i++)
	{
		float a = (float)i / mSteps;
		float b = 1 - a;
			
		for (unsigned int v = 0; v < 3; v++)
		{
			mVertices[i].pos[v] = 
				mControlPoints[0].pos[v] * (b * b) +
				mControlPoints[3].pos[v] * (2 * b * a) +
				mControlPoints[6].pos[v] * (a * a);

			mVertices[i].normal[v] = 
				mControlPoints[0].normal[v] * (b * b) +
				mControlPoints[3].normal[v] * (2 * b * a) +
				mControlPoints[6].normal[v] * (a * a);
		}

		for (unsigned int v = 0; v < 2; v++)
		{
			mVertices[i].uv[v] = 
				mControlPoints[0].uv[v] * (b * b) +
				mControlPoints[3].uv[v] * (2 * b * a) +
				mControlPoints[6].uv[v] * (a * a);

			mVertices[i].lmUv[v] = 
				mControlPoints[0].lmUv[v] * (b * b) +
				mControlPoints[3].lmUv[v] * (2 * b * a) +
				mControlPoints[6].lmUv[v] * (a * a);
		}
	}

	for (unsigned int i = 1; i <= mSteps; i++)
	{
		unsigned int j;
		float a = (float)i / mSteps;
		float b = 1.0 - a;

		q3BspVertex temp[3];
		for (j = 0; j < 3; j++)
		{
			unsigned int k = 3 * j;

			for (unsigned int v = 0; v < 3; v++)
			{
				temp[j].pos[v] = mControlPoints[k + 0].pos[v] * (b * b) +
					mControlPoints[k + 1].pos[v] * (2 * b * a) +
					mControlPoints[k + 2].pos[v] * (a * a);

				temp[j].normal[v] = mControlPoints[k + 0].normal[v] * (b * b) +
					mControlPoints[k + 1].normal[v] * (2 * b * a) +
					mControlPoints[k + 2].normal[v] * (a * a);
			}

			for (unsigned int v = 0; v < 2; v++)
			{
				temp[j].uv[v] = mControlPoints[k + 0].uv[v] * (b * b) +
					mControlPoints[k + 1].uv[v] * (2 * b * a) +
					mControlPoints[k + 2].uv[v] * (a * a);

				temp[j].lmUv[v] = mControlPoints[k + 0].lmUv[v] * (b * b) +
					mControlPoints[k + 1].lmUv[v] * (2 * b * a) +
					mControlPoints[k + 2].lmUv[v] * (a * a);
			}
		}

		for (j = 0; j <= mSteps; j++)
		{
			float a2 = (float)j / mSteps;
			float b2 = 1.0 - a2;

			for (unsigned int v = 0; v < 3; v++)
			{
				mVertices[i * (mSteps + 1) + j].pos[v] = 
					temp[0].pos[v] * (b2 * b2) +
					temp[1].pos[v] * (2 * b2 * a2) +
					temp[2].pos[v] * (a2 * a2);

				mVertices[i * (mSteps + 1) + j].normal[v] = 
					temp[0].normal[v] * (b2 * b2) +
					temp[1].normal[v] * (2 * b2 * a2) +
					temp[2].normal[v] * (a2 * a2);
			}

			for (unsigned int v = 0; v < 2; v++)
			{
				mVertices[i * (mSteps + 1) + j].uv[v] = 
					temp[0].uv[v] * (b2 * b2) +
					temp[1].uv[v] * (2 * b2 * a2) +
					temp[2].uv[v] * (a2 * a2);

				mVertices[i * (mSteps + 1) + j].lmUv[v] = 
					temp[0].lmUv[v] * (b2 * b2) +
					temp[1].lmUv[v] * (2 * b2 * a2) +
					temp[2].lmUv[v] * (a2 * a2);
			}
		}
	}

	// Compute Indices
	for (unsigned int row = 0; row < mSteps; row++)
	{
		for (unsigned int col = 0; col <= mSteps; col++)
		{
			mIndices[(row * (mSteps + 1) + col) * 2 + 1] = row * (mSteps + 1) + col;
			mIndices[(row * (mSteps + 1) + col) * 2] = (row + 1) * (mSteps + 1) + col;
		}
	}

	for (unsigned int row = 0; row < mSteps; row++)
	{
		mTrianglesPerRow[row] = 2 * (mSteps + 1);
		mRowIndices[row] = &mIndices[row * 2 * (mSteps + 1)];
	}
}
			
const unsigned int bezierPatch::getRowIndicesCount(const short i) const
{
	return mTrianglesPerRow[i];
}
			
const index_t* bezierPatch::getIndices(const short i) const
{
	return mRowIndices[i];
}
			
const unsigned int bezierPatch::getVertexCount() const
{
	return mNumOfVertices;
}

const q3BspVertex* bezierPatch::getVertices() const
{
	return mVertices;
}

const bezierPatch* bezierPatchSet::getPatch(const short i) const
{
	return mPatches[i];
}

const unsigned int bezierPatchSet::getPatchesCount() const
{
	return mPatchNum;
}
			
void bezierPatchSet::configure(const short i)
{
	mPatches = (bezierPatch**) memalign(32, sizeof(bezierPatch*) * i);
	if (!mPatches)
		S_LOG_INFO("Failed to allocate patch arrays for set.");

	mPatchNum = i;
	mPatchIndex = 0;
}

void bezierPatchSet::pushPatch(bezierPatch* patch)
{
	if (!mPatches)
		return;

	kAssert(patch);
	mPatches[mPatchIndex++] = patch;
}
			
}

