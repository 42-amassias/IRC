/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amassias <amassias@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 01:58:54 by amassias          #+#    #+#             */
/*   Updated: 2024/07/11 20:19:56 by amassias         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_H
# define CLIENT_H

# include <map>
# include <string>
# include <sys/socket.h>

# include "CommandBuffer.hpp"

# include "utils.hpp"

class Client
{
	public:
		Client(int fd, struct sockaddr const& address);
		~Client(void);

		std::string const&		getNickname() const;
		std::string const&		getUsername() const;
		std::string const&		getRealname() const;
		struct sockaddr const&	getSockaddr() const;
		bool					isInvisible() const;

		void		setNickname(std::string const& s);
		void		setUsername(std::string const& s);
		void		setRealname(std::string const& s);
		bool		isRegistered();

		void		receive(int fd);
		void		execPendingCommands(void);

		void		sendCommand(Command const& command);

	private:
		Client(void);
		void		execPRIVMSG(Command const& command);
		void		execPASS(Command const& command);
		void		execNICK(Command const& command);
		void		execUSER(Command const& command);
		void		execPING(Command const& command);
		void		execMODE(Command const& command);

	private:
		std::string		m_nickname;
		std::string		m_username;
		std::string		m_realname;
		std::string		m_server_password;
		bool			m_registered;
		bool			m_invisible;
		int				m_fd;
		struct sockaddr	m_addr;
		CommandBuffer	m_buffer;

		static const int	default_read_size = 1024;
		static const std::map<std::string, void (Client::*)(Command const&)>	command_function_map;
		static const std::pair<std::string, void (Client::*)(Command const&)>
			_command_function_map[];

	public:
		
		CREATE_EXCEPTION(ConnectionLost, "Connection lost");
		CREATE_EXCEPTION_MESSAGE(ReadError);
};

#endif

