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
	_setSkinData();
}

panel::panel(const vector2& pos, const vector2& dimension)
{
	mRectangle = rectangle(pos, dimension);
	_setSkinData();
}
			
void panel::_setSkinData()
{
	guiManager* gui = &guiManager::getSingleton();
	kAssert(gui);

	mSkinMaterial = gui->getSkinMaterial();
	mSkinDimensions = gui->getSkinDimensions();

	mSkin[E_TOPLEFT] = gui->getSkinDefinition("WindowTopLeft");
	mSkin[E_TOPRIGHT] = gui->getSkinDefinition("WindowTopRight");
	mSkin[E_TOPMIDDLE] = gui->getSkinDefinition("WindowTopEdge");

	mSkin[E_LEFT] = gui->getSkinDefinition("WindowLeftEdge");
	mSkin[E_RIGHT] = gui->getSkinDefinition("WindowRightEdge");
	mSkin[E_MIDDLE] = gui->getSkinDefinition("ClientBrush");

	mSkin[E_BOTTOMLEFT] = gui->getSkinDefinition("WindowBottomLeft");
	mSkin[E_BOTTOMRIGHT] = gui->getSkinDefinition("WindowBottomRight");
	mSkin[E_BOTTOMMIDDLE] = gui->getSkinDefinition("WindowBottomEdge");
}

void panel::draw()
{
	if (!mSkinMaterial)
		return;
	
	renderSystem* rs = root::getSingleton().getRenderSystem();

	mSkinMaterial->start();
	rs->setCulling(CULLMODE_NONE);

	rs->setDepthMask(false);
	rs->startVertices(VERTEXMODE_QUAD);

	// Top Left
	if (mSkin[E_TOPLEFT])
	{
		rectangle* rect = mSkin[E_TOPLEFT];

		rs->texCoord(rect->getPosition());
		rs->vertex(vector3(0, 0, -0.5));

		rs->texCoord(vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y));
		rs->vertex(vector3(rect->getDimension().x * mSkinDimensions.x, 0, -0.5));

		rs->texCoord(vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y));
		rs->vertex(vector3(rect->getDimension().x * mSkinDimensions.x, rect->getDimension().y * mSkinDimensions.y, -0.5));

		rs->texCoord(vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y));
		rs->vertex(vector3(0, rect->getDimension().y * mSkinDimensions.y, -0.5));
	}

	// Top Middle
	if (mSkin[E_TOPLEFT] && mSkin[E_TOPMIDDLE] && mSkin[E_TOPRIGHT])
	{
		rectangle* leftrect = mSkin[E_TOPLEFT];
		rectangle* rightrect = mSkin[E_TOPRIGHT];
		rectangle* rect = mSkin[E_TOPMIDDLE];

		vec_t x = leftrect->getDimension().x * mSkinDimensions.x;
		vec_t w = mRectangle.getDimension().x - (rightrect->getDimension().x + leftrect->getDimension().x) * mSkinDimensions.x;

		rs->texCoord(rect->getPosition());
		rs->vertex(vector3(x, 0, -0.5));

		rs->texCoord(vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y));
		rs->vertex(vector3(x + w, 0, -0.5));

		rs->texCoord(vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y));
		rs->vertex(vector3(x + w, rect->getDimension().y * mSkinDimensions.y, -0.5));

		rs->texCoord(vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y));
		rs->vertex(vector3(x, rect->getDimension().y * mSkinDimensions.y, -0.5));
	}

	// Top Right
	if (mSkin[E_TOPRIGHT])
	{
		rectangle* rect = mSkin[E_TOPRIGHT];

		vec_t x = mRectangle.getDimension().x - rect->getDimension().x * mSkinDimensions.x;

		rs->texCoord(rect->getPosition());
		rs->vertex(vector3(x, 0, -0.5));

		rs->texCoord(vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y));
		rs->vertex(vector3(x + rect->getDimension().x * mSkinDimensions.x, 0, -0.5));

		rs->texCoord(vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y));
		rs->vertex(vector3(x + rect->getDimension().x * mSkinDimensions.x, rect->getDimension().y * mSkinDimensions.y, -0.5));

		rs->texCoord(vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y));
		rs->vertex(vector3(x, rect->getDimension().y * mSkinDimensions.y, -0.5));
	}

	// Bottom Left
	if (mSkin[E_BOTTOMLEFT])
	{
		rectangle* rect = mSkin[E_BOTTOMLEFT];
		vec_t y = mRectangle.getDimension().y - rect->getDimension().y * mSkinDimensions.y;

		rs->texCoord(rect->getPosition());
		rs->vertex(vector3(0, y, -0.5));

		rs->texCoord(vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y));
		rs->vertex(vector3(rect->getDimension().x * mSkinDimensions.x, y, -0.5));

		rs->texCoord(vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y));
		rs->vertex(vector3(rect->getDimension().x * mSkinDimensions.x, y + rect->getDimension().y * mSkinDimensions.y, -0.5));

		rs->texCoord(vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y));
		rs->vertex(vector3(0, y + rect->getDimension().y * mSkinDimensions.y, -0.5));
	}

	// Bottom Middle
	if (mSkin[E_BOTTOMLEFT] && mSkin[E_BOTTOMMIDDLE] && mSkin[E_BOTTOMRIGHT])
	{
		rectangle* leftrect = mSkin[E_BOTTOMLEFT];
		rectangle* rightrect = mSkin[E_BOTTOMRIGHT];
		rectangle* rect = mSkin[E_BOTTOMMIDDLE];

		vec_t x = leftrect->getDimension().x * mSkinDimensions.x;
		vec_t y = mRectangle.getDimension().y - rect->getDimension().y * mSkinDimensions.y;
		vec_t w = mRectangle.getDimension().x - (rightrect->getDimension().x + leftrect->getDimension().x) * mSkinDimensions.x;

		rs->texCoord(rect->getPosition());
		rs->vertex(vector3(x, y, -0.5));

		rs->texCoord(vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y));
		rs->vertex(vector3(x + w, y, -0.5));

		rs->texCoord(vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y));
		rs->vertex(vector3(x + w, y + rect->getDimension().y * mSkinDimensions.y, -0.5));

		rs->texCoord(vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y));
		rs->vertex(vector3(x, y + rect->getDimension().y * mSkinDimensions.y, -0.5));
	}

	// Bottom Right
	if (mSkin[E_BOTTOMRIGHT])
	{
		rectangle* rect = mSkin[E_BOTTOMRIGHT];

		vec_t x = mRectangle.getDimension().x - rect->getDimension().x * mSkinDimensions.x;
		vec_t y = mRectangle.getDimension().y - rect->getDimension().y * mSkinDimensions.y;

		rs->texCoord(rect->getPosition());
		rs->vertex(vector3(x, y, -0.5));

		rs->texCoord(vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y));
		rs->vertex(vector3(x + rect->getDimension().x * mSkinDimensions.x, y, -0.5));

		rs->texCoord(vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y));
		rs->vertex(vector3(x + rect->getDimension().x * mSkinDimensions.x, y + rect->getDimension().y * mSkinDimensions.y, -0.5));

		rs->texCoord(vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y));
		rs->vertex(vector3(x, y + rect->getDimension().y * mSkinDimensions.y, -0.5));
	}

	// Middle Left
	if (mSkin[E_LEFT] && mSkin[E_TOPLEFT] && mSkin[E_BOTTOMLEFT])
	{
		rectangle* leftrect = mSkin[E_TOPLEFT];
		rectangle* rect = mSkin[E_LEFT];
		rectangle* bottomrect = mSkin[E_BOTTOMLEFT];

		vec_t y = leftrect->getDimension().y * mSkinDimensions.y;
		vec_t h = mRectangle.getDimension().y - y - (bottomrect->getDimension().y * mSkinDimensions.y);

		rs->texCoord(rect->getPosition());
		rs->vertex(vector3(0, y, -0.5));

		rs->texCoord(vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y));
		rs->vertex(vector3(rect->getDimension().x * mSkinDimensions.x, y, -0.5));

		rs->texCoord(vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y));
		rs->vertex(vector3(rect->getDimension().x * mSkinDimensions.x, y + h, -0.5));

		rs->texCoord(vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y));
		rs->vertex(vector3(0, y + h, -0.5));
	}

	// Middle
	if (mSkin[E_TOPLEFT] && mSkin[E_BOTTOMMIDDLE] && mSkin[E_MIDDLE] &&
		 mSkin[E_RIGHT] && mSkin[E_LEFT])
	{
		rectangle* toprect = mSkin[E_TOPLEFT];
		rectangle* bottomrect = mSkin[E_BOTTOMMIDDLE];
		rectangle* leftrect = mSkin[E_LEFT];
		rectangle* rightrect = mSkin[E_RIGHT];
		rectangle* rect = mSkin[E_MIDDLE];

		vec_t x = toprect->getDimension().x * mSkinDimensions.x;
		vec_t y = toprect->getDimension().y * mSkinDimensions.y;

		vec_t w = mRectangle.getDimension().x - (rightrect->getDimension().x + leftrect->getDimension().x) * mSkinDimensions.x;
		vec_t h = mRectangle.getDimension().y - (bottomrect->getDimension().y + toprect->getDimension().y) * mSkinDimensions.y;

		rs->texCoord(rect->getPosition());
		rs->vertex(vector3(x, y, -0.5));

		rs->texCoord(vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y));
		rs->vertex(vector3(x + w, y, -0.5));

		rs->texCoord(vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y));
		rs->vertex(vector3(x + w, y + h, -0.5));

		rs->texCoord(vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y));
		rs->vertex(vector3(x, y + h, -0.5));
	}

	// Middle Right 
	if (mSkin[E_RIGHT] && mSkin[E_TOPRIGHT] && mSkin[E_BOTTOMRIGHT])
	{
		rectangle* leftrect = mSkin[E_TOPRIGHT];
		rectangle* rect = mSkin[E_RIGHT];
		rectangle* bottomrect = mSkin[E_BOTTOMRIGHT];

		vec_t x = mRectangle.getDimension().x - rect->getDimension().x * mSkinDimensions.x;
		vec_t y = leftrect->getDimension().y * mSkinDimensions.y;
		vec_t h = mRectangle.getDimension().y - y - (bottomrect->getDimension().y * mSkinDimensions.y);

		rs->texCoord(rect->getPosition());
		rs->vertex(vector3(x, y, -0.5));

		rs->texCoord(vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y));
		rs->vertex(vector3(x + rect->getDimension().x * mSkinDimensions.x, y, -0.5));

		rs->texCoord(vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y));
		rs->vertex(vector3(x + rect->getDimension().x * mSkinDimensions.x, y + h, -0.5));

		rs->texCoord(vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y));
		rs->vertex(vector3(x, y + h, -0.5));
	}

	rs->endVertices();
	rs->setDepthMask(true);

	// Finish
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
