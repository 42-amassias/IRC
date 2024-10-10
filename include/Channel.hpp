#ifndef  CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <set>
# include <ctime>

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
		void	sendToAll(Command const& command, Client *sender, std::set<Client *> &filter); // Update the filter to the sended clients

		void	removeClient(Client *client); // Dont track freed data
		bool	empty() const;

		void	changeMode(std::string const& mode, std::vector<std::string> const& args, Client *client);

		Command				getModeCommand() const;
		std::string	const&	getTopic() const;
		void				sendTopic(Client *client) const;
		void				setTopic(std::string const& topic, Client *sender);
		bool				isInvited(Client *client) const;
		void				invite(Client *client, Client *to_invite);

	private:
		Channel();
		void	setTopic(std::string const& topic);
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
		std::string			m_topic_nick; // Last modifier
		std::time_t			m_topic_setat; // Timestamp
		std::set<Client *>	m_invited; // Invite list
		std::set<Client *>	m_clients; // Must contain m_flag_o

	public:
		CREATE_EXCEPTION(InvalidChannelName, "The given channel name isn't valid");
		CREATE_EXCEPTION(PasswordMismatch, "The given password isn't valid");
		CREATE_EXCEPTION(NotInvited, "Not on the invite list");
		CREATE_EXCEPTION(ChannelFull, "The channel is full");
		CREATE_EXCEPTION(AlreadyInChannel, "The client to invite is already in the channel");
		CREATE_EXCEPTION(NotRegistered, "Not in the current connected user channel");
		CREATE_EXCEPTION(NeedMoreParams, "Need more params for the mode change");
		CREATE_EXCEPTION(InvalidModeFlag, "The given mode is not valid");
		CREATE_EXCEPTION_MESSAGE(UnknownMode);
		CREATE_EXCEPTION(RequireOper, "Not in the operator list");
		CREATE_EXCEPTION(NotInChannel, "Requested user not in the clients list");

};

#endif
