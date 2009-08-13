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

#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "prerequisites.h"
#include "vector3.h"
#include "texture.h"
#include "wii/tev.h"

namespace k
{
	enum CullMode
	{
		CULLMODE_NONE,
		CULLMODE_BACK,
		CULLMODE_FRONT,
		CULLMODE_BOTH
	};

	enum TexEnvFunctions
	{
		TEXENV_REPLACE,
		TEXENV_MODULATE,
		TEXENV_BLEND,
		TEXENV_DECAL,
		TEXENV_ADD,

		TEXENV_MAX_ENV
	};

	#define K_MAX_STAGE_TEXTURES 16

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
			 * All textures within this stage.
			 */
			texture* mTextures[K_MAX_STAGE_TEXTURES];

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
			 * @name The name of the texture we are checking against.
			 */
			bool containsTexture(const std::string& name) const;

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

			void draw();
			void finish();
	};

	class DLL_EXPORT material
	{
		protected:
			vector3 mAmbient;
			vector3 mDiffuse;
			vector3 mSpecular;

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
			material();
			~material();

			/**
			 * Spawn material with a single texture.
			 */
			material(texture* tex);

			/**
			 * Spawn material with a single texture.
			 */
			material(const std::string& filename);

			void setAmbient(const vector3& color)
			{
				mAmbient = color;
			}

			void setDiffuse(const vector3& color)
			{
				mDiffuse = color;
			}

			void setSpecular(const vector3& color)
			{
				mSpecular = color;
			}

			void setCullMode(CullMode cull)
			{
				mCull = cull;
			}

			void setDepthTest(bool test)
			{
				mDepthTest = test;
			}

			void setDepthWrite(bool test)
			{
				mDepthWrite = test;
			}

			void setNoDraw(bool nd)
			{
				mNoDraw = nd;
			}

			void setContentFlags(int flags)
			{
				mContentFlags = flags;
			}

			void setEffectFlags(int flags)
			{
				mEffectFlags = flags;
			}

			void pushStage(materialStage* stage)
			{
				mStages.push_back(stage);
			}

			unsigned int getStagesCount() const
			{
				return mStages.size();
			}

			const materialStage* getStage(unsigned int index) const
			{
				return mStages[index];
			}

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

			bool getNoDraw() const
			{ return mNoDraw; }

			bool isOpaque() const
			{ return mIsOpaque; }

			int getContentFlags() const
			{ return mContentFlags; }
			
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

