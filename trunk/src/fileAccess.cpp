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

#include "fileAccess.h"
#include "logger.h"

namespace k {

parsingFile::parsingFile(const std::string& filename)
{
	mFilename = filename;

	std::ifstream input;
	input.open(filename.c_str(), std::ifstream::in);

	string = NULL;

	if (input.is_open())
	{
		unsigned int length = 0;

		input.seekg(0, std::ios::end);
		length = input.tellg();
		input.seekg(0, std::ios::beg);

		string = new char[length];
		if (!string)
		{
			S_LOG_INFO("Failed to allocate file string.");
			input.close();
			return;
		}

		input.read(string, length);
		input.close();

		feof = length;
		index = 0;
	}
}

parsingFile::~parsingFile()
{
	if (string)
		delete [] string;
}

bool parsingFile::isReady()
{
	if (string)
		return true;
	else
		return false;
}
		
const std::string& parsingFile::getFilename()
{
	return mFilename;
}

void parsingFile::skipNextToken()
{
	// TODO: Real Skipping
	getNextToken();
}

bool parsingFile::eof()
{
	if (index >= feof)
		return true;
	else
		return false;
}

std::string parsingFile::getNextToken()
{
	if (!string)
	{
		S_LOG_INFO("File is not ready!");
		return NULL;
	}

	// I hope we dont find a string greater than that.
	unsigned int bufferPtr = 0;
	char buffer[1024];
	char actualChar;

	memset(buffer, 0, 1024);
	actualChar = string[index];

	while (actualChar)
	{
		// Start parsing a string 'til we find
		// another double quotes
		if (actualChar == '\"')
		{
			actualChar = string[++index];
			while (actualChar != '\"')
			{
				buffer[bufferPtr++] = actualChar;
				actualChar = string[++index];
				// TODO: Check strings larger than 1024 chars
			}

			index++;

			buffer[++bufferPtr] = 0;
			return std::string(buffer);
		}
		else
		// Pay attention to carriage return
		if (actualChar == ' ' || actualChar == '\t' || actualChar == '\n' || actualChar == 0xd)
		{
			actualChar = string[++index];
			while (actualChar == '\n' || actualChar == ' ' || actualChar == '\t' || actualChar == 0xd)
				actualChar = string[++index];

			if (!buffer[0])
				continue;
			else
			{
				buffer[++bufferPtr] = 0;
				return std::string(buffer);
			}
		}
		else
		if (actualChar == '/' && string[index + 1] == '/')
		{
			while (actualChar != '\n')
				actualChar = string[++index];
				
			actualChar = string[++index];
			continue;
		}

		//
		buffer[bufferPtr++] = actualChar;
		actualChar = string[++index];
	}

	return std::string(buffer);
}

}

