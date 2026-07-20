/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gule-bat <gule-bat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/20 15:24:13 by gule-bat          #+#    #+#             */
/*   Updated: 2026/07/20 21:30:22 by gule-bat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <csignal>
#include <sys/epoll.h>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

class Client
{
	private:
		int Fd;
		std::string ip_addr;
	public:
		Client();
		Client(const Client &src);
		Client(int fd, std::string ipaddr);
		~Client();
		
		int		getFd();
		void	setFd(int fd);
		void	setIp(std::string s);
};

class Server
{
	private:
		unsigned int				Port;
		int							SocketId;
		int							epollFd;
		static bool 				signal;
		std::vector <Client> 		clients;
		std::vector<epoll_event> 	ev;
		unsigned int				maxEv;
	public:
		Server();
		Server(const Server &server);
		Server(unsigned int port, int Socket_id, unsigned int max_ev);
		
};