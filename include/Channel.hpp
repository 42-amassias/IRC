#ifndef  CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <set>

# include "Client.hpp"

# include "utils.hpp"

class Client;

class Channel
{
	public:
		Channel(std::string const& name, Client *owner);
		~Channel();

		void	join(std::string const& chan_key, Client *client);
		void	sendToAll(Command const& command, Client *sender);

		void	removeClient(Client *client); // Dont track freed data
		bool	empty() const;

	private:
		Channel();
		void	join(Client *client); // After validation
		void	sendToAll(Command const& command);

	private:
		bool				m_flag_i; // Invitation only;
		bool				m_flag_t; // true == operator only topic
		std::string			m_flag_k; // Password if m_flag_k.empty() no password
		std::set<Client *>	m_flag_o; // Set of operators
		std::size_t			m_flag_l; // Place limit

		const std::string	m_chan_name;
		std::string			m_topic;
		std::set<Client *>	m_invited; // Invite list
		std::set<Client *>	m_clients; // Must contain m_flag_o

	public:
		CREATE_EXCEPTION(InvalidChannelName, "The given channel name isn't valid");
		CREATE_EXCEPTION(PasswordMismatch, "The given password isn't valid");
		CREATE_EXCEPTION(NotInvited, "Not on the invite list");
		CREATE_EXCEPTION(ChannelFull, "The channel is full");
		CREATE_EXCEPTION(NotRegistered, "Not in the current connected user channel");

};

#endif
