#ifndef _LOGGER_H
#define _LOGGER_H

#include "prerequisites.h"
#include "singleton.h"

namespace k
{
	enum logMode
	{
		LOGMODE_STDOUT,
		LOGMODE_FILE
	};

	class logger : public singleton<logger>
	{
		private:
			std::string mLogFile;
			logMode mLoggingMode;

		public:
			logger(const std::string& logFilename);

			void setLogMode(logMode log);
			void info(const std::string& message);

			static logger& getSingleton();
	};
}

#endif

