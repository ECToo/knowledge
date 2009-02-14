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

void logger::infoArg(const char* message, va_list args)
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

