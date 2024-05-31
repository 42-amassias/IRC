/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amassias <amassias@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 01:58:48 by amassias          #+#    #+#             */
/*   Updated: 2024/05/28 03:33:55 by amassias         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Log.hpp"

#include <cstring>

int	main(void)
{
	try
	{
		Server::getInstance()->loop();
		Server::destroyInstance();
	} catch (std::exception &e)
	{
		Log::Error << "Server encountered a fatal error : " << e.what() << std::endl;

	}
	return (0);
}
