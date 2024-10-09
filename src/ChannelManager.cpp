#include "ChannelManager.hpp"


ChannelManager::ChannelManager() :
	m_channels()
{
}

ChannelManager::~ChannelManager()
{
	for (std::map<std::string, Channel *>::iterator itr = (m_channels).begin(); itr != (m_channels).end(); ++itr)
		delete itr->second;
}

void	ChannelManager::createOrJoin(std::string const& chan_name, std::string const& chan_key, Client *client)
{
	if (m_channels.count(chan_name))
	{
		m_channels[chan_name]->join(chan_key, client); 
		return ;
	}
	m_channels[chan_name] = new Channel(chan_name, client);
}

Channel	&ChannelManager::getChannel(std::string const& chan_name)
{
	if (!m_channels.count(chan_name))
		throw DoesNotExistException();
	return (*m_channels[chan_name]);
}

void	ChannelManager::sendToAll(Command const& command, Client *client)
{
	for (std::map<std::string, Channel *>::iterator itr = (m_channels).begin(); itr != (m_channels).end(); ++itr)
		try
		{
			itr->second->sendToAll(command, client);
		}
		catch (Channel::NotRegisteredException const& e) { /* ignore */ }
}

void	ChannelManager::removeClient(Client *client)
{
	for (std::map<std::string, Channel *>::iterator itr = (m_channels).begin(); itr != (m_channels).end();)
	{
		itr->second->removeClient(client);
		if (itr->second->empty())
		{
			delete itr->second;
			m_channels.erase(itr++);
		}
		else
			++itr;
	}
}
