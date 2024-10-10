/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ale-boud <ale-boud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 14:16:08 by ale-boud          #+#    #+#             */
/*   Updated: 2024/10/10 14:43:22 by ale-boud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <sstream>

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
	std::make_pair("QUIT", &Client::execQUIT),
	std::make_pair("TOPIC", &Client::execTOPIC),
	std::make_pair("MODE", &Client::execMODE),
	std::make_pair("INVITE", &Client::execINVITE),
	std::make_pair("PART", &Client::execPART),
	std::make_pair("KICK", &Client::execKICK),
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
	if (m_state == REGISTERED)
		Server::getChannelManager().sendToAll(CREATE_COMMAND(getPrefix(), "QUIT", "Quit: Leaving like a rockstar"), this);
	Server::getClientManager().removeClient(this);
	Server::getChannelManager().removeClient(this);
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
		// Log::Info << "recv() == " << ret << std::endl;
		if (ret == 0 || (ret <= 0 && errno == ECONNRESET))
			throw ConnectionLostException();
		else if (ret <= 0 && errno == EAGAIN)
			break ;
		else if (ret <= 0)
			throw ReadErrorException(std::strerror(errno));
		m_read_buffer.pushBack(buf, ret);
	}
}

void	Client::sendCommand(Command const& command)
{
	std::vector<char>	data = command.encode();

	if (m_write_buffer.empty())
		Server::getInstance().hasPendingSend(m_fd);
	std::ostream &os = Log::Info << "sending command : ";
	ITERATE_CONST(std::vector<char>, data, itr)
		os << *itr;
	os.flush();
	m_write_buffer.insert(m_write_buffer.end(), data.begin(), data.end());
}

void	Client::sendPendingCommand()
{
	while (!m_write_buffer.empty())
	{
		ssize_t ret = send(m_fd, m_write_buffer.data(), m_write_buffer.size(), 0);
		if (ret == 0 || (ret <= 0 && errno == ECONNRESET))
			throw ConnectionLostException();
		else if (ret <= 0 && errno == EAGAIN)
			break ;
		else if (ret <= 0)
			throw WriteErrorException(std::strerror(errno));
		m_write_buffer.erase(m_write_buffer.begin(), m_write_buffer.begin() + ret);
	}
	if (m_write_buffer.empty())
		Server::getInstance().noPendingSend(m_fd);
}

void	Client::execPendingCommands(void)
{
	Command	c;

	while (true)
	{
		try
		{
			c = m_read_buffer.popFront();
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
	if (!nickValidator(m_nickname))
	{
		sendCommand(CREATE_ERR_ERRONEUSNICKNAME(*this, m_nickname));
		m_state = RETRY;
		return ;
	}
	Server::getClientManager().addClient(this);
	sendCommand(CREATE_RPL_WELCOME(*this));
	m_state = REGISTERED;
}

void	Client::execNICK(Command const& command)
{
	if (command.getParameters().size() < 1)
		sendCommand(CREATE_ERR_NONICKNAMEGIVEN(*this));
	else if (m_state == REGISTERED && !nickValidator(command.getParameters()[0]))
		sendCommand(CREATE_ERR_ERRONEUSNICKNAME(*this, command.getParameters()[0]));
	else if (m_state == REGISTERED && Server::getClientManager().inUse(command.getParameters()[0]))
		sendCommand(CREATE_ERR_NICKNAMEINUSE(*this, command.getParameters()[0]));
	else if (m_state != REGISTERED)
	{
		m_nickname = command.getParameters()[0];
		if (m_state == RETRY)
			welcome();
	}
	else
	{
		Command	c(CREATE_COMMAND(getPrefix(), "NICK", command.getParameters()[0]));
		sendCommand(c);
		Server::getChannelManager().sendToAll(c, this);
		m_nickname = command.getParameters()[0];
	}
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
		std::string	chan_name(command.getParameters()[0]);
		chan_name.erase(chan_name.begin());
		try
		{
			Server::getChannelManager().
					getChannel(chan_name).
					sendToAll(CREATE_COMMAND(getPrefix(), "PRIVMSG", command.getParameters()[0], command.getParameters()[1]), this);
		}
		catch (ChannelManager::DoesNotExistException const& e)
		{
			sendCommand(CREATE_ERR_NOSUCHCHANNEL(*this, "#" + chan_name));
		}
		catch (Channel::NotRegisteredException const& e)
		{
			sendCommand(CREATE_ERR_NOTONCHANNEL(*this, "#" + chan_name));
		}
		catch (std::exception const& e)
		{
			Log::Error << "Error encountered when broadcasting message to channel : " << chan_name << std::endl;
		}
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

void	Client::execPART(Command const& command)
{
	std::vector<std::string> const	&params = command.getParameters();
	if (params.size() < 1)
		sendCommand(CREATE_ERR_NEEDMOREPARAMS(*this, command.getCommand()));
	else
	{
		std::string	reason;
		if (params.size() >= 2)
			reason = params[1];
		std::stringstream	chan_name_ss(params[0]); // Not the 1939/45 thing
		std::string			chan_name;
		while (std::getline(chan_name_ss, chan_name, ','))
		{
			if (chan_name.empty())
				continue ;
			if (chan_name[0] != '#')
			{
				sendCommand(CREATE_ERR_BADCHANMASK(*this, chan_name));
				continue ;
			}
			chan_name.erase(chan_name.begin());
			try
			{
				Server::getChannelManager().getChannel(chan_name).part(this, reason);
			}
			catch(ChannelManager::DoesNotExistException const& e)
			{
				sendCommand(CREATE_ERR_NOSUCHCHANNEL(*this, "#" + chan_name));
			}
			catch(Channel::NotRegisteredException const& e)
			{
				sendCommand(CREATE_ERR_NOTONCHANNEL(*this, "#" + chan_name));
			}
		}
	}
}

void	Client::execJOIN(Command const& command)
{
	std::vector<std::string> const	&params = command.getParameters();
	if (params.size() < 1)
		sendCommand(CREATE_ERR_NEEDMOREPARAMS(*this, command.getCommand()));
	else
	{
		std::stringstream	chan_name_ss(params[0]); // Not the 1939/45 thing
		std::stringstream	chan_key_ss(params.size() >= 2 ? params[1] : "");
		std::string			chan_name;
		std::string			chan_key;
		while (std::getline(chan_name_ss, chan_name, ','))
		{
			if (!std::getline(chan_key_ss, chan_key, ',')) // ignore if we no longer have key
				chan_key = "";
			if (chan_name.empty())
				continue ;
			if (chan_name[0] != '#')
			{
				sendCommand(CREATE_ERR_BADCHANMASK(*this, chan_name));
				continue ;
			}
			chan_name.erase(chan_name.begin());
			try
			{
				Server::getChannelManager().createOrJoin(chan_name, chan_key, this);
			}
			catch(Channel::InvalidChannelNameException const& e)
			{
				sendCommand(CREATE_ERR_NOSUCHCHANNEL(*this, chan_name));
			}
			catch(Channel::NotInvitedException const& e)
			{
				sendCommand(CREATE_ERR_INVITEONLYCHAN(*this, "#" + chan_name));
			}
			catch(Channel::ChannelFullException const& e)
			{
				sendCommand(CREATE_ERR_CHANNELISFULL(*this, "#" + chan_name));
			}
			catch (Channel::PasswordMismatchException const& e)
			{
				sendCommand(CREATE_ERR_BADCHANNELKEY(*this, "#" + chan_name));
			}
		}
	}
}

void	Client::execQUIT(Command const& command)
{
	std::vector<std::string>	params(command.getParameters());

	if (params.size() > 1)
		params.erase(params.begin()+1, params.end());
	if (params.size() >= 1)
		params[0] = "Quit: " + params[0];
	else
		params.push_back("Quit: leaving");
	Server::getChannelManager().sendToAll(Command(getPrefix(), "QUIT", params), this); // todo trim args
	m_state = QUIT;
	throw QuitMessageException();
}

void	Client::execKICK(Command const& command)
{
	std::vector<std::string> const	&params = command.getParameters();
	if (params.size() < 2)
	{
		sendCommand(CREATE_ERR_NEEDMOREPARAMS(*this, command.getCommand()));
		return ;
	}
	std::string	chan_name(params[0]);
	if (chan_name.empty())
		return ;
	if (chan_name[0] != '#')
	{
		sendCommand(CREATE_ERR_NOSUCHCHANNEL(*this, chan_name));
		return ;
	}
	chan_name.erase(chan_name.begin());
	try
	{
		Channel	&chan = Server::getChannelManager().getChannel(chan_name);
		std::string	comment = "get out";
		if (params.size() > 3)
			comment = params[2];
		std::stringstream	nickname_ss(params[1]); // Not the 1939/45 thing
		std::string			nickname;
		while (std::getline(nickname_ss, nickname, ','))
		{
			if (nickname.empty())
				continue ;
			try
			{
				Client	*to_kick = Server::getClientManager().getClient(nickname);
				if (to_kick == NULL)
				{
					// TODO check if its ignored or not
					continue ;
				}
				chan.kick(this, to_kick, comment);
			}
			catch(ChannelManager::DoesNotExistException const& e)
			{
				sendCommand(CREATE_ERR_NOSUCHCHANNEL(*this, "#" + chan_name));
			}
			catch(Channel::NotRegisteredException const& e)
			{
				sendCommand(CREATE_ERR_NOTONCHANNEL(*this, "#" + chan_name));
			}
			catch(Channel::RequireOperException const& e)
			{
				sendCommand(CREATE_ERR_CHANOPRIVSNEEDED(*this, "#" + chan_name));
			}
			catch (Channel::NotInChannelException const& e)
			{
				sendCommand(CREATE_ERR_USERNOTINCHANNEL(*this, nickname, "#" + chan_name));
			}
		}
	}
	catch (ChannelManager::DoesNotExistException const& e)
	{
		sendCommand(CREATE_ERR_NOSUCHCHANNEL(*this, "#" + chan_name));
	}
}

void	Client::execTOPIC(Command const& command)
{
	std::vector<std::string> const&	params = command.getParameters();
	if (params.size() < 1)
		sendCommand(CREATE_ERR_NEEDMOREPARAMS(*this, command.getCommand()));
	std::string	chan_name(params[0]);
	if (chan_name.empty())
		return ;
	if (chan_name[0] != '#')
	{
		sendCommand(CREATE_ERR_BADCHANMASK(*this, chan_name));
		return ;
	}
	chan_name.erase(chan_name.begin());
	try
	{
		Channel	&chan = Server::getChannelManager().getChannel(chan_name);
		if (params.size() == 1)
		{
			chan.sendTopic(this);
			return ;
		}
		chan.setTopic(params[1], this);
	}
	catch (ChannelManager::DoesNotExistException const& e)
	{
		sendCommand(CREATE_ERR_NOSUCHCHANNEL(*this, "#" + chan_name));
	}
	catch (Channel::RequireOperException const& e)
	{
		sendCommand(CREATE_ERR_CHANOPRIVSNEEDED(*this, "#" + chan_name));
	}
}

void	Client::execMODE(Command const& command)
{
	std::vector<std::string> const	&params = command.getParameters();
	if (params.size() < 1)
		sendCommand(CREATE_ERR_NEEDMOREPARAMS(*this, command.getCommand()));
	else if (params.size() < 2 || params[0].empty()) // TODO : size < 1 (MODE get info)
	{
		std::string	chan_name(params[0]);
		if (chan_name[0] != '#')
			return ;
		chan_name.erase(chan_name.begin());
		try
		{
			Channel	&chan = Server::getChannelManager().getChannel(chan_name);
			sendCommand(chan.getModeCommand());
		}
		catch (ChannelManager::DoesNotExistException const& e)
		{
			sendCommand(CREATE_ERR_NOSUCHCHANNEL(*this, "#" + chan_name));
		}
		catch (Channel::NotRegisteredException const& e)
		{
			sendCommand(CREATE_ERR_NOTONCHANNEL(*this, "#" + chan_name));
		}
	}
	else
	{
		std::string	chan_name(params[0]);
		if (chan_name[0] != '#')
			return ;
		chan_name.erase(chan_name.begin());
		try
		{
			Server::getChannelManager().getChannel(chan_name).changeMode(params[1], std::vector<std::string>(params.begin() + 2, params.end()), this);
		}
		catch (ChannelManager::DoesNotExistException const& e)
		{
			sendCommand(CREATE_ERR_NOSUCHCHANNEL(*this, "#" + chan_name));
		}
		catch (Channel::InvalidModeFlagException const& e)
		{
			sendCommand(CREATE_ERR_UMODEUNKNOWNFLAG(*this));
		}
		catch (Channel::UnknownModeException const& e)
		{
			sendCommand(CREATE_ERR_UNKNOWNMODE(*this, e.what()));
		}
		catch (Channel::NotRegisteredException const& e)
		{
			sendCommand(CREATE_ERR_NOTONCHANNEL(*this, "#" + chan_name));
		}
		catch (Channel::RequireOperException const& e)
		{
			sendCommand(CREATE_ERR_CHANOPRIVSNEEDED(*this, "#" + chan_name));
		}
		catch (Channel::NeedMoreParamsException const& e)
		{
			// sendCommand(CREATE_ERR_NEEDMOREPARAMS(*this, command.getCommand())); USE LESS
		}
		catch (Channel::NotInChannelException const& e)
		{
			sendCommand(CREATE_ERR_USERNOTINCHANNEL(*this, params[2], "#" + chan_name));
		}
	}
}

void	Client::execINVITE(Command const& command)
{
	std::vector<std::string> const	&params = command.getParameters();
	if (params.size() < 2)
	{
		sendCommand(CREATE_ERR_NEEDMOREPARAMS(*this, command.getCommand()));
		return ;
	}
	std::string	chan_name(params[1]);
	if (chan_name.empty() || chan_name[0] != '#')
	{
		sendCommand(CREATE_ERR_BADCHANMASK(*this, chan_name));
		return ;
	}
	chan_name.erase(chan_name.begin());
	Client	*to_invite = Server::getClientManager().getClient(params[0]);
	if (to_invite == NULL)
	{
		sendCommand(CREATE_ERR_NOSUCHNICK(*this, params[0]));
		return ;
	}
	try
	{
		Server::getChannelManager().getChannel(chan_name).invite(this, to_invite);
	}
	catch (ChannelManager::DoesNotExistException const& e)
	{
		sendCommand(CREATE_ERR_NOSUCHCHANNEL(*this, "#" + chan_name));
	}
	catch (Channel::NotRegisteredException const& e)
	{
		sendCommand(CREATE_ERR_NOTONCHANNEL(*this, "#" + chan_name));
	}
	catch (Channel::RequireOperException const& e)
	{
		sendCommand(CREATE_ERR_CHANOPRIVSNEEDED(*this, "#" + chan_name));
	}
	catch (Channel::AlreadyInChannelException const& e)
	{
		sendCommand(CREATE_ERR_USERONCHANNEL(*this, params[0], "#" + chan_name));
	}
}

// void	Client::execINVITE(Command const& command)
// {
// 	std::vector<std::string>	&params = command.getParameters();
// 	if (params.size() < 2)
// 	{
// 		sendCommand(CREATE_ERR_NEEDMOREPARAMS(*this, command.getCommand()));
// 	}
// 	else
// 	{
		
// 	}
// }
