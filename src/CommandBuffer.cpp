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
	return (Command(raw_command));
}

