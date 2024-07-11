/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   replies.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amassias <amassias@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 10:42:17 by amassias          #+#    #+#             */
/*   Updated: 2024/07/11 21:02:39 by amassias         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "replies.hpp"
#include "Server.hpp"

#include <sstream>

#define REPLY(_client_ptr, _code, ...)										\
		{																	\
			Command	_c;														\
																			\
			CREATE_COMMAND(_c, "localhost", #_code, "*", __VA_ARGS__);		\
			(_client_ptr)->sendCommand(_c);									\
		}																	\

namespace Replies
{
	namespace RPL
	{
		void	welcome(
					Client *client)
			REPLY(client, 001, client->getNickname());

		void	your_host(
					Client *client
					)
			REPLY(client, 002, client->getNickname(), "Your host is localhost, running version 42069");

		void	created(
					Client *client
					)
			REPLY(client, 003, client->getNickname(), "This server was created never");

		void	my_info(
					Client *client
					)
			REPLY(client, 004, client->getNickname());
		
		void	l_user_client(
					Client *client
					)
		{
			size_t					invisible_user_count(0);
			size_t					user_count(0);
			Command					c;
			std::stringstream		ss("");
			std::vector<Client *>	clients(Server::getInstance()->getClients());

			ITERATE_CONST(std::vector<Client *>, clients, itr)
			{
				if (!(*itr)->isRegistered())
					continue ;
				++user_count;
				if (!(*itr)->isInvisible())
					continue ;
				++invisible_user_count;
			}
			ss << "There are " << clients.size() << " users and " << invisible_user_count << " invisible on 1 servers";
			CREATE_COMMAND(c, "localhost", "251", client->getNickname(), ss.str());
			client->sendCommand(c);
		}

		void	l_user_me(
					Client *client
					)
		{
			Command					c;
			std::stringstream		ss("");
			std::vector<Client *>	clients(Server::getInstance()->getClients());

			ss << "I have " << clients.size() << " clients and 0 servers";
			CREATE_COMMAND(c, "localhost", "251", client->getNickname(), ss.str());
			client->sendCommand(c);
		}

		void	nam_reply(
					std::string const& channel_name,
					std::vector<Client *> const& clients,
					Client *client
					)
		{
			std::vector<std::string>	names;

			names.push_back("*");
			names.push_back(channel_name);
			ITERATE_CONST(std::vector<Client *>, clients, itr)
				names.push_back((*itr)->getNickname());
			client->sendCommand(Command("localhost", "353", names));
		}

		void	end_of_names(
					std::string const& channel_name,
					Client *client
					)
			REPLY(client, 366, channel_name, "End of NAMES list");
	}

	namespace ERR
	{
		void	no_such_chan(
					std::string const& channel_name,
					Client *client
					)
			REPLY(client, 403, channel_name, "No such channel");

		void	no_nickname_given(
					Client *client
					)
			REPLY(client, 431, "No nickname given");

		void	nick_collision(
					Client *client
					)
			REPLY(client, 436, "Nickname collision KILL");

		void	not_on_channel(
					std::string const& channel_name,
					Client *client
					)
			REPLY(client, 442, channel_name, "You're not on that channel");

		void	need_more_params(
					std::string const& command_name,
					Client *client
					)
			REPLY(client, 461, command_name, "Not enough parameters");
		
		void	already_registered(
					Client *client
					)
			REPLY(client, 462, "You may not register");

		void	chan_is_full(
					std::string const& channel_name,
					Client *client
					)
			REPLY(client, 471, channel_name, "Cannot join channel (+l)");

		void	invite_only_chan(
					std::string const& channel_name,
					Client *client
					)
			REPLY(client, 473, channel_name, "Cannot join channel (+i)");
		
		void	bad_chan_key(
					std::string const& channel_name,
					Client *client
					)
			REPLY(client, 473, channel_name, "Cannot join channel (+k)");
	}
}