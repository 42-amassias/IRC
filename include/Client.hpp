/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Antoine Massias <massias.antoine.pro@gm    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 01:58:54 by amassias          #+#    #+#             */
/*   Updated: 2024/06/06 08:03:21 by Antoine Mas      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_H
# define CLIENT_H

# include <string>
# include <sys/socket.h>

# include "CommandBuffer.hpp"

# include "utils.hpp"

class Client
{
	public:
		Client(struct sockaddr const& address);
		~Client(void);

		std::string const&	getNickname() const;
		std::string const&	getUsername() const;
		std::string const&	getRealname() const;
		struct sockaddr const&	getSockaddr() const;

		void		setNickname(std::string const& s);
		void		setUsername(std::string const& s);
		void		setRealname(std::string const& s);

		void		receive(int fd);
		void		execPendingCommands(void);

	private:
		Client(void);

	private:
		std::string		m_nickname;
		std::string		m_username;
		std::string		m_realname;
		bool			m_logged;
		struct sockaddr	m_addr;
		CommandBuffer	m_buffer;

		static const int	default_read_size = 1024;

	public:
		
		CREATE_EXCEPTION(ConnectionLost, "Connection lost");
		CREATE_EXCEPTION_MESSAGE(ReadError);
};

#endif

