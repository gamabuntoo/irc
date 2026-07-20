#include <iostream>
#include <vector>
#include <string>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <cerrno>

#define RED "\e[1;31m"
#define WHI "\e[0;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"

class Client
{
private:
    int Fd;
    std::string IPadd;
public:
    Client() : Fd(-1) {}

    int GetFd(){return Fd;}
    void SetFd(int fd){Fd = fd;}
    void setIpAdd(std::string ipadd){IPadd = ipadd;}
};

class Server
{
private:
    int Port;
    int SerSocketFd;
    int epollFd;

    static bool Signal;
    std::vector<Client> clients;
    std::vector<epoll_event> events;

    static const int MAX_EVENTS = 64;

public:
    Server() : Port(4444), SerSocketFd(-1), epollFd(-1), events(MAX_EVENTS) {}

    void ServerInit();
    void SerSocket();
    void AcceptNewClient();
    void ReceiveNewData(int fd);

    static void SignalHandler(int signum);

    void CloseFds();
    void ClearClients(int fd);
};

void Server::ClearClients(int fd)
{
    // Retire du set epoll
    if (epollFd != -1) {
        epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL); // ignorer l'erreur si déjà retiré
    }

    // Retire du vector des clients
    for (size_t i = 0; i < clients.size(); i++) {
        if (clients[i].GetFd() == fd) {
            clients.erase(clients.begin() + i);
            break;
        }
    }
}

bool Server::Signal = false;

void Server::SignalHandler(int signum)
{
    (void)signum;
    std::cout << std::endl << "Signal Received!" << std::endl;
    Server::Signal = true;
}

void Server::CloseFds()
{
    for (size_t i = 0; i < clients.size(); i++) {
        std::cout << RED << "Client <" << clients[i].GetFd() << "> Disconnected" << WHI << std::endl;
        close(clients[i].GetFd());
    }
    clients.clear();

    if (SerSocketFd != -1) {
        std::cout << RED << "Server <" << SerSocketFd << "> Disconnected" << WHI << std::endl;
        close(SerSocketFd);
    }

    if (epollFd != -1) {
        close(epollFd);
    }
}

void Server::ReceiveNewData(int fd)
{
    char buff[1024];
    std::memset(buff, 0, sizeof(buff));

    ssize_t bytes = recv(fd, buff, sizeof(buff) - 1, 0);

    if (bytes <= 0) {
        std::cout << RED << "Client <" << fd << "> Disconnected" << WHI << std::endl;
        ClearClients(fd);
        close(fd);
        return;
    }

    buff[bytes] = '\0';
    std::cout << YEL << "Client <" << fd << "> Data: " << WHI << buff;
    // TODO: parse / authent / handle commande...
}

void Server::AcceptNewClient()
{
    while (true) {
        sockaddr_in cliadd;
        socklen_t len = sizeof(cliadd);

        int incofd = accept(SerSocketFd, (sockaddr *)&cliadd, &len);
        if (incofd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // plus de clients en attente (socket non-bloquante)
                break;
            }
            std::cout << "accept() failed" << std::endl;
            break;
        }

        if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1) {
            std::cout << "fcntl() failed" << std::endl;
            close(incofd);
            continue;
        }

        Client cli;
        cli.SetFd(incofd);
        cli.setIpAdd(inet_ntoa(cliadd.sin_addr));
        clients.push_back(cli);

        epoll_event ev;
        std::memset(&ev, 0, sizeof(ev));
        ev.events = EPOLLIN;          // lecture disponible
        ev.data.fd = incofd;

        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, incofd, &ev) == -1) {
            std::cout << "epoll_ctl(ADD) failed" << std::endl;
            ClearClients(incofd);
            close(incofd);
            continue;
        }

        std::cout << GRE << "Client <" << incofd << "> Connected" << WHI << std::endl;
    }
}

void Server::SerSocket()
{
    int en = 1;
    sockaddr_in add;

    add.sin_family = AF_INET;
    add.sin_addr.s_addr = INADDR_ANY;
    add.sin_port = htons(this->Port);

    SerSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (SerSocketFd == -1)
        throw std::runtime_error("faild to create socket");

    if (setsockopt(SerSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
        throw std::runtime_error("faild to set option (SO_REUSEADDR) on socket");

    if (fcntl(SerSocketFd, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("faild to set option (O_NONBLOCK) on socket");

    if (bind(SerSocketFd, (struct sockaddr *)&add, sizeof(add)) == -1)
        throw std::runtime_error("faild to bind socket");

    if (listen(SerSocketFd, SOMAXCONN) == -1)
        throw std::runtime_error("listen() faild");

    epollFd = epoll_create1(0);
    if (epollFd == -1)
        throw std::runtime_error("epoll_create1() failed");

    // Ajouter le socket serveur dans epoll
    epoll_event ev;
    std::memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = SerSocketFd;

    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, SerSocketFd, &ev) == -1)
        throw std::runtime_error("epoll_ctl(ADD) on server socket failed");
}

void Server::ServerInit()
{
    this->Port = 4444;
    SerSocket();

    std::cout << GRE << "Server <" << SerSocketFd << "> Connected" << WHI << std::endl;
    std::cout << "Waiting to accept a connection...\n";

    while (Server::Signal == false) {
        int nfds = epoll_wait(epollFd, events.data(), (int)events.size(), -1);
        if (nfds == -1) {
            if (Server::Signal == false)
                throw std::runtime_error("epoll_wait() failed");
            continue;
        }

        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;

            if (events[i].events & EPOLLIN) {
                if (fd == SerSocketFd) {
                    AcceptNewClient();
                } else {
                    ReceiveNewData(fd);
                }
            }
        }
    }

    CloseFds();
}

int main()
{
    Server ser;
    std::cout << "---- SERVER ----" << std::endl;

    try {
        signal(SIGINT, Server::SignalHandler);
        signal(SIGQUIT, Server::SignalHandler);
        ser.ServerInit();
    } catch (const std::exception& e) {
        ser.CloseFds();
        std::cerr << e.what() << std::endl;
    }

    std::cout << "The Server Closed!" << std::endl;
    return 0;
}
