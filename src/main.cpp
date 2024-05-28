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
#include <vector>

int	main(void)
{
	Server::getInstance()->loop();
	Server::destroyInstance();
	return (0);
}
