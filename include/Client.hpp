/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ale-boud <ale-boud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 01:58:54 by amassias          #+#    #+#             */
/*   Updated: 2024/10/10 13:46:18 by ale-boud         ###   ########.fr       */
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

		std::string const&	getNickname() const;
		std::string const&	getUsername() const;
		std::string const&	getRealname() const;
		struct sockaddr const&	getSockaddr() const;
		std::string			getPrefix() const;

		void		setNickname(std::string const& s);
		void		setUsername(std::string const& s);
		void		setRealname(std::string const& s);
		bool		isRegistered();
		bool		isOperator();

		void		receive(int fd);
		void		execPendingCommands(void);

		void		sendCommand(Command const& command);
		void		sendPendingCommand();

		enum client_state {
			LOGIN,
			RETRY,
			REGISTERED,
			QUIT,
		};

	private:
		Client(void);

		void		welcome();

		void		execPASS(Command const& command);
		void		execNICK(Command const& command);
		void		execUSER(Command const& command);
		void		execPING(Command const& command);

		void		execPRIVMSG(Command const& command);
		void		execNOTICE(Command const& command);
		void		execOPER(Command const& command);
		void		execJOIN(Command const& command);
		void		execQUIT(Command const& command);
		void		execTOPIC(Command const& command);
		void		execMODE(Command const& command);
		void		execINVITE(Command const& command);
		void		execPART(Command const& command);
		void		execKICK(Command const& command);

	private:
		std::string		m_nickname;
		std::string		m_username;
		std::string		m_realname;
		std::string		m_userpwd;
		client_state	m_state;
		int				m_fd;
		bool			m_isoperator;
		struct sockaddr	m_addr;
		CommandBuffer	m_read_buffer;
		std::vector<char>	m_write_buffer;

		static const int	default_read_size = 1024;
		static const std::map<std::string, void (Client::*)(Command const&)>	command_function_map;
		static const std::map<std::string, void (Client::*)(Command const&)>	logged_command_function_map;
		static const std::pair<std::string, void (Client::*)(Command const&)>
			_command_function_map[];
		static const std::pair<std::string, void (Client::*)(Command const&)>
			_logged_command_function_map[];

		static const std::string	oper_username;
		static const std::string	oper_password;

	public:
		
		CREATE_EXCEPTION(ConnectionLost, "Connection lost");
		CREATE_EXCEPTION_MESSAGE(ReadError);
		CREATE_EXCEPTION_MESSAGE(WriteError);
		CREATE_EXCEPTION(BadPassword, "Bad password");
		CREATE_EXCEPTION(QuitMessage, "QUIT message received");
};

#endif

