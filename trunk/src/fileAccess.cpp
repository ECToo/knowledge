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

