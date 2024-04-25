#ifndef SERVER_HPP
# define SERVER_HPP

# include "server/Client.hpp"
# include "Exceptions.hpp"

# include <sys/socket.h>
# include <string>
# include <vector>
# include <sys/poll.h>


class Server
{
	public:
		class Config
		{
			public:
				Config(int port, std::string const& password);
				Config(Server::Config const& o);
				~Config(void);

				Server::Config&		operator=(Server::Config const& o);

				int					getPort(void) const;
				std::string const&	getPassword(void) const;

				void				setPort(int port);
				void				setPassword(std::string const& o);
			
			private:
				Config(void);

			private:
				int			m_port;
				std::string	m_password;
		};

	public:
		Server(Server::Config const& config);
		~Server(void);

	public:
		void	run(void);
		void	stop(void);

	private:
		Server(void);

	private:
		void _init(void);

	private:
		Server::Config const		m_config;
		int							m_socket;
		ClientManager				m_client_manager;
		std::vector<struct pollfd>	m_pollfds;

	public:
		CREATE_GENERIC_EXCEPTION(
			ServerAlreadyRunning,
			A server is already running
		);
		CREATE_GENERIC_EXCEPTION(
			ServerCouldntCreateSocket,
			Socket cant be created
		);
		CREATE_GENERIC_EXCEPTION(
			ServerStartupFailure,
			The server could not start
		);
};

#endif
