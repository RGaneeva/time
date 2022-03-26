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
        if (users[i].length() >= strlen(str.c_str()))
        {
            for (int j =0;j<strlen(str.c_str());j++)
            {
                if (users[i][j] == str[j])
                    ravno++;
            }
            if (ravno == strlen(str.c_str()))
                return 1;
            ravno = 0;
        }
        else
        {
            for (int j =0;j<strlen(users[i].c_str());j++)
            {
                if (users[i][j] == str[j])
                    ravno++;
            }
            if (ravno == strlen(str.c_str()))
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
        printf("<%s> :Nickname collision KILL\n",str.c_str());//436???
        return 1;
    }
    else
        return 0;
}

int Server::cmdNICK(string str, int n)//доб. замену ника
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
        users.push_back(nick);
        return 0;
    }
    else
        return 1;
}

int Server::parsBuffer(string str)
{
    int in = str.find("NICK");
    int ret = 0;
    if (str.find("NICK") == 0)
    {
        // cout << "hello\n";
        ret = cmdNICK(str, in);
    }
    return ret;
}