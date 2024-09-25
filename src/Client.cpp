/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ale-boud <ale-boud@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 14:16:08 by ale-boud          #+#    #+#             */
/*   Updated: 2024/09/25 17:34:54 by ale-boud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include <cerrno>
#include <cstring>

#include "Log.hpp"
#include "Client.hpp"
#include "Server.hpp"

const std::pair<std::string, void (Client::*)(Command const&)>
Client::_command_function_map[] = {
	std::make_pair("PRIVMSG", &Client::execPRIVMSG),
	std::make_pair("PASS", &Client::execPASS),
	std::make_pair("NICK", &Client::execNICK),
	std::make_pair("USER", &Client::execUSER),
};

const std::map<std::string, void (Client::*)(Command const&)>
Client::command_function_map(_command_function_map,
		_command_function_map + sizeof(_command_function_map) / sizeof(*_command_function_map));

Client::Client(int fd, struct sockaddr const& addr) :
	m_nickname(),
	m_username(),
	m_realname(),
	m_userpwd(),
	m_state(HANDSHAKE),
	m_fd(fd),
	m_addr(addr)
{
	Log::Debug << "New connection : "
		<< ipv4FromSockaddr(m_addr) << ":" << ntohs(((struct sockaddr_in *)&m_addr)->sin_port) << std::endl;
}

Client::Client(void) :
	m_state(HANDSHAKE)
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
	return m_state == REGISTERED;
}

void	Client::receive(int fd)
{
	char	buf[default_read_size];

	while (true)
	{
		ssize_t ret = recv(fd, buf, sizeof(buf), 0);
		Log::Info << "recv() == " << ret << std::endl;
		if (ret == 0 || (ret <= 0 && errno == ECONNRESET))
			throw ConnectionLostException();
		else if (ret <= 0 && errno == EWOULDBLOCK)
			break ;
		else if (ret <= 0)
			throw ReadErrorException(std::strerror(errno));
#ifdef DEBUG
		buf[ret] = '\0';
		Log::Info << "buf == \n" << buf << std::endl;
#endif
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
			std::string	uc = c.getCommand();
			std::transform(uc.begin(), uc.end(), uc.begin(), ::toupper);
			Log::Info << uc;
			ITERATE_CONST(std::vector<std::string>, c.getParameters(), it)
				*Log::Info.m_out << " " << *it;
			*Log::Info.m_out << " executed (" << ipv4FromSockaddr(m_addr) << ")" << std::endl;
			(this->*command_function_map.at(uc))(c);
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
	ITERATE_CONST(std::vector<std::string>, command.getParameters(), it)
		Log::Debug << "param : " << *it << std::endl;
}

void	Client::execPASS(Command const& command)
{
	Command	c;

	ITERATE_CONST(std::vector<std::string>, command.getParameters(), it)
		Log::Debug << "param : " << *it << std::endl;
	if (m_state != HANDSHAKE)
	{
		CREATE_COMMAND(c, "", ERR_ALREADYREGISTERED, "You may not reregister");
		sendCommand(c);
	}
	else if (command.getParameters().size() < 1)
	{
		CREATE_COMMAND(c, "", ERR_NEEDMOREPARAMS, "PASS", "Not enough parameters");
		sendCommand(c);
	}
	else if (!Server::getInstance()->checkPwd(command.getParameters()[0]))
	{
		CREATE_COMMAND(c, "", ERR_PASSWDMISMATCH, "Password incorrect");
		sendCommand(c);
	}
	m_state = LOGIN;
}

void	Client::execNICK(Command const& command)
{
	Command	c;

	if (command.getParameters().size() < 1)
	{
		CREATE_COMMAND(c, "", ERR_NEEDMOREPARAMS, "NICK", "Not enough parameters");
		sendCommand(c);
		return ;
	}
	if (!m_state)
	{
		m_nickname = command.getParameters()[0];
		return ;
	}
}

void	Client::execUSER(Command const& command)
{
	Command	c;

	if (m_state)
	{
		CREATE_COMMAND(c, "", ERR_ALREADYREGISTERED, "You may not reregister");
		sendCommand(c);
		return ;
	}
	if (command.getParameters().size() < 4)
	{
		CREATE_COMMAND(c, "", ERR_NEEDMOREPARAMS, "USER", "Not enough parameters");
		sendCommand(c);
		return ;
	}
	m_username = command.getParameters()[0];
	m_realname = command.getParameters()[3];
}
