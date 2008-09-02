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

parsingFile::parsingFile(const std::string& filename)
{
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
			input.close();
			return;
		}

		input.read(string, length);
		input.close();
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

void parsingFile::skipNextToken()
{
	// TODO: Real Skipping
	getNextToken();
}

const char* parsingFile::getNextToken()
{
	if (!string)
		return NULL;

	char token[1024];
	char actualChar;
	int len;

	memset(token, 0, 1024);
	memset(lastResult, 0, 1024);

	actualChar = string[index];
	while (actualChar)
	{
		// We have a comment here, take everything until the end of the line
		if (actualChar == '/' && string[index + 1] == '/')
		{
			// Next Character is a slash too
			index++;
			while (actualChar != '\n')
				actualChar = string[++index];

			actualChar = string[++index];
			continue;
		}
		else
		// Double Quotes, means a full string until the next double quotes
		if (actualChar == '\"')
		{
			actualChar = string[++index];

			if (actualChar == '\"')
				actualChar = string[++index];
			else
			while (actualChar && actualChar != '\"')
			{
				len = strlen(token);
				token[len] = actualChar;

				actualChar = string[++index];
			}

			if (actualChar == '\"')
				actualChar = string[++index];
		}
		
		// Tab, WhiteSpace, newline - breaks the string
		if (actualChar == ' ' || actualChar == '\t' || actualChar == '\n')
		{
			actualChar = string[++index];
			
			// If our token is empty, continue looking.
			// We dont want to spend time here
			if (!token[0])
				continue;
			else
				break;
		}

		// Insert Character into "Token"
		len = strlen(token);
		token[len] = actualChar;

		actualChar = string[++index];
	}

	len = strlen(token);
	if (!len)
		lastResult[0] = '\0';

	if (len > 1024)
	{
		strncpy(lastResult, token, 1024);
		lastResult[1024] = 0;
	}
	else
	{
		strncpy(lastResult, token, len);
		lastResult[len] = 0;
	}

	return lastResult;
}

