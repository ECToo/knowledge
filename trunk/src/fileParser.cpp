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

#include "fileParser.h"
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

		try
		{
			string = new char[length + 1];
			memset(string, 0, length);
			input.read(string, length);
			input.close();

			feof = length;
			index = 0;
		}

		catch (...)
		{
			S_LOG_INFO("Failed to allocate file string.");
			input.close();
			return;
		}
	}
}
		
parsingFile::parsingFile(char* str)
{
	if (!str)
	{
		S_LOG_INFO("Invalid pointer to string in file");
		return;
	}

	mFilename = "__FromString__";

	string = str;
	feof = strlen(str);
	index = 0;
}

parsingFile::~parsingFile()
{
	if (string && mFilename != "__FromString__")
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
	if (!string)
	{
		S_LOG_INFO("File is not ready!");
		return;
	}

	// Keep position ;)
	char actualChar = string[index];
	bool charWritten = false;

	while (actualChar)
	{
		// Start parsing a string 'til we find
		// another double quotes
		if (actualChar == '\"')
		{
			actualChar = string[++index];
			while (actualChar != '\"')
			{
				actualChar = string[++index];
				charWritten = true;
			
				if (index >= feof)
					return;
			}

			if (index >= feof)
				return;

			index++;
			return;
		}
		else
		// Pay attention to carriage return
		if (actualChar == ' ' || actualChar == '\t' || actualChar == '\n' || actualChar == 0xd)
		{
			actualChar = string[++index];
			while (actualChar == '\n' || actualChar == ' ' || actualChar == '\t' || actualChar == 0xd)
			{
				actualChar = string[++index];
				if (index >= feof)
					return;
			}

			if (index >= feof)
				return;

			if (!charWritten)
				continue;
			else
				return;
		}
		else
		if (actualChar == '/' && string[index + 1] == '/')
		{
			while (actualChar != '\n')
			{
				actualChar = string[++index];
				if (index >= feof)
					return;
			}
				
			actualChar = string[++index];
			if (index >= feof)
				return;

			continue;
		}

		//
		charWritten = true;
		actualChar = string[++index];
		if (index >= feof)
			return;
	}
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

	if (index >= feof)
		return std::string("");

	// I hope we dont find a string greater than that buffer.
	unsigned int bufferPtr = 0;
	char buffer[2048];
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
			if (index >= feof)
				return std::string(buffer);

			while (actualChar != '\"')
			{
				// Maximum string size, if you got 
				// anything greater than it, im sorry man
				if (bufferPtr > 2047)
				{
					S_LOG_INFO("Too big string detected (> 2048 chars), cutting now.");
					bufferPtr = 2047;
				}

				buffer[bufferPtr++] = actualChar;
				actualChar = string[++index];

				if (index >= feof)
					break;
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
			if (index >= feof)
				return std::string(buffer);

			while (actualChar == '\n' || actualChar == ' ' || actualChar == '\t' || actualChar == 0xd)
			{
				actualChar = string[++index];
				if (index >= feof)
					break;
			}

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
			{
				actualChar = string[++index];
				if (index >= feof)
					break;
			}
				
			actualChar = string[++index];
			if (index >= feof)
			{
				buffer[++bufferPtr] = 0;
				return std::string(buffer);
			}

			continue;
		}

		//
		buffer[bufferPtr++] = actualChar;
		actualChar = string[++index];

		if (index >= feof)
		{
			buffer[++bufferPtr] = 0;
			return std::string(buffer);
		}
	}

	return std::string(buffer);
}
		
vector2 parsingFile::getVector2()
{
	float x, y;

	std::string token = getNextToken();
	x = atof(token.c_str());
	token = getNextToken();
	y = atof(token.c_str());

	return vector2(x, y);
}

vector3 parsingFile::getVector3()
{
	float x, y, z;

	std::string token = getNextToken();
	x = atof(token.c_str());
	token = getNextToken();
	y = atof(token.c_str());
	token = getNextToken();
	z = atof(token.c_str());

	return vector3(x, y, z);
}

}

