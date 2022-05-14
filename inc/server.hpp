#ifndef SERVER_HPP
#define SERVER_HPP
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/event.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <fcntl.h>
#include <fstream>
#include "log.hpp"
#include <iostream>
#include <vector>
#include <list>
using namespace std;
#define ERROR ":server 433 Nickname is already in use\r\n"
#define SUCCESSCONNECT ":server 376 "

class chatroom
{
  public:
    list<string> users;
    string pass;
    string name;
    string topic;
    chatroom(string Name,string Pass) {name = Name;pass = Pass; topic = "";}
    ~chatroom() {}
    void addUser(string nik) {users.push_back(nik);}
    void printUsers()
    {
      for (list<string>::iterator it = users.begin();it != users.end();it++)
        cout<< *it<< endl;
    }
    void printName()
    {
      cout<< name << endl;
    }
    void printPass()
    {
      cout<< pass << endl;
    }
    void deleteUser(string nik) 
    {
      for (list<string>::iterator it = users.begin();it != users.end();it++)
      {
        string n = *it;
        if (n == nik)
          users.erase(it);
      }
    }
};

class Server
{
  public:
    Server(const char *addr, const int port,string pass, int backlog);
    ~Server();
    void onRead(struct kevent& event);
    void onEOF();
    int onClientConnect(struct kevent& event);
    int onClientDisconnect(struct kevent& event);
    void startServer();
    int parsBuffer(string &str, struct kevent &event);
    int cmdNICK(string &str, int n, struct kevent &event);
    int checkClient(string str);
    int Find(string str);
    int Find(string &str, string str2);
    void sendAnswer(struct kevent &event, string str);
    void cmdQUIT(struct kevent &event);
    void cmdPRIVMSG(string &str, struct kevent &e);
    int cmdPASS(string &str, struct kevent &e);
    string split(string str, char del);
    void cmdJOIN(string &str, struct kevent &event);
    int channelNameCheck(string str);
    int spaceCheck(string str);
    void chanPassNum(string chanName, string passName, struct kevent &event);
    string addUserToChan(struct kevent &event);
    void userAlreadyInChan(string chanCheck, struct kevent &event);

  private:
    int listen();
    int bind();
    int shutdown();
    int close();
    int initServer();
    struct sockaddr_in m_address;
    int m_sock_reuse;
    uintptr_t m_sock;
    int m_backlog;
    int m_kqueue;
    struct kevent m_event_subs;
    struct kevent m_event_list[32];
    char m_receive_buf[1024];
    list<string> users;
    list<struct kevent> fds;
    vector<chatroom> rooms;
    string sockstr;
    string serverpassword;
    enum SocketState 
    {
      INITIALIZED,
      BOUND,
      LISTENING,
      CLOSED
    };
    SocketState m_sock_state;
};
#endif