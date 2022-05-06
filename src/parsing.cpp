#include "server.hpp"
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
#include <stdio.h>
#include <cstring>
int Server::Find(string str)
{
    // printf("len: %lu\n", strlen(str[0].c_str()));
    int ravno = 0;
    for (list<string>::iterator i = users.begin();i!=users.end();i++)
    {
        if (strlen(i->c_str()) >= strlen(str.c_str()))
        {
            for (int j =0;j<strlen(str.c_str());j++)
            {
                string test = *i;
                if (test[j] == str[j])
                    ravno++;
            }
            if (ravno == strlen(str.c_str()) && strlen(i->c_str()) == strlen(str.c_str()))
                return 1;
            ravno = 0;
        }
    }
    // printf("ravno: %d\n", ravno);
    return 0;
}

int Server::Find(string &str, string str2)
{
    int res = -1;int ravno = 0;
    int len1 = strlen(str.c_str());
    int len2 = strlen(str2.c_str());
    if (len1 >= len2)
    {
        for (int i = 0;i<len2;i++)
        {
            if (str[i] == str2[i])
                ravno++;
            else
                ravno = 0;
        }
        if (ravno == len2)
        {
            res = 0;
            return res;
        }
    }
    return res;
}

int Server::checkClient(string str)
{
    int k = Find(str);
    if (k == 1)
    {
        printf("<%s> :Nickname is already in use\n",str.c_str());//436???
        return 1;
    }
    else
        return 0;
}

int Server::cmdNICK(string &str, int n, struct kevent &event)//доб. замену ника
{
    string nick(str.substr(n));
    nick = (nick.substr(nick.find_first_of(' ') + 1));
    size_t found = nick.find('\n');
    if (found > 0)
    {
        for (int p = 0;p<nick.size();p++)
            if (nick[p] == '\n'|| nick[p] == '\r')
                nick.erase(p);
    }
    // if (nick.length() < 1)
    //     ERR(":No nickname given", strerror(errno));//431???
    if (checkClient(nick) == 0)
    {
        printf("check: %s\n", nick.c_str());
        users.push_back(nick);
        fds.push_back(event);
        return 0;
    }
    else
        return 1;
}
void Server::cmdQUIT(struct kevent &event)
{
    list<string>::iterator it2 = users.begin();
    for (list<struct kevent>::iterator it = fds.begin();it!=fds.end() || it2!=users.end();it++, it2++)
    {
        if (it->ident == event.ident)
        {
            if (users.size() == 1)
            {
                users.clear();
                fds.clear();
                break;
            }
            else
                users.erase(it2);
            if (fds.size() == 1)
            {
                users.clear();
                fds.clear();
                break;
            }
            else
                fds.erase(it);
            it2 = users.begin();
            it = fds.begin();
        }
    }
    onClientDisconnect(event);
}
void Server::cmdPRIVMSG(string &str, struct kevent &e)
{
    int space = str.find(' ');
    int m = str.find(':');
    string nick = "";
    string nick2 = "";
    string message = "";
    list<struct kevent>::iterator it2 = fds.begin();
    list<string>::iterator it = users.begin();
    int l = 0;
    for (int i = space + 1;str[i] != ' ';i++,l++)
    {
        nick[l] = str[i];
    }
    printf("nick: %s\n", nick.c_str());
    for (int i = m;i<str.size();i++)
        message+=str[i];
    int j = 0;
    for (list<string>::iterator i = users.begin();i != users.end();i++)
    {
        string str = *i;
        int check = 0;
        printf("str: %s\n", str.c_str());
        if (strlen(str.c_str()) == strlen(nick.c_str()))
            for (int g=0;g<strlen(str.c_str());g++)
                if (str[g] == nick[g])
                    check++;
        if (check == strlen(str.c_str()))
            break;
        j++;
    }
    printf("j: %d\n", j);
    struct kevent event;
    for (int k = 0;k<=j;it2++,k++)
    {
        if (k==j)
            event = *it2;
    }
    printf("%lu\n", event.ident);
    int a = 0;
    for (list<struct kevent>::iterator i = fds.begin();i != fds.end();i++)
    {
        if (i->ident == e.ident)
            break;
        a++;
    }
    printf("a: %d\n", a);
    for (int k = 0;k<users.size();it++,k++)
    {
        string str = *it;
        printf("str2: %s\n", str.c_str());
        if (k==a)
        {
            for (int n = 0;n<strlen(str.c_str());n++)
                nick2[n] = str[n];
            // nick2 = str;
        }    
    }
    printf("nick2: %s\n", nick2.c_str());
    sendAnswer(event, ":server 301 "+nick2+" " + message);
}
int Server::cmdPASS(string &str, struct kevent &e)
{
    string pass = str.substr(str.find_first_of(':') + 1);
    size_t found = pass.find('\n');
    size_t found2 = pass.find('\r');
    if (found > 0)
        pass.erase(found);
    if (found2 > 0)
        pass.erase(found2);
    printf("pass: %s\n", pass.c_str());
    printf("pass len: %lu\n", strlen(pass.c_str()));
    if (pass == serverpassword)
        return 0;
    else
    {
        // sendAnswer(e, ERROR);
        onClientDisconnect(e);
        return 1;
    }
}
int Server::parsBuffer(string &str, struct kevent &event)
{
    int in = str.find("NICK");
    int ret = 0;
    if (str.find("PASS") != string::npos)
    {
        ret = cmdPASS(str, event);
        printf("ret: %d\n", ret);
    }
    if (ret == 1)
        return ret;
    if ((str.find("USER") != string::npos) && (str.find("PASS") == string::npos))
    {
        onClientDisconnect(event);
        return 1;
    }
    if (str.find("NICK") != string::npos)
    {
        ret = cmdNICK(str, in, event);
        if (ret == 0)
        {	
            string name = "";
            for (list<string>::iterator i = users.begin();i!=users.end();i++)
                name = i->c_str();
            sendAnswer(event, SUCCESSCONNECT + name +"\r\n");
            cout << "-------------------------------\n";
            cout << "users online: " << users.size() << endl;
            for (list<string>::iterator i = users.begin();i!= users.end();i++)
                printf("%s\n", i->c_str());
            cout << "-------------------------------\n";
        }
        else
        {
            sendAnswer(event, ERROR);
            sendAnswer(event, ":server 451 :You have not registered\r\n");
            onClientDisconnect(event);
        }
    }
    if (Find(str, "PING") == 0)
        sendAnswer(event,"PONG 10.21.32.116");
    if (Find(str, "QUIT") == 0)
        cmdQUIT(event);
    if (Find(str,"PRIVMSG") == 0)
        cmdPRIVMSG(str, event);
    if (Find(str,"LIST") == 0)
    {
        printf("users: %lu\n", users.size());
        printf("fds: %lu\n", fds.size());
        for (list<string>::iterator i = users.begin(); i!=users.end();i++)
            printf("%s: %lu\n", i->c_str(), strlen(i->c_str()));
        for (list<struct kevent>::iterator i = fds.begin();i!=fds.end();i++)
            printf("%lu\n", i->ident);
    }
    return ret;
}