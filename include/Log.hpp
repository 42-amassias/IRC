#ifndef  LOG_H
# define LOG_H

# include <iostream>
# include <ctime>
# include <strstream>

namespace Log
{
	class LogStream
	{
		private:
			std::ostrstream	*m_out;
			std::string		m_lvlname;
		public:
			LogStream(std::string const& lvlname, std::ostrstream *_out);
			~LogStream();

			template<typename T>
			std::ostrstream &operator<<(T _in);
	};
	
	std::string formattedDateTime(void);
}

# include "Log.ipp"

#endif

