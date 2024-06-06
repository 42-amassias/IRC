#include "Log.hpp"

#include "Command.hpp"

#include <stdexcept>

static
void	_skip_spaces(
			std::vector<char>::const_iterator& itr,
			std::vector<char>::const_iterator const& end
			);

static
std::string	_extract_word(
				std::vector<char>::const_iterator& itr,
				std::vector<char>::const_iterator const& end
				);

static
std::string	_extract_prefix(
				std::vector<char>::const_iterator& itr,
				std::vector<char>::const_iterator const& end
				);

static
std::string	_extract_command(
				std::vector<char>::const_iterator& itr,
				std::vector<char>::const_iterator const& end
				);

static
void	_extract_params(
			std::vector<char>::const_iterator& itr,
			std::vector<char>::const_iterator const& end,
			std::vector<std::string>& params
			);

Command::Command(void) :
	m_prefix(""),
	m_command(""),
	m_parameters()
{
}

Command::Command(Command const& o) :
	m_prefix(o.m_prefix),
	m_command(o.m_command),
	m_parameters(o.m_parameters)
{
}

Command::Command(std::vector<char> const& raw_command) :
	m_prefix(""),
	m_command(""),
	m_parameters()
{
	Log::Debug << "Creating command object for: \"" + std::string(raw_command.begin(), raw_command.end()) << "\"" << std::endl;
	try
	{
		std::vector<char>::const_iterator	itr(raw_command.begin());
		std::vector<char>::const_iterator	end(raw_command.end());

		m_prefix = _extract_prefix(itr, end);
		if (itr == end)
			throw std::runtime_error("Missing command");
		m_command = _extract_command(itr, end);
		_extract_params(itr, end, m_parameters);
		if (itr == end || *itr != '\r')
			throw std::runtime_error("Missing `CR`");
		// No need to check for LF because `CommandBuffer` already discards it.
		if (++itr != end)
			throw std::runtime_error("Too much dada");
	}
	catch (std::runtime_error const& e)
	{
		Log::Error << "Parsing error: " << e.what() << std::endl;
		throw std::runtime_error("Invalid command: " + std::string(raw_command.data()));
	}
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

static
void	_skip_spaces(
			std::vector<char>::const_iterator& itr,
			std::vector<char>::const_iterator const& end
			)
{
	if (itr == end || *itr != ' ')
		throw std::runtime_error("Missing white space");
	++itr;
	while (itr != end && *itr == ' ')
		++itr;
}

static
std::string	_extract_word(
				std::vector<char>::const_iterator& itr,
				std::vector<char>::const_iterator const& end
				)
{
	std::string	word;

	while (itr != end && std::isalnum(*itr))
		word += *itr++;
	return (word);
}

static
std::string	_extract_prefix(
				std::vector<char>::const_iterator& itr,
				std::vector<char>::const_iterator const& end
				)
{
	std::string	prefix;
	std::string	word;

	if (*itr != ':')
		return (prefix);
	word = _extract_word(++itr, end);
	if (word.empty() || itr == end)
		throw std::runtime_error("Invalid prefix");
	prefix += word;
	if (*itr == '!')
	{
		word = _extract_word(++itr, end);
		if (word.empty() || itr == end)
			throw std::runtime_error("Invalid username");
		prefix += '!' + word;
	}
	if (*itr == '@')
	{
		word = _extract_word(++itr, end);
		if (word.empty() || itr == end)
			throw std::runtime_error("Invalid host");
		prefix += '@' + word;
	}
	_skip_spaces(itr, end);
	return (prefix);
}

static
std::string	_extract_command(
				std::vector<char>::const_iterator& itr,
				std::vector<char>::const_iterator const& end
				)
{
	std::string	command;

	if (!std::isdigit(*itr))
	{
		while (itr != end && std::isalpha(*itr))
			command += *itr++;
		return (command);
	}
	for (size_t i = 0; i < 3; i++)
	{
		if (itr == end || !std::isdigit(*itr))
			throw std::runtime_error("Invalid command (ID)");
		command += *itr++;
	}
	return (command);
}

static
void	_extract_params(
			std::vector<char>::const_iterator& itr,
			std::vector<char>::const_iterator const& end,
			std::vector<std::string>& params
			)
{
	std::string	param;

	if (itr == end || *itr != ' ')
		return ;
	_skip_spaces(itr, end);
	if (itr == end)
		throw std::runtime_error("Params: missing data");
	if (*itr == ':')
	{
		++itr;
		while (itr != end && *itr != '\0' && *itr != '\r' && *itr != '\n')
			param += *itr++;
		params.push_back(param);
		return ;
	}
	while (itr != end && *itr != ' ' && *itr != '\0' && *itr != '\r' && *itr != '\n')
		param += *itr++;
	params.push_back(param);
	_extract_params(itr, end, params);
}
