#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP

# define CREATE_GENERIC_EXCEPTION(name, message)	\
	class name##Exception :							\
		public std::exception						\
	{												\
		const char*	what() const throw()			\
		{											\
			return (#message);						\
		}											\
	}												\

#endif
