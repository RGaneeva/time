#include "server.hpp"
using namespace std;
#include <stdio.h>
void Server::cmdNICK(string str, int n)
{
    string nick = "";
    vector<string> buff;
    int j = 0;
    cout << n << endl;
    for (int i = n; i < str.length();i++)
    {
        if (str[i] == ' ')
        {
            cout << "hello2\n";
            i++;int k = i;
            for (int h = 0;h< str.length() - i;h++)
            {
                cout << str.length() - i << endl;
                nick[h] = str[k];
                k++;
                cout << nick[h] << endl;
                cout << "hello3\n";
            }
            printf("%s\n", nick.c_str());
            break;
        }
    }
    cout << nick << endl;
    users.push_back(nick);
    // for (int i = 0; i< str.length();i++)
    // {
    //     nick[j] = str[i];
    //     j++;
    //     if (str[i] == ' ')
    //     {
    //         cout << nick << endl;
    //         buff.push_back(nick);
    //         nick = "";
    //         j = 0;
    //     }
    //     if (i == str.length() - 1)
    //         buff.push_back(nick);
    // }
    // for (int i = 0;i< buff.size(); i++)
    // {
    //     if (buff[i].find("NICK") != 0)
    //     {
    //         nick = buff[i + 1];
    //         break;
    //     }
    // }
    // cout << nick << endl;
    // users.push_back(nick);
}

void Server::parsBuffer(string str)
{
    int in = str.find("NICK");
    // cout << str << endl;
    if (str.find("NICK") != 0)
    {
        // cout << str.find("NICK");

        cout << "hello\n";
        cmdNICK(str, in);
    }
}