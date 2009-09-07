/*
    Copyright (C) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

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

#include "guiManager.h"
#include "inputManager.h"
#include "logger.h"
#include "renderer.h"
#include "root.h"
#include "resourceManager.h"
#include "tinystr.h"
#include "tinyxml.h"

namespace k {

const char* panelEdges[] =
{
	"WindowTopLeft",
	"WindowTopEdge",
	"WindowTopRight",

	"WindowLeftEdge",
	"ClientBrush",
	"WindowRightEdge",

	"WindowBottomLeft",
	"WindowBottomEdge",
	"WindowBottomRight"
};
			
void signalKeeper::connect(const std::string& sname, signalHandlerInfo_t* info)
{
	kAssert(info);
	mSignals[sname] = info;
}
			
void signalKeeper::disconnect(const std::string& sname)
{
	std::map<std::string, signalHandlerInfo_t*>::iterator sIt;

	sIt = mSignals.find(sname);
	if (sIt != mSignals.end())
	{
		delete sIt->second;
		mSignals.erase(sIt);
	}
}
			
void signalKeeper::callSignal(const std::string& sname, signalInfo_t signalInfo)
{
	std::map<std::string, signalHandlerInfo_t*>::iterator sIt;

	sIt = mSignals.find(sname);
	if (sIt != mSignals.end())
	{
		signalHandlerInfo_t* info = sIt->second;
		(info->handler->*info->function)(signalInfo, info->userData);
	}
}
			
panel::panel()
{
	mVertices = NULL;
	mUvs = NULL;

	_setSkinData(panelEdges);
}

panel::panel(const vector2& pos, const vector2& dimension)
{
	mVertices = NULL;
	mUvs = NULL;

	mRectangle = rectangle(pos, dimension);
	_setSkinData(panelEdges);
}

panel::~panel()
{
	while (!mChilds.empty())
	{
		std::vector<drawable2D*>::iterator it = mChilds.begin();
		delete (*it);
		mChilds.erase(it);
	}

	if (mVertices)
		free(mVertices);

	if (mUvs)
		free(mUvs);
}
			
void panel::_setSkinData(const char** skinNamesArray)
{
	guiManager* gui = &guiManager::getSingleton();

	mSkinMaterial = gui->getSkinMaterial();
	mSkinDimensions = gui->getSkinDimensions();

	bool missing = false;
	for (int i = 0; i < E_MAX_EDGES; i++)
	{
		mSkin[i] = gui->getSkinDefinition(panelEdges[i]);

		if (!mSkin[i])
		{
			S_LOG_INFO("Missing skin component for panel skin " + std::string(panelEdges[i]));
			missing = true;
		}
	}
		
	if (missing)
		return;

	if (!mVertices)
		mVertices = (vector3*) memalign(32, sizeof(vector3) * 4 * 9);

	if (!mUvs)
		mUvs = (vector2*) memalign(32, sizeof(vector2) * 4 * 9);

	if (!mVertices)
	{
		S_LOG_INFO("Failed to allocate widget vertices.");
		return;
	}

	if (!mUvs)
	{
		S_LOG_INFO("Failed to allocate widget uvs.");
		return;
	}
		
	// Aux vars;
	rectangle* rect = NULL;
	rectangle* toprect = NULL;
	rectangle* bottomrect = NULL;
	rectangle* leftrect = NULL;
	rectangle* rightrect = NULL;
	vec_t x, y, w, h;

	// Top Left
	rect = mSkin[E_TOPLEFT];
		
	mUvs[0] = rect->getPosition();
	mVertices[0] = vector3(0, 0, -0.5);

	mUvs[1] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y);
	mVertices[1] = vector3(rect->getDimension().x * mSkinDimensions.x, 0, -0.5);

	mUvs[2] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[2] = vector3(rect->getDimension().x * mSkinDimensions.x, rect->getDimension().y * mSkinDimensions.y, -0.5);

	mUvs[3] = vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[3] = vector3(0, rect->getDimension().y * mSkinDimensions.y, -0.5);

	// Top Middle
	leftrect = mSkin[E_TOPLEFT];
	rightrect = mSkin[E_TOPRIGHT];
	rect = mSkin[E_TOPMIDDLE];

	x = leftrect->getDimension().x * mSkinDimensions.x;
	w = mRectangle.getDimension().x - (rightrect->getDimension().x + leftrect->getDimension().x) * mSkinDimensions.x;

	mUvs[4] = rect->getPosition();
	mVertices[4] = vector3(x, 0, -0.5);

	mUvs[5] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y);
	mVertices[5] = vector3(x + w, 0, -0.5);

	mUvs[6] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[6] = vector3(x + w, rect->getDimension().y * mSkinDimensions.y, -0.5);

	mUvs[7] = vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[7] = vector3(x, rect->getDimension().y * mSkinDimensions.y, -0.5);

	// Top Right
	rect = mSkin[E_TOPRIGHT];
	x = mRectangle.getDimension().x - rect->getDimension().x * mSkinDimensions.x;

	mUvs[8] = rect->getPosition();
	mVertices[8] = vector3(x, 0, -0.5);

	mUvs[9] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y);
	mVertices[9] = vector3(x + rect->getDimension().x * mSkinDimensions.x, 0, -0.5);

	mUvs[10] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[10] = vector3(x + rect->getDimension().x * mSkinDimensions.x, rect->getDimension().y * mSkinDimensions.y, -0.5);

	mUvs[11] = vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[11] = vector3(x, rect->getDimension().y * mSkinDimensions.y, -0.5);

	// Bottom Left
	rect = mSkin[E_BOTTOMLEFT];
	y = mRectangle.getDimension().y - rect->getDimension().y * mSkinDimensions.y;

	mUvs[12] = rect->getPosition();
	mVertices[12] = vector3(0, y, -0.5);

	mUvs[13] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y);
	mVertices[13] = vector3(rect->getDimension().x * mSkinDimensions.x, y, -0.5);

	mUvs[14] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[14] = vector3(rect->getDimension().x * mSkinDimensions.x, y + rect->getDimension().y * mSkinDimensions.y, -0.5);

	mUvs[15] = vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[15] = vector3(0, y + rect->getDimension().y * mSkinDimensions.y, -0.5);

	// Bottom Middle
	leftrect = mSkin[E_BOTTOMLEFT];
	rightrect = mSkin[E_BOTTOMRIGHT];
	rect = mSkin[E_BOTTOMMIDDLE];

	x = leftrect->getDimension().x * mSkinDimensions.x;
	y = mRectangle.getDimension().y - rect->getDimension().y * mSkinDimensions.y;
	w = mRectangle.getDimension().x - (rightrect->getDimension().x + leftrect->getDimension().x) * mSkinDimensions.x;

	mUvs[16] = rect->getPosition();
	mVertices[16] = vector3(x, y, -0.5);

	mUvs[17] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y);
	mVertices[17] = vector3(x + w, y, -0.5);

	mUvs[18] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[18] = vector3(x + w, y + rect->getDimension().y * mSkinDimensions.y, -0.5);

	mUvs[19] = vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[19] = vector3(x, y + rect->getDimension().y * mSkinDimensions.y, -0.5);

	// Bottom Right
	rect = mSkin[E_BOTTOMRIGHT];

	x = mRectangle.getDimension().x - rect->getDimension().x * mSkinDimensions.x;
	y = mRectangle.getDimension().y - rect->getDimension().y * mSkinDimensions.y;

	mUvs[20] = rect->getPosition();
	mVertices[20] = vector3(x, y, -0.5);

	mUvs[21] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y);
	mVertices[21] = vector3(x + rect->getDimension().x * mSkinDimensions.x, y, -0.5);

	mUvs[22] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[22] = vector3(x + rect->getDimension().x * mSkinDimensions.x, y + rect->getDimension().y * mSkinDimensions.y, -0.5);

	mUvs[23] = vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[23] = vector3(x, y + rect->getDimension().y * mSkinDimensions.y, -0.5);

	// Middle Left
	leftrect = mSkin[E_TOPLEFT];
	rect = mSkin[E_LEFT];
	bottomrect = mSkin[E_BOTTOMLEFT];

	y = leftrect->getDimension().y * mSkinDimensions.y;
	h = mRectangle.getDimension().y - y - (bottomrect->getDimension().y * mSkinDimensions.y);

	mUvs[24] = rect->getPosition();
	mVertices[24] = vector3(0, y, -0.5);

	mUvs[25] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y);
	mVertices[25] = vector3(rect->getDimension().x * mSkinDimensions.x, y, -0.5);

	mUvs[26] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[26] = vector3(rect->getDimension().x * mSkinDimensions.x, y + h, -0.5);

	mUvs[27] = vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[27] = vector3(0, y + h, -0.5);

	// Middle
	toprect = mSkin[E_TOPLEFT];
	bottomrect = mSkin[E_BOTTOMMIDDLE];
	leftrect = mSkin[E_LEFT];
	rightrect = mSkin[E_RIGHT];
	rect = mSkin[E_MIDDLE];

	x = toprect->getDimension().x * mSkinDimensions.x;
	y = toprect->getDimension().y * mSkinDimensions.y;

	w = mRectangle.getDimension().x - (rightrect->getDimension().x + leftrect->getDimension().x) * mSkinDimensions.x;
	h = mRectangle.getDimension().y - (bottomrect->getDimension().y + toprect->getDimension().y) * mSkinDimensions.y;

	mUvs[28] = rect->getPosition();
	mVertices[28] = vector3(x, y, -0.5);

	mUvs[29] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y);
	mVertices[29] = vector3(x + w, y, -0.5);

	mUvs[30] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[30] = vector3(x + w, y + h, -0.5);

	mUvs[31] = vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[31] = vector3(x, y + h, -0.5);

	// Middle Right 
	leftrect = mSkin[E_TOPRIGHT];
	rect = mSkin[E_RIGHT];
	bottomrect = mSkin[E_BOTTOMRIGHT];

	x = mRectangle.getDimension().x - rect->getDimension().x * mSkinDimensions.x;
	y = leftrect->getDimension().y * mSkinDimensions.y;
	h = mRectangle.getDimension().y - y - (bottomrect->getDimension().y * mSkinDimensions.y);

	mUvs[32] = rect->getPosition();
	mVertices[32] = vector3(x, y, -0.5);

	mUvs[33] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y);
	mVertices[33] = vector3(x + rect->getDimension().x * mSkinDimensions.x, y, -0.5);

	mUvs[34] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[34] = vector3(x + rect->getDimension().x * mSkinDimensions.x, y + h, -0.5);

	mUvs[35] = vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[35] = vector3(x, y + h, -0.5);
}

void panel::draw()
{
	if (!mSkinMaterial || !mUvs || !mVertices)
		return;
	
	const index_t mIndices[] ATTRIBUTE_ALIGN(32) = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
		11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
		26, 27, 28, 29, 30, 31, 32, 33, 34, 35 };

	renderSystem* rs = root::getSingleton().getRenderSystem();

	mSkinMaterial->start();

	rs->setCulling(CULLMODE_NONE);
	rs->setDepthMask(false);

	rs->clearArrayDesc(VERTEXMODE_QUAD);
	rs->setVertexArray(mVertices[0].vec);
	rs->setVertexCount(36);

	rs->setTexCoordArray(mUvs[0].vec);
	rs->setVertexIndex(mIndices);
	rs->setIndexCount(36);

	rs->drawArrays();

	rs->setDepthMask(true);
	mSkinMaterial->finish();
}

template<> guiManager* singleton<guiManager>::singleton_instance = 0;

guiManager& guiManager::getSingleton()
{  
	kAssert(singleton_instance);
	return (*singleton_instance);  
}

guiManager::guiManager()
{
	mCursor = NULL;
}

guiManager::~guiManager()
{
	if (mCursor)
	{
		renderer::getSingleton().pop2D(mCursor);
		delete mCursor;
	}
}

void guiManager::setCursor(const std::string& mat, const vector2& scale)
{
	try
	{
		mCursor = new sticker(mat);
		mCursor->setScale(scale);
		mCursor->setZ(9999);

		renderer::getSingleton().push2D(mCursor);
		root::getSingleton().getRenderSystem()->showCursor(false);
	}
	
	catch (...)
	{
		S_LOG_INFO("Failed to create cursor sticker.");
	}
}
			
const vector2 guiManager::getCursorPosition() const
{
	if (mCursor)
		return mCursor->getAbsolutePosition();
	else
		return k::vector2(0,0);
}
			
const vector2& guiManager::getCursorDeltaPosition() const
{
	return mCursorDeltaPos;
}

void guiManager::update()
{
	if (mCursor)
	{
		vector2 newPos = inputManager::getSingleton().getWiiMotePosition(0);
		mCursorDeltaPos = newPos - mCursorLastPos;
		mCursorLastPos = newPos;

		mCursor->setPosition(mCursorLastPos);
	}
}
			
rectangle* guiManager::getSkinDefinition(const std::string& wname) const
{
	std::map<std::string, rectangle*>::const_iterator it = mSkinDefinitions.find(wname);
	if (it != mSkinDefinitions.end())
	{
		return it->second;
	}
	else
	{
		return NULL;
	}
}
			
void guiManager::setSkin(const std::string& matName, const std::string& guiFile)
{
	std::string fullPath;

	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc) 
	{
		fullPath = rsc->getRoot() + guiFile;
	}
	else
	{
		fullPath = guiFile;
	}

	material* guiMaterial = materialManager::getSingleton().getMaterial(matName);

	TiXmlDocument xmlDoc(fullPath.c_str());
	if (!xmlDoc.LoadFile())
	{
		S_LOG_INFO("Failed to load skin definitions " + guiFile + ".");
		return;
	}

	if (!guiMaterial)
	{
		S_LOG_INFO("Failed to find gui skin material, aborting.");
		return;
	}

	mGuiSkin = guiMaterial;
	const materialStage* matStage = guiMaterial->getStage(0);

	if (!matStage)
	{
		S_LOG_INFO("Skin material has no stages!");
		kAssert(matStage);
	}

	vec_t w = (vec_t)matStage->getWidth();
	vec_t h = (vec_t)matStage->getHeight();

	mSkinDimensions.x = w;
	mSkinDimensions.y = h;

	// Parse Definitions
	const TiXmlElement* rootElement = xmlDoc.RootElement();
	const TiXmlElement* nextElement = rootElement->FirstChildElement();
		
	while (nextElement)
	{
		rectangle* elementRect;

		try
		{
			elementRect = new rectangle();
			elementRect->setPosition(vector2(atoi(nextElement->Attribute("XPos")) / w, atoi(nextElement->Attribute("YPos")) / h));
			elementRect->setDimension(vector2(atoi(nextElement->Attribute("Width")) / w, atoi(nextElement->Attribute("Height")) / h));

			mSkinDefinitions[nextElement->Attribute("Name")] = elementRect;
		}

		catch (...)
		{
			S_LOG_INFO("Failed to allocate rectangle for gui definition " + std::string(nextElement->Attribute("Name")));
		}

		nextElement = nextElement->NextSiblingElement();
	}
}

}
