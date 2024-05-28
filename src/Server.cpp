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

#include "Server.hpp"
#include "utils.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

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
	m_client_sockets(),
	m_running(false)
{
	try
	{
		this->createSocket();
		this->bindSocket();
		this->makeSocketListen();
		this->setupSignals();
	}
	catch(const InitializationFailureException& e)
	{
		if (m_socket_fd >= 0)
			close(m_socket_fd);
		throw e;
	}
	m_running = true;
}

Server::~Server(void)
{
	ITERATE(std::vector<struct pollfd>, m_client_sockets, itr)
		close((*itr).fd);
	close(m_socket_fd);
	_setSigAction(SIGINT, &m_old_sigint_action, NULL);
	_setSigAction(SIGTERM, &m_old_sigterm_action, NULL);
	_setSigAction(SIGQUIT, &m_old_sigquit_action, NULL);
}

void	Server::createSocket(void)
{
	m_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket_fd < 0)
		throw InitializationFailureException("Could not create socket");
}

void	Server::bindSocket(void)
{
	const int	enable = 1;
	const int	options = SO_REUSEADDR | SO_REUSEPORT;

	if (setsockopt(m_socket_fd, SOL_SOCKET, options, &enable, sizeof(enable)))
		throw InitializationFailureException("Could not parameterize socket");
	m_sock_addr.sin_family = AF_INET;
	m_sock_addr.sin_addr.s_addr = INADDR_ANY;
	m_sock_addr.sin_port = htons(Server::default_port);
	if (bind(m_socket_fd, (struct sockaddr *)&m_sock_addr, sizeof(m_sock_addr)))
		throw InitializationFailureException("Could not bind socket");
	if (fcntl(m_socket_fd, F_SETFL, O_NONBLOCK))
		throw InitializationFailureException("Could not un-block socket");
}

void	Server::makeSocketListen(void)
{
	if (listen(m_socket_fd, Server::connection_request_queue_size))
		throw InitializationFailureException("Cannot listen on server socket");
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

void	Server::initiateConnection(void)
{
	size_t			address_length;
	int				connection;
	char			host[INET_ADDRSTRLEN];
	struct pollfd	pfd;

	address_length = sizeof(sockaddr);
	connection = accept(
		m_socket_fd,
		(struct sockaddr *)&m_sock_addr,
		(socklen_t *)&address_length
		);
	if (connection <= 0)
		return ;
	inet_ntop(AF_INET, &m_sock_addr.sin_addr, host, INET_ADDRSTRLEN);
	pfd.fd = connection;
	pfd.events = POLLIN;
	pfd.revents = 0;
	m_client_sockets.push_back(pfd);
	std::cout << "New connection(" << pfd.fd << ")" << std::endl;
}

void	Server::loop(void)
{
	while (m_running)
	{
		this->initiateConnection();
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
