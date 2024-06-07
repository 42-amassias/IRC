/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ale-boud <ale-boud@student.42lehavre.fr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:10:43 by ale-boud          #+#    #+#             */
/*   Updated: 2024/06/06 15:42:05 by ale-boud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

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
	try
	{
		std::vector<char>::const_iterator	itr(raw_command.begin());
		std::vector<char>::const_iterator	end(raw_command.end());

		m_prefix = _extract_prefix(itr, end);
		if (itr == end)
			throw Command::InvalidCommandException("Missing command");
		m_command = _extract_command(itr, end);
		if (itr == end || *itr != ' ')
			throw Command::InvalidCommandException("Missing space before params");
		_extract_params(itr, end, m_parameters);
		if (itr != end)
			throw Command::InvalidCommandException("Too much dada");
	}
	catch (Command::InvalidCommandException const& e)
	{
		throw Command::InvalidCommandException(std::string(e.what())
				+ " (\"" + std::string(raw_command.begin(), raw_command.end()) + "\")");
	}
}

Command::Command(
		std::string const& prefix,
		std::string const& command,
		std::vector<std::string> const& parameters) :
	m_prefix(prefix),
	m_command(command),
	m_parameters(parameters)
{
}

Command::~Command(void)
{
}

std::vector<char>	Command::encode() const
{
	std::vector<char>	encoded_cmd;

	if (!m_prefix.empty())
	{
		encoded_cmd.push_back(':');
		encoded_cmd.insert(encoded_cmd.end(), m_prefix.begin(), m_prefix.end());
		encoded_cmd.push_back(' ');
	}
	encoded_cmd.insert(encoded_cmd.end(), m_command.begin(), m_command.end());
	ITERATE_CONST(std::vector<std::string>, m_parameters, pit)
	{
		encoded_cmd.push_back(' ');
		if (pit->find(' ') != std::string::npos)
			encoded_cmd.push_back(':');
		encoded_cmd.insert(encoded_cmd.end(), pit->begin(), pit->end());
	}
	encoded_cmd.push_back('\r');
	encoded_cmd.push_back('\n');
	return encoded_cmd;
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
		throw Command::InvalidCommandException("Missing white space");
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
		throw Command::InvalidCommandException("Invalid prefix");
	prefix += word;
	if (*itr == '!')
	{
		word = _extract_word(++itr, end);
		if (word.empty() || itr == end)
			throw Command::InvalidCommandException("Invalid username");
		prefix += '!' + word;
	}
	if (*itr == '@')
	{
		word = _extract_word(++itr, end);
		if (word.empty() || itr == end)
			throw Command::InvalidCommandException("Invalid host");
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
		if (*itr == ' ')
			throw Command::InvalidCommandException("Unexcepted spaces before command");
		while (itr != end && std::isalpha(*itr))
			command += *itr++;
		return (command);
	}
	for (size_t i = 0; i < 3; i++)
	{
		if (itr == end || !std::isdigit(*itr))
			throw Command::InvalidCommandException("Invalid command (ID)");
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
		throw Command::InvalidCommandException("Params: missing data");
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

std::string const&				Command::getPrefix() const
{
	return m_prefix;
}

std::string const&				Command::getCommand() const
{
	return m_command;
}

std::vector<std::string> const&	Command::getParameters() const
{
	return m_parameters;
}

