/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gule-bat <gule-bat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/20 15:24:04 by gule-bat          #+#    #+#             */
/*   Updated: 2026/08/21 02:02:53 by gule-bat         ###   ########.fr       */
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
	w = fcntl(fd, F_GETFL); // get status of file descriptor 
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
	if (!s)
		throw std::runtime_error("Error while adding new client");
	clients.push_back(Client(cli_fd, std::string(s)));
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
		std::cout << GREEN << "new client connected at:" << clients.back().getIp() 
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


		// // send(cli_fd, "from server:\n", 14, MSG_NOSIGNAL); // echo back text
		// for (long unsigned int x = 0; x < this->clients.size(); x++) // echo to everyone connected
		// {
		// 	std::cout << this->clients.size() << " " << x << std::endl;
			
		// 	std::string st("from server: ");
		// 	st.append("from ip: ");
		// 	st.append(getIpFromFd(clients.at(x).getFd()));
		// 	st.append(" : "); st.append(buff); // COUILLE COTE SERVEUR SUR LA STRING
		// 	std::cout<<"sent to client:\n" << RED << st << "\n" << RESET << std::endl;
		// 	size_t s = send(clients.at(x).getFd(), st.c_str(), sizeof(char) * st.size()+1, MSG_NOSIGNAL); // echo back text
		// 	// size_t s = send(clients.at(x).getFd(), bf, std::strlen(bf), MSG_NOSIGNAL | MSG_DONTWAIT); // dont wait pas sur
		// 	if (s == 0)
		// 	{
		// 		if (errno == EAGAIN || errno == EWOULDBLOCK)			// BLOCK TO SEND TO CLIENTS BUT SUSPICIOUS 
		// 			break;
		// 		else
		// 		{
		// 			close(cli_fd);
		// 			return ;
		// 		}
		// 	}
		// 	// st =  "";
		// } // closing remaining clients sockets

void	Server::forwardData(epoll_event event, std::string buffer)
{
	std::string buff;
	int cli_fd = event.data.fd;

	for (long unsigned int x = 0; x < this->clients.size(); x++) // echo to everyone connected
	{
		std::cout << "size = " << this->clients.size() << " index= " << x << std::endl;
		if (cli_fd != clients.at(x).getFd())
		{
			// sendMessage(cli_fd, buffer);
			// sendMessage(cli_fd, "\n");
			std::string st;
			st.append("from server: ");
			st.append("from ip: ");
			st.append(getIpFromFd(clients.at(x).getFd()));
			st.append(" : "); st.append(buffer); // COUILLE COTE SERVEUR SUR LA STRING
			// std::cout<<"sent to client:\n" << RED << st << "\n" << RESET << std::endl;
			size_t s = send(clients.at(x).getFd(), st.c_str(), sizeof(char) * st.size(), MSG_NOSIGNAL); // echo back text
			// size_t s = send(clients.at(x).getFd(), bf, std::strlen(bf), MSG_NOSIGNAL | MSG_DONTWAIT); // dont wait pas sur
			if (s == 0)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					break;
				else
				{
					close(clients.at(x).getFd());
					// close(cli_fd);
					return ;
				}
			}
		}
		// st =  "";
	}
}

void	Server::sendMessage(int fd, std::string buffer)
{
	std::string buff;
	int cli_fd = fd;

	if (cli_fd == clients.at(getClientIdFromFd(fd)).getFd())
	{
		std::string st;
		// st.append(GREEN);
		st.append("from server: "); 
		st.append(getIpFromFd(clients.at(getClientIdFromFd(fd)).getFd()));
		st.append(" : ");
		// st.append(RESET);
		st.append(buffer); // COUILLE COTE SERVEUR SUR LA STRING
		// st.append("\n");
		// std::cout<<"sent to client:\n" << RED << st << "\n" << RESET << std::endl;
		size_t s = send(fd, st.c_str(), sizeof(char) * st.size()+1, MSG_NOSIGNAL); // echo back text
		// size_t s = send(clients.at(x).getFd(), bf, std::strlen(bf), MSG_NOSIGNAL | MSG_DONTWAIT); // dont wait pas sur
		if (s == 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return ;
			else
			{
				close(cli_fd);
				return ;
			}
		}
	}
}

void	Server::sendNeutralMessage(int fd, std::string buffer)
{
	std::string buff;
	int cli_fd = fd;

	if (cli_fd == clients.at(getClientIdFromFd(fd)).getFd())
	{
		std::string st;
		st.append(buffer); // COUILLE COTE SERVEUR SUR LA STRING
		size_t s = send(fd, st.c_str(), sizeof(char) * st.size()+1, MSG_NOSIGNAL); // echo back text
		if (s == 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return ;
			else
			{
				close(cli_fd);
				return ;
			}
		}
	}
}

void	Server::deleteUser(int cli_fd)
{
	for (long unsigned int x = 0; x < clients.size(); x++)
	{
		if (clients.at(x).getFd() == cli_fd)
		{
			clients.erase(clients.begin() + x);
			return ;
		}
	}
}

std::string	Server::receiveData(epoll_event event)
{
	char bf[1024];
	ssize_t ct = -1;
	std::string buff;
	int cli_fd = event.data.fd;
	memset(bf, 0, sizeof(bf));
	while ((ct = recv(cli_fd, bf, sizeof(bf), 0)) > 0)
	{
		if (ct > 0)
		{
			buff.append(bf, ct);
			bf[ct+1] = '\0';
		}
	}
	if (ct)
	{
		std::cout << MAGENTA << "\nFD n°" << cli_fd << YELLOW << " "<< getIpFromFd(cli_fd) << RESET << ": \n" << buff << std::endl;
		return (bf);
	}
	else if (ct == 0)
	{
		std::cout << RED << "client from fd n°" << cli_fd << " disconnected" << RESET <<  std::endl;
		close(cli_fd);
		deleteUser(cli_fd);
	}
	else if (ct == -1)
	{
		if (errno != EAGAIN) 
		{
			throw std::runtime_error("Error: ListeningLoop; recv err");
			close(cli_fd);
			deleteUser(cli_fd);
		}
	}
	if (buff == "\n")
		return ("newline");
	return ("empty");
}

int		Server::getClientIdFromFd(int fd)
{
	if (fd < 0)
	{
		throw std::runtime_error("Error while reading cliend fd to get his id");
		return (0);
	}
	for (long unsigned int x = 0; x < clients.size(); x++)
	{
		if (clients.at(x).getFd() == fd)
		{
			return (x);
		}
	}
	return (-1);
}

void	Server::setClientStatusId(std::stringstream &s, int fd)
{
	std::string user, nick;
	std::string line;
	std::getline(s, line, '\r');	
	nick = line.substr(line.find(" ")+1, line.size() - 6);
	
	std::getline(s, line, '\r');	
	std::stringstream b;	b << line;	std::getline(b, line, ':'); std::getline(b, line, '\r');
	user = line;
	
	clients[getClientIdFromFd(fd)].setLogged();
	clients[getClientIdFromFd(fd)].setIdentity(nick, user);	
}

int	Server::check_connection(std::string buffer, int fd)
{	
	if (clients[getClientIdFromFd(fd)].getLoggedStatus() == 1)
		return (0);
	std::stringstream 	s;
	std::string 		line;
	s << buffer;
	std::getline(s, line, '\n');
	if (line != "CAP LS \r\n")
	{
		sendNeutralMessage(fd, "CAP * LS :multi-prefix account-notify\r\n");		
		std::cout << GREEN << "Client handshake: Sending CAP *  LS to client\n\n" << RESET << std::endl;
	}
	std::getline(s, line, '\n');
	std::string fullstc("PASS " + passwd);
	fullstc.append("\r");
	if (fullstc == line)
	{
		std::cout << GREEN << "User n°" << fd << " well logged!" << RESET << std::endl;
		setClientStatusId(s, fd);
		std::cout << YELLOW << "User:\n" << RESET << clients[getClientIdFromFd(fd)].getUser() << YELLOW << "\nNick:\n" << RESET << clients[getClientIdFromFd(fd)].getNick()
		<< YELLOW << "\nIp:\n" << RESET << clients[getClientIdFromFd(fd)].getIp() << std::endl;
	}
	else 
	{
		std::cout << RED << "Error Password from client " << fd << RESET << std::endl;
		sendMessage(fd, "log error, please retry with another password");
		return (-1);
	}
	return (0);
}


// int	Server::check_connection(std::string buffer, int fd)
// {
// 	// static int is_ok = 3;
// 	// int size = std::strlen("PASS") + passwd.size();
	
// 	std::string fullstc("PASS " + passwd);
// 	if (clients[getClientIdFromFd(fd)].getLoggedStatus() == 1)
// 		return (0);
// 	if (buffer.find("CAP LS \r\n"))
// 	{
// 		sendNeutralMessage(fd, "CAP * LS :multi-prefix account-notify\r\n");
// 		std::cout << GREEN << "Sending CAP *  LS to client" << RESET << std::endl;
// 	}
// 	buffer.replace(buffer.end()-2, buffer.end(), "\0");
	
// 	std::string aaa = buffer.substr(buffer.find("\r\n")+2, fullstc.size());
// 	std::cout << "client pwd :" << aaa << std::endl;
// 	if (aaa.compare(fullstc) == 0)
// 	{
// 		std::cout << GREEN << "User n°" << fd << " well logged!" << RESET << std::endl;
// 		sendMessage(fd, "good password: Connection accepted\n");
// 		clients[getClientIdFromFd(fd)].setLogged();
// 		aaa = buffer.substr(buffer.find("\r\n")+2, (buffer.find("\r\n")+2) - aaa.size());
// 		std::cout << "username nick and rest=" << aaa << " \n" << buffer << std::endl;
// 		// std::stringstream s;
		
// 		// clients[getClientIdFromFd(fd)].setIdentity()
// 	}
// 	else 
// 	{
// 		std::cout << RED << "Error Password from client " << fd << RESET << std::endl;
// 		sendMessage(fd, "log error, please retry with another password");
// 		return (-1);
// 	}
// 	return (0);
// }


int Server::checkClientStatus(int fd, std::string buffer)
{
	int id = getClientIdFromFd(fd);
	if (id > static_cast<int>(clients.size()) || id < 0)
		return (-1);
	if ((clients[id].getLoggedStatus() == 0) && (check_connection(buffer, fd) == -1))
		return (-1);
	return (0);
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
			{
				std::string buffer;
				buffer = receiveData(event[x]);
				
				if (checkClientStatus(event[x].data.fd, buffer) == -1)
				{
					std::cout << "Client " << event[x].data.fd << " disconnected" << std::endl;
					close(event[x].data.fd);
					deleteUser(event[x].data.fd);
					break;
				}
				// forwardData(event[x], buffer);
				// std::cout << GREEN << "Event Received: " << buffer << RESET << std::endl;
			}
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
	passwd = password;
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