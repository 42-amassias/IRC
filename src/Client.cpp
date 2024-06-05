#include "Client.hpp"

Client::Client() : m_logged(false) {}

Client::~Client() {}

std::string	Client::getNickname() const
{
	return m_nickname;
}

std::string	Client::getUsername() const
{
	return m_username;
}

std::string	Client::getRealname() const
{
	return m_realname;
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

