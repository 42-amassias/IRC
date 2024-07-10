/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amassias <amassias@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 14:05:02 by amassias          #+#    #+#             */
/*   Updated: 2024/07/08 09:35:59 by amassias         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Client.hpp"

# include <string>

class Channel
{
	public:
		typedef enum e_flags
		{
			NONE		= 0,
			INVITE_ONLY	= 1 << 0,
			TOPIC		= 1 << 1,
			PASSWORD	= 1 << 2,
			LIMIT		= 1 << 3,
		}	t_flags;

	public:
		Channel(std::string const& name, Client* owner_ptr);
		~Channel(void);

		void	setPassword(std::string const& password);

		void	addClient(Client* client_ptr, std::string const& password);
		void	removeClient(Client* client_ptr);
		void	invite(std::string const& nick);
		void	kick(std::string const& nick);

	private:
		t_flags						m_flags;
		std::string					m_name;
		std::string					m_topic;
		std::string					m_password;
		size_t						m_limit;
		std::vector<Client *>		m_clients;
		std::vector<Client *>		m_operators;
		std::vector<std::string>	m_invite_list;
};

#endif