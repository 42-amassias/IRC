/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ale-boud <ale-boud@student.42lehavre.fr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 14:16:08 by ale-boud          #+#    #+#             */
/*   Updated: 2024/06/06 15:42:05 by ale-boud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <cstring>

#include "Log.hpp"
#include "Client.hpp"
#include "Server.hpp"

const std::pair<std::string, void (Client::*)(Command const&)>
Client::_command_function_map[] = {
	std::make_pair("PRIVMSG", &Client::execPRIVMSG),
	std::make_pair("PASS", &Client::execPASS),
};

const std::map<std::string, void (Client::*)(Command const&)>
Client::command_function_map(_command_function_map,
		_command_function_map + sizeof(_command_function_map) / sizeof(*_command_function_map));

Client::Client(int fd, struct sockaddr const& addr) :
	m_nickname(),
	m_username(),
	m_realname(),
	m_registered(false),
	m_fd(fd),
	m_addr(addr)
{
	Log::Info << "New connection : "
		<< ipv4FromSockaddr(m_addr) << ":" << ntohs(((struct sockaddr_in *)&m_addr)->sin_port) << std::endl;
}

Client::Client(void) :
	m_registered(false)
{
}

Client::~Client(void)
{
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
	return m_registered;
}

void	Client::receive(int fd)
{
	char	buf[default_read_size];

	while (true)
	{
		ssize_t ret = recv(fd, buf, sizeof(buf), 0);
		Log::Debug << "recv() == " << ret << std::endl;
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

	Log::Info << "sending command : " << data.data() << std::endl;
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
			(this->*command_function_map.at(c.getCommand()))(c);
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
		}
	}
}

void	Client::execPRIVMSG(Command const& command)
{
	Log::Debug << "PRIVMSG executed (" << ipv4FromSockaddr(m_addr) << ")" << std::endl;
	ITERATE_CONST(std::vector<std::string>, command.getParameters(), it)
		Log::Debug << "param : " << *it << std::endl;
}

void	Client::execPASS(Command const& command)
{
	Command	c;
	Log::Debug << "PASS executed (" << ipv4FromSockaddr(m_addr) << ")" << std::endl;
	ITERATE_CONST(std::vector<std::string>, command.getParameters(), it)
		Log::Debug << "param : " << *it << std::endl;
	if (m_registered)
	{
		CREATE_COMMAND(c, "", "462", "You may not reregister");
		sendCommand(c);
	}
	else if (command.getParameters().size() < 1)
	{
		CREATE_COMMAND(c, "", "461", "PASS :Not enough parameters");
		sendCommand(c);
	}
	else if (Server::getInstance()->checkPwd(command.getParameters()[0]))
	{

	}
	else
	{
	
	}
}

