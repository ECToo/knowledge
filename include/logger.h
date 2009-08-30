/*
    Copyright (C) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

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

	/**
	 * \brief Logger class.
	 * This singleton will be created at the creation of k::root and will have its
	 * file path set to the file name passed on the root constructor. It can make logging
	 * to files and to screen (STDOUT)
	 */
	class DLL_EXPORT logger : public singleton<logger>
	{
		private:
			std::string mLogFile;
			logMode mLoggingMode;

		public:
			/**
			 * Creates the loggin file, replacing it if it exists.
			 *
			 * @param logFilename The path of the log file.
			 */
			logger(const std::string& logFilename);

			/**
			 * Set the log mode.
			 * @param log The Log mode, @see k::logMode
			 */
			void setLogMode(logMode log);
			
			/**
			 * Outputs log information.
			 * @param message The string to be outputed
			 */
			void info(const std::string& message);

			/**
			 * Outputs log information.
			 * @param message The string to be outputed
			 * @param args The arguments for the string.
			 */
			void infoArg(const char* message, va_list args);

			/**
			 * Returns the logger singleton instance.
			 */
			static logger& getSingleton();
	};

	extern void assertFail(const char* file, int line);

	#define kAssert(expr) ((expr) ? (void)0 : k::assertFail(__FILE__, __LINE__))
}

#endif

