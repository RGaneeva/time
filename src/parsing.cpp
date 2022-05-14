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
    size_t ravno = 0;
    for (list<string>::iterator i = users.begin();i!=users.end();i++)
    {
        if (strlen(i->c_str()) >= strlen(str.c_str()))
        {
            for (size_t j =0;j<strlen(str.c_str());j++)
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
        for (unsigned long p = 0;p<nick.size();p++)
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
    for (int i = space + 1;str[i] != ' ';i++)
    {
        nick += str[i];
    }
    for (unsigned long i = m;i<str.size();i++)
        message+=str[i];
    int j = 0;
    for (list<string>::iterator i = users.begin();i != users.end();i++)
    {
        string str = *i;
        size_t check = 0;
        if (strlen(str.c_str()) == strlen(nick.c_str()))
            for (size_t g=0;g<strlen(str.c_str());g++)
                if (str[g] == nick[g])
                    check++;
        if (check == strlen(str.c_str()))
            break;
        j++;
    }
    struct kevent event;
    for (int k = 0;k<=j;it2++,k++)
    {
        if (k==j)
            event = *it2;
    }
    unsigned long a = 0;
    for (list<struct kevent>::iterator i = fds.begin();i != fds.end();i++)
    {
        if (i->ident == e.ident)
            break;
        a++;
    }
    for (unsigned long k = 0;k<users.size();it++,k++)
    {
        string str = *it;
        printf("str2: %s\n", str.c_str());
        if (k==a)
        {
            for (size_t n = 0;n<strlen(str.c_str());n++)
                nick2 += str[n];
        }    
    }
    string *str2 = new string(":"+nick2+"! PRIVMSG "+nick+" "+ message + "\r\n");
    sendAnswer(event, str2[0]);
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
        onClientDisconnect(e);
        return 1;
    }
}

string Server::split(string str, char del)
{
    string *strings = new string("");
    for (size_t i = 0;i<str.size();i++)
    {
        if (str[i] != del)
            strings[0] += str[i];
        else
            return strings[0];
    }
    return strings[0];
}

int Server::spaceCheck(string str)
{
    int k = 0;
    for (size_t i = 0;i<str.size();i++)
    {
        if (str[i] == ' ')
            k++;
    }
    return k;
}

int Server::channelNameCheck(string str)
{
    for (size_t l = 0; l < str.size(); l++)
    {
        if (str[l] == ' ' || str[l] == ',' || str[l] == '\r' || str[l] == '\n' || str[l] == '\0')
        {
            cout << "error" << endl;
            return (-1);
        }
    }
    return 0;
}

void Server::chanPassNum(string chanName, string passName, struct kevent &event)
{
    size_t j = 0;
    for (size_t i = 0; i < chanName.size(); i++)
    {
        if (chanName[i] == ',')
            j++;
    }
    size_t chan_number = j + 1;
    size_t k = 0;
    for (size_t i = 0; i < passName.size(); i++)
    {
        if (passName[i] == ',')
            k++;
    }
    size_t pass_number = k + 1;
    if (pass_number > chan_number)
    {
        sendAnswer(event, ":server 464 :Password incorrect\r\n");
        return ;
    }
}

string Server::addUserToChan(struct kevent &event)
{
    unsigned long p = 0;
    for (list<struct kevent>::iterator q = fds.begin(); q != fds.end();q++)
    {
        if (q->ident == event.ident)
            break;
        p++;
    }
    string userToChan = "";
    list<string>::iterator iter = users.begin();
    for (unsigned long r = 0;r <= p; iter++, r++)
    {
        if (r == p)
            userToChan += *iter;
    }
    rooms[rooms.size()-1].users.push_back(userToChan);
    return (userToChan);
}

void Server::userAlreadyInChan(string chanCheck, struct kevent &event)
{
    for(vector<chatroom>::iterator iter2 = rooms.begin(); iter2 !=rooms.end(); iter2++)
    {
        if(iter2->name == chanCheck)//проверка, если уже есть такой канал
        {
            unsigned long p = 0;
            for (list<struct kevent>::iterator q = fds.begin(); q != fds.end();q++)//поиск текущего пользователя
            {
                if (q->ident == event.ident)
                    break;
                p++;
            }
            string userInChan = "";
            list<string>::iterator iter = users.begin();
            for (unsigned long r = 0;r <= p; iter++, r++)
            {
                if (r == p)
                    userInChan += *iter;
            }
            for(list<string>::iterator iter3 = iter2->users.begin(); iter3 != iter2->users.end(); iter3++)
            {//проверка, есть ли пользователь в списке данного канала
                if(*iter3 == userInChan)
                {
                    sendAnswer(event, ":server 443 "+chanCheck+" "+userInChan+" :is already on channel\r\n");
                    return ;
                }
                else
                    iter2->users.push_back(userInChan);
            }
        }
    }
}

void Server::cmdJOIN(string &str, struct kevent &event) //а если несколько имен каналов в одном сообщении? - сделано
{//добавить проверку, был ли этот канал создан ранее, если да - юзер просто юзер, если нет и это новый - юзер - оператор
// - будем просто проверять при применении команды оператора - юзер в листе по итератору 0 - и есть оператор
//добавить - у юзера не может быть больше 10 каналов
    if(str.size() < 8) //нужно было проверить - проверено
        sendAnswer(event, ":server 461 JOIN :Not enough parameters\r\n");
    else
    {
        string chanName = "";
        string chanNameLast = "";
        string passName = "";
        string passNameLast = "";
        string wherefind = str.substr((str.find("JOIN") + 4), (str.size() - 5));
        if ((wherefind.find('#') == 1) || (wherefind.find('&') == 1))
        {
            wherefind = wherefind.substr(wherefind.find_first_of(' ') + 1);
            chanName = split(wherefind, ' ');
            if (spaceCheck(wherefind) > 2)
            {
                sendAnswer(event, ":server 403 "+chanName+ ":No such channel\r\n");
                return ;
            } 
            if (spaceCheck(wherefind) > 0 && spaceCheck(wherefind) < 2)
                passName = wherefind.substr(wherefind.find(' ') + 1);//исправлено - не работает корректно если ввести только одно слово(имя канала) 
            cout << wherefind << " - wherefind name" << endl;
            cout << chanName << " - channel name" << endl;
            cout << passName << " - pass name" << endl;
            chanPassNum(chanName, passName, event);//проверка чтобы паролей было не больше каналов
            size_t i = 0;
            string userInChan = "";
            while(i < chanName.size())//проверка имен каналов
            {
                if (chanName[i] == ',')
                {
                    string chanCheck = split(chanName, ',');
                    // userAlreadyInChan(chanCheck, event);
                    for(vector<chatroom>::iterator iter2 = rooms.begin(); iter2 !=rooms.end(); iter2++)
                    {
                        if(iter2->name == chanCheck)//проверка, если уже есть такой канал
                        {
                            unsigned long p = 0;
                            for (list<struct kevent>::iterator q = fds.begin(); q != fds.end();q++)//поиск текущего пользователя
                            {
                                if (q->ident == event.ident)
                                    break;
                                p++;
                            }
                            
                            list<string>::iterator iter = users.begin();
                            for (unsigned long r = 0;r <= p; iter++, r++)
                            {
                                if (r == p)
                                    userInChan += *iter;
                            }
                            for(list<string>::iterator iter3 = iter2->users.begin(); iter3 != iter2->users.end(); iter3++)
                            {//проверка, есть ли пользователь в списке данного канала
                                if(*iter3 == userInChan)
                                {
                                    sendAnswer(event, ":server 443 "+chanCheck+" "+userInChan+" :is already on channel\r\n");
                                    cout << "11111" << endl;
                                    return ;
                                }
                                else
                                    iter2->users.push_back(userInChan);
                            }
                            cout << userInChan << "21" << endl;
                        }
                    }
                    if(chanCheck.size() > 200 || (chanCheck[0] != '#' && chanCheck[0] != '&') || channelNameCheck(chanCheck) < 0)
                    {//некорректно обрабатывает # & в начале - сделано
                        sendAnswer(event, ":server 403 "+chanCheck+ ":No such channel\r\n");
                        return ;
                    }
                    chatroom a(chanCheck, "");
                    rooms.push_back(a);
                    string userToChan = addUserToChan(event);
                    if(rooms[rooms.size()-1].topic == "")//дописать ф-ю вывода списка юзеров в данном канале
                        sendAnswer(event, ":server 331 "+userToChan+" "+chanCheck+ " :No topic is set\r\n");
                    else
                        sendAnswer(event, ":server 332 "+userToChan+" "+chanCheck+" :"+rooms[rooms.size()-1].topic+ "\r\n");
                    chanName = chanName.substr(chanName.find(','));
                    i = 0;
                }   
                i++;    
            }
            chanNameLast = chanName.substr(chanName.find(',') + 1);//проверка и добавление последнего имени канала
            chanNameLast = split(chanNameLast, '\r');
            for(vector<chatroom>::iterator iter2 = rooms.begin(); iter2 !=rooms.end(); iter2++)
            {
                if(iter2->name == chanNameLast)//проверка, если уже есть такой канал
                {
                    unsigned long p = 0;
                    for (list<struct kevent>::iterator q = fds.begin(); q != fds.end();q++)//поиск текущего пользователя
                    {
                        if (q->ident == event.ident)
                            break;
                        p++;
                    }
                    
                    list<string>::iterator iter = users.begin();
                    for (unsigned long r = 0;r <= p; iter++, r++)
                    {
                        if (r == p)
                            userInChan += *iter;
                    }
                    for(list<string>::iterator iter3 = iter2->users.begin(); iter3 != iter2->users.end(); iter3++)
                    {//проверка, есть ли пользователь в списке данного канала
                        if(*iter3 == userInChan)
                        {
                            sendAnswer(event, ":server 443 "+chanNameLast+" "+userInChan+" :is already on channel\r\n");
                            cout << "22222" << endl;
                            return ;
                        }
                        else
                            iter2->users.push_back(userInChan);
                    }
                }
            }
            if(chanNameLast.size() > 200 || (chanNameLast[0] != '#' && chanNameLast[0] != '&') || channelNameCheck(chanNameLast) < 0)
            {
                sendAnswer(event, ":server 403 "+chanNameLast+ " :No such channel\r\n");
                return ;
            }
            chatroom b(chanNameLast, "");
            rooms.push_back(b);
            string userToChan = addUserToChan(event);
            if(rooms[rooms.size()-1].topic == "")
                sendAnswer(event, ":server 331 "+userToChan+" "+chanNameLast+ " :No topic is set\r\n");
            else
                sendAnswer(event, ":server 332 "+userToChan+" "+chanNameLast+" :"+rooms[rooms.size()-1].topic+ "\r\n");//что тут тоже надо было проверить??
            size_t l = 0;//добавить добавление паролей - добавлено
            size_t m = 0;
            while (l < passName.size())//проверка и добавление паролей
            {
                if (passName[l] == ',')
                {
                    string passCheck = split(passName, ',');
                    rooms[m].pass = passCheck;
                    m++;
                    passName = passName.substr(passName.find(','));
                    l = 0;
                }
                l++;
            }
            passNameLast = passName.substr(passName.find(',') + 1);//проверка и добавление последнего пароля
            rooms[m].pass = passNameLast;
            for(vector<chatroom>::iterator it = rooms.begin(); it !=rooms.end(); it++)
            {
                it->printName();
                it->printPass();
                for(list<string>::iterator iterat = it->users.begin(); iterat != it->users.end(); iterat++)
                    cout << *iterat << endl;
                cout << "1" << endl;
            }   
        }
        else
           sendAnswer(event, ":server 403 :No such channel\r\n");
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
    if (Find(str, "JOIN") == 0)
        cmdJOIN(str, event);
    return ret;
}