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

	extern void DLL_EXPORT assertFail(const char* file, int line);

	#define kAssert(expr) ((expr) ? (void)0 : k::assertFail(__FILE__, __LINE__))
}

#endif

