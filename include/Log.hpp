#ifndef  LOG_H
# define LOG_H

# include <iostream>
# include <ctime>

# define COLOR_SHELL_RED "\e[31m"
# define COLOR_SHELL_BLUE "\e[34m"
# define COLOR_SHELL_YELLOW "\e[33m"
# define COLOR_SHELL_GRAY "\e[37m"
# define COLOR_SHELL_DEFAULT "\e[0m"

namespace Log
{
	class LogStream
	{
		private:
			std::ostream	*m_out;
			std::string		m_lvlname;
			std::string		m_lvlcolor;
		public:
			LogStream(
					std::string const& lvlname,
					std::ostream *_out,
					std::string const& lvlcolor);
			~LogStream();

			template<typename T>
				std::ostream &operator<<(T _in);
	};

	extern LogStream	Debug;
	extern LogStream	Info;
	extern LogStream	Warn;
	extern LogStream	Error;

	std::string formattedDateTime(void);
}

# include "Log.ipp"

#endif

