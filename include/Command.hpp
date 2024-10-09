/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ale-boud <ale-boud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:11:01 by ale-boud          #+#    #+#             */
/*   Updated: 2024/10/09 08:52:08 by ale-boud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef  COMMAND_HPP
# define COMMAND_HPP

# include <string>
# include <vector>
# include <iomanip>

# include "utils.hpp"

# define CREATE_COMMAND(_prefix, _command, ...)	(Command(_prefix, _command, (const std::string[]){__VA_ARGS__}))

# define CREATE_COMMAND_CLIENT(_c, _prefix, _command, ...)	\
{															\
	std::string t[] = {m_nickname, __VA_ARGS__};			\
	_c = Command(_prefix, _command,							\
			std::vector<std::string>(t,						\
				t + sizeof(t) / sizeof(*t)));				\
}

# define SEND_COMMAND_CLIENT(_prefix, _command, ...)		\
{															\
	Command	__c;											\
	std::string t[] = {m_nickname, __VA_ARGS__};			\
	__c = Command(_prefix, _command,						\
			std::vector<std::string>(t,						\
				t + sizeof(t) / sizeof(*t)));				\
	sendCommand(__c);										\
}

# define WELCOME_MESSAGE (								\
"__        __   _                            _        \n"\
"\\ \\      / /__| | ___ ___  _ __ ___   ___  | |_ ___  \n"\
" \\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\ | __/ _ \\ \n"\
"  \\ V  V /  __/ | (_| (_) | | | | | |  __/ | || (_) |\n"\
"   \\_/\\_/ \\___|_|\\___\\___/|_|_|_| |_|\\___|  \\__\\___/ \n"\
"           _____ _____   ___ ____   ____             \n"\
"          |  ___|_   _| |_ _|  _ \\ / ___|            \n"\
"          | |_    | |    | || |_) | |                \n"\
"          |  _|   | |    | ||  _ <| |___             \n"\
"          |_|     |_|___|___|_| \\_\\\\____|            \n"\
"                   |_____|                           \n"\
)

# define CREATE_ERR(client, errname, ...) (CREATE_COMMAND("", errname, (client).getNickname(), __VA_ARGS__))
# define CREATE_ERR_NEEDMOREPARAMS(client, command) (CREATE_ERR(client, ERR_NEEDMOREPARAMS, (command), "Not enough parameters"))
# define CREATE_ERR_ALREADYREGISTERED(client) (CREATE_ERR(client, ERR_ALREADYREGISTERED, "You may not reregister"))
# define CREATE_ERR_NICKNAMEINUSE(client, nick) (CREATE_ERR(client, ERR_NICKNAMEINUSE, (nick), "Nickname is already in use"))
# define CREATE_ERR_ERRONEUSNICKNAME(client, nick) (CREATE_ERR(client, ERR_ERRONEUSNICKNAME, (nick), "Erroneus nickname"))
# define CREATE_ERR_NOSUCHNICK(client, nickorchan) (CREATE_ERR(client, ERR_NOSUCHNICK, (nickorchan), "No such nick/channel"))
# define CREATE_ERR_TOOMANYTARGETS(client) (CREATE_ERR(client, ERR_TOOMANYTARGETS, "Too many targets"))
# define CREATE_ERR_PASSWDMISMATCH(client) (CREATE_ERR(client, ERR_PASSWDMISMATCH, "User or password incorrect"))
# define CREATE_ERR_UNKNOWNCOMMAND(client, command) (CREATE_ERR(client, ERR_UNKNOWNCOMMAND, (command), "Unknown command"))
# define CREATE_ERR_NOTREGISTERED(client) (CREATE_ERR(client, ERR_NOTREGISTERED, "You have not registered"))
# define CREATE_ERR_NOORIGIN(client) (CREATE_ERR(client, ERR_NOORIGIN, "No origin specified"))
# define CREATE_ERR_NOSUCHCHANNEL(client, chan_name) (CREATE_ERR(client, ERR_NOSUCHCHANNEL, (chan_name), "No such channel"))
# define CREATE_ERR_INVITEONLYCHAN(client, chan_name) (CREATE_ERR(client, ERR_INVITEONLYCHAN, (chan_name), "Cannot join channel (+i)"))
# define CREATE_ERR_CHANNELISFULL(client, chan_name) (CREATE_ERR(client, ERR_CHANNELISFULL, (chan_name), "Cannot join channel (+l)"))
# define CREATE_ERR_CANNOTSENDTOCHAN(client, chan_name) (CREATE_ERR(client, ERR_CANNOTSENDTOCHAN, (chan_name), "Cannot sent to channel"))

# define CREATE_RPL_WELCOME(client) (CREATE_COMMAND("", RPL_WELCOME, (client).getNickname(), WELCOME_MESSAGE))
# define CREATE_RPL_YOUREOPER(client) (CREATE_COMMAND("", RPL_YOUREOPER, (client).getNickname(), "You are now an IRC operator"))
# define CREATE_RPL_TOPIC(client, chan_name, topic) (CREATE_COMMAND("", RPL_TOPIC, (client).getNickname(), (chan_name), (topic)))
# define CREATE_RPL_ENDOFNAMES(client, chan_name) (CREATE_COMMAND("", RPL_ENDOFNAMES, (client).getNickname(), (chan_name), "End of NAMES list"))

enum	irc_errcode {
	ERR_NEEDMOREPARAMS=461,
	ERR_ALREADYREGISTERED=462,
	ERR_PASSWDMISMATCH=464,
	ERR_UNKNOWNCOMMAND=421,
	ERR_NONICKNAMEGIVEN=431,
	ERR_ERRONEUSNICKNAME=432,
	ERR_NOTREGISTERED=451,
	ERR_NICKNAMEINUSE=433,
	ERR_NICKCOLLISION=436,
	ERR_NOORIGIN=409,
	ERR_TOOMANYTARGETS=407,
	ERR_NOSUCHCHANNEL=403,
	ERR_CHANNELISFULL=471,
	ERR_CANNOTSENDTOCHAN=404,
	ERR_INVITEONLYCHAN=473,
	ERR_NOSUCHNICK=401,
};

enum	irc_rplcode {
	RPL_WELCOME=001,
	RPL_YOUREOPER=381,
	RPL_TOPIC=332,
	RPL_NAMREPLY=353,
	RPL_ENDOFNAMES=366,
};

class Command
{
	public:
		Command(Command const& o);
		Command(std::vector<char> const& raw_command);
		Command(std::string const& prefix,
				int command,
				std::vector<std::string> const& parameters);
		template<std::size_t N>
		Command(std::string const& prefix,
				int command,
				std::string const (&parameters)[N]) :
			m_prefix(prefix),
			m_command(static_cast< std::ostringstream & >( (std::ostringstream() << std::dec << std::setw(3) << std::setfill('0') << command)).str()),
			m_parameters(parameters, parameters + sizeof(parameters) / sizeof(*parameters))
		{
		}
		Command(std::string const& prefix,
				std::string const& command,
				std::vector<std::string> const& parameters);
		template<std::size_t N>
		Command(std::string const& prefix,
				std::string const& command,
				std::string const (&parameters)[N]) :
			m_prefix(prefix),
			m_command(command),
			m_parameters(parameters, parameters + sizeof(parameters) / sizeof(*parameters))
		{
		}
		Command(void);
		~Command();

		Command&	operator=(Command const& o);

		std::string const&	getPrefix() const;
		std::string const&	getCommand() const;
		std::vector<std::string> const&	getParameters() const;

		void	setPrefix(std::string const& _prefix);

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
