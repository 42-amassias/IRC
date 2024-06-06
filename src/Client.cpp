#include <cerrno>
#include <cstring>

#include "Log.hpp"
#include "Client.hpp"

Client::Client(struct sockaddr const& addr) :
	m_nickname(),
	m_username(),
	m_realname(),
	m_logged(false),
	m_addr(addr)
{
	Log::Info << "New connection : "
		<< ipv4FromSockaddr(m_addr) << ":" << ntohs(((struct sockaddr_in *)&m_addr)->sin_port) << std::endl;
}

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

struct sockaddr	Client::getSockaddr() const
{
	return m_addr;
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
	char	buf[default_read_size];

	while (true)
	{
		ssize_t ret = recv(fd, buf, sizeof(buf), 0);
		Log::Debug << "recv() == " << ret << std::endl;
		if (ret == 0 || (ret <= 0 && errno == ECONNRESET))
			throw ConnectionLostException();
		else if (ret <= 0 && errno == EWOULDBLOCK)
			break ;
		else if (ret <= 0)
			throw ReadErrorException(std::strerror(errno));
		m_buffer.pushBack(buf, ret);
	}
}

void	Client::execPendingCommands()
{
	Command	c;
	while (true)
	{
		try
		{
			c = m_buffer.popFront();
		}
		catch (CommandBuffer::NoPendingCommandException const& e)
		{
			break ;
		}
	}
}

