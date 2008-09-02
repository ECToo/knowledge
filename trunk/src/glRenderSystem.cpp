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

#include "glRenderSystem.h"
#ifndef __WII__

namespace k {

glRenderSystem::glRenderSystem()
{
}

glRenderSystem::~glRenderSystem()
{
	deinitialise();
}

void glRenderSystem::initialise()
{
	SDL_Init(SDL_INIT_VIDEO);
}

void glRenderSystem::deinitialise()
{
	SDL_Quit();
}

void glRenderSystem::configure()
{
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	// All vertices will be passed by arrays so...
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	// Set Depth
	setClearDepth(1.0f);
	setClearColor(vector3(0, 0, 0));
	setDepthTest(true);

	glDepthFunc(GL_LEQUAL);
	
	// Shading
	setShadeModel(SHADEMODEL_SMOOTH);

	// Culling
	setCulling(CULLMODE_NONE);

	// Some openGL improves
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void glRenderSystem::createWindow(const int w, const int h)
{
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	mScreen = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE | SDL_OPENGL | SDL_HWPALETTE);

	mScreenSize = vector2(w, h);
	
	// Setup viewport
	setViewPort(0, 0, w, h);

	// Setup perspective
	setMatrixMode(MATRIXMODE_PROJECTION);
	identityMatrix();
	setPerspective(90, (float)w/h, 0.1f, 5000.0f);

	setMatrixMode(MATRIXMODE_MODELVIEW);
	identityMatrix();
}

void glRenderSystem::destroyWindow()
{
}

void glRenderSystem::frameStart()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void glRenderSystem::frameEnd()
{
	SDL_GL_SwapBuffers();
}

void glRenderSystem::setClearColor(const vector3& color)
{
	glClearColor(color.x, color.y, color.z, 1.0f);
}

void glRenderSystem::setClearDepth(const vec_t amount)
{
	glClearDepth(amount);
}

void glRenderSystem::setDepthTest(bool test)
{
	if (test)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void glRenderSystem::setShadeModel(ShadeModel model)
{
	int shadeMode;
	switch(model)
	{
		case SHADEMODEL_FLAT:
		default:
			shadeMode = GL_FLAT;
			break;

		case SHADEMODEL_SMOOTH:
			shadeMode = GL_SMOOTH;
			break;
	}

	glShadeModel(shadeMode);
}

void glRenderSystem::setMatrixMode(MatrixMode mode)
{
	int matrixMode;
	switch (mode)
	{
		case MATRIXMODE_MODELVIEW:
			matrixMode = GL_MODELVIEW;
			break;
		case MATRIXMODE_PROJECTION:
			matrixMode = GL_PROJECTION;
			break;
	}

	glMatrixMode(matrixMode);
}

void glRenderSystem::pushMatrix()
{
	glPushMatrix();
}

void glRenderSystem::popMatrix()
{
	glPopMatrix();
}

void glRenderSystem::identityMatrix()
{
	glLoadIdentity();
}

void glRenderSystem::translateScene(vec_t x, vec_t y, vec_t z)
{
	glTranslatef(x, y, z);
}

void glRenderSystem::rotateScene(vec_t angle, vec_t x, vec_t y, vec_t z)
{
	glRotatef(angle, x, y, z);
}

void glRenderSystem::scaleScene(vec_t x, vec_t y, vec_t z)
{
	glScalef(x, y, z);
}

void glRenderSystem::setViewPort(int x, int y, int w, int h)
{
	glViewport(x, y, w, h);
}

void glRenderSystem::setPerspective(vec_t fov, vec_t aspect, vec_t near, vec_t far)
{
	gluPerspective(fov, aspect, near, far);
}

void glRenderSystem::setOrthographic(vec_t left, vec_t right, vec_t bottom, vec_t top, vec_t near, vec_t far)
{
	glOrtho(left, right, bottom, top, near, far);
}

void glRenderSystem::setCulling(CullMode culling)
{
	int cullMode;
	switch(culling)
	{
		case CULLMODE_NONE:
			glDisable(GL_CULL_FACE);
			break;
		case CULLMODE_BACK:
			glEnable(GL_CULL_FACE);
			cullMode = GL_BACK;
			break;
		case CULLMODE_FRONT:
			glEnable(GL_CULL_FACE);
			cullMode = GL_FRONT;
			break;
		case CULLMODE_BOTH:
			glEnable(GL_CULL_FACE);
			cullMode = GL_FRONT_AND_BACK;
			break;
	};

	if (culling != CULLMODE_NONE)
		glCullFace(cullMode);
}
			
void glRenderSystem::startVertices(VertexMode mode)
{
	switch (mode)
	{
		default:
		case VERTEXMODE_TRIANGLES: 
			glBegin(GL_TRIANGLES);
			break;

		case VERTEXMODE_QUAD:
			glBegin(GL_QUADS);
			break;
	};
}

void glRenderSystem::vertex(const vector3& vert)
{
	glVertex3f(vert.x, vert.y, vert.z);
}

void glRenderSystem::normal(const vector3& norm)
{
	glNormal3f(norm.x, norm.y, norm.z);
}

void glRenderSystem::color(const vector3& col)
{
	glColor3f(col.x, col.y, col.z);
}
			
void glRenderSystem::texCoord(const vector2& coord)
{
	glTexCoord2f(coord.x, coord.y);
}

void glRenderSystem::endVertices()
{
	glEnd();
}

void glRenderSystem::matAmbient(const vector3& color)
{
	float v[4];

	v[0] = color.x;
	v[1] = color.y;
	v[2] = color.z;
	v[3] = 1.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, v);
}

void glRenderSystem::matDiffuse(const vector3& color)
{
	float v[4];

	v[0] = color.x;
	v[1] = color.y;
	v[2] = color.z;
	v[3] = 1.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, v);
}

void glRenderSystem::matSpecular(const vector3& color)
{
	float v[4];

	v[0] = color.x;
	v[1] = color.y;
	v[2] = color.z;
	v[3] = 1.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, v);
}
			
void glRenderSystem::bindTexture(GLuint tex)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);
}
			
unsigned int glRenderSystem::getScreenWidth()
{
	return mScreenSize.x;
}

unsigned int glRenderSystem::getScreenHeight()
{
	return mScreenSize.y;
}

} // namespace k

#endif

