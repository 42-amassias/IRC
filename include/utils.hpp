/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amassias <amassias@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 01:58:36 by amassias          #+#    #+#             */
/*   Updated: 2024/05/28 02:35:59 by amassias         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# define ITERATE(type, iterable, itr)										\
	for (																	\
		type::iterator itr = (iterable).begin();							\
		itr != (iterable).end();											\
		++itr																\
	)

# define CREATE_EXCEPTION(name, message)									\
	class name##Exception : 												\
		public std::exception												\
	{																		\
		public:																\
			const char*	what(void) const throw()							\
			{																\
				return (message);											\
			}																\
	}

# define CREATE_EXCEPTION_MESSAGE(name)										\
	class name##Exception : 												\
		public std::exception												\
	{																		\
		public:																\
			name##Exception(std::string const& message) :					\
				exception(),												\
				m_message(message)											\
			{}																\
																			\
			virtual ~name##Exception(void) throw()							\
			{}																\
																			\
			const char*	what(void) const throw()							\
			{																\
				return (m_message.c_str());									\
			}																\
																			\
		private:															\
			std::string const	m_message;									\
	}

#endif

