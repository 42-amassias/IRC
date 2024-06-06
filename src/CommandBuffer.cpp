/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandBuffer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ale-boud <ale-boud@student.42lehavre.fr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:10:48 by ale-boud          #+#    #+#             */
/*   Updated: 2024/06/06 15:42:46 by ale-boud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>

#include "CommandBuffer.hpp"

CommandBuffer::CommandBuffer() :
	m_buffer()
{}

CommandBuffer::~CommandBuffer() {}

void	CommandBuffer::pushBack(const char *data, size_t len)
{
	m_buffer.insert(m_buffer.end(), data, data+len);
}

Command	CommandBuffer::popFront(void)
{
	std::vector<char>::iterator	nl_it = std::find(m_buffer.begin(), m_buffer.end(), '\n');

	if (nl_it == m_buffer.end())
		throw NoPendingCommandException();
	std::vector<char>	raw_command(m_buffer.begin(), nl_it);
	m_buffer.erase(m_buffer.begin(), nl_it + 1);
	if (raw_command.size() == 0 || raw_command.back() != '\r')
		throw Command::InvalidCommandException("No CR in command");
	raw_command.pop_back();
	return (Command(raw_command));
}

