#include "server/Chanel.hpp"

Chanel::Config::Config(std::string const& topic) :
	m_topic(topic),
	m_invite_only(CHANEL_CONFIG_DEFAULT_INVITE_ONLY),
	m_topic_restriction(CHANEL_CONFIG_DEFAULT_TOPIC_RESTRICTION),
	m_password(CHANEL_CONFIG_DEFAULT_PASSSWORD),
	m_limit(CHANEL_CONFIG_DEFAULT_LIMIT)
{
}

Chanel::Config::Config(Config const& o)
{
	*this = o;
}

Chanel::Config::~Config(void)
{
}

Chanel::Config	&Chanel::Config::operator=(Chanel::Config const& o)
{
	if (this == &o)
		return (*this);
	m_topic = o.m_topic;
	m_invite_only = o.m_invite_only;
	m_topic_restriction = o.m_topic_restriction;
	m_password = o.m_password;
	m_limit = o.m_limit;
	return (*this);
}

void	Chanel::Config::setTopic(std::string const& topic)
{
	m_topic = topic;
}

void	Chanel::Config::setInviteOnly(bool flag)
{
	m_invite_only = flag;
}
void	Chanel::Config::setTopicRestriction(bool flag)
{
	m_topic_restriction = flag;
}
void	Chanel::Config::setPassword(std::string const& password)
{
	m_password = password;
}
void	Chanel::Config::setLimit(size_t limit)
{
	m_limit = limit;
}

std::string const&	Chanel::Config::getTopic(void) const
{
	return (m_topic);
}

bool	Chanel::Config::isInviteOnly(void) const
{
	return (m_invite_only);
}

bool	Chanel::Config::hasTopicRestriction(void) const
{
	return (m_topic_restriction);
}

std::string const&	Chanel::Config::getPassword(void) const
{
	return (m_password);
}

size_t				Chanel::Config::getLimit(void) const
{
	return (m_limit);
}
