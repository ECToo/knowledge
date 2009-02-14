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

#ifndef _LOGGER_H
#define _LOGGER_H

#include "prerequisites.h"
#include "singleton.h"

#define S_LOG_INFO(X)(logger::getSingleton().info(X))
#define K_LOG_INFO(X)(k::logger::getSingleton().info(X))
#define S_LOG_INFO_ARG(X,Y)(logger::getSingleton().infoArg(X,Y))
#define K_LOG_INFO_ARG(X,Y)(k::logger::getSingleton().infoArg(X,Y))

namespace k
{
	enum logMode
	{
		LOGMODE_STDOUT,
		LOGMODE_FILE,
		LOGMODE_BOTH
	};

	class DLL_EXPORT logger : public singleton<logger>
	{
		private:
			std::string mLogFile;
			logMode mLoggingMode;

		public:
			logger(const std::string& logFilename);

			void setLogMode(logMode log);
			void info(const std::string& message);
			void infoArg(const char* message, va_list args);

			static logger& getSingleton();
	};

	extern void assertFail(const char* file, int line);

	#define kAssert(expr) ((expr) ? (void)0 : k::assertFail(__FILE__, __LINE__))
}

#endif

