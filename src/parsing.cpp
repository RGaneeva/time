#include "server.hpp"
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
#include <stdio.h>
#include <cstring>
int Server::Find(string &str)
{
    int ravno = 0;
    for (int i = 0;i<users.size();i++)
    {
        if (strlen(users[i].c_str()) >= strlen(str.c_str()))
        {
            for (int j =0;j<strlen(str.c_str());j++)
            {
                if (users[i][j] == str[j])
                    ravno++;
            }

            if (ravno == strlen(str.c_str()) && strlen(users[i].c_str()) == strlen(str.c_str()))
                return 1;
            ravno = 0;
        }
    }
    return 0;
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

int Server::cmdNICK(string str, int n, struct kevent &event)//доб. замену ника
{
    string nick = "";
    vector<string> buff;
    int j = 0;
    for (int i = n; i < str.length();i++)
    {
        if (str[i] == ' ')
        {
            i++;
            int k = i;
            for (int h = 0;h< str.length() - i;h++)
            {
                if(str[k] >= 33 && str[k] <= 126)
                {
                    nick[h] = str[k];
                    k++;
                }
            }
            break;
        }
    }
    printf("%s\n", nick.c_str());
    if (nick.length() < 1)
        ERR(":No nickname given", strerror(errno));//431???
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

int Server::parsBuffer(string str, struct kevent &event)
{
    int in = str.find("NICK");
    int ret = 0;
    if (str.find("NICK") >= 0)
    {
        // cout << "hello\n";
        ret = cmdNICK(str, in, event);
    }
    if (str.find("PING") == 0)
        sendAnswer(event,"PONG 10.21.21.52");
    // if (str.find("QUIT") == 0)
    // {
    //     vector<string>::iterator it2 = users.begin();
    //     for (vector<struct kevent>::iterator it = fds.begin();it!=fds.end();it++)
    //     {
    //     	if (it->ident == event.ident)
    //     	{
    //             if (users.size() == 1 || users.size() == 0)
    //                 users.clear();
    //             else
    //                 users.erase(it2);
    //             if (fds.size() == 1 || fds.size() == 0)
    //                 fds.clear();
    //             else
    //     		    fds.erase(it);
    //     	}
    //     	it2++;
    //     }
    // }
    return ret;
}