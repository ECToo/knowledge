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

#ifdef __WII__
#ifndef _TEV_H_
#define _TEV_H_

#include "prerequisites.h"
#include "fileParser.h"
#include "singleton.h"

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

