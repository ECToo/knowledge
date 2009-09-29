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

#include "fontManager.h"
#include "logger.h"
#include "root.h"
#include "textureManager.h"
#include "resourceManager.h"

namespace k {

// Helper function
int readEndianSafeInt(FILE* f)
{
	int value;
	kAssert(f != NULL);

	if (fread(&value, sizeof(int), 1, f) > 0)
		return readLEInt(value);
	else
		return 0;
}

float readEndianSafeFloat(FILE* f)
{
	kAssert(f != NULL);

	float data;
	if (fread(&data, sizeof(float), 1, f) > 0)
	{
		return readLEFloat(data);
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

	// No Parent
	mDrawableAttach = NULL;

	std::string fullPath = datName;
	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc) fullPath = rsc->getRoot() + datName;

	FILE* datFile = fopen(fullPath.c_str(), "rb");
	if (!datFile)
	{
		S_LOG_INFO("Failed to open dat file " + datName);
		return;
	}

	mMaxHeight = 0;
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

		if (glyph->height > mMaxHeight)
			mMaxHeight = glyph->height;

		// TODO: We will need the image name
		// for larger paged fonts.
		//
		// Skip unneeded data like image name.
		fseek(datFile, 36, SEEK_CUR);
	}

	// Line Height
	mLineHeight = mMaxHeight;

	// A good line diff
	mMaxHeight += mMaxHeight * 0.5f;

	// Clear Contents
	mContents.clear();
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

	vec_t width = 0;
	vec_t height = 0;
	int baseHeight = 0;

	for (unsigned int i = 0; i < mContents.length(); i++)
	{
		if (mContents[i] == '\n')
			height += mMaxHeight;
		else
		{
			width += mGlyphs[(int)mContents[i]].xSkip;
			baseHeight = std::max(baseHeight, mGlyphs[(int)mContents[i]].imgHeight);
		}
	}

	mRectangle.setDimension(vector2(width, baseHeight + height));
	mDrawRegionSize = vector2(width, baseHeight + height);
}

void bitmapText::draw()
{
	if (!mContents.length())
		return;

	renderSystem* rs = root::getSingleton().getRenderSystem();

	// Material setup
	mMaterial->start();

	// Indepedent of material
	rs->setCulling(CULLMODE_NONE);
	rs->setDepthMask(false);

	rs->startVertices(VERTEXMODE_QUAD);

	vec_t x = mRectangle.getPosition().x;
	vec_t y = mRectangle.getPosition().y;

	for (unsigned int i = 0; i < mContents.length(); i++)
	{
		if (mContents[i] == '\n')
		{
			y += mMaxHeight;
			x = mRectangle.getPosition().x;
		}
		else
		{
			x += _drawChar(x, y, mContents[i]);
		}
	}

	rs->endVertices();

	// Take it back
	mMaterial->finish();
	rs->setDepthMask(true);
}

}

