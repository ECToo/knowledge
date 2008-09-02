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

#include "material.h"
#include "root.h"

namespace k {

void material::prepare()
{
	// Material Properties
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	rs->matAmbient(mAmbient);
	rs->matDiffuse(mDiffuse);
	rs->matSpecular(mSpecular);

	// Cycle through textures
	std::list<texture*>::iterator it;
	for (it = mTextures.begin(); it != mTextures.end(); it++)
	{
		texture* tex = (*it);
		assert(tex != NULL);
		
		tex->draw();
	}
}
			
void material::setAmbient(const vector3& color)
{
	mAmbient = color;
}

void material::setDiffuse(const vector3& color)
{
	mDiffuse = color;
}

void material::setSpecular(const vector3& color)
{
	mSpecular = color;
}
			
void material::pushTexture(texture* tex)
{
	assert(tex != NULL);
	mTextures.push_back(tex);
}

}

