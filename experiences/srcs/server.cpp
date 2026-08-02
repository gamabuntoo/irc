/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gule-bat <gule-bat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/20 15:24:04 by gule-bat          #+#    #+#             */
/*   Updated: 2026/08/02 17:37:43 by gule-bat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

		// unsigned int				Port;
		// int							SocketFd;
		// int							epollFd;
		//struct sockaddr_in 			address;
		// static bool 					signal;
		// std::vector <Client> 		clients;
		// std::vector <epoll_event> 	ev;
		// unsigned int				maxEv;
		
volatile sig_atomic_t gsign_ = 0;

		
Server::Server()
{
	maxEv = MAX_EVENT;
}


Server::Server(const Server &server)
{
	this->Port = server.Port;
	this->SocketFd = server.SocketFd;
	this->epollFd = server.epollFd;
	this->signal = server.signal;
	this->address = server.address;
	this->clients = server.clients;
	this->ev = server.ev;
	this->maxEv = server.maxEv;
}

void	Server::Clean_exit()
{
	

}

void Server::signalHandler(int signum)
{
	std::cout << "Signal received: " << signum <<", exiting..." << std::endl;
	// std::exit(0);
	gsign_ = signum;
}

void	Server::signalInit()
{
	std::signal(SIGINT, signalHandler);
}

void	Server::set_sock_non_blocking(int fd)
{
	int x = -1;
	int w = -1;
	w = fcntl(fd, F_GETFL);
	if (w < 0)
		throw std::runtime_error("set_sock_non_blocking, fcntl 1rst call error with fd");
	x = fcntl(fd, F_SETFL, w | O_NONBLOCK);
	if (x < 0)
		throw std::runtime_error("set_sock_non_blocking, fcntl 2nd call error with fd");
	
}

void	Server::epoll_init()
{
	epollFd = epoll_create1(0);
	if (epollFd < 0)
		throw std::runtime_error("Error: Server::EpollInit; epollcreate1() error");

	struct epoll_event event;
	std::memset(&event, 0, sizeof(event));
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = SocketFd;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, SocketFd, &event) == -1)
		throw std::runtime_error("Error: Server::EpollInit; Epollctl() error");
	ev.push_back(event);
}

void	Server::AddClientToStruct(struct sockaddr_in cli, int cli_fd)
{
	char st[INET_ADDRSTRLEN];
	const char *s = inet_ntop(AF_INET, &cli.sin_addr, st, sizeof(st));
	clients.push_back(Client(cli_fd, s));
}

void	Server::AddNewClient(epoll_event event)
{
	while (1)
	{
		struct sockaddr_in cli_addr;
		socklen_t cli_len = sizeof(cli_addr);
		int cli_fd = accept(SocketFd, (struct sockaddr *)&cli_addr, &cli_len);
		if (cli_fd < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK) // content red
				break ;
			else
				throw std::runtime_error("Error: Server::Addnewclient(); client fd error"), close(cli_fd);
		}
		set_sock_non_blocking(cli_fd);
		event.events = EPOLLIN | EPOLLET;
		event.data.fd = cli_fd;
		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, cli_fd, &event) == -1)
			throw std::runtime_error("Error: Server::AddNewClient; Epoll_ctl error"), close(cli_fd);
		AddClientToStruct(cli_addr, cli_fd);
		std::cout << GREEN << "client paquet received from:" << clients.back().getIp() 
		<< " and from fd: " << clients.back().getFd() << RESET << "\n" << std::endl;
	}
}

std::string		Server::getIpFromFd(int cli_fd)
{
	for (long unsigned int x = 0; x < clients.size(); x++)
	{
		if (cli_fd == clients.at(x).getFd())
			return (clients.at(x).getIp());
	}
	return ("0.0.0.0 ip error ");
}

void	Server::receiveData(epoll_event event)
{
	char bf[1024];
	ssize_t ct = -1;
	int cli_fd = event.data.fd;
	memset(bf, 1024, sizeof(bf));
	std::cout << GREEN << "From: " << MAGENTA << "FD n°" << cli_fd << RESET << " "<< getIpFromFd(cli_fd) << "\n\t" << std::endl;
	std::string buff;
	while ((ct = recv(cli_fd, bf, sizeof(bf), 0)) > 0)
	{
		write(STDOUT_FILENO, bf, ct);
		buff.append(bf, ct);
		send(cli_fd, "from server:\n", 14, MSG_NOSIGNAL); // echo back text
		size_t s = send(cli_fd, buff.c_str(), buff.size(), MSG_NOSIGNAL);
		if (s == 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			continue ;
			else
			{
				close(cli_fd);
				return ;
			}
		}
	}
	if (ct == 0)
	{
		std::cout << RED << "client disconnected" << RESET <<  std::endl;
		close(cli_fd);
	}
	else if (ct == -1) 
	{
		if (errno != EAGAIN) 
		{
			throw std::runtime_error("Error: ListeningLoop; recv err");
			close(cli_fd);
		}
	}
}

void	Server::ListeningLoop()
{
	if (bind(SocketFd, (struct sockaddr*)&address, sizeof(address)) == -1)
		throw std::runtime_error("Error: Server::ListeningLoop; binding error ( bind() )");
	if (listen(SocketFd, SOMAXCONN) == -1)
		throw std::runtime_error("Error: Server::ListeningLoop; listen error");
	epoll_init();
	
	
	epoll_event event[maxEv];
	while (1 || gsign_)
	{
		std::cout << GREEN << "-----------" << RESET << std::endl;
		int ct_ev = epoll_wait(epollFd, event, maxEv, -1);
		if (ct_ev == -1)
		{
			std::cerr << "epoll_wait . . . ." << std::endl;
			break;
		}
		for (int x = 0; x < ct_ev; x++)
		{
			
			if (event[x].data.fd == SocketFd)
				AddNewClient(event[x]);
			else
				receiveData(event[x]);
		}
	}
	std::cout << RED << "shutting down server" << RESET << std::endl;
}


int	Server::Setup_server()
{
	SocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (SocketFd < 0)
		throw std::runtime_error("Error: socket fd init error");
	address.sin_family = AF_INET;
	// address.sin_addr.s_addr = inet_addr(IP_ADDR);
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(Port); //htons(PORT) sinon
	int t = 1;
	if (setsockopt(SocketFd, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(int)) < 0) //suspicieux
		throw std::runtime_error("Error: setsockopt issue at Server::setup_server()");
	set_sock_non_blocking(SocketFd);

	char st[INET_ADDRSTRLEN];
	const char *s = inet_ntop(AF_INET, &address.sin_addr, st, sizeof(st));
	
	std::cout << YELLOW << "Server addr: " << s << std::endl;
	
	ListeningLoop();
	return (1);
}

Server::Server(int port, std::string password)
{
	signalInit();
	SocketFd = -1;
	epollFd = -1;
	maxEv = MAX_EVENT;
	(void)password; //temporaire
	ev = std::vector<epoll_event>(maxEv);
	Port = port;
	if (port < 0)
		throw std::runtime_error("Error port, please put a valid port ");
	std::memset(&address, 0, sizeof(address));
	Setup_server();
}


Server::~Server()
{
	close(SocketFd);
	if (this->epollFd != -1 && this->clients.size() != 0)
		epoll_ctl(epollFd, EPOLL_CTL_DEL, this->clients.front().getFd(), &(ev.front()));
	if (this->clients.size() > 0)
	{
		for (long unsigned int x = 0; x < this->clients.size(); x++)
		{
			close(clients.at(x).getFd());
		} // closing remaining clients sockets

	}
	close(epollFd);
}