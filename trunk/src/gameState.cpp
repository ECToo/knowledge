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

#include "gameState.h"
#include "logger.h"

namespace k {

template<> stateManager* k::singleton<stateManager>::singleton_instance = 0;

stateManager& stateManager::getSingleton()
{  
	kAssert(singleton_instance);
	return (*singleton_instance);  
}

stateManager::stateManager()
{
	mActiveState = NULL;
}

gameState* stateManager::findState(const std::string& name)
{
	std::map<std::string, gameState*>::const_iterator it = mStates.find(name);

	if (it != mStates.end())
		return it->second;
	else
		return NULL;
}

void stateManager::setState(const std::string& name)
{
	gameState* mState = findState(name);
	if (mState)
	{
		if (mActiveState)
			mActiveState->end();

		mActiveState = mState;
		mActiveState->start();
	}
	else
	{
		S_LOG_INFO("Game State " + name + " doesnt exist.");
	}
}

void stateManager::pushState(const std::string& name, gameState* newState)
{
	kAssert(newState);
	if (findState(name))
	{
		S_LOG_INFO("Game State already pushed to the manager.");
		return;
	}

	mStates[name] = newState;
}

void stateManager::popState(const std::string& name)
{
	std::map<std::string, gameState*>::iterator it = mStates.find(name);
	if (it != mStates.end())
	{
		// Call ending function on the state
		it->second->end();

		mStates.erase(it);
	}
}
		
void stateManager::frameStart()
{
	if (mActiveState)
		mActiveState->frameStart();
}

void stateManager::frameEnd()
{
	if (mActiveState)
		mActiveState->frameEnd();
}

} // namespace k

