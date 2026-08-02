/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gule-bat <gule-bat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/20 15:24:08 by gule-bat          #+#    #+#             */
/*   Updated: 2026/08/02 13:25:07 by gule-bat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

Client::Client() : Fd(-1), ip_addr("")
{
}

Client::Client(const Client &src)
{
	if (src.Fd == -1 && src.ip_addr == "")
		std::cerr << MAGENTA <<"Client warning: Copy constructor: You copied an empty client" << RESET << std::endl;
	this->Fd = src.Fd;
	this->ip_addr = src.ip_addr;
}
Client::Client(int fd, std::string ipaddr)
{
	if (fd < 0/* || ipaddr not well formated*/)
		throw std::runtime_error("\033[31mClient error: Constructor: values negative or invalid\033[0m");
	this->Fd = fd;
	this->ip_addr = ipaddr;
}

Client::~Client()
{
	// close fd ??
	std::cout << YELLOW <<"Client destroyed" << RESET << std::endl;
}
		
int		Client::getFd()
{
	return (this->Fd);
}

void	Client::setFd(int fd)
{
	if (fd < 0)
		std::cerr << MAGENTA <<"Client warning: setFd(): be careful, negative fd set" << RESET << std::endl;
	this->Fd = fd;
}
void	Client::setIp(std::string s)
{
	if (s == "" /*|| invalidIpFormat(s) */)
		std::cerr << MAGENTA <<"Client warning: setIp(): be careful, empty or invalid ip set" << RESET << std::endl;
	this->ip_addr = s;
}

std::string	Client::getIp()
{
	return (this->ip_addr);
}