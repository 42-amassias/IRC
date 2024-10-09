#include "Log.hpp"
#include "utils.hpp"

#include "ClientManager.hpp"

ClientManager::ClientManager() :
	m_logged_clients()
{
}

ClientManager::~ClientManager()
{
}

void	ClientManager::addClient(Client *client)
{
	Log::Debug << "New client logged : " << client->getNickname() << std::endl;
	m_logged_clients.insert(client);
}

void	ClientManager::removeClient(Client *client)
{
	Log::Debug << "Removed client : " << client->getNickname() << std::endl;
	m_logged_clients.erase(client);
}

bool	ClientManager::inUse(std::string const& nickname) const
{
	ITERATE_CONST(std::set<Client *>, m_logged_clients, itr)
		if ((*itr)->getNickname() == nickname)
			return (true);
	return (false);
}


Client	*ClientManager::getClient(std::string const& nickname)
{
	ITERATE(std::set<Client *>, m_logged_clients, itr)
		if ((*itr)->getNickname() == nickname)
			return (*itr);
	return (NULL);
}
