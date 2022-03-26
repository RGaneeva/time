#include "server.hpp"
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
#include <stdio.h>

void Server::checkClient(string str, vector<string> users)
{
    typedef vector<int>::iterator iterator;
    iterator it = find(users.begin(), users.end(), str);
    if(it > 0)
        ERR("<str> :Nickname collision KILL", strerror(errno));//436???

}

void Server::cmdNICK(string str, int n)//доб. замену ника
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
    if (nick.length() < 1)
        ERR(":No nickname given", strerror(errno));//431???
    
    printf("%s\n", nick.c_str());
    users.push_back(nick);

}

void Server::parsBuffer(string str)
{
    int in = str.find("NICK");
    if (str.find("NICK") != 0)
    {
        cout << "hello\n";
        cmdNICK(str, in);
    }
}