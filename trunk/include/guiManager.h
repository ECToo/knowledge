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

#ifndef _GUI_MANAGER_H
#define _GUI_MANAGER_H

#include "prerequisites.h"
#include "fileAccess.h"
#include "singleton.h"
#include "sticker.h"

namespace k
{
	class guiManager : public singleton<guiManager>
	{
		protected:
			sticker* mCursor;

		public:
			/**
			 * Controls the GUI System
			 * Keep in mind that this class is automatically 
			 * instantiated by the root
			 */
			guiManager();
			~guiManager();

			static guiManager& getSingleton();

			/**
			 * Set the cursor to show on screen on mouse events
			 * keep in mind that this class needs an renderer
			 * to work automatically. If you dont want to use a renderer
			 * get the cursor sticker and draw it by yourself in your render loop.
			 */
			void setCursor(std::string mat, vector2 scale);
			void setCursorPos(vector2 pos);
			sticker* getCursor();
	};
}

#endif

