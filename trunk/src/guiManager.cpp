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
		renderer* rs = root::getSingleton().getRenderer();
		kAssert(rs != NULL);

		rs->pop2D(mCursor);
		delete mCursor;
	}
}

void guiManager::setCursor(std::string mat, vector2 scale)
{
	mCursor = new sticker(mat);
	kAssert(mCursor != NULL);

	mCursor->setScale(scale);
	mCursor->setZ(-1.0f);

	#ifndef __WII__
	mCursorSize = scale.x;
	#endif

	renderer* rs = root::getSingleton().getRenderer();
	kAssert(rs != NULL);

	rs->push2D(mCursor);

	#ifndef __WII__
	SDL_ShowCursor(false);
	#endif
}

void guiManager::setCursorPos(vector2 pos)
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
