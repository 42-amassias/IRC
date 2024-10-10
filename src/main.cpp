/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ale-boud <ale-boud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 01:58:48 by amassias          #+#    #+#             */
/*   Updated: 2024/10/10 04:02:18 by ale-boud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Log.hpp"

#include <cstdlib>
#include <cstring>

#define PORT_MAX 65535

enum ProgramArgs
{
	PROG_PN,
	PROG_PORT,
	PROG_PWD,
	PROG__NBARGS,
};

static const char*	pn;

// Prints the program usage.
static void	usage(void);

// Returns `true` on error.
static bool	check_arg_count(int argc, const char **argv);

// Returns `true` on error.
static bool	parse_port(const char* str_port, int& port);

// Returns `true` on error.
static bool	check_password(const char* str_password);

int	main(int argc, char **argv)
{
	int	port;

	pn = argv[PROG_PN];
	if (check_arg_count(argc, (const char **)argv)
		|| parse_port(argv[PROG_PORT], port)
		|| check_password(argv[PROG_PWD]))
	{
		usage();
		return (EXIT_FAILURE);
	}
	try
	{
		Server::getInstance().setPwd(argv[PROG_PWD]);
		Server::getInstance().setPort(port);
		Server::getInstance().init();
		Server::getInstance().loop();
	} catch (const std::exception &e)
	{
		Log::Error << "Server encountered a fatal error : " << e.what() << std::endl;
	}
	Server::destroyInstance();
	return (0);
}

static void	usage(void)
{
	std::cerr << "usage: " << pn << " [PORT] [PASSWORD]" << std::endl;
}

static bool	check_arg_count(int argc, const char **argv)
{
	if (argc == 3)
		return (false);
	std::cerr << argv[PROG_PN] << ": Invalid number of arguments." << std::endl;
	return (true);
}

static bool	parse_port(const char* str_port, int& port)
{
	port = 0;
	for (const char *str = str_port; *str != '\0'; ++str)
	{
		port = 10 * port + *str - '0';
		if (!std::isdigit(*str) || port > PORT_MAX)
		{
			std::cerr << pn << ": Invalid port number." << std::endl;
			return (true);
		}
	}
	return (false);
}

static bool	check_password(const char* str_password)
{
	for (const char* itr = str_password; *itr; ++itr)
		if (*itr == '\0' || *itr == '\n' || *itr == '\r' || *itr == ':')
		{
			std::cerr << pn << ": Invalid password." << std::endl;
			return (true);
		}
	return (false);
}
