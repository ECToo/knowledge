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

#ifndef _FILEACCESS_H
#define _FILEACCESS_H

#include "prerequisites.h"
#include "vector2.h"
#include "vector3.h"

namespace k {

/**
 * \brief A class to parse script files.
 * This class will parse a script (ascii text mode) file in wich
 * components are separated by white spaces, new line or tabs. The parser
 * will ignore // comments.
 */
class DLL_EXPORT parsingFile
{
	private:
		/**
		 * Filename of the script we are parsing.
		 */
		std::string mFilename;

		/**
		 * Are we in the end of the file?
		 */
		unsigned long feof;

		/**
		 * Were we are in the file
		 */
		unsigned long index;

		/**
		 * The whole file contents are saved here.
		 */
		char* string;
	
	public:
		/**
		 * Creates a parsingFile with the specified filename.
		 *
		 * @param filename The name of the file to be parsed.
		 */
		parsingFile(const std::string& filename);

		/**
		 * Creates a parsingFile with the specified file contents.
		 *
		 * @param string The file contents in a string.
		 */
		parsingFile(char* string);
		~parsingFile();

		/**
		 * Returns true if we reached the end of file, false otherwise.
		 */
		bool eof();

		/**
		 * Returns true if the file can be parsed, false otherwise.
		 */
		bool isReady();

		/**
		 * Ignores the next token in the file.
		 */
		void skipNextToken();

		/**
		 * Get the next token of the file.
		 */
		std::string getNextToken();

		/**
		 * Returns a vector2 from the next token, skipping it.
		 */
		vector2 getVector2();

		/**
		 * Returns a vector3 from the next token, skipping it.
		 */
		vector3 getVector3();

		/**
		 * Returns the path of the file we are parsing.
		 */
		const std::string& getFilename();
};

}

#endif

