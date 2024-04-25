#include "server/Client.hpp"

#include <iostream>

ClientManager::ClientManager(void) :
	m_clients(),
	m_current_client_id(0)
{
}

ClientManager::~ClientManager(void)
{
	std::vector<ClientManager::AClient*>::iterator	itr;

	for (itr = m_clients.begin(); itr != m_clients.end(); ++itr)
		delete (*itr);
}

ClientManager::AClient const&	ClientManager::getClient(std::string const& name) const
{
	return (**_c_getClient(name));
}

ClientManager::AClient const&	ClientManager::getClient(client_id_t id) const
{
	return (**_c_getClient(id));
}

void	ClientManager::destroyClient(std::string const& name)
{
	destroyClient(_m_getClient(name));
}

void	ClientManager::destroyClient(client_id_t id)
{
	destroyClient(_m_getClient(id));
}

void	ClientManager::destroyClient(ClientManager::client_iterator_t client)
{
	delete *client;
	m_clients.erase(client);
}

ClientManager::client_iterator_t	ClientManager::_m_getClient(std::string const& name)
{
	client_iterator_t	itr;

	for (itr = m_clients.begin(); itr != m_clients.end(); ++itr)
		if (name == (*itr)->getName())
			return (itr);
	throw UserNameNotFoundException();
}

ClientManager::client_iterator_t	ClientManager::_m_getClient(client_id_t id)
{
	client_iterator_t	itr;

	for (itr = m_clients.begin(); itr != m_clients.end(); ++itr)
		if (id == (*itr)->getId())
			return (itr);
	throw IdNotFoundException();
}

ClientManager::client_const_iterator_t	ClientManager::_c_getClient(std::string const& name) const
{
	client_const_iterator_t	itr;

	for (itr = m_clients.begin(); itr != m_clients.end(); ++itr)
		if (name == (*itr)->getName())
			return (itr);
	throw UserNameNotFoundException();
}

ClientManager::client_const_iterator_t	ClientManager::_c_getClient(client_id_t id) const
{
	ClientManager::client_const_iterator_t	itr;

	for (itr = m_clients.begin(); itr != m_clients.end(); ++itr)
		if (id == (*itr)->getId())
			return (itr);
	throw ClientManager::IdNotFoundException();
}

void	ClientManager::canUseUserName(std::string const& name) const
{
	try { _c_getClient(name); } catch (const std::exception &e) { return ; }
	throw ClientManager::UserNameTakenException();
}

void	ClientManager::canUseNickName(std::string const& nick) const
{
	try { _c_getClient(nick); } catch (const std::exception &e) { return ; }
	throw ClientManager::NickNameTakenException();
}
