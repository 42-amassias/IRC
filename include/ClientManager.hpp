#ifndef  CLIENTMANAGER_HPP
# define CLIENTMANAGER_HPP

# include <set>

# include "Client.hpp"

class ClientManager
{
	public:
		ClientManager();
		~ClientManager();

		void	addClient(Client *client);
		void	removeClient(Client *client);

		bool	inUse(std::string const& nickname) const;

	private:
		std::set<Client *>	m_logged_clients;
};

#endif
