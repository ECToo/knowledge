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

#ifndef _FILEACCESS_H
#define _FILEACCESS_H

#include "prerequisites.h"

namespace k {

class DLL_EXPORT parsingFile
{
	private:
		std::string mFilename;

		unsigned long feof;
		unsigned long index;
		char* string;
	
	public:
		parsingFile(const std::string& filename);
		~parsingFile();

		bool eof();
		bool isReady();
		void skipNextToken();
		std::string getNextToken();

		const std::string& getFilename();
};

}

#endif

