/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chanel.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amassias <amassias@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 14:05:02 by amassias          #+#    #+#             */
/*   Updated: 2024/06/06 19:11:15 by amassias         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANEL_HPP
# define CHANEL_HPP

# include "Client.hpp"

# include <string>

class Chanel
{
	public:
		typedef enum e_flags
		{
			NONE		= 0b0000,
			INVITE_ONLY	= 0b0001,
			TOPIC		= 0b0010,
			PASSWORD	= 0b0100,
			LIMIT		= 0b1000,
		}	flags;

	public:
		Chanel(std::string const& name, Client* owner_ptr);
		~Chanel(void);

		void	setPassword(std::string const& password);

		void	addClient(Client* client_ptr, std::string const& password);
		void	removeClient(Client* client_ptr);
		void	invite(std::string const& nick);
		void	kick(std::string const& nick);
	
	private:
		flags						m_flags;
		std::string					m_name;
		std::string					m_topic;
		std::string					m_password;
		size_t						m_limit;
		std::vector<Client *>		m_client;
		std::vector<Client *>		m_operators;
		std::vector<std::string>	m_invite_list;
};

#endif