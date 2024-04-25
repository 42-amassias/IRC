#ifndef CHANEL_HPP
# define CHANEL_HPP

# include "Exceptions.hpp"
# include "server/Client.hpp"

# include <string>
# include <vector>

# define CHANEL_CONFIG_DEFAULT_INVITE_ONLY false
# define CHANEL_CONFIG_DEFAULT_TOPIC_RESTRICTION false
# define CHANEL_CONFIG_DEFAULT_PASSSWORD ""
# define CHANEL_CONFIG_DEFAULT_LIMIT 0

class Chanel
{
	public:
		class Config
		{
			public:
				Config(std::string const& topic);
				Config(Config const& o);
				~Config(void);

				Config&	operator=(Config const& o);

				void				setTopic(std::string const& topic);
				void				setInviteOnly(bool flag);
				void				setTopicRestriction(bool flag);
				void				setPassword(std::string const& password);
				void				setLimit(size_t limit);

				std::string const&	getTopic(void) const;
				bool				isInviteOnly(void) const;
				bool				hasTopicRestriction(void) const;
				std::string const&	getPassword(void) const;
				size_t				getLimit(void) const;

			private:
				Config(void);
			
			private:
				std::string	m_topic;
				bool		m_invite_only;
				bool		m_topic_restriction;
				std::string	m_password;
				size_t		m_limit;
		};

	public:
		Chanel(Chanel::Config const& config);

		Chanel::Config&	getConfig(void);
		void			addClient(ClientManager::AClient const* client, std::string const& password);
		void			removeClient(ClientManager::AClient const* client);
		void			addClientToWhiteList(ClientManager::AClient const* client);

	private:
		Chanel(void);
	
	private:
		Chanel::Config								m_config;
		std::vector<ClientManager::AClient const*>	m_clients;
		std::vector<ClientManager::AClient const*>	m_invite_list;
	
	public:
		CREATE_GENERIC_EXCEPTION(
			UserNotInvited,
			"User was not invited to chanel"
		);
		CREATE_GENERIC_EXCEPTION(
			UserLimitReached,
			"User cannot join channel, too many users"
		);
		CREATE_GENERIC_EXCEPTION(
			ChanelInvalidPassword,
			"User has incorrecto password for chanel"
		);
		CREATE_GENERIC_EXCEPTION(
			UserAplreadyJoined,
			"User has already joined chanel"
		);
};

#endif