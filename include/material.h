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

#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "prerequisites.h"
#include "color.h"
#include "texture.h"
#include "wii/tev.h"

namespace k
{
	/**
	 * Culling modes.
	 */
	enum CullMode
	{
		CULLMODE_NONE,
		CULLMODE_BACK,
		CULLMODE_FRONT,
		CULLMODE_BOTH
	};

	/**
	 * Texture environment functions.
	 */
	enum TexEnvFunctions
	{
		TEXENV_REPLACE,
		TEXENV_MODULATE,
		TEXENV_BLEND,
		TEXENV_DECAL,
		TEXENV_ADD,

		TEXENV_MAX_ENV
	};

	/**
	 * \brief The material (texture) stage.
	 * Each material has a number of sub-texture stages, wich
	 * are responsible to draw textures and generate coordinates.
	 */
	class DLL_EXPORT materialStage
	{
		protected:
			vector2 mScale;

			/**
			 * Texture scrolling.
			 */
			vector2 mScroll;
			vector2 mScrolled;

			/**
			 * Texture rotation.
			 */
			float mAngle;
			float mRotate;

			/**
			 * Texture coordinate type, check @TextureCoordType
			 */
			TextureCoordType mCoordType;

			/**
			 * Stage index
			 */
			unsigned short mIndex;

			/**
			 * Blendfunc
			 * Note that if both src and dst are ZERO
			 * blend will be disabled
			 */
			unsigned short mBlendSrc, mBlendDst;

			/**
			 * Blending Operation
			 */
			unsigned int mTexEnv;

			/**
			 * Number of textures allocated for this stage
			 */
			unsigned int mTexturesCount;

			/**
			 * All textures within this stage.
			 */
			texture** mTextures;

			/**
			 * Last time this was called
			 */
			long mLastFeedTime;

			/**
			 * Number of frame in case this stage is animated.
			 */
			unsigned int mNumberOfFrames;

			/**
			 * Animation frame rate
			 */
			vec_t mFrameRate;

			/**
			 * Actual frame we are rendering
			 */
			vec_t mCurrentFrame;

		public:

			/**
			 * Create a new material stage. Each stage will be drawn by
			 * parent material ordered by their index.
			 * @index The index of the new stage in material.
			 */
			materialStage(unsigned short index);

			/**
			 * Destroys the material stage, freeing used memory.
			 */
			virtual ~materialStage();

			/**
			 * Set the number of textures on this stage.
			 */
			void setTexturesCount(unsigned int count);

			/**
			 * Set texture environment function, @see TexEnvFunctions.
			 *
			 * @tev New texture environment function.
			 */
			void setEnv(unsigned int tev);

			/**
			 * Set type of texture coordinate generation. @see TextureCoordType.
			 * @type The new texture coordinate generation type.
			 */
			void setCoordType(TextureCoordType type);

			/**
			 * Set stage blending.
			 * @src Source Blending mode
			 * @dst Destination Blending mode
			 */
			void setBlendMode(unsigned short src, unsigned short dst);

			/**
			 * Set the amount this stage texture scrolls in each direction (x,y) per second.
			 * @scroll The amount texture scrolls in each direction.
			 */
			void setScroll(vector2 scroll);

			/**
			 * Set the amount this stage texture is scaled.
			 * @scale The new stage texture scale
			 */
			void setScale(vector2 scale);

			/**
			 * Set the amount this stage texture is rotated per second.
			 * @angle The angular velocity.
			 */
			void setRotate(float angle);

			/**
			 * Set the stage texture.
			 * @tex A pointer to the texture.
			 * @index Index of the texture, up K_MAX_STAGE_TEXTURES
			 */
			void setTexture(texture* tex, unsigned int index);

			/**
			 * Does this stage contains the texture?
			 * @tname The name of the texture we are checking against.
			 */
			bool containsTexture(const std::string& tname) const;

			/**
			 * Does this stage contains opaque textures?
			 */
			bool isOpaque() const;

			/**
			 * Used internally to force current frame calculation.
			 */
			void feedAnims();

			/**
			 * Is this stage animated?
			 */
			bool isAnimated() const
			{
				return mNumberOfFrames > 0;
			}

			/**
			 * Set the number of frames on the stage.
			 */
			void setNumberOfFrames(unsigned int i)
			{
				mNumberOfFrames = i;
			}

			/**
			 * Set animation frame rate
			 */
			void setFrameRate(vec_t framerate)
			{
				mFrameRate = framerate;
			}

			/**
			 * Get animation frame rate
			 */
			vec_t getFrameRate() const
			{
				return mFrameRate;
			}

			/**
			 * Return stage texture width.
			 */
			unsigned int getWidth() const;

			/**
			 * Return stage texture height.
			 */
			unsigned int getHeight() const;

			/**
			 * Return the number of images this stage has.
			 */
			unsigned int getImagesCount() const;

			/** 
			 * Get a material texture by index.
			 * @index Texture index.
			 */
			const texture* getTexture(unsigned int index) const;

			/**
			 * Start drawing this stage.
			 */
			void draw();

			/**
			 * Finish drawing this stage.
			 */
			void finish();
	};

	/**
	 * \brief The material class.
	 * Please check the material class documentation link for more information
	 */
	class DLL_EXPORT material
	{
		protected:
			color mAmbient;
			color mDiffuse;
			color mSpecular;

			CullMode mCull;

			bool mDepthTest;
			bool mDepthWrite;

			bool mNoDraw;
			bool mIsOpaque;

			/**
			 * This isnt a boolean because if we
			 * are not receiving light and light
			 * is enabled, we need to enable it again
			 * after rendering, so we set mReceiveLight (internally) to
			 * -getEnabledLightCount() and then on finish() we turn it back to zero.
			 */
			int mReceiveLight;

			int mContentFlags;
			int mEffectFlags;

			std::vector<materialStage*> mStages;

		public:
			/**
			 * Constructor.
			 */
			material();

			/**
			 * Destructor.
			 */
			~material();

			/**
			 * Spawn material with a single texture.
			 */
			material(texture* tex);

			/**
			 * Spawn material with a single texture.
			 */
			material(const std::string& filename);

			/**
			 * Set Material ambient component
			 * @param color The new component values.
			 */
			void setAmbient(const color& clr)
			{
				mAmbient = clr;
			}

			/**
			 * Set Material diffuse component
			 * @param color The new component values.
			 */
			void setDiffuse(const color& clr)
			{
				mDiffuse = clr;
			}

			/**
			 * Set Material specular component
			 * @param color The new component values.
			 */
			void setSpecular(const color& clr)
			{
				mSpecular = clr;
			}

			/**
			 * Set material culling mode, @see CullMode
			 * @param cull The new culling mode.
			 */
			void setCullMode(CullMode cull)
			{
				mCull = cull;
			}

			/**
			 * Set material depth test.
			 */
			void setDepthTest(bool test)
			{
				mDepthTest = test;
			}

			/**
			 * Set material depth write.
			 */
			void setDepthWrite(bool test)
			{
				mDepthWrite = test;
			}

			/**
			 * If you set this to false, material (and objects connected to it) will be ignored by the engine.
			 * @param nd Whenever we should draw or not this material.
			 */
			void setNoDraw(bool nd)
			{
				mNoDraw = nd;
			}

			/**
			 * Set material content flags, in case you want relate this material
			 * with sounds or any other flag' able property.
			 *
			 * @param flags The new content flags.
			 */
			void setContentFlags(int flags)
			{
				mContentFlags = flags;
			}

			/**
			 * Set material effect flags, in case you want relate this material
			 * with sounds or any other flag' able property.
			 *
			 * @param flags The new effect flags.
			 */
			void setEffectFlags(int flags)
			{
				mEffectFlags = flags;
			}

			/**
			 * Push an materialStage to this material.
			 * @param stage The new stage, wich will be deallocated by material destructor.
			 */
			void pushStage(materialStage* stage)
			{
				mStages.push_back(stage);
			}

			/**
			 * Get the number of stages in material.
			 */
			unsigned int getStagesCount() const
			{
				return mStages.size();
			}

			/**
			 * Get an material stage by the index, note that the engine will not
			 * check if the stage is valid, make sure to do so with getStagesCount()
			 *
			 * @param index The index of the desired stage.
			 */
			const materialStage* getStage(unsigned int index) const
			{
				return mStages[index];
			}

			/**
			 * Returns true if material contains a texture.
			 *
			 * @param name The texture name.
			 */
			bool containsTexture(const std::string& name) const
			{
				std::vector<materialStage*>::const_iterator it;
				for (it = mStages.begin(); it != mStages.end(); it++)
				{
					if ((*it)->containsTexture(name))
						return true;
				}

				return false;
			}

			/**
			 * Returns true if this material is not going to be drawn.
			 */
			bool getNoDraw() const
			{ return mNoDraw; }

			/**
			 * Returns true if this material or any stage on it its not transparent.
			 */
			bool isOpaque() const
			{ 
				std::vector<materialStage*>::const_iterator it;
				for (it = mStages.begin(); it != mStages.end(); it++)
				{
					if (!(*it)->isOpaque())
						return false;
				}

				return true; 
			}

			/**
			 * Return the material content flags
			 */
			int getContentFlags() const
			{ return mContentFlags; }
			
			/**
			 * Return the material effect flags
			 */
			int getEffectFlags() const
			{ return mEffectFlags; }

			/**
			 * Set if the material receives light.
			 */
			void setReceiveLight(bool recv)
			{ if (recv) mReceiveLight = 1; else mReceiveLight = 0; }

			/**
			 * Does this material receive light?
			 */
			bool getReceiveLight() const
			{ return mReceiveLight ? true : false; } 

			/**
			 * Set material stuff before drawing.
			 */
			void start();

			/**
			 * Unset material stuff after drawing.
			 */
			void finish();
	};
}

#endif

