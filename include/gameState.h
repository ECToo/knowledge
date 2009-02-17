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

#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "prerequisites.h"
#include "singleton.h"

namespace k 
{

class gameState 
{
	public:
		virtual ~gameState() {}

		virtual void start() = 0;
		virtual void end() = 0;

		virtual void frameStart() = 0;
		virtual void frameEnd() = 0;
};

class stateManager : public k::singleton<stateManager>
{
	protected:
		std::map<std::string, gameState*> mStates;
		gameState* mActiveState;

	public:
		stateManager();
		~stateManager();

		static stateManager& getSingleton();

		gameState* findState(const std::string&);
		void setState(const std::string&);

		void pushState(const std::string&, gameState*);
		void popState(const std::string&);

		void frameStart();
		void frameEnd();
};

}

#endif

