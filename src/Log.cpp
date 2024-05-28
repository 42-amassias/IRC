#include <sstream>
#include <iomanip>

#include "Log.hpp"

std::string	Log::formattedDateTime(void)
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

