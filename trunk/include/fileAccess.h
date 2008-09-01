#ifndef _FILEACCESS_H
#define _FILEACCESS_H

#include "prerequisites.h"

class parsingFile
{
	private:
		unsigned long index;
		char* string;
		char lastResult[1024];
	
	public:
		parsingFile(const std::string& filename);
		~parsingFile();

		bool isReady();
		void skipNextToken();
		const char* getNextToken();
};

#endif

