/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ale-boud <ale-boud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 14:16:08 by ale-boud          #+#    #+#             */
/*   Updated: 2024/10/09 05:31:17 by ale-boud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include <cerrno>
#include <cstring>

#include "Log.hpp"
#include "Client.hpp"
#include "Server.hpp"

const std::string	Client::oper_username("john");
const std::string	Client::oper_password("cena") ;

const std::pair<std::string, void (Client::*)(Command const&)>
Client::_command_function_map[] = {
	std::make_pair("PASS", &Client::execPASS),
	std::make_pair("NICK", &Client::execNICK),
	std::make_pair("USER", &Client::execUSER),
	std::make_pair("PING", &Client::execPING),
};

const std::map<std::string, void (Client::*)(Command const&)>
Client::command_function_map(_command_function_map,
		_command_function_map + sizeof(_command_function_map) / sizeof(*_command_function_map));

const std::pair<std::string, void (Client::*)(Command const&)>
Client::_logged_command_function_map[] = {
	std::make_pair("PRIVMSG", &Client::execPRIVMSG),
	std::make_pair("NOTICE", &Client::execNOTICE),
	std::make_pair("OPER", &Client::execOPER),
	std::make_pair("JOIN", &Client::execJOIN),
};

const std::map<std::string, void (Client::*)(Command const&)>
Client::logged_command_function_map(_logged_command_function_map,
		_logged_command_function_map + sizeof(_logged_command_function_map) / sizeof(*_logged_command_function_map));

Client::Client(int fd, struct sockaddr const& addr) :
	m_nickname("*"),
	m_username(),
	m_realname(),
	m_userpwd(),
	m_state(LOGIN),
	m_fd(fd),
	m_isoperator(false),
	m_addr(addr)
{
	Log::Info << "New connection : "
		<< ipv4FromSockaddr(m_addr) << ":" << ntohs(((struct sockaddr_in *)&m_addr)->sin_port) << std::endl;
}

Client::~Client(void)
{
	Server::getClientManager().removeClient(this);
}

std::string const&	Client::getNickname(void) const
{
	return (m_nickname);
}

std::string const&	Client::getUsername(void) const
{
	return (m_username);
}

std::string const&	Client::getRealname(void) const
{
	return (m_realname);
}

struct sockaddr const&	Client::getSockaddr(void) const
{
	return (m_addr);
}

std::string	Client::getPrefix() const
{
	return (m_nickname + "!" + m_username + "@" + ipv4FromSockaddr(m_addr));
}

void	Client::setNickname(std::string const& s)
{
	m_nickname = s;
}

void	Client::setUsername(std::string const& s)
{
	m_username = s;
}

void	Client::setRealname(std::string const& s)
{
	m_realname = s;
}

bool	Client::isRegistered()
{
	return (m_state == REGISTERED);
}

bool	Client::isOperator()
{
	return (m_isoperator);
}

void	Client::receive(int fd)
{
	char	buf[default_read_size];

	while (true)
	{
		ssize_t ret = recv(fd, buf, sizeof(buf), 0);
		if (ret == 0 || (ret <= 0 && errno == ECONNRESET))
			throw ConnectionLostException();
		else if (ret <= 0 && errno == EWOULDBLOCK)
			break ;
		else if (ret <= 0)
			throw ReadErrorException(std::strerror(errno));
		m_buffer.pushBack(buf, ret);
	}
}

void	Client::sendCommand(Command const& command)
{
	std::vector<char>	data = command.encode();

	std::ostream &os = Log::Info << "sending command : ";
	ITERATE_CONST(std::vector<char>, data, itr)
		os << *itr;
	os.flush();
	// TODO unblocked send
	send(m_fd, data.data(), data.size(), 0);
}

void	Client::execPendingCommands(void)
{
	Command	c;

	while (true)
	{
		try
		{
			c = m_buffer.popFront();
			std::string	uc = c.getCommand();
			std::transform(uc.begin(), uc.end(), uc.begin(), ::toupper);
			Log::Debug << "Exec command :\"" << uc << "\" (" << ipv4FromSockaddr(m_addr) << ")" << std::endl;
			ITERATE_CONST(std::vector<std::string>, c.getParameters(), itr)
				Log::Info << "ARGS: " << *itr << std::endl;
			if (command_function_map.count(uc))
				(this->*command_function_map.at(uc))(c);
			else if (logged_command_function_map.count(uc) && m_state == REGISTERED)
				(this->*logged_command_function_map.at(uc))(c);
			else if (logged_command_function_map.count(uc) && m_state != REGISTERED)
				sendCommand(CREATE_ERR_NOTREGISTERED(*this));
			else
				(this->*command_function_map.at(uc))(c); // THROW out_of_range tricks
		}
		catch (CommandBuffer::NoPendingCommandException const& e)
		{
			break ;
		}
		catch (Command::InvalidCommandException const& e)
		{
			Log::Warn << "Invalid command received: " << e.what() << std::endl;
		}
		catch (std::out_of_range const& e)
		{
			Log::Warn << "Unknown command : " << c.getCommand() << std::endl;
			sendCommand(CREATE_ERR_UNKNOWNCOMMAND(*this, c.getCommand()));
		}
	}
}

void	Client::welcome()
{
	Command	c;
	if (!Server::getInstance().checkPwd(m_userpwd))
	{
		sendCommand(CREATE_ERR_PASSWDMISMATCH(*this));
		sendCommand(CREATE_COMMAND("", "ERROR", "Closing link: " + ipv4FromSockaddr(m_addr) + " (Bad password)"));
		throw BadPasswordException();
	}
	if (Server::getClientManager().inUse(m_nickname))
	{
		sendCommand(CREATE_ERR_NICKNAMEINUSE(*this, m_nickname));
		m_state = RETRY;
		return ;
	}
	if (!usernickValidator(m_nickname))
	{
		sendCommand(CREATE_ERR_ERRONEUSNICKNAME(*this, m_nickname));
		m_state = RETRY;
		return ;
	}
	Server::getClientManager().addClient(this);
	sendCommand(CREATE_RPL_WELCOME(*this));
	m_state = REGISTERED;
}

void	Client::execPRIVMSG(Command const& command)
{
	if (command.getParameters().size() < 2)
		sendCommand(CREATE_ERR_NEEDMOREPARAMS(*this, command.getCommand()));
	else if (command.getParameters()[0].find(',') != std::string::npos)
		sendCommand(CREATE_ERR_TOOMANYTARGETS(*this));
	else if (command.getParameters()[0][0] != '#')
	{
		Client	*client = Server::getClientManager().getClient(command.getParameters()[0]);
		if (client == NULL)
			sendCommand(CREATE_ERR_NOSUCHNICK(*this, command.getParameters()[0]));
		else
			client->sendCommand(CREATE_COMMAND(getPrefix(), "PRIVMSG", command.getParameters()[0], command.getParameters()[1]));
	}
	else
	{
		// TODO: channel
	}
}

// same as privmsg without sending any reply back
void	Client::execNOTICE(Command const& command)
{
	if (command.getParameters().size() < 2)
		return ;
	else if (command.getParameters()[0].find(',') != std::string::npos)
		return ;
	else if (command.getParameters()[0][0] != '#')
	{
		Client	*client = Server::getClientManager().getClient(command.getParameters()[0]);
		if (client == NULL)
			return ;
		else
			client->sendCommand(CREATE_COMMAND(getPrefix(), "NOTICE", command.getParameters()[0], command.getParameters()[1]));
	}
	else
	{
		// TODO: channel ???
	}
}

void	Client::execPASS(Command const& command)
{
	if (m_state != LOGIN)
		sendCommand(CREATE_ERR_ALREADYREGISTERED(*this));
	else if (command.getParameters().size() < 1)
		sendCommand(CREATE_ERR_NEEDMOREPARAMS(*this, command.getCommand()));
	else
		m_userpwd = command.getParameters()[0];
}

void	Client::execNICK(Command const& command)
{
	if (command.getParameters().size() < 1)
		sendCommand(CREATE_ERR_NEEDMOREPARAMS(*this, command.getCommand()));
	else if (m_state == REGISTERED && !usernickValidator(command.getParameters()[0]))
		sendCommand(CREATE_ERR_ERRONEUSNICKNAME(*this, command.getParameters()[0]));
	else if (m_state == REGISTERED && Server::getClientManager().inUse(command.getParameters()[0]))
		sendCommand(CREATE_ERR_NICKNAMEINUSE(*this, command.getParameters()[0]));
	else
	{
		m_nickname = command.getParameters()[0];
		if (m_state == RETRY)
			welcome();
	}
}

void	Client::execUSER(Command const& command)
{
	if (m_state != LOGIN)
		sendCommand(CREATE_ERR_ALREADYREGISTERED(*this));
	else if (command.getParameters().size() < 4)
		sendCommand(CREATE_ERR_NEEDMOREPARAMS(*this, command.getCommand()));
	else
	{
		m_username = command.getParameters()[0];
		m_realname = command.getParameters()[3];
		welcome();
	}
}

void	Client::execPING(Command const& command)
{
	if (command.getParameters().size() < 1)
		sendCommand(CREATE_ERR_NOORIGIN(*this));
	else
		sendCommand(CREATE_COMMAND("", "PONG", "localhost", command.getParameters()[0]));
}

void	Client::execOPER(Command const& command)
{
	if (command.getParameters().size() < 2)
		sendCommand(CREATE_ERR_NEEDMOREPARAMS(*this, command.getCommand()));
	else if (command.getParameters()[0] != oper_username || command.getParameters()[1] != oper_password)
		sendCommand(CREATE_ERR_PASSWDMISMATCH(*this));
	else
	{
		sendCommand(CREATE_RPL_YOUREOPER(*this));
		if (!m_isoperator)
			Server::getClientManager().sendAll(CREATE_COMMAND(getPrefix(), "MODE", "+o", m_nickname));
		m_isoperator = true;
	}
}

void	Client::execJOIN(Command const& command)
{
	
}
