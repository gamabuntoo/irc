/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gule-bat <gule-bat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/20 15:24:04 by gule-bat          #+#    #+#             */
/*   Updated: 2026/08/26 23:18:38 by gule-bat         ###   ########.fr       */
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
	std::cout << GREEN << "\nSignal received: " << signum <<", exiting..." << RESET << std::endl;
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

std::string Server::getClientFormattedName(Client &cli)
{
	std::string st = ":" + cli.getNick() + "!" + cli.getUser() + "@" + cli.getIp() + " ";
	return (st);
}

void	Server::sendMessage(int fd, std::string buffer) // for forwarding to multiple clients after log etc
{
	std::string buff;
	int cli_fd = fd;

	if (cli_fd == clients.at(getClientIdFromFd(fd)).getFd())
	{
		std::string st = getClientFormattedName(clients[getClientIdFromFd(fd)]);
		st.append(buffer);
		std::cout << BLUE << "To client:\n" << RESET << st << "\n" << std::endl;
		size_t s = send(fd, st.c_str(), st.size(), MSG_NOSIGNAL); // echo back text
		if (s == 0)
		{
			std::cout << RED << "Error while sending message\n" << RESET << std::endl;
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

void	Server::sendMessage(int fd, std::string buffer, std::string prefix) // for forwarding to multiple clients after log etc
{
	std::string buff;
	int cli_fd = fd;

	if (cli_fd == clients.at(getClientIdFromFd(fd)).getFd())
	{
		std::string st = prefix;
		st.append(buffer);
		std::cout << BLUE << "To client:\n" << RESET << st << "\n" << std::endl;
		size_t s = send(fd, st.c_str(), st.size(), MSG_NOSIGNAL); // echo back text
		if (s == 0)
		{
			std::cout << RED << "Error while sending message\n" << RESET << std::endl;
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
		std::string s_name(SERVER_NAME);
		std::string st(":" + s_name + " ");
		st.append(buffer); // COUILLE COTE SERVEUR SUR LA STRING
		std::cout << BLUE << "To client:\n" << RESET << st << "\n" << std::endl;
		size_t s = send(fd, st.c_str(), st.size(), MSG_NOSIGNAL); // echo back text
		if (s == 0)
		{
			std::cout << RED << "Error while sending message\n" << RESET << std::endl;
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
			for (long unsigned int g = 0; g < channels.size(); g++)
			{
				if (channels[g].isUserPresent(clients[x].getNick()) >= 0)
					channels[g].removeUser(clients[x].getNick());
				if (channels[g].getSize() == 0)
					channels.erase(channels.begin() + g);
			}
			close(cli_fd);
			std::cout << RED << "Client: " << std::endl; 
			clients.at(x).printClientInfo();
			std::cout << RED << "disconnected" << RESET << std::endl;
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
		return (buff);
	}
	else if (ct == 0)
	{
		deleteUser(cli_fd);
	}
	else if (ct == -1)
	{
		if (errno != EAGAIN) 
		{
			throw std::runtime_error("Error: ListeningLoop; recv err");
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
	if (line.find("\nNICK", 0))
		nick = line.substr(line.find(" ")+1, line.size());
// parsing suspicieux	
	std::getline(s, line, '\r');
	if (line.find("USER ", 0))
	{
		std::stringstream b;	b << line;	std::getline(b, line, ':'); std::getline(b, line, '\r');
		user = line;
	}
	clients[getClientIdFromFd(fd)].setIdentity(nick, user, 2);
}

int	Server::clientPassword(std::stringstream &s, int fd)
{
	std::string line;
	std::getline(s, line, '\n');
	std::string fullstc("PASS " + passwd);

	fullstc.append("\r");
	if (fullstc == line)
	{
		std::cout << GREEN << "User n°" << fd << " well logged!" << YELLOW << "PASS passed\n" << RESET << std::endl;
		setClientStatusId(s, fd);
		clients[getClientIdFromFd(fd)].printClientInfo();
	}
	else 
	{
		std::cout << RED << "Error Password from client " << fd << RESET << std::endl;
		// sendMessage(fd, "log error, please try again with another password");
		return (-1);
	}
	return (1);	
}

int	Server::check_connection(std::string buffer, int fd)
{
	if (clients[getClientIdFromFd(fd)].getLoggedStatus() == true 
		|| ((clients[getClientIdFromFd(fd)].getNick() != DEFAULT_NICK) && (clients[getClientIdFromFd(fd)].getLoggedStatus() == false)))
		return (0);
	std::stringstream 	s;
	std::string 		line;
	s << buffer;
	std::getline(s, line, '\n');
	if (line != "CAP LS \r\n")
	{
		sendNeutralMessage(fd, "CAP * LS :multi-prefix account-notify\r\n");
		std::cout << GREEN << "Client handshake: Sending CAP * LS to client\n" << RESET << std::endl;
		if (!clientPassword(s, fd))
			return (-1);
		// for (long unsigned int x = 0; x < clients.size(); x++) // TEMPORARY REMOVE FOR TESTING
		// {
		// 	if ((clients.at(x).getIp() == clients.at(getClientIdFromFd(fd)).getIp()) && (clients.size() > 1 && static_cast<int>(x) != getClientIdFromFd(fd)))
		// 	{
		// 		sendNeutralMessage(fd, "Already logged somewhere else");
		// 		deleteUser(clients.at(getClientIdFromFd(fd)).getFd());
		// 		return (-1);
		// 	}
		// }
	}
	return (0);
}

int Server::checkClientStatus(int fd, std::string buffer)
{
	int id = getClientIdFromFd(fd);
	if (id > static_cast<int>(clients.size()) || id < 0)
		return (-1);
	
	if ((clients[id].getLoggedStatus() == false) && (check_connection(buffer, fd) == -1))
		return (-1);
	return (0);
}

void	Server::capRequests(int fd, std::string buffer)
{
	std::cout << YELLOW << "Into cap requests" << RESET << std::endl;
	if (clients[getClientIdFromFd(fd)].getLoggedStatus() == true && buffer.find("CAP ") != std::string::npos)
	{
		std::cout << RED << "CAP TEST ALREADY PASSED DONT TRY TO BAIT THE SERVER\n" << RESET << std::endl;
		return ;
	}
	if (buffer == "CAP REQ :multi-prefix\r\n" && clients[getClientIdFromFd(fd)].getNick() != DEFAULT_NICK)
	{
		std::string aaaa = clients[getClientIdFromFd(fd)].getNick();
		std::string c("CAP " + aaaa);
		c.append(" ACK :multi-prefix\r\n");
		sendNeutralMessage(fd, c);
		// sendNeutralMessage(fd, "CAP * ACK :multi-prefix\r\n");
		std::cout << GREEN << "Client handshake: Sending CAP * ACK to client\n" << RESET << std::endl;
	}
	else if (buffer == "CAP END\r\n")
	{
		std::string c("001 " + clients[getClientIdFromFd(fd)].getNick());
		c.append(" :Bienvenue . . . .\r\n");
		sendNeutralMessage(fd, c);
		clients[getClientIdFromFd(fd)].setLogged();
		std::cout << GREEN << "Client " << clients[getClientIdFromFd(fd)].getNick() << " logged, CAP PASSED\n" << "stat log: " << clients[getClientIdFromFd(fd)].getLoggedStatus() << std::endl;
	}
	// else if (buffer.find(" LS\r\n") != std::string::npos)
	// {
	// 	std::stringstream tmp(buffer);
	// 	std::string aaa;
	// 	std::string bbb;
	// 	tmp >> bbb;
	// 	tmp >> aaa;
	// 	aaa.append("doesn't exist\r\n");
	// 	sendNeutralMessage(fd, "CAP command:" + aaa);
	// }
}

int		Server::nickCommand(Client &cli, std::string buffer)
{
	std::string line = buffer.substr(5);
	std::cout << "id|" << line << "|" << line.compare(" ") << " " << line.size() << std::endl;
	if (buffer.compare("NICK \r\n") == 0)
	{
		sendNeutralMessage(cli.getFd(), "Erroneous nickname: empty\r\n");
		return (-1);
	}
	if (line.size() <= 3 || (buffer.compare(0, 7, "NICK \r\n") == 0))
	{
		sendNeutralMessage(cli.getFd(), "Erroneous nickname\r\n"); // check spaces etc... (parser pr tt carrément)
		return (-1);
	}
	line.resize(line.size() - 2);
	std::string rps = "NICK :" + line + "\r\n";
	sendMessage(cli.getFd(), rps);
	cli.setIdentity(line, "", 1);
	cli.printClientInfo();
	//check for namechanges in channels
	return (0);
}

void	Server::pongUserListRoutine(Client &cli)
{
	std::cout << BOLDMAGENTA << "Currently logged:\n" << RESET << std::endl;
	for (long unsigned int x = 0; x < clients.size(); x++)
	{
		std::cout << "stat: " << clients[x].getLoggedStatus() << std::endl;
		if (clients[x].getFd() == cli.getFd())
		{
			std::cout << BOLDYELLOW << "Ping transnmitter: ";
			clients[x].printClientInfo();
			continue ;
		}
		if (clients[x].getLoggedStatus() != true)
		{
			std::cout << RED << "Login required: ";
			clients[x].printClientInfo();
		}
		else
			clients[x].printClientInfo();
	}
}

void	Server::pongChannelsListRoutine()
{
	for (long unsigned int x = 0; x < channels.size(); x++)
	{
		std::cout << GREEN << "#"<< channels[x].getName() << " with " << channels[x].getSize() << " users connected"<< RESET << std::endl;
		std::cout << "Topic:\t" << channels[x].getTopic() << std::endl;
	}
}

void	Server::pongCommand(Client &cli, std::string buffer)
{
	std::cout << "parsing ping ..." << std::endl;
	std::string bf = "PONG ";
	bf.append(buffer.substr(5));
	sendNeutralMessage(cli.getFd(), bf);
	std::cout << GREEN << "PONG response sent to " << cli.getNick() << " at " << cli.getIp() << RESET << std::endl;
	pongUserListRoutine(cli);
	pongChannelsListRoutine();
}

void	Server::sendJoinInfo(Client &cli, std::string channel, int i)
{
	sendMessage(cli.getFd(), "JOIN :#" + channel + "\r\n");
	sendNeutralMessage(cli.getFd(), "324 " + cli.getNick() + " #" + channel + " " + "+nt\r\n");
	sendNeutralMessage(cli.getFd(), "332 " + cli.getNick() + " #" + channel + " :" + channels[i].getTopic() + "\r\n");
	std::string str = "353 " + cli.getNick() + " = #" + channel + " :";
	for (long unsigned int x = 0; x < clients.size();  x++)
	{
		if (channels[i].isUserPresent(clients[x].getNick()) >= 0)
		{
			if (channels[i].isOperator(clients[x].getNick()) == true)
				str.append("@" + clients[x].getNick() + " ");
			else
				str.append("+" + clients[x].getNick() + " ");
		}
	}
	if (str[str.size()-1] == ' ')
		str.insert(str.size()-1, "\r\n");
	sendNeutralMessage(cli.getFd(), str);
	sendNeutralMessage(cli.getFd(), "366 " + cli.getNick() + " #" + channel + " :End of /NAMES list\r\n");
}

void	Server::joinCommand(Client &cli , std::string channel)
{
	long unsigned int i = 0;	

	while (i < channels.size())
	{
		if (channel == channels[i].getName())
		{
			if (channels[i].addUser(cli.getNick()) == 1)
				return sendJoinInfo(cli, channel, i);
			else
			{
				// send message to client to inform that name already exists
				std::cout << "Username already present in channel" << std::endl;
				return ;
			}
		}
		if (channels[i].getSize() == 0)
		{
			channels.erase(channels.begin() + i);
			std::cout << "channel erased" << std::endl;
		}
		i++;
	}
	if (i == channels.size())
	{
		channels.push_back(Channel(channel, cli, "", 0, 1, 0, 0, "", 0));
		sendJoinInfo(cli, channel, i);
		return ;
	}
}

void	Server::topicCommand(Client &cli, std::string channel)
{
	std::cout << "TOPIC" << channel << std::endl;
	std::string aaa;
	aaa = channel.substr(1, channel.find(" ")-1);
	for (long unsigned int i = 0; i < channels.size(); i++)
	{
		std::cout << "TROPIC" << " " << channel << " " << aaa << " " << channels[i].getName() << std::endl;
		if (channels[i].getName() == aaa && (channels[i].isOperator(cli.getNick()) == true && channels[i].isOpOnly())) // op protextion
		{
			std::string s = channel.substr(channel.find(" :")+2);
			s.append("\r\n");
			channels[i].setTopic(s);
			std::string reply = "TOPIC #" + channels[i].getName() + " :" + s;
			// sendMessage(cli.getFd(), reply);
			for (long unsigned int x = 0; x < clients.size();  x++)
			{
				if (channels[i].isUserPresent(clients[x].getNick()) >= 0)
					sendMessage(clients[x].getFd(), reply);
			}
		}
		else if ((aaa == channels[i].getName() && channels[i].getName().find(":") == std::string::npos))
		{
			std::string rpl = "332 " + cli.getNick() + " #";
			rpl.append(channels[i].getName() + " :" + channels[i].getTopic() + "\r\n");
			sendMessage(cli.getFd(), rpl);
		}
	}

}

void	Server::privmsgCommand(Client &cli, std::list<std::string> l)
{	
	std::cout << "privmsg 1 -\n" << l.front() << "|\n" << l.back() << std::endl;
	for (long unsigned int i = 0; i < channels.size(); i++)
	{
		if (l.front() == channels[i].getName() && channels[i].isUserPresent(cli.getNick()) >= 0)
		{
			for (long unsigned int x = 0; x < clients.size(); x++)
			{
				std::cout << "privmsg 2 -" << l.front() << "|" << l.back() << std::endl;
				std::cout << "privmsg 2 -" << l.front() << "|" << l.back() << std::endl;
				if (channels[i].isUserPresent(clients[x].getNick()) >= 0 && clients[x].getNick() != cli.getNick())
					sendMessage(clients[x].getFd(), "PRIVMSG #" + l.front() + " :" + l.back() + "\r\n", getClientFormattedName(cli));
			}
		}
		// else
		// { client non autorisé }
	}
}

void	Server::processCommand(int fd, std::string buffer)
{
	std::cout << "parsing ..." << std::endl;
	if (fd < 0)
	{
		std::cout << RED << "Fd error" << RESET << std::endl;
		return ;
	}
	if (clients[getClientIdFromFd(fd)].getLoggedStatus() == false)
		return capRequests(fd, buffer);
	else
	{
		std::string tmp = buffer.substr(6);
		tmp.erase(tmp.size()-2);
		std::cout << "parsing else ..." << std::endl;
		if (buffer.compare(0, 5, "NICK ") == 0)
		{
			if (nickCommand(clients[getClientIdFromFd(fd)], buffer) == 0)
				std::cout << BOLDGREEN << "client NICK changed " << RESET << std::endl;
			else
			{
				std::string err = "433 " + clients[getClientIdFromFd(fd)].getNick();
				err.append(tmp);
				err.append(" :Nickname is already in use\r\n");
				sendNeutralMessage(fd, err);
				return ;
			}
		}
		else if ((buffer.compare(0, 5, "PING ")) == 0)
			pongCommand(clients[getClientIdFromFd(fd)], buffer);
		else if ((buffer.compare(0, 6, "JOIN #")) == 0)
			joinCommand(clients[getClientIdFromFd(fd)] ,tmp);
		else if ((buffer.compare(0, 7, "TOPIC #")) == 0)
		{
			std::string tp;
			tp = buffer.substr(buffer.find("#"));
			tp.erase(tp.size()-2);
			topicCommand(clients[getClientIdFromFd(fd)], tp);
		}
		else if (buffer.compare(0, 6, "QUIT :") == 0)	
			deleteUser(fd);
		else if (buffer.compare(0, 9, "PRIVMSG #") == 0)
		{
			std::cout << "privmsg" << std::endl;
			std::list<std::string> l;
			// l.assign(3, "");
			std::string g = (buffer.substr(9, buffer.find(" :", 9)));
			// g = g.substr(0, g.find(""))
			l.push_back(g.substr(0, g.find(" :")));
			l.push_back(buffer.substr(buffer.find(" :")+2));
			l.back().erase(l.back().size()-2);
			privmsgCommand(clients[getClientIdFromFd(fd)], l);
			// m. = buffer.substr(9, buffer.find(" ", 9));
		}
		else if (buffer.compare(0, 6, "PART #") == 0)
		{
			std::cout << " part parsing "<< tmp << std::endl;
			std::string chan = tmp.substr(0, tmp.find(" :")-1);
			std::string rs = tmp.substr(chan.size());
			std::cout << "part parsing "<< chan << " " << rs << std::endl;
			// PARSING:			Part command parsing error
			// partCommand(tmp);
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
			std::cerr << "epoll_wait end . . . ." << std::endl;
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
					deleteUser(event[x].data.fd);
					break;
				}
				else
					processCommand(event[x].data.fd, buffer);
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