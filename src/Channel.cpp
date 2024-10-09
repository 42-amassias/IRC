
#include "Log.hpp"

#include "Channel.hpp"

Channel::Channel(std::string const& name, Client *owner) :
	m_flag_i(false),
	m_flag_t(true),
	m_flag_k(""),
	m_flag_o(),
	m_flag_l(0),
	m_chan_name(name),
	m_topic(owner->getNickname() + "' channel"),
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
	if (!m_flag_k.empty() && m_flag_k != chan_key)
		throw PasswordMismatchException();
	if (m_flag_l != 0 && m_clients.size() >= m_flag_l)
		throw ChannelFullException();
	if (m_flag_i && !m_invited.count(client))
		throw NotInvitedException();
	else
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

void	Channel::join(Client *client)
{
	m_clients.insert(client);
	sendToAll(CREATE_COMMAND(client->getPrefix(), "JOIN", "#" + m_chan_name));
	client->sendCommand(CREATE_RPL_TOPIC(*client, m_chan_name, m_topic));
	ITERATE(std::set<Client *>, m_clients, itr)
	{
		std::string s = (m_flag_o.count((*itr)) ? "@" : "") + (*itr)->getNickname();
		client->sendCommand(CREATE_COMMAND("", RPL_NAMREPLY, client->getNickname(), "=", "#" + m_chan_name, s));
	}
	client->sendCommand(CREATE_RPL_ENDOFNAMES(*client, m_chan_name));
}
