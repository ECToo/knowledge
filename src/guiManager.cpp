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

#include "guiManager.h"
#include "inputManager.h"
#include "logger.h"
#include "renderer.h"
#include "root.h"
#include "resourceManager.h"
#include "tinystr.h"
#include "tinyxml.h"

namespace k {

const char* panelWidgetEdges[] =
{
	"WindowTopLeft",
	"WindowTopEdge",
	"WindowTopRight",

	"WindowLeftEdge",
	"ClientBrush",
	"WindowRightEdge",

	"WindowBottomLeft",
	"WindowBottomEdge",
	"WindowBottomRight",
	NULL
};

const char* buttonWidgetEdges[] =
{
	"ButtonLeftNormal",
	"ButtonMiddleNormal",
	"ButtonRightNormal",
	NULL
};

const char* buttonHighlightWidgetEdges[] =
{
	"ButtonLeftHighlight",
	"ButtonMiddleHighlight",
	"ButtonRightHighlight",
	NULL
};

const char* buttonPushedWidgetEdges[] =
{
	"ButtonLeftPushed",
	"ButtonMiddlePushed",
	"ButtonRightPushed",
	NULL
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
			
bool signalKeeper::callSignal(const std::string& sname, signalInfo_t signalInfo)
{
	std::map<std::string, signalHandlerInfo_t*>::iterator sIt;
	std::map<std::string, signalHandlerInfo_t*>::iterator lIt;

	lIt = mInternalSignals.find(sname);
	if (lIt != mInternalSignals.end())
	{
		signalHandlerInfo_t* info = lIt->second;

		// Check if handler blocks the signal
		if ((info->handler->*info->function)(signalInfo, info->userData))
			return true;
	}

	sIt = mSignals.find(sname);
	if (sIt != mSignals.end())
	{
		signalHandlerInfo_t* info = sIt->second;

		// Check if handler blocks the signal
		if ((info->handler->*info->function)(signalInfo, info->userData))
			return true;
	}

	return false;
}
			
bool widget::isPointerInside(const vector2& pos) const
{
	const vector2& rectPos = mRectangle.getPosition();
	const vector2& rectDimension = mRectangle.getDimension();

	if (pos.x < rectPos.x || pos.x > (rectPos.x + rectDimension.x) ||
		 pos.y < rectPos.y || pos.y > (rectPos.y + rectDimension.y))
	{
		return false;
	}
	else
	{
		return true;
	}
}

void panelSkin::setSkinData(const char** skinArray, rectangle* parentRect)
{
	guiManager* gui = &guiManager::getSingleton();

	mSkinMaterial = gui->getSkinMaterial();
	mSkinDimensions = gui->getSkinDimensions();

	bool missing = false;
	for (int i = 0; i < E_MAX_EDGES; i++)
	{
		mSkin[i] = gui->getSkinDefinition(skinArray[i]);

		if (!mSkin[i])
		{
			S_LOG_INFO("Missing skin component for panelWidget skin " + std::string(skinArray[i]));
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
	w = parentRect->getDimension().x - (rightrect->getDimension().x + leftrect->getDimension().x) * mSkinDimensions.x;

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
	x = parentRect->getDimension().x - rect->getDimension().x * mSkinDimensions.x;

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
	y = parentRect->getDimension().y - rect->getDimension().y * mSkinDimensions.y;

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
	y = parentRect->getDimension().y - rect->getDimension().y * mSkinDimensions.y;
	w = parentRect->getDimension().x - (rightrect->getDimension().x + leftrect->getDimension().x) * mSkinDimensions.x;

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

	x = parentRect->getDimension().x - rect->getDimension().x * mSkinDimensions.x;
	y = parentRect->getDimension().y - rect->getDimension().y * mSkinDimensions.y;

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
	h = parentRect->getDimension().y - y - (bottomrect->getDimension().y * mSkinDimensions.y);

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

	w = parentRect->getDimension().x - (rightrect->getDimension().x + leftrect->getDimension().x) * mSkinDimensions.x;
	h = parentRect->getDimension().y - (bottomrect->getDimension().y + toprect->getDimension().y) * mSkinDimensions.y;

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

	x = parentRect->getDimension().x - rect->getDimension().x * mSkinDimensions.x;
	y = leftrect->getDimension().y * mSkinDimensions.y;
	h = parentRect->getDimension().y - y - (bottomrect->getDimension().y * mSkinDimensions.y);

	mUvs[32] = rect->getPosition();
	mVertices[32] = vector3(x, y, -0.5);

	mUvs[33] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y);
	mVertices[33] = vector3(x + rect->getDimension().x * mSkinDimensions.x, y, -0.5);

	mUvs[34] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[34] = vector3(x + rect->getDimension().x * mSkinDimensions.x, y + h, -0.5);

	mUvs[35] = vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[35] = vector3(x, y + h, -0.5);
}

void panelSkin::drawWidgetSkin(const vector2& pos)
{
	if (!mSkinMaterial || !mUvs || !mVertices)
		return;
	
	const index_t mIndices[] ATTRIBUTE_ALIGN(32) = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
		11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
		26, 27, 28, 29, 30, 31, 32, 33, 34, 35 };

	renderSystem* rs = root::getSingleton().getRenderSystem();

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->translateScene(pos.x, pos.y, 0);

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

bool widget::mousemove(signalInfo_t info, void* userData)
{
	signalInfo_t newInfo;
	newInfo.mouseState.mPosition = info.mouseState.mPosition;
	newInfo.mouseState.buttons = info.mouseState.buttons;
	newInfo.key = info.key;

	if (isPointerInside(info.mouseState.mPosition))
	{
		if (!mMouseIn)
		{
			newInfo.type = SIGNAL_MOUSEIN;

			mMouseIn = true;
			callSignal("mouseIn", newInfo);
		}
	}
	else
	{
		if (mMouseIn)
		{
			newInfo.type = SIGNAL_MOUSEOUT;

			mMouseIn = false;
			callSignal("mouseOut", newInfo);
		}
	}

	return false;
}

panelWidget::panelWidget() : panelSkin()
{
	_registerSignals();
	setSkinData(panelWidgetEdges, &mRectangle);
}

panelWidget::panelWidget(const vector2& pos, const vector2& dimension) : panelSkin()
{
	mRectangle = rectangle(pos, dimension);
	_registerSignals();

	setSkinData(panelWidgetEdges, &mRectangle);
}

panelWidget::~panelWidget()
{
	while (!mChilds.empty())
	{
		std::vector<drawable2D*>::iterator it = mChilds.begin();
		delete (*it);
		mChilds.erase(it);
	}
}
			
void panelWidget::draw()
{
	// Only Draw panelWidget skin for now.
	drawWidgetSkin(mRectangle.getPosition());
}

void panelWidget::_registerSignals()
{
	// mouse move
	try
	{
		signalHandlerInfo_t* info = new signalHandlerInfo_t;
		info->handler = this;
		info->function = (signalFunctionPtr)(&k::panelWidget::mousemove);
		info->userData = NULL;

		mInternalSignals["mouseMove"] = info;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to register panel mousemove signal.");
	}

	// mouse in
	try
	{
		signalHandlerInfo_t* info = new signalHandlerInfo_t;
		info->handler = this;
		info->function = (signalFunctionPtr)(&k::panelWidget::mousein);
		info->userData = NULL;

		mInternalSignals["mouseIn"] = info;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to register panel mousein signal.");
	}

	// mouse out
	try
	{
		signalHandlerInfo_t* info = new signalHandlerInfo_t;
		info->handler = this;
		info->function = (signalFunctionPtr)(&k::panelWidget::mouseout);
		info->userData = NULL;

		mInternalSignals["mouseOut"] = info;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to register panel mousein signal.");
	}
}
			
bool panelWidget::mousein(signalInfo_t info, void* userData)
{
	return false;
}

bool panelWidget::mouseout(signalInfo_t info, void* userData)
{
	return false;
}

bool panelWidget::mousemove(signalInfo_t info, void* userData)
{
	widget::mousemove(info, userData);

	return false;
}

void buttonSkin::setSkinData(const char** skinArray, rectangle* parentRect)
{
	guiManager* gui = &guiManager::getSingleton();

	mSkinMaterial = gui->getSkinMaterial();
	mSkinDimensions = gui->getSkinDimensions();

	bool missing = false;
	for (int i = 0; i < 3; i++)
	{
		mSkin[i] = gui->getSkinDefinition(skinArray[i]);

		if (!mSkin[i])
		{
			S_LOG_INFO("Missing skin component for button skin " + std::string(skinArray[i]));
			missing = true;
		}
	}
		
	if (missing)
		return;

	if (!mVertices)
		mVertices = (vector3*) memalign(32, sizeof(vector3) * 4 * 3);

	if (!mUvs)
		mUvs = (vector2*) memalign(32, sizeof(vector2) * 4 * 3);

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
	rectangle* rect = mSkin[1];
	rectangle* leftrect = mSkin[0];
	rectangle* rightrect = mSkin[2];
	vec_t x, w;

	// Left
	mUvs[0] = leftrect->getPosition();
	mVertices[0] = vector3(0, 0, -0.5);

	mUvs[1] = vector2(leftrect->getPosition().x + leftrect->getDimension().x, leftrect->getPosition().y);
	mVertices[1] = vector3(leftrect->getDimension().x * mSkinDimensions.x, 0, -0.5);

	mUvs[2] = vector2(leftrect->getPosition().x + leftrect->getDimension().x, leftrect->getPosition().y + leftrect->getDimension().y);
	mVertices[2] = vector3(leftrect->getDimension().x * mSkinDimensions.x, leftrect->getDimension().y * mSkinDimensions.y, -0.5);

	mUvs[3] = vector2(leftrect->getPosition().x, leftrect->getPosition().y + leftrect->getDimension().y);
	mVertices[3] = vector3(0, leftrect->getDimension().y * mSkinDimensions.y, -0.5);

	// Middle
	x = leftrect->getDimension().x * mSkinDimensions.x;
	w = parentRect->getDimension().x - ((leftrect->getDimension().x + rightrect->getDimension().x) * mSkinDimensions.x);

	mUvs[4] = rect->getPosition();
	mVertices[4] = vector3(x, 0, -0.5);

	mUvs[5] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y);
	mVertices[5] = vector3(x + w, 0, -0.5);

	mUvs[6] = vector2(rect->getPosition().x + rect->getDimension().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[6] = vector3(x + w, rect->getDimension().y * mSkinDimensions.y, -0.5);

	mUvs[7] = vector2(rect->getPosition().x, rect->getPosition().y + rect->getDimension().y);
	mVertices[7] = vector3(x, rect->getDimension().y * mSkinDimensions.y, -0.5);

	// Right
	x = parentRect->getDimension().x - (rightrect->getDimension().x * mSkinDimensions.x);

	mUvs[8] = rightrect->getPosition();
	mVertices[8] = vector3(x, 0, -0.5);

	mUvs[9] = vector2(rightrect->getPosition().x + rightrect->getDimension().x, rightrect->getPosition().y);
	mVertices[9] = vector3(x + rightrect->getDimension().x * mSkinDimensions.x, 0, -0.5);

	mUvs[10] = vector2(rightrect->getPosition().x + rightrect->getDimension().x, rightrect->getPosition().y + rightrect->getDimension().y);
	mVertices[10] = vector3(x + rightrect->getDimension().x * mSkinDimensions.x, rightrect->getDimension().y * mSkinDimensions.y, -0.5);

	mUvs[11] = vector2(rightrect->getPosition().x, rightrect->getPosition().y + rightrect->getDimension().y);
	mVertices[11] = vector3(x, rightrect->getDimension().y * mSkinDimensions.y, -0.5);
}

void buttonSkin::drawWidgetSkin(const vector2& pos)
{
	if (!mSkinMaterial || !mUvs || !mVertices)
		return;
	
	const index_t mIndices[] ATTRIBUTE_ALIGN(32) = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

	renderSystem* rs = root::getSingleton().getRenderSystem();

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->translateScene(pos.x, pos.y, 0);

	mSkinMaterial->start();

	rs->setCulling(CULLMODE_NONE);
	rs->setDepthMask(false);

	rs->clearArrayDesc(VERTEXMODE_QUAD);
	rs->setVertexArray(mVertices[0].vec);
	rs->setVertexCount(12);

	rs->setTexCoordArray(mUvs[0].vec);
	rs->setVertexIndex(mIndices);
	rs->setIndexCount(12);

	rs->drawArrays();

	rs->setDepthMask(true);
	mSkinMaterial->finish();
}

buttonWidget::buttonWidget() : buttonSkin()
{
	mMouseWasDown = false;

	_registerSignals();

	setSkinData(buttonWidgetEdges, &mRectangle);
	mHighlightSkin.setSkinData(buttonHighlightWidgetEdges, &mRectangle);

	try
	{
		guiManager* gui = &guiManager::getSingleton();
		mText = new bitmapText(gui->getFontDatFile(), gui->getFontMaterial());
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate button label.");
	}

	mState = STATE_NORMAL;
}

buttonWidget::buttonWidget(const vector2& pos, const vector2& dimension) : buttonSkin()
{
	mMouseWasDown = false;

	mRectangle = rectangle(pos, dimension);
	_registerSignals();

	setSkinData(buttonWidgetEdges, &mRectangle);
	mHighlightSkin.setSkinData(buttonHighlightWidgetEdges, &mRectangle);

	try
	{
		guiManager* gui = &guiManager::getSingleton();
		mText = new bitmapText(gui->getFontDatFile(), gui->getFontMaterial());
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate button label.");
	}

	mState = STATE_NORMAL;
}
			
void buttonWidget::setDimension(const vector2& dimension)
{
	mRectangle.setDimension(dimension);
	setSkinData(buttonWidgetEdges, &mRectangle);
	mHighlightSkin.setSkinData(buttonHighlightWidgetEdges, &mRectangle);
}

buttonWidget::~buttonWidget()
{
	if (mText)
		delete mText;
}
			
void buttonWidget::setText(const std::string& text)
{
	kAssert(mText);
	mText->setText(text);

	// Position the text in the center of the button
	vector2 size = mRectangle.getDimension();
	vector2 textSize = mText->getDimension();
	vector2 newPos;

	size *= 0.5;
	textSize *= 0.5;
	newPos.x = size.x - textSize.x;
	newPos.y = size.y;

	mText->setPosition(newPos);
}

const std::string& buttonWidget::getText() const
{
	kAssert(mText);
	return mText->getText();
}
			
void buttonWidget::draw()
{
	// Draw Skin
	switch (mState)
	{
		default:
		case STATE_NORMAL:
			drawWidgetSkin(mRectangle.getPosition());
			break;
		case STATE_HIGHLIGHTED:
			mHighlightSkin.drawWidgetSkin(mRectangle.getPosition());
			break;
	};

	// Limit and draw text
	kAssert(mText);
	mText->draw();
}

void buttonWidget::_registerSignals()
{
	// mouse move
	try
	{
		signalHandlerInfo_t* info = new signalHandlerInfo_t;
		info->handler = this;
		info->function = (signalFunctionPtr)(&k::buttonWidget::mousemove);
		info->userData = NULL;

		mInternalSignals["mouseMove"] = info;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to register panel mousemove signal.");
	}

	// mouse in
	try
	{
		signalHandlerInfo_t* info = new signalHandlerInfo_t;
		info->handler = this;
		info->function = (signalFunctionPtr)(&k::buttonWidget::mousein);
		info->userData = NULL;

		mInternalSignals["mouseIn"] = info;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to register panel mousein signal.");
	}

	// mouse out
	try
	{
		signalHandlerInfo_t* info = new signalHandlerInfo_t;
		info->handler = this;
		info->function = (signalFunctionPtr)(&k::buttonWidget::mouseout);
		info->userData = NULL;

		mInternalSignals["mouseOut"] = info;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to register panel mousein signal.");
	}

	// mouse down 
	try
	{
		signalHandlerInfo_t* info = new signalHandlerInfo_t;
		info->handler = this;
		info->function = (signalFunctionPtr)(&k::buttonWidget::mousedown);
		info->userData = NULL;

		mInternalSignals["mouseDown"] = info;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to register panel mousedown signal.");
	}

	// mouse up
	try
	{
		signalHandlerInfo_t* info = new signalHandlerInfo_t;
		info->handler = this;
		info->function = (signalFunctionPtr)(&k::buttonWidget::mouseup);
		info->userData = NULL;

		mInternalSignals["mouseUp"] = info;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to register panel mouseup signal.");
	}
}
			
bool buttonWidget::mousein(signalInfo_t info, void* userData)
{
	mState = STATE_HIGHLIGHTED;

	return false;
}

bool buttonWidget::mouseout(signalInfo_t info, void* userData)
{
	mState = STATE_NORMAL;

	return false;
}

bool buttonWidget::mousemove(signalInfo_t info, void* userData)
{
	widget::mousemove(info, userData);

	return false;
}

bool buttonWidget::mousedown(signalInfo_t info, void* userData)
{
	mMouseWasDown = true;
	return true;
}

bool buttonWidget::mouseup(signalInfo_t info, void* userData)
{
	if (mMouseWasDown)
	{
		// generate clicked event
		callSignal("clicked", info);
	}
	
	mMouseWasDown = false;
	return true;
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
	mLastButton = 0;
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
			
void guiManager::pushWidget(widget* w)
{
	kAssert(w);
	mWidgets.push_back(w);
}

void guiManager::update()
{
	if (mCursor && inputManager::getSingleton().isPeripheralActive(INPUT_MOUSE))
	{
		inputPeripheral* mouse = inputManager::getSingleton().getDevice(INPUT_MOUSE);

		mCursorDeltaPos = mouse->getDeltaPosition();
		mCursorLastPos = mouse->getPosition();

		mCursor->setPosition(mouse->getPosition());
	}
		
	// Send mouse moved event to all widgets where mouse is.
	if (mCursorDeltaPos.x != 0 || mCursorDeltaPos.y != 0)
	{
		signalInfo_t newInfo;
		newInfo.type = SIGNAL_MOUSEMOVE;
		newInfo.mouseState.mPosition = mCursorLastPos;

		// TODO: Get Buttons from inputManager
		newInfo.mouseState.buttons = 0;

		// TODO: Get Keys from inputManager
		newInfo.key = 0;

		std::vector<widget*>::iterator it;
		for (it = mWidgets.begin(); it != mWidgets.end(); it++)
		{
			widget* w = (*it);

			if (w->callSignal("mouseMove", newInfo))
				break;
		}
	}

	// Send mouse down OR mouse up
	unsigned int newButton = 0;
	
	/*
	newButton |= (inputManager::getSingleton().getWiiMoteDown(0, WIIMOTE_BUTTON_A) ? 0x1 : 0x0);
	newButton |= (inputManager::getSingleton().getWiiMoteDown(0, WIIMOTE_BUTTON_B) ? 0x2 : 0x0);
	*/

	if (newButton & 0x1)
	{
		signalInfo_t newInfo;
		newInfo.type = SIGNAL_MOUSEDOWN;
		
		newInfo.mouseState.mPosition = mCursorLastPos;
		newInfo.mouseState.buttons = BUTTON_LEFT;

		// TODO: Get Keys from inputManager
		newInfo.key = 0;

		// Button is Down
		std::vector<widget*>::iterator it;
		for (it = mWidgets.begin(); it != mWidgets.end(); it++)
		{
			widget* w = (*it);
			
			if (!w->isPointerInside(mCursorLastPos))
				continue;

			if (w->callSignal("mouseDown", newInfo))
				break;
		}
	}
	else
	if (mLastButton & 0x1)
	{
		signalInfo_t newInfo;
		newInfo.type = SIGNAL_MOUSEUP;
		
		newInfo.mouseState.mPosition = mCursorLastPos;
		newInfo.mouseState.buttons = BUTTON_LEFT;

		// TODO: Get Keys from inputManager
		newInfo.key = 0;

		// Button up
		std::vector<widget*>::iterator it;
		for (it = mWidgets.begin(); it != mWidgets.end(); it++)
		{
			widget* w = (*it);
			
			if (!w->isPointerInside(mCursorLastPos))
				continue;

			if (w->callSignal("mouseUp", newInfo))
				break;
		}
	}

	mLastButton = newButton;
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

	while (rootElement)
	{
		if (!strncmp(rootElement->Value(), "Imageset", 8))
		{
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
		else
		if (!strncmp(rootElement->Value(), "font", 4))
		{
			mFont.first = rootElement->Attribute("material");	
			mFont.second = rootElement->Attribute("dat");	
		}

		rootElement = rootElement->NextSiblingElement();
	}
}

}
