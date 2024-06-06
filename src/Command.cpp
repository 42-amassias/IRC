#include "Log.hpp"

#include "Command.hpp"

Command::Command(void)
{
}

Command::Command(Command const& o) :
	m_prefix(o.m_prefix),
	m_command(o.m_command),
	m_parameters(o.m_parameters)
{
}

Command::Command(std::vector<char> const& raw_command)
{
	Log::Debug << std::string(raw_command.begin(), raw_command.end()) << std::endl;
}

Command::~Command(void)
{
}

Command&	Command::operator=(Command const& o)
{
	if (this == &o)
		return (*this);
	m_prefix = o.m_prefix;
	m_command = o.m_command;
	m_parameters = o.m_parameters;
	return (*this);
}
