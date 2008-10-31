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

#include "sprite.h"
#include "logger.h"
#include "root.h"
#include "camera.h"

namespace k {

sprite::sprite(material* mat, vec_t radi)
{
	assert(mat != NULL);

	mRadius = radi;
	mMaterial = mat;	
	mInvalidTransPos = false;

	// Generate mTransPos
	calculateTransPos();
}

sprite::~sprite()
{
}

void sprite::calculateTransPos()
{
	if (GLEW_ARB_point_sprite)
		return;

	// Render must be valid
	renderer* mRenderer = root::getSingleton().getRenderer();
	assert(mRenderer != NULL);

	camera* mCam = mRenderer->getCamera();
	assert(mCam != NULL);

	vector3 sprZ = mPosition - mCam->getPosition();
	sprZ.normalise();

	/*
	// Direction of "right" can be determined by 
	// the up vector cross the forward direction
	vector3 up = vector3::unit_y;
	vector3 sprX = up.crossProduct(sprZ);
	sprX.normalise();

	// Same goes to the up direction
	vector3 sprY = sprZ.crossProduct(sprX);
	sprY.normalise();
	*/
	vector3 sprX = mCam->getRight();
	vector3 sprY = mCam->getUp();

	matrix4 rot;
	rot.m[0][0] = sprX.x;
	rot.m[0][1] = sprX.y;
	rot.m[0][2] = sprX.z;
	
	rot.m[1][0] = sprY.x;
	rot.m[1][1] = sprY.y;
	rot.m[1][2] = sprY.z;
	
	rot.m[2][0] = sprZ.x;
	rot.m[2][1] = sprZ.y;
	rot.m[2][2] = sprZ.z;

	matrix4 trans;
	trans.m[3][0] = mPosition.x;
	trans.m[3][1] = mPosition.y;
	trans.m[3][2] = mPosition.z;

	mTransPos = rot * trans;

	// TransPos is now valid =]
	mInvalidTransPos = false;
}
			
void sprite::setPosition(vector3& pos)
{
	mPosition = pos;
	invalidate();
}

const vector3& sprite::getPosition()
{
	return mPosition;
}

material* sprite::getMaterial()
{
	return mMaterial;
}

vec_t sprite::getRadius()
{
	return mRadius;
}

void sprite::setMaterial(material* mat)
{
	assert(mat != NULL);
	mMaterial = mat;
}

void sprite::setRadius(vec_t rad)
{
	if (GLEW_ARB_point_sprite)
	{
		float maxSize = 0.0f;
		glGetFloatv(GL_POINT_SIZE_MAX_ARB, &maxSize);
		if (rad > maxSize)
			S_LOG_INFO("Radius is greater than supported sprite point size.");
	}

	mRadius = rad;
}

void sprite::invalidate()
{
	mInvalidTransPos = true;
}

static GLfloat distanceAtt[] = {1, 0, 0.5f};
void sprite::draw()
{
	if (mRadius == 0)
		return;

	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	if (GLEW_ARB_point_sprite)
	{
		rs->setDepthMask(false);
		glEnable(GL_POINT_SPRITE);

		assert(mMaterial != NULL);
		mMaterial->prepare();

		float maxSize = 0.0f;
		glGetFloatv(GL_POINT_SIZE_MAX_ARB, &maxSize);

		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
		glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, distanceAtt);
		glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, 60.0f);
		glPointParameterf(GL_POINT_SIZE_MIN, 1.0f);
		glPointParameterf(GL_POINT_SIZE_MAX, maxSize);
		glPointSize(mRadius*rs->getScreenHeight());

		glBegin(GL_POINTS);
			glVertex3f(mPosition.x, mPosition.y, mPosition.z);
		glEnd();

		mMaterial->finish();
		glDisable(GL_POINT_SPRITE);
		rs->setDepthMask(true);

		return;
	}

	if (mInvalidTransPos)
		calculateTransPos();

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);

	#ifdef __WII__
	rs->multMatrix(mFinal.m);
	#else
	rs->multMatrix(mTransPos.m[0]);
	#endif

	// We dont need depth masking on this case
	rs->setDepthMask(false);

	assert(mMaterial != NULL);
	mMaterial->prepare();

	rs->startVertices(VERTEXMODE_QUAD);
		rs->texCoord(vector2(0, 0));
		rs->vertex(vector3(-1*mRadius, -1*mRadius, 0));

		rs->texCoord(vector2(1, 0));
		rs->vertex(vector3(1*mRadius, -1*mRadius, 0));

		rs->texCoord(vector2(1, 1));
		rs->vertex(vector3(1*mRadius, 1*mRadius, 0));

		rs->texCoord(vector2(0, 1));
		rs->vertex(vector3(-1*mRadius, 1*mRadius, 0));
	rs->endVertices();

	//
	rs->setDepthMask(true);
	mMaterial->finish();
}

}

