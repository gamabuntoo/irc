/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gule-bat <gule-bat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 20:31:49 by gule-bat          #+#    #+#             */
/*   Updated: 2026/08/29 19:48:17 by gule-bat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

Channel::Channel()
{
	this->name = "";
	this->invite_only_f = 0;
	this->topic_only_op_f = 1;
	this->channel_password_f = 0;
	this->user_limit_f = 0;
	this->channel_passwd = "";
	this->user_limit_s = 0;
	this->topic = "";
}

Channel::Channel(std::string name, Client &creator, std::string topic, int invite_only_f, int topic_only_op_f, int channel_password_f, int user_limit_f, std::string channel_passwd, unsigned int user_limit_s)
{
	this->name = name;	
	nicks.push_back(creator.getNick());
	operators.push_back(creator.getNick());
	this->topic = topic;
	this->invite_only_f = invite_only_f;
	this->topic_only_op_f = topic_only_op_f;
	this->channel_password_f = channel_password_f;
	this->user_limit_f = user_limit_f;
	if (channel_passwd != "" && channel_password_f == 1)
		this->channel_passwd = channel_passwd;
	else
		this->channel_passwd = "";
	if (user_limit_f == 1)
		this->user_limit_s = user_limit_s;
	else
		this->user_limit_s = 0;
	std::cout << MAGENTA << "Channel created : #" << this->name << RESET << "by user :" << creator.getNick() << std::endl;
}

Channel::Channel(const Channel &src)
{
	this->name = src.name;
	nicks = src.nicks;
	operators = src.operators;
	this->topic = src.topic;
	this->invite_only_f = src.invite_only_f;
	this->topic_only_op_f = src.topic_only_op_f;
	this->channel_password_f = src.channel_password_f;
	this->user_limit_f = src.user_limit_f;
	this->channel_passwd = src.channel_passwd;
	this->user_limit_s = src.user_limit_s;
	std::cout << MAGENTA << "Channel copied : #" << this->name << RESET << std::endl;
}


Channel &Channel::operator=(const Channel &src)
{
	if (this == &src)
		return (*this);
	this->name = src.name;	
	nicks = src.nicks;
	operators = src.operators;
	this->topic = src.topic;
	this->invite_only_f = src.invite_only_f;
	this->topic_only_op_f = src.topic_only_op_f;
	this->channel_password_f = src.channel_password_f;
	this->user_limit_f = src.user_limit_f;
	this->channel_passwd = src.channel_passwd;
	this->user_limit_s = src.user_limit_s;
	std::cout << MAGENTA << "Channel constructor copied : #" << this->name << RESET << std::endl;
	return (*this);
}

Channel::~Channel()
{
	std::cout << YELLOW <<"Channel #" << name << " destroyed" << RESET << std::endl;
}

std::string	Channel::getName()
{
	return (this->name);
}

long unsigned int	Channel::getSize()
{
	return (this->nicks.size());
}

int		Channel::addUser(std::string nick)
{
	for (long unsigned int x = 0; x < nicks.size(); x++)
	{
		if (nick == nicks[x])
			return (std::cout << BOLDRED << "Username " << nick << "already present in channel #" << name << RESET << std::endl ,-1);
	}
	this->nicks.push_back(nick);
	std::cout << GREEN << "Channel:: User " << nick << " joined #" << this->name << RESET << std::endl; 
	return (1);
}

int		Channel::isUserPresent(std::string nick)
{
	for (long unsigned int x = 0; x < nicks.size(); x++)
	{
		if (nicks[x] == nick)
			return (x);
	}
	return (-1);
}

void		Channel::removeUser(std::string nick)
{
	long int i = 0;
	i = isUserPresent(nick);
	for (long unsigned int x = 0; x < operators.size(); x++)
	{
		if (nick == operators[x])
			operators[x].erase();
	}
	if (i >= 0)
		nicks.erase(nicks.begin() + i);
	std::cout << RED << "User :" << WHITE << nick << RED << "disconnected from #" << RESET << name << "| users connected right now: " << nicks.size() << std::endl;
}

void	Channel::setTopic(std::string topic)
{
	this->topic = topic;
}

bool		Channel::isOperator(std::string nick)
{
	std::vector<std::string>::iterator i = std::find(operators.begin(), operators.end(), nick);
	if (i != operators.end())
		return (true);
	return (false);
}

std::string	Channel::getTopic()
{
	return (this->topic);
}

bool		Channel::isOpOnly()
{
	return (this->topic_only_op_f);
}

void	Channel::addChanOperator(std::string nick)
{
	if (isOperator(nick) == true)
	{
		std::cout << RED << " Error can't add " << nick << " to operators: already set" << RESET << std::endl;
		return ;
	}
	else
	{
		operators.push_back(nick);
		std::cout << "User " << nick << " set as operator" << std::endl;
	}
}

int	Channel::getOperatorSize()
{
	return (operators.size());
}