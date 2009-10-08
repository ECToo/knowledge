/*
Copyright (c) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "glRenderSystem.h"
#include "materialManager.h"
#include "textureManager.h"

#ifndef __WII__

namespace k {

glRenderSystem::glRenderSystem()
{
	mActiveMaterial = NULL;
	mLastLightIndex = 0;
}

glRenderSystem::~glRenderSystem()
{
	deinitialize();
}

void glRenderSystem::initialize()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
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
			
void glRenderSystem::setTexEnv(texEnvMode mode, int stage)
{
	GLuint mod = GL_REPLACE;

	switch (mode)
	{
		case TEX_ENV_BLEND:
			mod = GL_BLEND;
			break;
		case TEX_ENV_ADD:
			mod = GL_ADD;
			break;
		case TEX_ENV_DECAL:
			mod = GL_DECAL;
			break;
		case TEX_ENV_MODULATE:
			mod = GL_MODULATE;
			break;
		case TEX_ENV_REPLACE:
			break;
	}

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mod);
}
			
void glRenderSystem::setTexEnv(const std::string& baseEnv, int stage) 
{
	GLuint mod = GL_REPLACE;
	if (baseEnv == "add")
		mod = GL_ADD;
	else
	if (baseEnv == "modulate")
		mod = GL_MODULATE;
	else
	if (baseEnv == "decal")
		mod = GL_DECAL;
	else
	if (baseEnv == "blend")
		mod = GL_BLEND;

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mod);
}

void glRenderSystem::configure()
{
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	// Set Depth
	setClearDepth(1.0f);
	setClearColor(color(0, 0, 0, 0));
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
		
	// Initially we dont want it
	mRenderToTexture = false;

	if (getPointSpriteSupport())
	{
		float distanceAtt[3] = { 1.0, 0.005, 0.005 };
		glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, distanceAtt);
	}

	// Ask System Materials and Textures to be created
	textureManager::getSingleton().createSystemTextures();
	materialManager::getSingleton().createSystemMaterials();
}

void glRenderSystem::createWindow(const int w, const int h)
{
	mScreen = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE | SDL_OPENGL | SDL_HWPALETTE | SDL_GL_DOUBLEBUFFER);
	if (!mScreen)
	{
		S_LOG_INFO("Failed to create Window, are you sure your video depth is set to 32bits?");
		return;
	}

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

	SDL_SetGamma(1.2, 1.2, 1.2);

	// Call Configure
	configure();
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
			
void glRenderSystem::showCursor(bool show)
{
	SDL_ShowCursor(show);
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

void glRenderSystem::setClearColor(const color& clr)
{
	glClearColor(clr.r, clr.g, clr.b, clr.a);
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
	if (mActiveMaterial && mActiveMaterial->getNoDraw())
		return;

	switch (mode)
	{
		case VERTEXMODE_POINTS:
			glBegin(GL_POINTS);
			break;
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
	if (mActiveMaterial && mActiveMaterial->getNoDraw())
		return;

	glVertex3f(vert.x, vert.y, vert.z);
}

void glRenderSystem::normal(const vector3& norm)
{
	if (mActiveMaterial && mActiveMaterial->getNoDraw())
		return;
	
	glNormal3f(norm.x, norm.y, norm.z);
}

void glRenderSystem::vcolor(const color& col)
{
	if (mActiveMaterial && mActiveMaterial->getNoDraw())
		return;
	
	glColor4f(col.r, col.g, col.b, col.a);
}
			
void glRenderSystem::texCoord(const vector2& coord)
{
	if (mActiveMaterial && mActiveMaterial->getNoDraw())
		return;

	unsigned int texUnits = 1;

	if (mActiveMaterial)
		texUnits = mActiveMaterial->getStagesCount();

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
	if (mActiveMaterial && mActiveMaterial->getNoDraw())
		return;

	glEnd();
}

void glRenderSystem::matAmbient(const color& col)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, col.c);
}

void glRenderSystem::matDiffuse(const color& col)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, col.c);
}

void glRenderSystem::matSpecular(const color& col)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, col.c);
}
			
void glRenderSystem::genTexture(uint32_t w, uint32_t h, uint32_t bpp, platformTexturePointer* tex)
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
	if (mActiveMaterial && mActiveMaterial->getNoDraw())
		return;

	if (mNormalArray || (mUsingVBO && mNormalOffset != -1))
	{
		glEnableClientState(GL_NORMAL_ARRAY);

		if (mUsingVBO)
			glNormalPointer(GL_FLOAT, mNormalStride, (char*)NULL + mNormalOffset);
		else
		{
			kAssert(mNormalArray);
			glNormalPointer(GL_FLOAT, mNormalStride, mNormalArray);
		}
	}
	else
	{
		glDisableClientState(GL_NORMAL_ARRAY);
	}

	unsigned int texUnits = mActiveMaterial->getStagesCount();
	for (unsigned int i = texUnits; i < MAX_TEXCOORD; i++)
	{
		// In case we specified an array
		if (mTexCoordArray[i])
		{
			texUnits = i + 1;
			continue;
		}

		glClientActiveTextureARB(GL_TEXTURE0_ARB + i);
		glActiveTextureARB(GL_TEXTURE0_ARB + i);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
	}
			
	if (texUnits)
	{
		for (unsigned int i = 0; i < texUnits; i++)
		{
			glClientActiveTextureARB(GL_TEXTURE0_ARB + i);
			glActiveTextureARB(GL_TEXTURE0_ARB + i);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnable(GL_TEXTURE_2D);
				
			int found = 0;
			for (found = i; found > 0; found--)
			{
				if (mTexCoordArray[i])
					break;
			}

			if (mUsingVBO)
				glTexCoordPointer(2, GL_FLOAT, mTexCoordStride[found], (char*)NULL + mTexCoordOffset[found]);
			else
				glTexCoordPointer(2, GL_FLOAT, mTexCoordStride[found], mTexCoordArray[found]);
		}
	}
	else
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
		
	if (mVertexArray || (mUsingVBO && mVertexOffset != -1))
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		
		if (mUsingVBO)
			glVertexPointer(3, GL_FLOAT, mVertexStride, (char*)NULL + mVertexOffset);
		else
		{
			kAssert(mVertexCount);
			kAssert(mIndexCount);
			kAssert(mVertexArray);
			glVertexPointer(3, GL_FLOAT, mVertexStride, mVertexArray);
		}
	}

	GLuint drawMode = GL_TRIANGLES;
	if (mIndexDrawMode == VERTEXMODE_QUAD)
		drawMode = GL_QUADS;
	else
	if (mIndexDrawMode == VERTEXMODE_TRI_STRIP)
		drawMode = GL_TRIANGLE_STRIP;
	else
	if (mIndexDrawMode == VERTEXMODE_POINTS)
		drawMode = GL_POINTS;

	if (mUsingVBO)
		glDrawRangeElements(drawMode, 0, mVertexCount - 1, mIndexCount, GL_UNSIGNED_INT, (char*)NULL + mIndexOffset);
	else
		glDrawRangeElements(drawMode, 0, mVertexCount - 1, mIndexCount, GL_UNSIGNED_INT, mIndexArray);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}
			
void glRenderSystem::copyToTexture(platformTexturePointer* tex)
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
		S_LOG_INFO("Failed to allocate memory for screenshot.");
		return;
	}

	char* imgData;
	try
	{
		imgData = new char[mScreenSize[0] * mScreenSize[1] * 3];
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate memory for screenshot.");
		FreeImage_Unload(newImage);

		return;
	}

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
	delete [] imgData;
}
			
bool glRenderSystem::isLightOn()
{
	return glIsEnabled(GL_LIGHTING);
}

void glRenderSystem::setLighting(bool status)
{
	if (status)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_NORMALIZE);
	}
	else
	{
		for (int i = 0; i < 8; i++)
			glDisable(GL_LIGHT0 + i);

		glDisable(GL_LIGHTING);
		glDisable(GL_NORMALIZE);

		mLastLightIndex = 0;
	}
}
			
void glRenderSystem::setLight(unsigned int index, bool status)
{
	kAssert(index < 8);

	if (status)
	{
		glEnable(GL_LIGHT0 + index);

		if ((index + 1) > mLastLightIndex)
			mLastLightIndex = index + 1;
	}
	else
		glDisable(GL_LIGHT0 + index);
}

void glRenderSystem::setLightPosition(unsigned int i, const vector3& p, bool directional)
{
	kAssert(i < 8);

	// Light position is in world space
	setMatrixMode(MATRIXMODE_MODELVIEW);
	identityMatrix();

	GLfloat position[4] = {p.x, p.y, p.z, directional ? 0 : 1 };
	glLightfv(GL_LIGHT0 + i, GL_POSITION, position);
}

void glRenderSystem::setLightAmbient(unsigned int i, const color& a)
{
	kAssert(i < 8);
	glLightfv(GL_LIGHT0 + i, GL_AMBIENT, a.c);
}

void glRenderSystem::setLightSpecular(unsigned int i, const color& s)
{
	kAssert(i < 8);
	glLightfv(GL_LIGHT0 + i, GL_SPECULAR, s.c);
}

void glRenderSystem::setLightDiffuse(unsigned int i, const color& d)
{
	kAssert(i < 8);
	glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, d.c);
}

void glRenderSystem::setLightAttenuation(unsigned int i, const vector3& att)
{
	kAssert(i < 8);
	glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, att.x);
	glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, att.y);
	glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, att.y);
}
			
bool glRenderSystem::getPointSpriteSupport()
{
	// Detect ATI cards (broken POINT_SPRITE)
	if (GL_ATI_vertex_array_object || GL_ATI_draw_buffers)
		return false;

	if (GLEW_ARB_point_sprite)
		return true;
	else
		return false;
}

float glRenderSystem::getPointSpriteMaxSize()
{
	if (!getPointSpriteSupport())
		return 0;

	float size;
	glGetFloatv(GL_POINT_SIZE_MAX_ARB, &size);

	return size;
}

void glRenderSystem::setPointSprite(bool enabled)
{
	if (!getPointSpriteSupport())
		return;

	if (enabled)
	{
		glEnable(GL_POINT_SPRITE);
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	}
	else
	{
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_FALSE);
		glDisable(GL_POINT_SPRITE);
	}
}

void glRenderSystem::setPointSpriteSize(vec_t size)
{
	if (!getPointSpriteSupport())
		return;

	glPointParameterf(GL_POINT_SIZE_MIN, size);
	glPointParameterf(GL_POINT_SIZE_MAX, getPointSpriteMaxSize());
	glPointSize(size * getScreenHeight());
}
			
void glRenderSystem::drawPointSprites(const vec_t* positions, unsigned int numPositions)
{
	if (!getPointSpriteSupport())
		return;

	kAssert(positions);
	
	glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, positions);
	glDrawArrays(GL_POINTS, 0, numPositions);

	glDisableClientState(GL_VERTEX_ARRAY);
}
			
void glRenderSystem::setPointSpriteAttenuation(vec_t* att)
{
	if (!getPointSpriteSupport())
		return;

	kAssert(att);
	glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, att);
}
			
bool glRenderSystem::getVBOSupport()
{
	return GLEW_ARB_vertex_buffer_object;
}

void glRenderSystem::genVBO(platformVBO* target)
{
	kAssert(target);
	glGenBuffers(1, target);
}

void glRenderSystem::bindVBO(platformVBO* target, VBOArrayType type = VBO_ARRAY)
{
	if (type == VBO_ARRAY)
	{
		if (target)
			glBindBuffer(GL_ARRAY_BUFFER, (*target));
		else
			glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else
	{
		if (target)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*target));
		else
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}
			
void glRenderSystem::setVBOData(VBOArrayType type, int size, void* data, VBOUsage usage)
{
	GLuint t = GL_ARRAY_BUFFER;
	if (type == VBO_ELEMENT_ARRAY)
		t = GL_ELEMENT_ARRAY_BUFFER;

	GLuint use;
	switch (usage)
	{
		default:
		case VBO_STATIC_DRAW:
			use = GL_STATIC_DRAW;
			break;
		case VBO_STATIC_READ:
			use = GL_STATIC_READ;
			break;
		case VBO_STATIC_COPY:
			use = GL_STATIC_COPY;
			break;
		case VBO_DYNAMIC_DRAW:
			use = GL_DYNAMIC_DRAW;
			break;
		case VBO_DYNAMIC_READ:
			use = GL_DYNAMIC_READ;
			break;
		case VBO_DYNAMIC_COPY:
			use = GL_DYNAMIC_COPY;
			break;
		case VBO_STREAM_DRAW:
			use = GL_STREAM_DRAW;
			break;
		case VBO_STREAM_READ:
			use = GL_STREAM_READ;
			break;
		case VBO_STREAM_COPY:
			use = GL_STREAM_COPY;
			break;
	};

	glBufferData(t, size, data, use);
}

void glRenderSystem::delVBO(platformVBO* target)
{
	kAssert(target);
	glDeleteBuffers(1, target);
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

