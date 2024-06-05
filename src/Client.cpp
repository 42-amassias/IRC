#include "Log.hpp"
#include "Client.hpp"

Client::Client(struct sockaddr addr) :
	m_nickname(),
	m_username(),
	m_realname(),
	m_logged(false),
	m_addr(addr)
{}

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

void	Client::receive(int fd)
{
	char	buf[1024];
	
	ssize_t ret = recv(fd, buf, 1024, 0);
	if (ret == 0)
		throw ConnectionLostException();
	else if(ret <= 0)
		throw ReadErrorException();
	Log::Debug << "Readed : " << ret << std::endl;
}

