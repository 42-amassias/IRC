/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amassias <amassias@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 01:58:44 by amassias          #+#    #+#             */
/*   Updated: 2024/05/28 03:41:58 by amassias         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Log.hpp"
#include "Server.hpp"
#include "utils.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#include <iostream>

static void	_setSigAction(
				int signal,
				struct sigaction *action,
				struct sigaction *save
				);

static void _handleSignal(
				const int signal
				);

Server*	Server::s_server_instance_ptr = NULL;

Server*	Server::getInstance(void)
{
	if (s_server_instance_ptr == NULL)
		s_server_instance_ptr = new Server();
	return (s_server_instance_ptr);
}

void	Server::destroyInstance(void)
{
	delete s_server_instance_ptr;
	s_server_instance_ptr = NULL;
}

void	Server::stop(void)
{
	m_running = false;
}

Server::Server(void) :
	m_socket_fd(-1),
	m_sock_addr(),
	m_pollfds(),
	m_running(false)
{
	try
	{
		this->createSocket();
		this->bindSocket();
		this->listenSocket();
		this->setupSignals();
	}
	catch(const InitializationFailureException& e)
	{
		if (m_socket_fd >= 0)
			close(m_socket_fd);
		throw e;
	}
	m_running = true;
	Log::Info << "Server started successfully on " << std::endl;
}

Server::~Server(void)
{
	ITERATE(std::vector<struct pollfd>, m_pollfds, itr)
		close((*itr).fd);
	close(m_socket_fd);
	_setSigAction(SIGINT, &m_old_sigint_action, NULL);
	_setSigAction(SIGTERM, &m_old_sigterm_action, NULL);
	_setSigAction(SIGQUIT, &m_old_sigquit_action, NULL);
}

void	Server::createSocket(void)
{
	const int	enable = 1;
	const int	options = SO_REUSEADDR | SO_REUSEPORT;

	m_socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (m_socket_fd < 0)
		throw InitializationFailureException("Could not create socket");
	if (fcntl(m_socket_fd, F_SETFL, O_NONBLOCK))
		throw InitializationFailureException("Could not un-block socket");
	if (setsockopt(m_socket_fd, SOL_SOCKET, options, &enable, sizeof(enable)))
		throw InitializationFailureException("Could not parameterize socket");
	m_pollfds.push_back((struct pollfd){.fd = m_socket_fd, .events = POLLIN, .revents = 0});
	Log::Debug << "Socket created successfully (fd = " << m_socket_fd << ")" << std::endl;
}

void	Server::bindSocket(void)
{
	m_sock_addr.sin_family = AF_INET;
	m_sock_addr.sin_addr.s_addr = INADDR_ANY;
	m_sock_addr.sin_port = htons(Server::default_port);
	if (bind(m_socket_fd, (struct sockaddr *)&m_sock_addr, sizeof(m_sock_addr)))
		throw InitializationFailureException("Could not bind socket");
	Log::Debug << "Socket binded to port " << Server::default_port << std::endl;
}

void	Server::listenSocket(void)
{
	if (listen(m_socket_fd, Server::connection_request_queue_size))
		throw InitializationFailureException("Cannot listen on server socket");
	Log::Debug << "Socket is now listening" << std::endl;
}

void	Server::setupSignals(void)
{
	struct sigaction	action;

	action.sa_handler = _handleSignal;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	_setSigAction(SIGINT, &action, &m_old_sigint_action);
	_setSigAction(SIGTERM, &action, &m_old_sigterm_action);
	_setSigAction(SIGQUIT, &action, &m_old_sigquit_action);
}

void	Server::acceptConnection(void)
{
	size_t			address_length;
	int				fd;
	char			host[INET_ADDRSTRLEN];

	address_length = sizeof(sockaddr);
	fd = accept(
		m_socket_fd,
		(struct sockaddr *)&m_sock_addr,
		(socklen_t *)&address_length
		);
	if (fd <= 0)
		throw SocketFailureException("`accept()` failed on a socket");
	inet_ntop(AF_INET, &m_sock_addr.sin_addr, host, INET_ADDRSTRLEN);
	m_pollfds.push_back((struct pollfd){.fd = fd, .events = POLLIN, .revents = 0});
	Log::Info << "New connection (fd = " << fd << ")" << std::endl;
}

void	Server::loop(void)
{
	int							ret;
	std::vector<struct pollfd>	pollfds_copy;

	while (m_running)
	{
		ret = poll(m_pollfds.data(), m_pollfds.size(), DEFAULT_POLL_TIMEOUT);
		if (ret < 0)
		{
			if (errno == EINTR)
				continue ;
			throw SocketFailureException("Unable to `poll()` the pollfds");
		}
		if (ret == 0)
		{
			Log::Warn << "`poll()` reached the timeout (" << DEFAULT_POLL_TIMEOUT / 1000 << "s)" << std::endl;
			continue ;
		}
		pollfds_copy = m_pollfds;
		ITERATE(std::vector<struct pollfd>, pollfds_copy, itr)
		{
			if (itr->revents == 0)
				continue ;
			if (itr->revents != POLLIN)
				throw SocketFailureException("Incoherent revents on a pollfd");
			if (itr->fd == m_socket_fd)
			{
				acceptConnection();
				continue ;
			}
		}
	}
}

static void _handleSignal(
				const int signal
				)
{
	(void)signal;
	Server::getInstance()->stop();
}

static void	_setSigAction(
				int signal,
				struct sigaction *action,
				struct sigaction *save
				)
{
	// Not catching return values of sigaction(3), impossible for it to fail.
	if (save != NULL)
	{
		sigaction(signal, NULL, save);
		if (save->sa_handler == SIG_IGN)
			return ;
	}
	sigaction(signal, action, NULL);
}
