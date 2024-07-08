/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ale-boud <ale-boud@student.42lehavre.fr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:11:01 by ale-boud          #+#    #+#             */
/*   Updated: 2024/06/06 15:42:05 by ale-boud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef  COMMAND_HPP
# define COMMAND_HPP

# include <string>
# include <vector>

# include "utils.hpp"

# define CREATE_COMMAND(_c, _prefix, _command, ...)	\
{													\
	std::string t[] = {__VA_ARGS__};				\
	_c = Command(_prefix, _command,					\
			std::vector<std::string>(t,				\
				t + sizeof(t) / sizeof(*t)));		\
}

class Command
{
	public:
		Command(Command const& o);
		Command(std::vector<char> const& raw_command);
		Command(std::string const& prefix,
				std::string const& command,
				std::vector<std::string> const& parameters);
		Command(void);
		~Command();

		Command&	operator=(Command const& o);

		std::string const&	getPrefix() const;
		std::string const&	getCommand() const;
		std::vector<std::string> const&	getParameters() const;

		std::vector<char>	encode() const;
	
	private:

	private:
		std::string					m_prefix;
		std::string					m_command;
		std::vector<std::string>	m_parameters;

	public:
		CREATE_EXCEPTION_MESSAGE(InvalidCommand);

};

#endif
