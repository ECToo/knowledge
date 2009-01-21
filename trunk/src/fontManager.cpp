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

#include "fontManager.h"
#include "logger.h"
#include "root.h"
#include "textureManager.h"
#include "resourceManager.h"

#ifdef __WII__
#define byteSwap4(Y) (((Y & 0xff)<<24)|((Y & 0xff00) << 8)|((Y & 0xff0000) >> 8)|((Y & 0xff000000) >> 24))
#else
#define byteSwap4(Y) (Y)
#endif

namespace k {

/*
template<> fontManager* singleton<fontManager>::singleton_instance = 0;

fontManager& fontManager::getSingleton()
{  
	assert(singleton_instance);
	return (*singleton_instance);  
}
*/

// Helper function
int readEndianSafeInt(FILE* f)
{
	int value;
	assert(f != NULL);

	if (fread(&value, 4, 1, f) > 0)
		return byteSwap4(value);
	else
		return 0;
}

float readEndianSafeFloat(FILE* f)
{
	assert(f != NULL);

	union
	{
		float f;
		int i;
	} data;

	if (fread(&data.f, 4, 1, f) > 0)
	{
		data.i = byteSwap4(data.i);
		return data.f;
	}
	else 
	{
		return 0.0f;
	}
}
			
bitmapText::bitmapText(const std::string& datName, const std::string& matName)
{
	mMaterial = materialManager::getSingleton().getMaterial(matName);
	if (!mMaterial)
	{
		S_LOG_INFO("Failed to load material " + matName);
		return;
	}

	std::string fullPath = datName;
	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc)
	{
		fullPath = rsc->getRoot() + datName;
	}

	FILE* datFile = fopen(fullPath.c_str(), "rb");
	if (!datFile)
	{
		S_LOG_INFO("Failed to open dat file " + datName);
		return;
	}

	for (int i = 0; i < 255; i++)
	{
		doom3Glyph* glyph = &mGlyphs[i];

		glyph->height = readEndianSafeInt(datFile);
		glyph->top = readEndianSafeInt(datFile);
		glyph->bottom = readEndianSafeInt(datFile);
		glyph->pitch = readEndianSafeInt(datFile);
		glyph->xSkip = readEndianSafeInt(datFile);
		glyph->imgWidth = readEndianSafeInt(datFile);
		glyph->imgHeight = readEndianSafeInt(datFile);
		glyph->s = readEndianSafeFloat(datFile);
		glyph->t = readEndianSafeFloat(datFile);
		glyph->s2 = readEndianSafeFloat(datFile);
		glyph->t2 = readEndianSafeFloat(datFile);

		// TODO: We will need the image name
		// for larger paged fonts.
		//
		// Skip unneeded data like image name.
		fseek(datFile, 36, SEEK_CUR);
	}
}
			
bitmapText::~bitmapText()
{
}

vec_t bitmapText::_drawChar(vec_t posX, vec_t y, char c)
{
	doom3Glyph* activeGlyph = &mGlyphs[(int)c];
	if (!activeGlyph)
	{
		S_LOG_INFO("Warning, glyph for " + std::string(&c) + " not found.");
		return 0;
	}

	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	rs->texCoord(vector2(activeGlyph->s, activeGlyph->t));
	rs->vertex(vector3(posX, y, -0.5));

	rs->texCoord(vector2(activeGlyph->s2, activeGlyph->t));
	rs->vertex(vector3(posX + activeGlyph->imgWidth, y, -0.5));

	rs->texCoord(vector2(activeGlyph->s2, activeGlyph->t2));
	rs->vertex(vector3(posX + activeGlyph->imgWidth, y + activeGlyph->imgHeight, -0.5));

	rs->texCoord(vector2(activeGlyph->s, activeGlyph->t2));
	rs->vertex(vector3(posX, y + activeGlyph->imgHeight, -0.5));

	return activeGlyph->xSkip;
}

void bitmapText::setText(const std::string& text)
{
	mContents = text;
}

void bitmapText::draw()
{
	if (!mContents.length())
		return;

	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	// Material setup
	mMaterial->prepare();

	// Indepedent of material
	rs->setCulling(CULLMODE_NONE);
	rs->setDepthMask(false);

	rs->startVertices(VERTEXMODE_QUAD);

	vec_t x = mPosition.x;
	for (unsigned int i = 0; i < mContents.length(); i++)
		x += _drawChar(x, mPosition.y, mContents[i]);

	rs->endVertices();

	// Take it back
	mMaterial->finish();
	rs->setDepthMask(true);
}

}

