template<typename T>
std::ostream	&Log::LogStream::operator<<(T _in)
{
	return ((*this->m_out) << "[" << this->m_lvlcolor << this->m_lvlname << COLOR_SHELL_DEFAULT << "]"
		<< "[" << Log::formattedDateTime() << "] "
		<< _in);
}

