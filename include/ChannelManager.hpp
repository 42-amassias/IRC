#ifndef  CHANNELMANAGER_HPP
# define CHANNELMANAGER_HPP

# include <map>
# include <string>

# include "Client.hpp"
# include "Channel.hpp"

# include "utils.hpp"

class ChannelManager
{
	public:
		ChannelManager();
		~ChannelManager();

		void	createOrJoin(std::string const& chan_name, std::string const& chan_key, Client *client);

		Channel	&getChannel(std::string const& chan_name);

		void	sendToAll(Command const& command, Client *client); // To propage QUIT message
		void	removeClient(Client *client); // Don't track freed data lol

	private:
		std::map<std::string, Channel *>	m_channels;

	public:
		CREATE_EXCEPTION(DoesNotExist, "The asked channel does not exist");

};

#endif
