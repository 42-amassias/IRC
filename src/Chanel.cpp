/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chanel.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amassias <amassias@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 16:09:23 by amassias          #+#    #+#             */
/*   Updated: 2024/06/06 18:49:53 by amassias         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Chanel.hpp"

#include <algorithm>

Chanel::Chanel(std::string const& name, Client* owner_ptr) :
	m_flags(flags::NONE),
	m_topic(name),
	m_password(""),
	m_limit(0),
	m_client(),
	m_operators(),
	m_invite_list()
{
	m_client.push_back(owner_ptr);
	m_operators.push_back(owner_ptr);
}

Chanel::~Chanel(void)
{
	// Send message to all clients thast the chanel is no longer
}

void	Chanel::addClient(Client* client_ptr, std::string const& password)
{
	std::vector<std::string>::iterator	itr;

	if (m_flags & flags::PASSWORD)
	{
		if (password != m_password)
			throw std::exception();
	}
	if (m_flags & flags::INVITE_ONLY)
	{
		itr = std::find(m_invite_list.begin(), m_invite_list.end(), client_ptr->getNickname());
		if (itr == m_invite_list.end())
			throw std::exception();
		m_invite_list.erase(itr);
	}
	m_client.push_back(client_ptr);
}

void	Chanel::removeClient(Client* client_ptr)
{
	std::vector<Client *>::iterator	itr;

	itr = std::find(m_client.begin(), m_client.end(), client_ptr);
	if (itr != m_client.end())
		m_client.erase(itr);
	itr = std::find(m_operators.begin(), m_operators.end(), client_ptr);
	if (itr != m_operators.end())
		m_operators.erase(itr);
}
