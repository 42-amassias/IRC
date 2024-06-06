#ifndef  COMMAND_HPP
# define COMMAND_HPP

# include <string>
# include <vector>

class Command
{
	public:
		Command();
		Command(std::vector<char> const& raw_command);
		~Command();

	private:
		std::string					m_prefix;
		std::string					m_command;
		std::vector<std::string>	m_parameters;
};

#endif

