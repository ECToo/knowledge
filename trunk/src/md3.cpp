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

#include "md3.h"
#include "logger.h"
#include "root.h"
#include "materialManager.h"
#include "resourceManager.h"

namespace k {

void md3Surface::_clean()
{
	if (mVertices)
		free(mVertices);

	if (mIndices)
		free(mIndices);

	if (mUVs)
		free(mUVs);

	// TODO: Clean material
}

void md3Surface::draw(short frameNum)
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	kAssert(rs);

	if (mMaterial)
		mMaterial->prepare();

	rs->clearArrayDesc();
	rs->setVertexArray(mVertices[frameNum * mVerticesCount].pos, sizeof(md3RealVertex_t));
	rs->setNormalArray(mVertices[frameNum * mVerticesCount].normal, sizeof(md3RealVertex_t));
	rs->setTexCoordArray(mUVs[0].uv, sizeof(md3TexCoord_t));
		
	rs->setVertexCount(mVerticesCount);
	rs->setIndexCount(mIndicesCount);
	rs->setVertexIndex(mIndices[0].indices);

	rs->drawArrays();

	if (mMaterial)
		mMaterial->finish();
}

void md3model::_clean()
{
	if (mFrames)
		free(mFrames);

	if (mTags)
		free(mTags);

	if (mSurfaces)
		free(mSurfaces);
}

/**
 * Yes i know i should share this between bsp64.cpp and md3.cpp
 * Im taking care of this later.
 */
static inline texture* getNewTexture(const std::string& filename)
{
	resourceManager* rscMgr = &resourceManager::getSingleton();
	textureManager* texMgr = &textureManager::getSingleton();
	kAssert(texMgr);
	kAssert(rscMgr);

	std::string fullPath = filename;
	if (rscMgr) fullPath = rscMgr->getRoot() + filename;

	// First, lets test if default filename exists.
	FILE* testFile = fopen(fullPath.c_str(), "rb");
	if (testFile)
	{
		fclose(testFile);

		texMgr->allocateTextureData(fullPath);
		return texMgr->getTexture(fullPath);
	}

	// Ok, file doesnt exist, lets check the extension:
	std::string extension = getExtension(filename);
	std::string newFile = filename.substr(0, filename.size() - extension.size());

	if (extension == ".jpg" || extension == ".JPG" || extension == ".jpeg" || extension == ".JPEG")
	{
		// Test TGA
		newFile += ".tga";
	}
	else
	{
		// Test JPEG
		newFile += ".jpg";
	}
	
	fullPath = newFile;
	if (rscMgr) fullPath = rscMgr->getRoot() + newFile;

	testFile = fopen(fullPath.c_str(), "rb");
	if (testFile)
	{
		fclose(testFile);

		texMgr->allocateTextureData(newFile);
		return texMgr->getTexture(newFile);
	}
	else
	{
		S_LOG_INFO("Texture " + filename + " not found.");
		return NULL;
	}
}
		
md3model::md3model(const std::string& filename)
{
	// Get file full path if resource manager is available.
	std::string fullPath = filename;
	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc) fullPath = rsc->getRoot() + filename;

	// Before reading, reset all the pointers
	mFrames = NULL;
	mTags = NULL;
	mSurfaces = NULL;
	mCurrentFrame = 0;

	// Ok, start parsing file.
	FILE* md3File = fopen(fullPath.c_str(), "rb");
	if (!md3File)
	{
		S_LOG_INFO("Failed to read md3 file " + fullPath);
		return;
	}

	// Get md3 header
	md3Header_t modelHeader;
	if (fread(&modelHeader, sizeof(md3Header_t), 1, md3File) <= 0)
	{
		S_LOG_INFO("Failed to read md3 header.");
		return;
	}

	if (strncmp(modelHeader.ident, "IDP3", 4))
	{
		S_LOG_INFO("IDENT (" + std::string(modelHeader.ident) + ") on md3 model doesnt match IDP3.");
		return;
	}

	// Version
	if (readLEInt(modelHeader.version) != 15)
	{
		S_LOG_INFO("Invalid md3 version, expected 15.");
		return;
	}

	// Allocate the frames
	mFramesCount = readLEInt(modelHeader.numFrames);
	mFrames = (md3Frame_t*) malloc(sizeof(md3Frame_t) * mFramesCount);
	if (!mFrames)
	{
		S_LOG_INFO("Failed to read md3 frames.");
		return;
	}

	// Read Frames
	fseek(md3File, readLEInt(modelHeader.offsetFrames), SEEK_SET);
	for (int i = 0; i < mFramesCount; i++)
	{
		md3Frame_t tempFrame;
		if (fread(&tempFrame, sizeof(md3Frame_t), 1, md3File) <= 0)
		{
			S_LOG_INFO("Failed to read md3 frame.");
			_clean();

			return;
		}

		mFrames[i].mins[0] = readLEFloat(tempFrame.mins[0]);
		mFrames[i].mins[1] = readLEFloat(tempFrame.mins[1]);
		mFrames[i].mins[2] = readLEFloat(tempFrame.mins[2]);

		mFrames[i].maxs[0] = readLEFloat(tempFrame.maxs[0]);
		mFrames[i].maxs[1] = readLEFloat(tempFrame.maxs[1]);
		mFrames[i].maxs[2] = readLEFloat(tempFrame.maxs[2]);

		mFrames[i].origin[0] = readLEFloat(tempFrame.origin[0]);
		mFrames[i].origin[1] = readLEFloat(tempFrame.origin[1]);
		mFrames[i].origin[2] = readLEFloat(tempFrame.origin[2]);

		mFrames[i].radius = readLEFloat(tempFrame.radius);

		strncpy(mFrames[i].name, tempFrame.name, 16);
	}

	// Allocate the tags
	mTagsCount = readLEInt(modelHeader.numTags);
	mTags = (md3Tag_t*) malloc(sizeof(md3Tag_t) * mTagsCount);
	if (!mTags)
	{
		S_LOG_INFO("Failed to allocate md3 tags.");
		_clean();

		return;
	}

	// Read Tags
	fseek(md3File, readLEInt(modelHeader.offsetTags), SEEK_SET);
	for (int i = 0; i < mTagsCount; i++)
	{
		md3Tag_t tempTag;
		if (fread(&tempTag, sizeof(md3Tag_t), 1, md3File) <= 0)
		{
			S_LOG_INFO("Failed to read tag from md3 file.");
			_clean();

			return;
		}

		strncpy(mTags[i].name, tempTag.name, 64);

		mTags[i].origin[0] = readLEFloat(tempTag.origin[0]);
		mTags[i].origin[1] = readLEFloat(tempTag.origin[1]);
		mTags[i].origin[2] = readLEFloat(tempTag.origin[2]);

		mTags[i].axis[0][0] = readLEFloat(tempTag.axis[0][0]);
		mTags[i].axis[0][1] = readLEFloat(tempTag.axis[0][1]);
		mTags[i].axis[0][2] = readLEFloat(tempTag.axis[0][2]);

		mTags[i].axis[1][0] = readLEFloat(tempTag.axis[1][0]);
		mTags[i].axis[1][1] = readLEFloat(tempTag.axis[1][1]);
		mTags[i].axis[1][2] = readLEFloat(tempTag.axis[1][2]);

		mTags[i].axis[2][0] = readLEFloat(tempTag.axis[2][0]);
		mTags[i].axis[2][1] = readLEFloat(tempTag.axis[2][1]);
		mTags[i].axis[2][2] = readLEFloat(tempTag.axis[2][2]);
	}

	// Surfaces
	mSurfacesCount = readLEInt(modelHeader.numSurfaces);
	mSurfaces = (md3Surface*) malloc(sizeof(md3Surface) * mSurfacesCount);
	if (!mSurfaces)
	{
		S_LOG_INFO("Failed to allocate md3 surfaces.");
		_clean();

		return;
	}

	// Read Surfaces
	fseek(md3File, readLEInt(modelHeader.offsetSurfaces), SEEK_SET);
	for (int i = 0; i < mSurfacesCount; i++)
	{
		long int surfaceStartOffset = ftell(md3File);

		md3Surface_t tempSurface;
		if (fread(&tempSurface, sizeof(md3Surface_t), 1, md3File) <= 0)
		{
			S_LOG_INFO("Failed to read md3 surface.");
			_clean();

			return;
		}

		// Read Shaders
		int numShaders = readLEInt(tempSurface.numShaders);
		md3Shader_t* mShaders = (md3Shader_t*) malloc(sizeof(md3Shader_t) * numShaders);
		if (!mShaders)
		{
			S_LOG_INFO("Failed to allocate surface shaders.");
			_clean();

			return;
		}

		fseek(md3File, surfaceStartOffset + readLEInt(tempSurface.offsetShaders), SEEK_SET);
		if (fread(mShaders, sizeof(md3Shader_t) * numShaders, 1, md3File) <= 0)
		{
			S_LOG_INFO("Failed to read surface shaders.");
			free(mShaders);
			_clean();

			return;
		}

		// Reset material
		mSurfaces[i].mMaterial = NULL;

		// For each shader, search a material containing the texture.
		for (int shader = 0; shader < numShaders; shader++)
		{
			material* mat = materialManager::getSingleton().getMaterialWithFilename(mShaders[shader].name);

			// Material not found, we are going to create
			// a simple material with this texture on it only.
			if (!mat)
			{
				texture* newTexture = getNewTexture(std::string(mShaders[shader].name));
				if (newTexture)
				{
					mat = materialManager::getSingleton().createMaterial(mShaders[shader].name);
					mat->setSingleTexture(newTexture);

					mSurfaces[i].mMaterial = mat;
				}
			}
		}

		// we are done with shaders, free it
		free(mShaders);

		// Read Triangles
		mSurfaces[i].mIndicesCount = readLEInt(tempSurface.numTris);
		mSurfaces[i].mIndices = (md3Triangle_t*) malloc(sizeof(md3Triangle_t) * mSurfaces[i].mIndicesCount);
		if (!mSurfaces[i].mIndices)
		{
			S_LOG_INFO("Failed to allocate surface indices.");
			_clean();

			return;
		}

		fseek(md3File, surfaceStartOffset + readLEInt(tempSurface.offsetTriangles), SEEK_SET);
		for (int surf = 0; surf < mSurfaces[i].mIndicesCount; surf++)
		{
			md3Triangle_t tempTriangle;
			if (fread(&tempTriangle, sizeof(md3Triangle_t), 1, md3File) <= 0)
			{
				S_LOG_INFO("Failed to read surface triangle.");
				free(mSurfaces[i].mIndices);
				_clean();

				return;
			}

			mSurfaces[i].mIndices[surf].indices[0] = readLEInt(tempTriangle.indices[0]);
			mSurfaces[i].mIndices[surf].indices[1] = readLEInt(tempTriangle.indices[1]);
			mSurfaces[i].mIndices[surf].indices[2] = readLEInt(tempTriangle.indices[2]);
		}

		// The right number of indices gets multiplied by 3 (we have the number of tris)
		mSurfaces[i].mIndicesCount *= 3;

		// allocate uv's
		mSurfaces[i].mUVCount = readLEInt(tempSurface.numVerts);
		mSurfaces[i].mUVs = (md3TexCoord_t*) malloc(sizeof(md3TexCoord_t) * mSurfaces[i].mUVCount);
		if (!mSurfaces[i].mUVs)
		{
			S_LOG_INFO("Failed to allocate surface uvs.");
			free(mSurfaces[i].mIndices);
			_clean();

			return;
		}

		// read uvs
		fseek(md3File, surfaceStartOffset + readLEInt(tempSurface.offsetUV), SEEK_SET);
		for (int uv = 0; uv < mSurfaces[i].mUVCount; uv++)
		{
			md3TexCoord_t tempUV;
			if (fread(&tempUV, sizeof(md3TexCoord_t), 1, md3File) <= 0)
			{
				S_LOG_INFO("Failed to read surface uv.");
				free(mSurfaces[i].mIndices);
				free(mSurfaces[i].mUVs);
				_clean();

				return;
			}

			mSurfaces[i].mUVs[uv].uv[0] = readLEFloat(tempUV.uv[0]);
			mSurfaces[i].mUVs[uv].uv[1] = readLEFloat(tempUV.uv[1]);
		}

		// allocate vertices
		int tempVertexCount = readLEInt(tempSurface.numVerts) * readLEInt(tempSurface.numFrames);
		mSurfaces[i].mVerticesCount = readLEInt(tempSurface.numVerts);
		mSurfaces[i].mVertices = (md3RealVertex_t*) malloc(sizeof(md3RealVertex_t) * tempVertexCount);

		if (!mSurfaces[i].mVertices)
		{
			S_LOG_INFO("Failed to allocate surface vertices.");
			free(mSurfaces[i].mIndices);
			free(mSurfaces[i].mUVs);
			_clean();

			return;
		}

		// allocate temp vertices
		md3Vertex_t* tempVertices = (md3Vertex_t*) malloc(sizeof(md3Vertex_t) * tempVertexCount);
		if (!tempVertices)
		{
			S_LOG_INFO("Failed to allocate surface temporary vertices.");
			free(mSurfaces[i].mIndices);
			free(mSurfaces[i].mUVs);
			free(mSurfaces[i].mVertices);
			_clean();

			return;
		}

		// read temp vertices
		fseek(md3File, surfaceStartOffset + readLEInt(tempSurface.offsetVertex), SEEK_SET);
		if (fread(tempVertices, sizeof(md3Vertex_t), tempVertexCount, md3File) <= 0)
		{
			S_LOG_INFO("Failed to read surface temporary vertices.");
			free(mSurfaces[i].mIndices);
			free(mSurfaces[i].mUVs);
			free(mSurfaces[i].mVertices);
			_clean();

			return;
		}

		// Convert vertices
		for (int vertex = 0; vertex < tempVertexCount; vertex++)
		{
			const float vertexMultiplier = 0.015625f;

			mSurfaces[i].mVertices[vertex].pos[0] = readLEShort(tempVertices[vertex].coord[0]) * vertexMultiplier;
			mSurfaces[i].mVertices[vertex].pos[1] = readLEShort(tempVertices[vertex].coord[1]) * vertexMultiplier;
			mSurfaces[i].mVertices[vertex].pos[2] = readLEShort(tempVertices[vertex].coord[2]) * vertexMultiplier;

	 		float lat = tempVertices[vertex].normal[0] * (2.0f * M_PI) / 255.0f;
	 		float lng = tempVertices[vertex].normal[1] * (2.0f * M_PI) / 255.0f;

			mSurfaces[i].mVertices[vertex].normal[0] = cos(lat) * sin(lng);
			mSurfaces[i].mVertices[vertex].normal[1] = sin(lat) * sin(lng);
			mSurfaces[i].mVertices[vertex].normal[2] = cos(lng);
		}

		// We dont need the temporary vertices anymore, free it
		free(tempVertices);

		// Copy name
		strncpy(mSurfaces[i].name, tempSurface.name, 64);

		// FrameCount
		mSurfaces[i].mFrameCount = readLEInt(tempSurface.numFrames);
	}
}
		
void md3model::setFrame(short i)
{
	if (i < mFramesCount)
		mCurrentFrame = i;
}

int md3model::getFramesCount() const
{
	return mFramesCount;
}
		
void md3model::draw()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	kAssert(rs);

	// Rotate and Translate
	vector3 finalPos = getPosition();
	quaternion finalOrientation = getOrientation();

	vec_t angle;
	vector3 axis;
	finalOrientation.toAxisAngle(angle, axis);

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->translateScene(finalPos.x, finalPos.y, finalPos.z);
	rs->rotateScene(angle, axis.x, axis.y, axis.z);
	rs->scaleScene(mScale.x, mScale.y, mScale.z);

	for (int i = 0; i < mSurfacesCount; i++)
		mSurfaces[i].draw(mCurrentFrame);
}

const boundingBox& md3model::getAABoundingBox()
{
	return boundingBox(vector3::zero, vector3::zero);
}

const boundingBox& md3model::getBoundingBox()
{
	return boundingBox(vector3::zero, vector3::zero);
}

}
