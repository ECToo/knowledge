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

#ifndef LITTLE_ENDIAN
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
	float value;
	assert(f != NULL);

	#ifndef LITTLE_ENDIAN
	if (fread(&value, 4, 1, f) > 0)
	{
		union 
		{
			float f;
			char b[4];
		} dat1, dat2;

		dat1.f = value;
		dat2.b[0] = dat1.b[3];
		dat2.b[1] = dat1.b[2];
		dat2.b[2] = dat1.b[1];
		dat2.b[3] = dat1.b[0];

		return dat2.f;
	}
	else 
	{
		return 0.0f;
	}
	#else
	if (fread(&value, 4, 1, f) > 0)
		return value;
	else
		return 0.0f;
	#endif
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
		return 0;

	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	vec_t realY = y;

	#ifndef __WII__
	// The position for openGL should be switched in
	// Y axis because it starts from bottom while on
	// wii it starts on the top.
	realY = rs->getScreenHeight() - y;
	#endif

	rs->texCoord(vector2(activeGlyph->s, activeGlyph->t2));
	rs->vertex(vector3(posX, realY, mZ));

	rs->texCoord(vector2(activeGlyph->s2, activeGlyph->t2));
	rs->vertex(vector3(posX + activeGlyph->imgWidth, realY, mZ));

	rs->texCoord(vector2(activeGlyph->s2, activeGlyph->t));
	rs->vertex(vector3(posX + activeGlyph->imgWidth, realY + activeGlyph->imgHeight, mZ));

	rs->texCoord(vector2(activeGlyph->s, activeGlyph->t));
	rs->vertex(vector3(posX, realY + activeGlyph->imgHeight, mZ));

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

	mMaterial->prepare();

	rs->setDepthMask(false);
	rs->startVertices(VERTEXMODE_QUAD);

	vec_t x = mPosition.x;
	for (unsigned int i = 0; i < mContents.length(); i++)
		x += _drawChar(x, mPosition.y, mContents[i]);

	rs->endVertices();
	rs->setDepthMask(true);

	mMaterial->finish();
}

}

