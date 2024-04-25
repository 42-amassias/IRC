#include "server/Server.hpp"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <netinet/in.h>

Server::Server(Server::Config const& config) :
	m_config(config),
	m_client_manager()
{

}

Server::~Server(void)
{
}

void	Server::_init(void)
{
	int	on = 1;
	if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw ServerStartupFailureException();
	if (setsockopt(m_socket, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &on, sizeof(on)))
		throw ServerStartupFailureException();
	struct sockaddr_in addr_in = {
		.sin_family = AF_INET,
		.sin_port = htons(m_config.getPort()),
		.sin_addr = {INADDR_ANY},
	};
	if (ioctl(m_socket, FIONBIO, &on) < 0)
		throw ServerStartupFailureException();
	if (bind(m_socket, reinterpret_cast<struct sockaddr *>(&addr_in),
				sizeof(addr_in)) < 0)
		throw ServerStartupFailureException();
	if (listen(m_socket, 0) < 0)
		throw ServerStartupFailureException();
	m_pollfds.push_back((struct pollfd){.fd = m_socket, .events = POLLIN});
}

void	Server::run(void)
{
	int	ret;

	_init();
	while (true)
	{
		ret = poll(m_pollfds.data(), m_pollfds.size(), 20000);

	}
}

void	Server::stop(void)
{
}
