/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gule-bat <gule-bat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/20 15:24:13 by gule-bat          #+#    #+#             */
/*   Updated: 2026/08/29 04:50:38 by gule-bat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <vector>
#include <csignal>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <csignal>
#include <sstream>
#include <algorithm>
#include <list>

#include <unistd.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>

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

#define PORT 			8080
#define MAX_EVENT		10
#define IP_ADDR			"127.0.0.1"
#define DEFAULT_USER	"3pmra5r8\"/\"-qr^$afùqldmw,roz0°3=rpa"
#define DEFAULT_NICK	"/rp3arqlpzm08qa\"5$^3=\"aù-frdmow°,"
#define SERVER_NAME		"serveur.irc.cool"


#define INVITE_ONLY		65
#define TOPIC_OP_ONLY	66
#define	CHANNEL_PASSWD	67
#define USER_LIMIT		68
#define EMPTY_CHAN_PASSW ""



#define SHREX "\n⣿⣿⣿⠋⢩⢹⣿⣿⣿⣿⣽⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n⣿⣿⡧⣦⠄⢧⡙⢿⣟⢁⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿\n⣿⣿⣷⣶⣶⣦⡈⠂⠄⠸⠿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠉⠰\n⣿⣿⣿⣿⣿⣿⣿⠄⠄⠄⢒⣂⠄⠙⢿⣿⣿⡿⠛⢛⣻⣿⣿⡟⢁⣠⣴\n⣿⣿⣿⣿⣿⣿⠇⢇⡄⣆⣤⣀⣦⡄⢈⣉⣛⣭⡀⠙⠭⡛⠿⣿⣻⣿⣿\n⣿⣿⣿⣿⣿⣿⠄⠄⣿⣿⣿⣿⣿⢃⣿⣿⣿⣿⣿⣶⣷⡾⣼⣿⠈⠉⠄\n⣿⣿⣿⣿⣿⡇⠄⠄⢿⣿⣿⣿⣥⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⠄⠄\n⣿⣿⣿⡿⠋⠄⠄⠄⢸⣿⡿⠿⠄⠈⠛⢟⣿⣿⣿⣿⣿⣿⣿⣿⣿⣆⠄\n⣿⣿⠟⠁⠄⠄⠄⠄⠄⢠⣄⣀⡲⢦⣤⣼⣿⡿⣿⣿⣿⣿⣿⣿⣿⣿⡀\n⠋⠄⠄⠄⠄⠄⠄⠄⠄⠈⢿⣟⠻⠿⣿⣿⣿⣷⣾⣿⣿⣿⣿⢿⣿⣿⡇\n⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠈⠻⣷⣶⣾⣿⣿⣿⣿⣿⣿⠟⢡⣿⣿⣿⡟\n⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⣉⣹⣿⣿⣿⣿⠟⠁⣰⣿⣿⣿⣿⡇\n⣧⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠈⠉⠉⠙⠛⠉⠁⢀⣼⣿⣿⣿⣿⡟⠄"

// class Parser
// {
// 	private:
			
// 	public:
	
// };

class Client
{
	private:
		int Fd;
		bool		logged;
		std::string ip_addr;
		std::string nick;
		std::string user;
	public:
		Client();
		Client(const Client &src);
		Client(int fd, std::string ipaddr);
		~Client();

		int			getFd();
		void		setFd(int fd);
		void		setIp(std::string s);
		void		setIdentity(std::string nick, std::string user, int flag);
		std::string			getIp();
		std::string 		getNick();
		std::string			getUser();
		void				printClientInfo();
		void		setLogged();
		bool		getLoggedStatus();
};

class Channel
{
	private:
		std::string					name;
		std::vector<std::string> 	nicks;		// contains users nicknames
		std::vector<std::string>	operators;	// contains op nick (server can't host the same nick 2 times)
		std::vector<std::string>	invited; 	// stock nickname of invited user till he's connected (not done yet)
		std::string					topic;
		bool 				invite_only_f;
		bool 				topic_only_op_f;
		bool 				channel_password_f;
		bool 				user_limit_f;
		std::string 	channel_passwd;
		unsigned int 	user_limit_s;

	public:
		Channel();
		Channel(std::string name, Client &creator, std::string topic, int invite_only_f, int topic_only_op_f, int channel_password_f, int user_limit_f, std::string channel_passwd, unsigned int user_limit_s);
		Channel(const 	Channel &src);
		Channel &operator=(const Channel &src);
		~Channel();
		std::string			getName();
		long unsigned int	getSize();
		void				setTopic(std::string topic);
		std::string				getTopic();
		int			addUser(std::string nick);
		void		removeUser(std::string nick);
		int			isUserPresent(std::string nick);
		bool		isOperator(std::string nick);
		bool		isOpOnly();
		void		addChanOperator(std::string nick);
};

class Server
{
	private:
		unsigned int				Port;
		int							SocketFd;
		int							epollFd;
		bool 							signal;
		struct sockaddr_in			address;
		std::vector <Client> 		clients;
		std::vector <Channel>		channels;
		std::vector<epoll_event> 	ev;
		unsigned int				maxEv;
		std::string					passwd;
	public:
		Server();
		Server(const Server &server);
		Server(int port, std::string password);
		~Server();
		// Server(unsigned int port, int Socket_id, unsigned int max_ev);
		int 			Setup_server();

		void			set_sock_non_blocking(int fd);

		void			ListeningLoop();
		void			epoll_init();

		void			signalInit();
		static void		signalHandler(int signum);


		void			AddClientToStruct(struct sockaddr_in cli, int cli_fd);
		std::string		receiveData(epoll_event event);
		
		// void			forwardData(epoll_event event, std::string buffer);
		void			sendMessage(int fd, std::string buffer);
		void			sendNeutralMessage(int fd, std::string buffer);
		void			sendMessage(int fd, std::string buffer, std::string prefix);

		void			processCommand(int fd, std::string buffer);
		int				nickCommand(Client &cli, std::string buffer);
		void			pongCommand(Client &cli, std::string buffer);	
		void			pongUserListRoutine(Client &cli);
		void			pongChannelsListRoutine();
		void			joinCommand(Client &cli , std::string channel);
		void			sendJoinInfo(Client &cli, std::string channel, int i);
		void			topicCommand(Client &cli, std::string channel);
		void			privmsgCommand(Client &cli, std::list<std::string> l);
		void			partCommand(Client &cli, std::string channel, std::string reason);
		
		int				check_connection(std::string buffer, int fd);
		void			AddNewClient(epoll_event event);
		int				clientPassword(std::stringstream &s, int fd);
		
		void			setClientStatusId(std::stringstream &s, int fd);
		int				checkClientStatus(int fd, std::string buffer);
		void			capRequests(int fd, std::string buffer);
		void			deleteUser(int cli_fd);
		
		int				getClientIdFromFd(int fd);
		std::string		getIpFromFd(int cli_fd);
		std::string 	getClientFormattedName(Client &cli);
		int				getChannelIdFromName(std::string ch_name);

		void			Clean_exit();

};