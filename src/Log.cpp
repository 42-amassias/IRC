#include <sstream>
#include <iostream>
#include <iomanip>

#include "Log.hpp"

namespace Log
{
	LogStream Debug("DEBUG", &std::cout, COLOR_SHELL_BLUE);
	LogStream Info("INFO", &std::cout, COLOR_SHELL_GRAY);
	LogStream Warn("WARN", &std::cerr, COLOR_SHELL_YELLOW);
	LogStream Error("ERROR", &std::cerr, COLOR_SHELL_RED);
	
	LogStream::LogStream(
			std::string const& lvlname,
			std::ostream *_out,
			std::string const& lvlcolor) :
		m_lvlname(lvlname),
		m_lvlcolor(lvlcolor),
		m_out(_out)
	{
	}
	
	LogStream::~LogStream()
	{
	}
	
	std::string	formattedDateTime(void)
	{
		time_t				t = time(NULL);
		struct std::tm		*ltm = localtime(&t);
		std::stringstream	ss;

		if (ltm == NULL)
			return ("LOCAL TIME ERROR");
		ss << std::setfill('0')
			<< std::setw(2) << ltm->tm_mon + 1
			<< "/" << std::setw(2) << ltm->tm_mday
			<< "/" << ltm->tm_year + 1900
			<< " " << std::setw(2) << ltm->tm_hour
			<< ":" << std::setw(2) << ltm->tm_min
			<< ":" << std::setw(2) << ltm->tm_sec;
		return (ss.str());
	}
}

