#include <climits>
#include <cstdlib>

#include "Log.hpp"
#include "Channel.hpp"

Channel::Channel(std::string const& name, Client *owner) :
	m_flag_i(false),
	m_flag_t(true),
	m_flag_k(""),
	m_flag_o(),
	m_flag_l(0),
	m_chan_name(name),
	m_topic(""),
	m_topic_nick(""),
	m_topic_setat(0),
	m_invited(),
	m_clients()
{
	if (!name.empty() && !usernickValidator(name))
		throw InvalidChannelNameException();
	Log::Debug << "Channel #" << name << " created by " << owner->getNickname() << std::endl;
	m_flag_o.insert(owner);
	join(owner);
}

Channel::~Channel()
{
	Log::Debug << "#" + m_chan_name << " channel deleted beceause of no one is connected" << std::endl;
}

void	Channel::join(std::string const& chan_key, Client *client)
{
	if (m_clients.count(client))
		return ;
	if (!m_flag_k.empty() && m_flag_k != chan_key)
		throw PasswordMismatchException();
	if (m_flag_l != 0 && m_clients.size() >= m_flag_l)
		throw ChannelFullException();
	if (m_flag_i && !m_invited.count(client))
		throw NotInvitedException();
	else if (m_flag_i)
		m_invited.erase(client);
	join(client);
}

void	Channel::sendToAll(Command const& command)
{
	ITERATE(std::set<Client *>, m_clients, itr)
		(*itr)->sendCommand(command);
}

void	Channel::removeClient(Client *client)
{
	m_clients.erase(client);
	m_invited.erase(client);
	m_flag_o.erase(client);
}

void	Channel::invite(Client *client, Client *to_invite)
{
	if (!m_clients.count(client))
		throw NotRegisteredException();
	if (!m_flag_o.count(client))
		throw RequireOperException();
	if (m_clients.count(to_invite))
		throw AlreadyInChannelException();
	if (m_flag_i)
		m_invited.insert(to_invite);
	client->sendCommand(CREATE_RPL_INVITING(*client, to_invite->getNickname(), "#" + m_chan_name));
	to_invite->sendCommand(CREATE_COMMAND(client->getPrefix(), "INVITE", to_invite->getNickname(), "#" + m_chan_name));
}

bool	Channel::empty() const
{
	return (m_flag_o.empty() && m_clients.empty());
}

void	Channel::sendToAll(Command const& command, Client *sender)
{
	if (!m_clients.count(sender))
		throw NotRegisteredException();
	ITERATE(std::set<Client *>, m_clients, itr)
		if ((*itr) != sender)
			(*itr)->sendCommand(command);
}

void	Channel::sendToAll(Command const& command, Client *sender, std::set<Client *> &filter)
{
	if (!m_clients.count(sender))
		throw NotRegisteredException();
	ITERATE(std::set<Client *>, m_clients, itr)
		if ((*itr) != sender && !filter.count((*itr)))
		{
			(*itr)->sendCommand(command);
			filter.insert((*itr));
		}
}

void	Channel::changeMode(std::string const& mode, std::vector<std::string> const& args, Client *client)
{
	if (!m_clients.count(client))
		throw NotRegisteredException();
	if (mode.size() < 2 || (mode[0] != '-' && mode[0] != '+'))
		throw InvalidModeFlagException();
	if (!m_flag_o.count(client))
		throw RequireOperException();
	bool	set = mode[0] == '+';
	std::string	sended_mode(mode);
	std::string::iterator	itr = sended_mode.begin()+1;
	std::vector<std::string>	sended_args(args);
	std::vector<std::string>::iterator	args_itr = sended_args.begin();
	while (itr != sended_mode.end())
	{
		std::set<Client *>::iterator	clients_itr;
		char	*end;
		std::size_t	tmp;
		switch (*itr)
		{
		case '+':
		case '-':
			set = *itr == '+';
			break ;
		case 't':
			m_flag_t = set;
			break ;
		case 'i':
			if (!set)
				m_invited.clear();
			m_flag_i = set;
			break ;
		case 'k':
			if (!set)
			{
				m_flag_k.clear();
				break ;
			}
			if (set && args_itr == sended_args.end())
				break ;
			m_flag_k = (*args_itr);
			// *(args_itr++) = "[hidden password]"; IRSSI is piece of shit
			break ;
		case 'l':
			if (!set)
			{
				m_flag_l = 0;
				break ;
			}
			if (set && args_itr == sended_args.end())
				break ;
			tmp = std::strtoul((*args_itr).c_str(), &end, 10);
			if (tmp == ULONG_MAX || tmp == 0)
				throw InvalidModeFlagException();
			m_flag_l = tmp;
			++args_itr;
			break ;
		case 'o':
			clients_itr = m_clients.begin();
			while (clients_itr != m_clients.end())
			{
				if ((*clients_itr)->getNickname() == (*args_itr))
				{
					if (!m_flag_o.count((*clients_itr)) && set) // Not already op and set ?
						m_flag_o.insert((*clients_itr));
					if (m_flag_o.count((*clients_itr)) && !set) // Allready op and unset ?
						m_flag_o.erase((*clients_itr));
					break ;
				}
				++clients_itr;
			}
			if (clients_itr == m_clients.end())
				throw NotInChannelException();
			++args_itr;
			break ;
		default:
			client->sendCommand(CREATE_ERR_UNKNOWNMODE(*client, std::string(1, *itr)));
			itr = sended_mode.erase(itr);
			continue ;
		}
		++itr;
	}
	sended_args.erase(args_itr, sended_args.end());
	Command	c(client->getPrefix(), "MODE", (const std::string[]){"#" + m_chan_name, sended_mode});
	ITERATE(std::vector<std::string>, sended_args, _itr)
		c.addParameter(*_itr);
	sendToAll(c);
}

void	Channel::join(Client *client)
{
	std::string	real_chan_name = "#" + m_chan_name;
	m_clients.insert(client);
	sendToAll(CREATE_COMMAND(client->getPrefix(), "JOIN", real_chan_name));
	if (m_topic_setat != 0)
		sendTopic(client);
	ITERATE(std::set<Client *>, m_clients, itr)
	{
		std::string s = (m_flag_o.count((*itr)) ? "@" : "") + (*itr)->getNickname();
		client->sendCommand(CREATE_COMMAND("", RPL_NAMREPLY, client->getNickname(), "=", real_chan_name, s));
	}
	client->sendCommand(CREATE_RPL_ENDOFNAMES(*client, real_chan_name));
}

Command	Channel::getModeCommand() const
{
	Command	c("", "MODE", (const std::string[]){"#" + m_chan_name});
	std::string	mode("+");
	std::vector<std::string>	args;
	if (m_flag_i)
		mode.push_back('i');
	if (!m_flag_k.empty())
	{
		mode.push_back('k');
		args.push_back(m_flag_k);
	}
	if (m_flag_t)
		mode.push_back('t');
	if (m_flag_l != 0)
	{
		mode.push_back('l');
		args.push_back(SSTR(m_flag_l));
	}
	if (mode == "+")
		mode = "-";
	else
		mode.push_back('-');
	if (!m_flag_i)
		mode.push_back('i');
	if (m_flag_k.empty())
		mode.push_back('k');
	if (!m_flag_t)
		mode.push_back('t');
	if (m_flag_l == 0)
		mode.push_back('l');
	if (*(mode.end()-1) == '-') // mode.size > 1
		mode.erase(mode.end()-1);
	c.addParameter(mode);
	ITERATE(std::vector<std::string>, args, itr)
		c.addParameter(*itr);
	return (c);
}

bool	Channel::isInvited(Client *client) const
{
	return (m_invited.count(client) || !m_flag_i);
}

std::string	const&	Channel::getTopic() const
{
	return (m_topic);
}

void	Channel::setTopic(std::string const& topic, Client *sender)
{
	if (m_flag_t && !m_flag_o.count(sender))
		throw RequireOperException();
	m_topic_nick = sender->getNickname();
	m_topic_setat = std::time(NULL);
	setTopic(topic);
	ITERATE(std::set<Client *>, m_clients, itr)
		sendTopic((*itr));
}

void	Channel::sendTopic(Client *client) const
{
	if (m_topic_setat == 0)
		client->sendCommand(CREATE_RPL_NOTOPIC(*client, "#" + m_chan_name));
	else
	{
		client->sendCommand(CREATE_RPL_TOPIC(*client, "#" + m_chan_name, m_topic));
		client->sendCommand(CREATE_RPL_TOPICWHOTIME(*client, "#" + m_chan_name, m_topic_nick, SSTR(m_topic_setat)));
	}
}

void	Channel::setTopic(std::string const& topic)
{
	m_topic = topic;
}
