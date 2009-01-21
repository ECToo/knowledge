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

#include "loadscr.h"
#include "logger.h"
#include "rendersystem.h"
#include "textureManager.h"
#include "root.h"

namespace k {

void bgLoadScreen::loadBg(const std::string& filename)
{
	k::textureManager* texMgr = &k::textureManager::getSingleton();

	texMgr->allocateTextureData(filename);
	mBackground = texMgr->getTexture(filename);
}

void bgLoadScreen::update(const std::string& filename)
{
	k::renderSystem* rs = k::root::getSingleton().getRenderSystem();

	rs->frameStart();
	rs->setCulling(k::CULLMODE_NONE);

	rs->setMatrixMode(k::MATRIXMODE_PROJECTION);
	rs->setOrthographic(0, rs->getScreenWidth(), rs->getScreenHeight(), 0, -1, 1);

	rs->setMatrixMode(k::MATRIXMODE_MODELVIEW);
	rs->identityMatrix();

	rs->setDepthMask(false);

	rs->bindTexture(mBackground->mId[0], 0);
	rs->startVertices(k::VERTEXMODE_QUAD);
		rs->texCoord(k::vector2(0.0f, 0.0f));
		rs->vertex(k::vector3(0, 0, -0.5));
		rs->texCoord(k::vector2(1.0f, 0.0f));
		rs->vertex(k::vector3(rs->getScreenWidth(), 0, -0.5));
		rs->texCoord(k::vector2(1.0f, 1.0f));
		rs->vertex(k::vector3(rs->getScreenWidth(), rs->getScreenHeight(), -0.5));
		rs->texCoord(k::vector2(0.0f, 1.0f));
		rs->vertex(k::vector3(0, rs->getScreenHeight(), -0.5));
	rs->endVertices();

	rs->setDepthMask(true);
	rs->unBindTexture(0);

	rs->setMatrixMode(k::MATRIXMODE_PROJECTION);
	rs->setOrthographic(-10.0f, 10.0f, -10.0f, 10.0f, -1, 1);

	rs->frameEnd();
}

}

