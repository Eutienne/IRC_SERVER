#include "irc_server.hpp"
#include "cmd_controller.hpp"
#include "print.hpp"

IrcServer::IrcServer(int port, std::string password)
{
    std::cout << "CALL CONSTRUCTOR...\n";
    _mServer = new Server(port, password);
}

IrcServer::IrcServer(const IrcServer &server)
{

    std::cout << server._mNbclients<< "COPY CONSTRUCTOR...";
}

IrcServer::~IrcServer()
{
    std::vector<Client *>::iterator it = clients_.begin();
    while (it != clients_.end())
    {
        std::cout << "REMOVING->" << *it << "\n";
        delete *it;
        it++;
    }
    clients_.clear();
}

void    IrcServer::start()
{
    _mServer->init();
    print("INFO", "starting server...");
    CmdController cmd(*this);
    while (_mServer->isRunning())
    {
        _mServer->runOnce();
        while (Message *msg = &_mServer->getNextMsg())
        {
            if (msg)
            {
                #if 1
                    print("DEBUG", "incoming msg - " + msg->getData());
                #endif
                cmd.execute(msg);
                std::cout << "AFTER EXEC\n";
                delete(msg);
            }
            else
            {
                std::cout << "BREAKING...\n";
                break;
            }
            std::cout << "msg\n";
        }
    }
}

bool IrcServer::isNickInUse(const std::string &nickname)
{
    std::vector<Client *>::const_iterator it = clients_.begin();
    while (it != clients_.end())
    {
        if ((*it)->getNick() == nickname)
            return true;
        it++;
    }
    return false;
}

Client* IrcServer::getClientByName(std::string name)
{
    std::vector<Client *>::const_iterator it = clients_.begin();
    while (it != clients_.end())
    {
        if ((*it)->getNick() == name)
            return *it;
        it++;
    }
    return NULL;
}

Client* IrcServer::getClientByStream(TcpStream *stream)
{
    std::vector<Client *>::const_iterator it = clients_.begin();
    while (it != clients_.end())
    {
        if ((*it)->getStream() == *stream)
            return *it;
        it++;
    }
    return NULL;
}

void IrcServer::addClient(Client *cl)
{
    clients_.push_back(cl);
}

void IrcServer::removeClient(Client *cl)
{
    std::vector<Client *>::iterator it = clients_.begin();
    while (it != clients_.end())
    {
        if (*it == cl)
            break;
        it++;
    }
    if (it != clients_.end())
    {
        delete *it;
        clients_.erase(it);
    }
}

bool    IrcServer::isChannel(std::string channel)
{
    if (this->channel_.find(channel) != this->channel_.end())
        return true;
    return false;
}

bool    IrcServer::isInChannel(std::string channel, std::string nick)
{
    if (this->channel_[channel].find(nick) != this->channel_[channel].end())
        return true;
    else if (this->channel_[channel].find("@" + nick) != this->channel_[channel].end())
        return true;
    return false;
}

void    IrcServer::addChannel(std::string channel)
{
    std::set<std::string> set;
    this->channel_.insert(std::make_pair(channel, set));
}

void    IrcServer::addInChannel(std::string channel, std::string nick)
{
    this->channel_[channel].insert(nick);
}

void    IrcServer::removeInChannel(std::string channel, std::string nick)
{
    if (this->channel_[channel].find(nick) != this->channel_[channel].end())
        this->channel_[channel].erase(nick);
    else if (this->channel_[channel].find("@" + nick) != this->channel_[channel].end())
        this->channel_[channel].erase('@' + nick);
}

std::set<std::string>&   IrcServer::getChannel(std::string channel)
{
    return this->channel_[channel];
}

void IrcServer::stop(void)
{
    std::cout << "Stopping server...\n";
     _mServer->stop();
}

bool IrcServer::isRunning(void) const { return _mServer->isRunning(); }
