#include "Log.hpp"

#include "Command.hpp"

Command::Command() {}

Command::Command(std::vector<char> const& raw_command)
{
	Log::Debug << std::string(raw_command.begin(), raw_command.end()) << std::endl;
}

Command::~Command() {}

