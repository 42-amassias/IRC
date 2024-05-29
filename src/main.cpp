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

#include <iostream>

int	main(void)
{
	Log::Debug << "Starting server..." << 5432543 << std::endl;
	Log::Info << "Starting server..." << std::endl;
	Log::Warn << "Starting server..." << std::endl;
	Log::Error << "Starting server..." << std::endl;
	Server::getInstance()->loop();
	Server::destroyInstance();
	return (0);
}
