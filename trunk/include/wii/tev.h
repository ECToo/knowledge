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

#ifdef __WII__
#ifndef _TEV_H_
#define _TEV_H_

#include "prerequisites.h"
#include "fileParser.h"
#include "singleton.h"

/**
 * Standard TEV Definitions.
 * Remember the equation-> input(a,b,c,d) -> a*(1 - c) + b*c -> op -> bias + d -> scale -> clamp -> output
 *
 * For the input the following are valid:
 * colorZero, colorOne, colorHalf, colorQuarter -> self explanatory
 * colorC0, colorC1, colorC2 -> color from registers
 * alphaC0, alphaC1, alphaC2 -> alpha from registers
 * colorPrevious, alphaPrevious -> color and alpha from previous TEV stage
 * textureColor, textureAlpha -> color/alpha from texture
 * rasterizedColor, rasterizedAlpha -> self explanatory too
 *
 * For the equation:
 * operations: 
 * 	add, sub, comp_R_gt, comp_R_eq
 * 	comp_GR_gt, comp_GR_eq, comp_BGR_gt, comp_BGR_eq,
 * 	comp_RGB_gt, comp_RGB_eq, comp_A_gt, comp_A_eq
 *
 * bias:
 * 	zero, addHalf, subHalf
 *
 * scale:
 * 	1.0, 2.0, 4.0, 0.5
 *
 * clamp:
 * 	true, false
 *
 * clampMode:
 * 	linear, great, equal, less
 *
 * output:
 * 	reg0, reg1, reg2, regPrev
 *
 */

/* In case you missed the ADD (tev.script) script, here it is.

tev add
{
	stage 0
	{
		// Color Input -> tevColorABCD a b c d
		tevColorABCD colorZero colorZero colorZero textureColor
	
		// Alpha Input -> tevAlphaABCD a b c d
		tevAlphaABCD alphaZero alphaZero alphaZero textureAlpha
	}
	
	stage all
	{
		// Color Input -> tevColorABCD a b c d
		tevColorABCD textureColor colorZero colorZero rasterizedColor
	
		// Alpha Input -> tevAlphaABCD a b c d
		tevAlphaABCD textureAlpha alphaZero alphaZero rasterizedAlpha
	}	
	
	// Color Operation - tevColorOp op bias scale clamp clampMode output
	tevColorOp add zero 1.0 true linear regPrev
	tevAlphaOp add zero 1.0 true linear regPrev
}

*/

namespace k
{
	typedef struct
	{
		u8 index;
		u8 ca, cb, cc, cd;
		u8 aa, ab, ac, ad;
	} tevStage;

	class DLL_EXPORT tev
	{
		private:
			std::map<int, tevStage*> mCustomStages;
			tevStage* mGeneralStage;

			u8 op, bias, scale, clamp, clampMode, out;
			u8 a_op, a_bias, a_scale, a_clamp, a_clampMode, a_out;

		public:
			tev()
			{
				mGeneralStage = NULL;
				mCustomStages.clear();
			}
			~tev()
			{
				if (mGeneralStage)
					delete mGeneralStage;

				std::map<int, tevStage*>::iterator it;
				for (it = mCustomStages.begin(); it != mCustomStages.end(); it++)
					delete it->second;

				mCustomStages.clear();
			}

			void setOp(u8 o, u8 b, u8 scl, u8 cla, u8 claM, u8 ou);
			void setAlphaOp(u8 o, u8 b, u8 scl, u8 cla, u8 claM, u8 ou);
			void pushStage(tevStage* newStage, int index = -1);
			void setup(int stage);
	};

	class DLL_EXPORT tevManager : public singleton<tevManager>
	{
		private:
			std::map<std::string, tev*> mTevs;

		public:
			tevManager();
			~tevManager();

			static tevManager& getSingleton();

			tev* createTev(const std::string& name);
			tev* getTev(const std::string& name);
			void destroyTev(const std::string& name);

			void parseTev(tev* t, parsingFile* file);
			void parseStage(tev* t, parsingFile* file, int index);
			void parseTevScript(parsingFile* file);
			void parseTevScript(const std::string& filename);
	};
}

#endif
#endif

