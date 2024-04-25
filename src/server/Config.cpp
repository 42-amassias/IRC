#include "server/Server.hpp"

Server::Config::Config(int port, std::string const& password) :
	m_port(port),
	m_password(password)
{
}

Server::Config::Config(Server::Config const& o)
{
	*this = o;
}

Server::Config::~Config(void)
{
}

Server::Config	&Server::Config::operator=(Server::Config const& o)
{
	if (this == &o)
		return (*this);
	m_port = o.m_port;
	m_password = o.m_password;
	return (*this);
}

int	Server::Config::getPort(void) const
{
	return (m_port);
}

std::string const&	Server::Config::getPassword(void) const
{
	return (m_password);
}

void	Server::Config::setPort(int port)
{
	m_port = port;
}

void	Server::Config::setPassword(std::string const& password)
{
	m_password = password;
}
