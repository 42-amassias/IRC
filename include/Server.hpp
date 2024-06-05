/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amassias <amassias@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 01:58:52 by amassias          #+#    #+#             */
/*   Updated: 2024/05/28 03:34:04 by amassias         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
# define SERVER_H

# include <netinet/in.h>
# include <signal.h>
# include <vector>
# include <map>

# include "Client.hpp"

# include "utils.hpp"

# define CONNECTION_REQUEST_QUEUE_SIZE 128
# define DEFAULT_POLL_TIMEOUT (60 * 1000)

class Server
{
	public:
		~Server(void);

		static Server*	getInstance(void);
		static void		destroyInstance(void);

		void	init(void);
		void	loop(void);
		void	stop(void);

		bool	checkPwd(std::string const &pwd) const;
		void	setPwd(std::string const &pwd);

		void	setPort(int port);
		int		getPort(void) const;

	private:
		Server(void);

		void	createSocket(void);
		void	bindSocket(void);
		void	listenSocket(void);
		void	setupSignals(void);

		void	acceptConnection(void);

	private:
		static const uint16_t	default_port = 6969;
		static const int		connection_request_queue_size = 128;

		static Server	*s_server_instance_ptr;
	
	private:
		int							m_socket_fd;
		int							m_port;
		sockaddr_in					m_sock_addr;
		std::string					m_pwd;
		std::vector<struct pollfd>	m_pollfds;
		std::map<int, Client *>		m_clients;
		volatile bool				m_running;
		struct sigaction			m_old_sigint_action;
		struct sigaction			m_old_sigterm_action;
		struct sigaction			m_old_sigquit_action;

	public:
		CREATE_EXCEPTION_MESSAGE(InitializationFailure);
		CREATE_EXCEPTION_MESSAGE(SocketFailure);
};

#endif

