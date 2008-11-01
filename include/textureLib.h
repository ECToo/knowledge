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

#ifndef _TEXTURE_LIB_H
#define _TEXTURE_LIB_H

#include "prerequisites.h"
#include "texture.h"

namespace k
{
	/**
	 * This function will create a new texture based
	 * on the file type. On openGL this will be done by
	 * DevIL and on Wii it will be done by a set of custom
	 * functions.
	 */
	texture* createRawTexture(const std::string& filename);
}

#endif

