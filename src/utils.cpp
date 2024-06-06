#include "utils.hpp"


std::string	ipv4FromSockaddr(struct sockaddr const &addr)
{
	char	buf[INET_ADDRSTRLEN];

	inet_ntop(addr.sa_family, &((struct sockaddr_in *)&addr)->sin_addr, buf, INET_ADDRSTRLEN);
	return (std::string(buf));
}

std::string	ipv4FromSockaddr(struct sockaddr_in const &addr)
{
	char	buf[INET_ADDRSTRLEN];

	inet_ntop(addr.sin_family, &addr.sin_addr, buf, INET_ADDRSTRLEN);
	return (std::string(buf));

}

