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

#include "logger.h"

namespace k {

template<> logger* singleton<logger>::singleton_instance = 0;

logger& logger::getSingleton()
{  
	assert(singleton_instance);
	return (*singleton_instance);  
}

logger::logger(const std::string& logFilename)
{
	mLogFile = logFilename;
	
	// Reset File
	FILE* logFile = fopen(mLogFile.c_str(), "w");
	if (logFile)
	{
		fprintf(logFile, "========= Powered by knowledge %s =========\n", K_VERSION);
		fclose(logFile);
	}
}
			
void logger::setLogMode(logMode log)
{
	mLoggingMode = log;
}

void logger::infoArg(const char* message, std::va_list args)
{
	switch (mLoggingMode)
	{
		case LOGMODE_STDOUT:
			vprintf(message, args);
			break;
		case LOGMODE_BOTH:
			vprintf(message, args);
		case LOGMODE_FILE:
			{
				FILE* logFile = fopen(mLogFile.c_str(), "a");
				if (logFile)
				{
					vfprintf(logFile, message, args);
					fclose(logFile);
				}
			}
			break;
	};
}

void logger::info(const std::string& message)
{
	switch (mLoggingMode)
	{
		case LOGMODE_STDOUT:
			printf("%s\n", message.c_str());
			break;
		case LOGMODE_FILE:
			{
				FILE* logFile = fopen(mLogFile.c_str(), "a");
				if (logFile)
				{
					fprintf(logFile, "%s\n", message.c_str());
					fclose(logFile);
				}
			}
			break;
		case LOGMODE_BOTH:
			{
				printf("%s\n", message.c_str());

				FILE* logFile = fopen(mLogFile.c_str(), "a");
				if (logFile)
				{
					fprintf(logFile, "%s\n", message.c_str());
					fclose(logFile);
				}
			}
			break;
	};
}
	
void DLL_EXPORT assertFail(const char* file, int line)
{
	std::stringstream assertMsg;
	assertMsg << "Assertion failed on " << std::string(file);
	assertMsg << ":" << line;

	S_LOG_INFO(assertMsg.str());

	exit(1);
}

} // namespace

