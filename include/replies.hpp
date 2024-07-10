/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amassias <amassias@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 10:39:55 by amassias          #+#    #+#             */
/*   Updated: 2024/07/10 21:33:54 by amassias         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Command.hpp"

namespace Replies
{
	namespace RPL
	{
		// Code: 353
		void	nam_reply(
					std::string const& channel_name,
					std::vector<Client *> const& clients,
					Client *client
					);
		
		// Code: 366
		void	end_of_names(
					std::string const& channel_name,
					Client *client
					);
	}

	namespace ERR
	{
		// Code: 403
		void	no_such_chan(
					std::string const& channel_name,
					Client *client
					);

		// Code: 442
		void	not_on_channel(
					std::string const& channel_name,
					Client *client
					);

		// Code: 461
		void	need_more_params(
					std::string const& command_name,
					Client *client
					);

		// Code: 462
		void	already_registered(
					Client *client
					);

		// Code: 471
		void	chan_is_full(
					std::string const& channel_name,
					Client *client
					);

		// Code: 473
		void	invite_only_chan(
					std::string const& channel_name,
					Client *client
					);

		// Code: 475
		void	bad_chan_key(
					std::string const& channel_name,
					Client *client
					);
	}
}
