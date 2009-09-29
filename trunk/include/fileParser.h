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

		/**
		 * Destructor, deallocating file string.
		 */
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

