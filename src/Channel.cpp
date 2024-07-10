/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amassias <amassias@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 09:31:47 by amassias          #+#    #+#             */
/*   Updated: 2024/07/08 15:41:45 by amassias         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "replies.hpp"

#include <algorithm>

#define FIND(itr, v) (std::find((itr).begin(), (itr).end(), (v)))
#define CONTAINS(itr, v) (FIND((itr), (v)) != (itr).end())
#define PASSWORD_PROTECTED (m_flags & PASSWORD)
#define INVITE_PROTECTED (m_flags & INVITE_ONLY)
#define LIMIT_PROTECTED (m_flags & LIMIT)

Channel::Channel(
	std::string const& name,
	Client* owner_ptr
	) :
	m_flags(NONE),
	m_name(name),
	m_topic(name),
	m_password(""),
	m_limit(0),
	m_clients(),
	m_operators(),
	m_invite_list()
{
	m_clients.push_back(owner_ptr);
	m_operators.push_back(owner_ptr);
}

Channel::~Channel(void)
{
	// Kick all remaining users
}

void	Channel::setPassword(
			std::string const& password
			)
{
	m_password = password;
}

void	Channel::addClient(
			Client* client_ptr,
			std::string const& password
			)
{
	std::vector<std::string>::iterator	invite_itr;
	std::vector<Client *>::iterator		client_itr;
	bool								limit_check;
	bool								invite_check;
	bool								password_check;
	Command								command;

	if (CONTAINS(m_clients, client_ptr))
	{
		// TODO: Throw error & send message ?
		return ;
	}
	invite_itr = FIND(m_invite_list, client_ptr->getNickname());
	limit_check = !LIMIT_PROTECTED || m_clients.size() < m_limit;
	invite_check = !INVITE_PROTECTED || invite_itr != m_invite_list.end();
	password_check = !PASSWORD_PROTECTED || password == m_password;
	if (!limit_check)
	{
		Replies::ERR::chan_is_full(m_name, client_ptr);
		return ;
	}
	if (!invite_check)
	{
		Replies::ERR::invite_only_chan(m_name, client_ptr);
		return ;
	}
	if (!password_check)
	{
		Replies::ERR::bad_chan_key(m_name, client_ptr);
		return ;
	}
	// ITERATE_CONST(std::vector<Client *>, m_clients, itr)
	// 	Replies::RPL::
	Replies::RPL::nam_reply(m_name, m_clients, client_ptr);
	Replies::RPL::end_of_names(m_name, client_ptr);
	m_clients.push_back(client_ptr);
	if (INVITE_PROTECTED)
		m_invite_list.erase(invite_itr);
	
}

void	Channel::removeClient(
			Client* client_ptr
			)
{
	std::vector<Client *>::iterator		client_itr;
	std::vector<std::string>::iterator	invite_itr;

	// Client
	client_itr = std::find(m_clients.begin(), m_clients.end(), client_ptr);
	if (client_itr != m_clients.end())
	{
		m_clients.erase(client_itr);
		// TODO: Send message to client ?
	}
	else
	{
		// TODO: Throw error
	}
	// Operators
	client_itr = std::find(m_operators.begin(), m_operators.end(), client_ptr);
	if (client_itr != m_operators.end())
		m_operators.erase(client_itr);
	// Invite list
	invite_itr = std::find(m_invite_list.begin(), m_invite_list.end(), client_ptr->getNickname());
	if (invite_itr != m_invite_list.end())
		m_invite_list.erase(invite_itr);
}

void	Channel::invite(
			std::string const& nick
			)
{
	std::vector<Client *>::const_iterator	itr;
	
	for (itr = m_clients.begin(); itr != m_clients.end(); ++itr)
		if ((*itr)->getNickname() == nick)
		{
			// TODO: Throw exception
			return ;
		}
	m_invite_list.push_back(nick);
}

void	Channel::kick(
			std::string const& nick
			)
{
	std::vector<Client *>::iterator	itr;

	for (itr = m_clients.begin(); itr != m_clients.end(); ++itr)
		if ((*itr)->getNickname() == nick)
		{
			// TODO: Send message to client
			m_clients.erase(itr);
			return ;
		}
	// TODO: Throw exception
}

