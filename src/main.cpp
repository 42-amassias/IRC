#include "server/Server.hpp"

#include <cstdlib>

int	main(void)
{
	Server server = Server(Server::Config(6969, "123soleil"));

	server.run();
	return (EXIT_SUCCESS);
}
