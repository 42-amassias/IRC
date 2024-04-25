#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Exceptions.hpp"

# include <string>
# include <vector>

typedef unsigned int	client_id_t;

class ClientManager
{
	public:
		class AClient
		{
			public:
				virtual ~AClient(void)
				{
				}

				client_id_t			getId(void) const { return (m_id); }
				std::string const&	getName(void) const { return (m_name); }
				std::string const&	getNick(void) const { return (m_nick); }
			
			public:
				virtual void		sendMessage(std::string const& message) const = 0;
			
			protected:
				AClient(
					client_id_t id,
					std::string const& name,
					std::string const& nick
					) :
					m_id(id),
					m_name(name),
					m_nick(nick)
				{
				}

				AClient(
					client_id_t id,
					std::string const& name
					) :
					m_id(id),
					m_name(name),
					m_nick(name)
				{
				}

			private:
				client_id_t const	m_id;
				std::string const	m_name;
				std::string 		m_nick;
		};

		class ABot :
			public AClient
		{
			public:
				virtual ~ABot(void)
				{
				}

				virtual void	sendMessage(std::string const& message) const = 0;

			protected:
				ABot(
					client_id_t id,
					std::string const& name,
					std::string const& nick
					) :
					AClient(id, name, nick)
				{
				}

				ABot(
					client_id_t id,
					std::string const& name
					) :
					AClient(id, name, name)
				{
				}
		};

	private:
		typedef std::vector<AClient*>::iterator			client_iterator_t;
		typedef std::vector<AClient*>::const_iterator	client_const_iterator_t;

	public:
		ClientManager(void);
		~ClientManager(void);

		template<typename T>
		AClient const&			createClient(std::string const& name, std::string const& nick)
		{
			AClient	*client;
			canUseUserName(name);
			canUseNickName(nick);
			client = new T(m_current_client_id++, name, nick);
			m_clients.push_back(client);
			return (*client);
		}

		template<typename T>
		AClient const&			createClient(std::string const& name)
		{
			return (this->createClient<T>(name, name));
		}

		AClient const&			getClient(std::string const& name) const;
		AClient const&			getClient(client_id_t id) const;

		void					destroyClient(std::string const& name);
		void					destroyClient(client_id_t id);

	private:
		client_iterator_t		_m_getClient(std::string const& name);
		client_iterator_t		_m_getClient(client_id_t id);

		client_const_iterator_t	_c_getClient(std::string const& name) const;
		client_const_iterator_t	_c_getClient(client_id_t id) const;

		void					destroyClient(client_iterator_t client);

		void					canUseUserName(std::string const& name) const;
		void					canUseNickName(std::string const& name) const;

	private:
		std::vector<AClient*>	m_clients;
		client_id_t				m_current_client_id;
	
	public:
		CREATE_GENERIC_EXCEPTION(
			UserNameTaken,
			User name is already in use
		);
		CREATE_GENERIC_EXCEPTION(
			NickNameTaken,
			Nick name is already in use
		);
		CREATE_GENERIC_EXCEPTION(
			UserNameNotFound,
			User name not found
		);
		CREATE_GENERIC_EXCEPTION(
			NickNameNotFound,
			Nick name not found
		);
		CREATE_GENERIC_EXCEPTION(
			IdNotFound,
			Id not found
		);
};

#endif