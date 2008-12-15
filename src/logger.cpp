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
		fprintf(logFile, "Knowledge 1.0 - Test Version\n");
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
			std::cout << message << std::endl;
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
				std::cout << message << std::endl;

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

} // namespace

