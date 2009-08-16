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

#include "bsp46.h"
#include "camera.h"
#include "color.h"
#include "config.h"
#include "drawable.h"
#include "fileParser.h"
#include "fontManager.h"
#include "gameState.h"
#include "guiManager.h"
#include "inputManager.h"
#include "keysyms.h"
#include "light.h"
#include "loadscr.h"
#include "logger.h"
#include "material.h"
#include "materialManager.h"
#include "matrix3.h"
#include "matrix4.h"
#include "md3.h"
#include "md5.h"
#include "particle.h"
#include "physicsManager.h"
#include "prerequisites.h"
#include "quaternion.h"
#include "ray.h"
#include "renderer.h"
#include "rendersystem.h"
#include "resourceManager.h"
#include "root.h"
#include "singleton.h"
#include "sprite.h"
#include "sticker.h"
#include "texture.h"
#include "textureManager.h"
#include "thread.h"
#include "timer.h"
#include "tinystr.h"
#include "tinyxml.h"
#include "vector2.h"
#include "vector3.h"
#include "world.h"

// Platform Files
#ifndef __WII__
	#include "pc/prerequisites.h"
	#include "pc/glRenderSystem.h"
#else
	#include "wii/prerequisites.h"
	#include "wii/wiiRenderSystem.h"
	#include "wii/tev.h"
#endif

