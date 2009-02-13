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

#ifdef __WII__
#include "tev.h"
#include "logger.h"
	
namespace k {

void tev::pushStage(tevStage* newStage, int index)
{
	kAssert(newStage != NULL);

	if (index == -1)
		mGeneralStage = newStage;
	else
	{
		mCustomStages[index] = newStage;

		std::stringstream st;
		st << "Custom stage " << index << " added.";
		S_LOG_INFO(st.str());
	}
}

void tev::setOp(u8 o, u8 b, u8 scl, u8 cla, u8 claM, u8 ou)
{
	op = o;
	bias = b;
	scale = scl;
	clamp = cla;
	clampMode = claM;
	out = ou;
}

void tev::setAlphaOp(u8 o, u8 b, u8 scl, u8 cla, u8 claM, u8 ou)
{
	a_op = o;
	a_bias = b;
	a_scale = scl;
	a_clamp = cla;
	a_clampMode = claM;
	a_out = ou;
}

void tev::setup(int stage)
{
	tevStage* thisStage = NULL;

	std::map<int, tevStage*>::const_iterator mIt = mCustomStages.find(stage);
	if (mIt != mCustomStages.end())
		thisStage = mIt->second;
	else
		thisStage = mGeneralStage;

	GX_SetTevColorIn(GX_TEVSTAGE0 + stage, thisStage->ca, thisStage->cb,
			thisStage->cc, thisStage->cd);
	GX_SetTevAlphaIn(GX_TEVSTAGE0 + stage, thisStage->aa, thisStage->ab,
			thisStage->ac, thisStage->ad);

	GX_SetTevColorOp(GX_TEVSTAGE0 + stage, op, bias, scale, clamp, out);
	GX_SetTevAlphaOp(GX_TEVSTAGE0 + stage, a_op, a_bias, a_scale, a_clamp, a_out);
}

template<> tevManager* singleton<tevManager>::singleton_instance = 0;

tevManager& tevManager::getSingleton()
{
	kAssert(singleton_instance);
	return (*singleton_instance);  
}

tevManager::tevManager()
{
	mTevs.clear();
}

tevManager::~tevManager()
{
	std::map<std::string, tev*>::iterator it;
	for (it = mTevs.begin(); it != mTevs.end();)
	{
		tev* thisTev = it->second;
		mTevs.erase(it++);

		delete thisTev;
	}
}

tev* tevManager::createTev(const std::string& name)
{
	tev* newTev = getTev(name);
	if (newTev)
	{
		return newTev;
	}
	else
	{
		newTev = new tev();
		kAssert(newTev != NULL);

		mTevs[name] = newTev;
		return newTev;
	}
}

tev* tevManager::getTev(const std::string& name)
{
	std::map<std::string, tev*>::const_iterator mIt = mTevs.find(name);
	if (mIt != mTevs.end())
	{
		return mIt->second;
	}
	else
	{
		return NULL;
	}
}

void tevManager::destroyTev(const std::string& name)
{
	tev* thisTev = getTev(name);
	if (thisTev)
	{
		std::map<std::string, tev*>::iterator it;
		for(it = mTevs.begin(); it != mTevs.end(); )
		{
			if (it->first == name)
			{
				thisTev = it->second;
				mTevs.erase(it++);
				delete thisTev;

				return;
			}
			else
			{
				++it;
			}
		}
	}
}

u8 getOp(const std::string& token)
{
	if (token == "add")
		return GX_TEV_ADD;
	else
	if (token == "sub")
		return GX_TEV_SUB;
	else
	if (token == "comp_R_gt")
		return GX_TEV_COMP_R8_GT;
	else
	if (token == "comp_R_eq")
		return GX_TEV_COMP_R8_EQ;
	else
	if (token == "comp_GR_gt")
		return GX_TEV_COMP_GR16_GT;
	else
	if (token == "comp_GR_eq")
		return GX_TEV_COMP_GR16_EQ;
	else
	if (token == "comp_BGR_gt")
		return GX_TEV_COMP_BGR24_GT;
	else
	if (token == "comp_BGR_eq")
		return GX_TEV_COMP_BGR24_EQ;
	else
	if (token == "comp_RGB_gt")
		return GX_TEV_COMP_RGB8_GT;
	else
	if (token == "comp_RGB_eq")
		return GX_TEV_COMP_RGB8_EQ;
	else
	if (token == "comp_A_gt")
		return GX_TEV_COMP_A8_GT;
	else
	if (token == "comp_A_eq")
		return GX_TEV_COMP_R8_EQ;
	else
	{
		S_LOG_INFO("Invalid Tev Operation " + token);
		return GX_TEV_ADD;
	}
}

u8 getBias(const std::string& token)
{
	if (token == "zero")
		return GX_TB_ZERO;
	else
	if (token == "addHalf")
		return GX_TB_ADDHALF;
	else
	if (token == "subHalf")
		return GX_TB_SUBHALF;
	else
	{
		S_LOG_INFO("Invalid bias " + token);
		return GX_TB_ZERO;
	}
}

u8 getScale(const std::string& token)
{
	float scale = atof(token.c_str());

	if (scale == 1)
		return GX_CS_SCALE_1;
	else
	if (scale == 2)
		return GX_CS_SCALE_2;
	else
	if (scale == 4)
		return GX_CS_SCALE_4;
	else
	if (scale == 0.5)
		return GX_CS_DIVIDE_2;
	else
	{
		S_LOG_INFO("Invalid scale " + token);
		return GX_CS_SCALE_1;
	}
}

u8 getClamp(const std::string& token)
{
	if (token == "true")
		return GX_TRUE;
	else
		return GX_FALSE;
}

u8 getClampMode(const std::string& token)
{
	if (token == "linear")
		return GX_TC_LINEAR;
	else
	if (token == "great")
		return GX_TC_GE;
	else
	if (token == "equal")
		return GX_TC_EQ;
	else
	if (token == "less")
		return GX_TC_LE;
	else
	{
		S_LOG_INFO("Invalid clamp mode " + token);
		return GX_TC_LINEAR;
	}
}

u8 getOutput(const std::string& token)
{
	if (token == "reg0")
		return GX_TEVREG0;
	else
	if (token == "reg1")
		return GX_TEVREG1;
	else
	if (token == "reg2")
		return GX_TEVREG2;
	else
	if (token == "regPrev")
		return GX_TEVPREV;
	else
	{
		S_LOG_INFO("Invalid output " + token);
		return GX_TEVPREV;
	}
}

void tevManager::parseTev(tev* t, parsingFile* file)
{
	kAssert(t != NULL);
	kAssert(file != NULL);

	std::string token;
	unsigned int openBraces = 1;

	file->skipNextToken(); // { 

	while (openBraces)
	{
		if (file->eof())
			return;

		if (token == "stage")
		{
			// Get the stage # or "all" for generic
			token = file->getNextToken();

			if (token == "all")
				parseStage(t, file, -1);
			else
				parseStage(t, file, atoi(token.c_str()));
		}
		else
		if (token == "tevColorOp" || token == "tevAlphaOp")
		{
			std::string originalToken = token;
			u8 op, bias, scale, clamp, clampMode, out;

			// Op
			token = file->getNextToken();
			op = getOp(token);

			// Bias
			token = file->getNextToken();
			bias = getBias(token);

			// Scale
			token = file->getNextToken();
			scale = getScale(token);

			// Clamp
			token = file->getNextToken();
			clamp = getClamp(token);

			// ClampMode
			token = file->getNextToken();
			clampMode = getClampMode(token);

			// Output
			token = file->getNextToken();
			out = getOutput(token);

			if (originalToken == "tevColorOp")
				t->setOp(op, bias, scale, clamp, clampMode, out);
			else
				t->setAlphaOp(op, bias, scale, clamp, clampMode, out);
		}

		token = file->getNextToken();

		// Script properties
		if (token == "}")
			openBraces--;
		else
		if (token == "{")
			openBraces++;
	}
}

u8 getAlphaIn(const std::string& token)
{
	if (token == "previousAlpha")
		return GX_CA_APREV;
	else
	if (token == "alphaC0")
		return GX_CA_A0;
	else
	if (token == "alphaC1")
		return GX_CA_A1;
	else
	if (token == "alphaC2")
		return GX_CA_A2;
	else
	if (token == "textureAlpha")
		return GX_CA_TEXA;
	else
	if (token == "rasterizedAlpha")
		return GX_CA_RASA;
	else
	if (token == "alphaZero")
		return GX_CA_ZERO;
	else
	{
		S_LOG_INFO("Invalid alpha parameter " + token);
		return GX_CA_APREV;
	}
}

u8 getColorIn(const std::string& token)
{
	if (token == "colorOne")
		return GX_CC_ONE;
	else
	if (token == "colorHalf")
		return GX_CC_HALF;
	else
	if (token == "colorQuarter")
		return GX_CC_KONST;
	else
	if (token == "colorZero")
		return GX_CC_ZERO;
	else
	if (token == "alphaZero")
		return GX_CA_ZERO;
	else
	if (token == "colorC0")
		return GX_CC_C0;
	else
	if (token == "colorC1")
		return GX_CC_C1;
	else
	if (token == "colorC2")
		return GX_CC_C2;
	else
	if (token == "alphaC0")
		return GX_CC_A0;
	else
	if (token == "alphaC1")
		return GX_CC_A1;
	else
	if (token == "alphaC2")
		return GX_CC_A2;
	else
	if (token == "colorPrevious")
		return GX_CC_CPREV;
	else
	if (token == "alphaPrevious")
		return GX_CC_APREV;
	else
	if (token == "textureColor")
		return GX_CC_TEXC;
	else
	if (token == "textureAlpha")
		return GX_CC_TEXA;
	else
	if (token == "rasterizedColor")
		return GX_CC_RASC;
	else
	if (token == "rasterizedAlpha")
		return GX_CC_RASA;
	else
	if (token == "previousColor")
		return GX_CC_CPREV;
	else
	if (token == "previousAlpha")
		return GX_CC_APREV;
	else
	{
		S_LOG_INFO("Invalid color parameter " + token);
		return GX_CC_CPREV;
	}
}

void tevManager::parseStage(tev* t, parsingFile* file, int index)
{
	kAssert(t != NULL);
	kAssert(file != NULL);

	std::string token;
	unsigned int openBraces = 1;

	// Up to 16 stages
	if (index > 15)
	{
		std::stringstream num;
		num << index;

		S_LOG_INFO("Invalid stage number " + num.str());
		while(true)
		{
			token = file->getNextToken();
			if (token == "{")
				openBraces++;
			else
			if (token == "}")
			{
				if (openBraces == 0)
					break;
				else
					openBraces--;
			}
		}

		return;
	}

	// Ok read the stages correctly
	tevStage* newStage = new tevStage;
	kAssert(newStage != NULL);
	memset(newStage, 0, sizeof(tevStage));

	// Set Index
	newStage->index = index;
	openBraces = 1;

	file->skipNextToken(); // { 
	while (openBraces)
	{
		if (file->eof())
			return;

		if (token == "tevColorABCD")
		{
			u8 abcd[4];

			for (int i = 0; i < 4; i++)
			{
				token = file->getNextToken();
				abcd[i] = getColorIn(token);
			}

			newStage->ca = abcd[0];
			newStage->cb = abcd[1];
			newStage->cc = abcd[2];
			newStage->cd = abcd[3];
		}
		else
		if (token == "tevAlphaABCD")
		{
			u8 abcd[4];

			for (int i = 0; i < 4; i++)
			{
				token = file->getNextToken();
				abcd[i] = getAlphaIn(token);
			}

			newStage->aa = abcd[0];
			newStage->ab = abcd[1];
			newStage->ac = abcd[2];
			newStage->ad = abcd[3];
		}

		token = file->getNextToken();

		// Script properties
		if (token == "}")
			openBraces--;
		else
		if (token == "{")
			openBraces++;
	}

	// add stage
	t->pushStage(newStage, index);
}

void tevManager::parseTevScript(parsingFile* file)
{
	kAssert(file != NULL);

	if (!file->isReady())
	{
		S_LOG_INFO("File " + file->getFilename() + " is not ready to be parsed.");
		return;
	}

	std::string token = file->getNextToken();
	while (!file->eof())
	{
		if (token == "tev")
		{
			// Tev name
			token = file->getNextToken();

			// Check for tev existance
			tev* newTev = getTev(token);
			if (newTev)
			{
				S_LOG_INFO("tev " + token + " already exists.");

				file->skipNextToken(); // {
				unsigned int openBraces = 1;

				while(true)
				{
					token = file->getNextToken();
					if (token == "{")
						openBraces++;
					else
					if (token == "}")
					{
						if (openBraces == 0)
							break;
						else
							openBraces--;
					}
				}
			}
			else
			// Create a new TEV
			{
				newTev = createTev(token);
				kAssert(newTev != NULL);

				S_LOG_INFO("Parsing tev " + token + "...");
				parseTev(newTev, file);
				S_LOG_INFO("Finished parsing tev");
			}
		}

		// Next Token
		token = file->getNextToken();
	}
}

}
#endif

