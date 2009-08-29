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
#include "logger.h"
#include "renderer.h"
#include "root.h"

namespace k {

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

void guiManager::setCursorPos(const vector2& pos)
{
	if (mCursor)
	{
		mCursor->setPosition(pos);
	}
}

sticker* guiManager::getCursor()
{
	return mCursor;
}

}
