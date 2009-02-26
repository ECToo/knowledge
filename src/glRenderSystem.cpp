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
	deinitialize();
}

void glRenderSystem::initialize()
{
	SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);
}

void glRenderSystem::deinitialize()
{
	SDL_Quit();
}
			
void glRenderSystem::setWireFrame(bool wire)
{
	if (wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void glRenderSystem::configure()
{
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

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

	// TODO: Anti Aliasing? 
	glEnable(GL_POLYGON_SMOOTH);
}

void glRenderSystem::createWindow(const int w, const int h)
{
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	mScreen = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE | SDL_OPENGL | SDL_HWPALETTE);

	mScreenSize[0] = w;
	mScreenSize[1] = h;
	
	// Setup viewport
	setViewPort(0, 0, w, h);

	// Setup perspective
	setMatrixMode(MATRIXMODE_PROJECTION);
	identityMatrix();
	setPerspective(90, (float)w/h, 0.1f, 5000.0f);

	setMatrixMode(MATRIXMODE_MODELVIEW);
	identityMatrix();

	// Glew initialization
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		S_LOG_INFO("Failed to initialize GLEW.");
	}

	// Create screenshot memory area
	glGenTextures(1, &mScreenshotTex);
	glBindTexture(GL_TEXTURE_2D, mScreenshotTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mScreenSize[0], 
			mScreenSize[1], 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void glRenderSystem::setBlendMode(unsigned short src, unsigned short dst)
{
	glBlendFunc(src, dst);
}

void glRenderSystem::setBlend(bool state)
{
	if (state)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
}
			
void glRenderSystem::setDepthMask(bool mask)
{
	glDepthMask(mask);
}

void glRenderSystem::destroyWindow()
{
}
			
void glRenderSystem::setWindowTitle(const std::string& title)
{
	SDL_WM_SetCaption(title.c_str(), "");
}

void glRenderSystem::frameStart()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void glRenderSystem::frameEnd()
{
	if (mRenderToTexture)
	{
		glFlush();

		if (!mRttTarget)
		{
			S_LOG_INFO("Render to texture target not defined, aborting.");
			kAssert(mRttTarget);
		}

		copyToTexture(mRttTarget);

		// Clean Textures
		mRenderToTexture = false;
		mRttTarget = NULL;
	}
	else
	{
		SDL_GL_SwapBuffers();
	}

	mActiveMaterial = NULL;
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
	unsigned short matrixMode = GL_MODELVIEW;
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
							
void glRenderSystem::copyMatrix(const matrix4& mat)
{
	glLoadMatrixf(mat.m[0]);
}

void glRenderSystem::multMatrix(const matrix4& mat)
{
	glMultMatrixf(mat.m[0]);
}

matrix4 glRenderSystem::getModelView()
{
	matrix4 result;
	glGetFloatv(GL_MODELVIEW_MATRIX, result.m[0]);

	return result;
}

void glRenderSystem::getModelView(float mat[][4])
{
	glGetFloatv(GL_MODELVIEW_MATRIX, mat[0]);
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

void glRenderSystem::setPerspective(vec_t fov, vec_t aspect, vec_t nearP, vec_t farP)
{
	glLoadIdentity();
	gluPerspective(fov, aspect, nearP, farP);
}

void glRenderSystem::setOrthographic(vec_t left, vec_t right, vec_t bottom, vec_t top, vec_t nearP, vec_t farP)
{
	glLoadIdentity();
	glOrtho(left, right, bottom, top, nearP, farP);
}

void glRenderSystem::setCulling(CullMode culling)
{
	unsigned short cullMode = GL_BACK;
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
		case VERTEXMODE_LINE:
			glBegin(GL_LINES);
			break;
		case VERTEXMODE_TRI_STRIP:
			glBegin(GL_TRIANGLE_STRIP);
			break;
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
	unsigned int texUnits = 1;

	if (mActiveMaterial)
		texUnits = mActiveMaterial->getTextureUnits();

	if (texUnits > 1)
	{
		for (unsigned int i = 0; i < texUnits; i++)
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB + i, coord.x, coord.y);
	}
	else
	{
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, coord.x, coord.y);
	}
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
			
void glRenderSystem::genTexture(uint32_t w, uint32_t h, uint32_t bpp, kTexture* tex)
{
	kAssert(tex);

	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void glRenderSystem::bindTexture(GLuint* tex, int chan)
{
	kAssert(tex);

	glClientActiveTextureARB(GL_TEXTURE0_ARB + chan);
	glActiveTextureARB(GL_TEXTURE0_ARB + chan);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex[0]);
}
			
void glRenderSystem::unBindTexture(int chan)
{
	glClientActiveTextureARB(GL_TEXTURE0_ARB + chan);
	glActiveTextureARB(GL_TEXTURE0_ARB + chan);

	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void glRenderSystem::drawArrays()
{
	if (mVertexArray)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, mVertexArray);
	}

	if (mNormalArray)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, mNormalArray);
	}

	unsigned int texUnits = 1;

	if (mActiveMaterial)
		texUnits = mActiveMaterial->getTextureUnits();

	if (mTexCoordArray)
	{
		if (texUnits > 1)
		{
			for (unsigned int i = 0; i < texUnits; i++)
			{
				glClientActiveTextureARB(GL_TEXTURE0_ARB + i);
				glActiveTextureARB(GL_TEXTURE0_ARB + i);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);

				glTexCoordPointer(2, GL_FLOAT, 0, mTexCoordArray);
			}
		}
		else
		{
			glClientActiveTextureARB(GL_TEXTURE0_ARB);
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glTexCoordPointer(2, GL_FLOAT, 0, mTexCoordArray);
		}
	}

	glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, mIndexArray);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	if (texUnits > 1)
	{
		for (unsigned int i = 0; i < texUnits; i++)
		{
			glClientActiveTextureARB(GL_TEXTURE0_ARB + i);
			glActiveTextureARB(GL_TEXTURE0_ARB + i);

			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}
	else
	{
		glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glActiveTextureARB(GL_TEXTURE0_ARB);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}
			
void glRenderSystem::copyToTexture(kTexture* tex)
{
	kAssert(tex);
	bindTexture(tex, 0);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, mRttDimensions[0], mRttDimensions[1], 0);
}

void glRenderSystem::screenshot(const char* filename)
{
	glBindTexture(GL_TEXTURE_2D, mScreenshotTex);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, mScreenSize[0],
			mScreenSize[1], 0);

	FIBITMAP* newImage = FreeImage_Allocate(mScreenSize[0], mScreenSize[1], 24);
	if (!newImage)
	{
		lowMemory:

		S_LOG_INFO("Failed to allocate memory for screenshot.");
		return;
	}

	char* imgData = (char*)malloc(mScreenSize[0] * mScreenSize[1] * 3);
	if (!imgData)
		goto lowMemory;

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);

	// Width * BPP
	uint32_t pitch = mScreenSize[0] * 3;

	// Write data
	for (uint32_t y = 0; y < mScreenSize[1]; y++)
	{
		uint32_t Ypitch = y * pitch;
		for (uint32_t x = 0; x < mScreenSize[0]; x++)
		{
			uint32_t tripleX = x * 3;

			RGBQUAD color;
			color.rgbRed = imgData[Ypitch + tripleX];
			color.rgbGreen = imgData[Ypitch + tripleX + 1];
			color.rgbBlue = imgData[Ypitch + tripleX + 2];

			FreeImage_SetPixelColor(newImage, x, y, &color);
		}
	}

	FreeImage_Save(FIF_JPEG, newImage, filename, JPEG_QUALITYSUPERB);
	FreeImage_Unload(newImage);
	free(imgData);
}

unsigned int glRenderSystem::getScreenWidth()
{
	return mScreenSize[0];
}

unsigned int glRenderSystem::getScreenHeight()
{
	return mScreenSize[1];
}

} // namespace k

#endif

