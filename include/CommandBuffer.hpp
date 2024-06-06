/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandBuffer.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ale-boud <ale-boud@student.42lehavre.fr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:11:06 by ale-boud          #+#    #+#             */
/*   Updated: 2024/06/06 15:11:07 by ale-boud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef  COMMANDBUFFER_H
# define COMMANDBUFFER_H

# include <cstddef>
# include <vector>

# include "Command.hpp"

# include "utils.hpp"

class CommandBuffer
{
	public:
		CommandBuffer();
		~CommandBuffer();

		void	pushBack(const char *data, size_t len);
		Command	popFront(void);

	private:
		std::vector<char>	m_buffer;

	public:
		CREATE_EXCEPTION(NoPendingCommand, "There is no pending command in the buffer");

};

#endif

