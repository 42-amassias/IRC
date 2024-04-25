#include "server/Chanel.hpp"

Chanel::Chanel(Chanel::Config const& config) :
	m_config(config),
	m_clients(),
	m_invite_list()
{
}

Chanel::Config&	Chanel::getConfig(void)
{
	return (m_config);
}

static std::vector<ClientManager::AClient const*>::iterator	_search_for(
	ClientManager::AClient const* client,
	std::vector<ClientManager::AClient const*>& list
	)
{
	std::vector<ClientManager::AClient const*>::iterator	itr;

	for (itr = list.begin(); itr != list.end(); ++itr)
		if (client->getId() == (*itr)->getId())
			return (itr);
	return (list.end());
}

static bool	_is_in(
	ClientManager::AClient const* client,
	std::vector<ClientManager::AClient const*> list
	)
{
	return (_search_for(client, list) != list.end());
}

void	Chanel::addClient(ClientManager::AClient const* client, std::string const& password)
{
	if (_is_in(client, m_clients))
		throw Chanel::UserAplreadyJoinedException();
	if (m_config.getLimit() > 0 && m_clients.size() >= m_config.getLimit())
		throw Chanel::UserLimitReachedException();
	if (m_config.isInviteOnly() && !_is_in(client, m_invite_list))
		throw Chanel::UserNotInvitedException();
	if (!m_config.getPassword().empty() && m_config.getPassword() != password)
		throw Chanel::ChanelInvalidPasswordException();
	m_clients.push_back(client);
}

//TODO
void	Chanel::removeClient(ClientManager::AClient const* client)
{
	std::vector<ClientManager::AClient const*>::iterator	itr;
	
	itr = _search_for(client, m_clients);
	if (itr == m_clients.end())
		return ;
	m_clients.erase(itr);
	// TODO: Does the invite list has to be cleared?
	// itr = _search_for(client, m_invite_list);
	// if (itr == m_invite_list.end())
	// 	return ;
	// m_invite_list.erase(itr);
}

void	Chanel::addClientToWhiteList(ClientManager::AClient const* client)
{
	if (!m_config.isInviteOnly())
		return ;
	if (_is_in(client, m_invite_list))
		return ;
	m_invite_list.push_back(client);
}
