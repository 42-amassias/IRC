/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.ipp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ale-boud <ale-boud@student.42lehavre.fr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:11:12 by ale-boud          #+#    #+#             */
/*   Updated: 2024/06/06 15:11:14 by ale-boud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

template<typename T>
std::ostream	&Log::LogStream::operator<<(T _in)
{
	return ((*this->m_out) << "[" << this->m_lvlcolor << this->m_lvlname << COLOR_SHELL_DEFAULT << "]"
		<< "[" << Log::formattedDateTime() << "] "
		<< _in);
}

