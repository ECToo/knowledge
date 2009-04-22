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

#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "prerequisites.h"
#include "vector3.h"
#include "texture.h"
#include "tev.h"


namespace k
{
	enum CullMode
	{
		CULLMODE_NONE,
		CULLMODE_BACK,
		CULLMODE_FRONT,
		CULLMODE_BOTH
	};

	class DLL_EXPORT material
	{
		protected:
			vector3 mAmbient;
			vector3 mDiffuse;
			vector3 mSpecular;

			CullMode mCull;
			bool mDepthTest;
			bool mNoDraw;

			unsigned int mTextureUnits;

			int mContentFlags;
			int mEffectFlags;

			std::list<textureStage*> mTextures;

		public:
			material();
			~material();

			void setAmbient(const vector3& color);
			void setDiffuse(const vector3& color);
			void setSpecular(const vector3& color);
			void setCullMode(CullMode cull);
			void setDepthTest(bool test);
			void setNoDraw(bool nd);

			void setContentFlags(int flags);
			void setEffectFlags(int flags);

			int getContentFlags(); 
			int getEffectFlags(); 

			void setTextureUnits(unsigned int tex);
			void setSingleTexture(unsigned int w, unsigned int h, kTexture* tex);
			void setSingleTexture(texture* tex);
			void pushTexture(textureStage* tex);

			unsigned int getNumberOfTextureStages() const;
			textureStage* getTextureStage(unsigned short index);

			bool containsTexture(const std::string& name);
			bool getNoDraw();

			/**
			 * Prepare material, before drawing.
			 */
			void prepare();

			/**
			 * Reset material set properties, after drawing.
			 */
			void finish();

			unsigned int getTextureUnits();
	};
}

#endif

