#include "channel_mode.hpp"

template<typename T>
static std::string to_string(const T & value) 
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

static void erasechar(Args& arg, char c)
{
    if (arg.arg2.find(c) != std::string::npos)
        arg.arg2.erase(arg.arg2.begin() + arg.arg2.find(c));
}

ChannelMode::ChannelMode() : opper_(0)
{
    topic_ = new Topic();
}

ChannelMode::~ChannelMode()
{
    delete topic_;
}

void    ChannelMode::setChan(std::string name)
{
    channel_ = name;
}

bool    ChannelMode::Invite() const
{
    return (opper_ & (1<<1));
}

void    ChannelMode::removeInvite(const Client& cl)
{
    for(std::set<Client *>::iterator it = inventation_.begin(); it != inventation_.end(); it++)
    {
        if ((*it)->getNick() == cl.getNick())
        {
            this->inventation_.erase(it);
            return;
        }
    }
}

bool    ChannelMode::isInvite(const Client& cl)
{
    for(std::set<Client *>::iterator it = inventation_.begin(); it != inventation_.end(); it++)
    {
        if ((*it)->getNick() == cl.getNick())
            return true;
    }
    return false;
}

void    ChannelMode::addInvite(Client& cl)
{
    inventation_.insert(&cl);
}

void    ChannelMode::seton(char c, std::istringstream& ss, Client& cl, std::string& reply, const CmdController& controller)
{
    std::string word;
    Args* arg = &controller.getParser().getArgument();
    switch (c)
    {
        case 'o':
            if (ss>>word && word.size() > 0)
            {
                Channel *channel = &controller.getServer().getChannel(channel_);
                if (channel->isInChannel(word))
                {
                    channel->addOperator(*controller.getServer().getClientByName(word));
                    if (arg->arg4 == "")
                        arg->arg4 = word;
                    else
                        arg->arg4 += " " + word;
                }
                else
                    erasechar(*arg, 'o');
            }
            else
                reply = std::string(ERR_HELPOPERATOR(cl.getNick(), channel_));
            break;
        case 'i': opper_ |= (1 << 1);
            break;
        case 't': opper_ |= (1 << 2);
            break;
        case 'n': opper_ |= (1 << 3);
            break;
        case 'l':
            if (ss>>word && atoi(word.c_str()) > 0)
            {
                opper_ |= (1 << 4);
                setClientLimit(atoi(word.c_str()));
                if (arg->arg4 == "")
                    arg->arg4 = word;
                else
                    arg->arg4 += " " + word;
            }
            else
                erasechar(*arg, 'l');
            break;
        case 'b':
            if (ss>>word && word.size() > 0)
            {
                setBan(word, cl.getNick());
                if (arg->arg4 == "")
                    arg->arg4 = word;
                else
                    arg->arg4 += " " + word;
            }
            else if(reply == "" && arg->arg2.length() == 2)
                getBan(cl.getNick(), reply);
            else
                erasechar(*arg, 'b');
            break;
        case 'k':
            if (ss>>word && word.size() > 0)
            {
                opper_ |= (1 << 6);
                setPassword(word);
                if (arg->arg4 == "")
                    arg->arg4 = word;
                else
                    arg->arg4 += " " + word;
            }
            else
                erasechar(*arg, 'k');     
            break;
        default:
            erasechar(*arg, c);
            break;
    }
}

void    ChannelMode::setoff(char c, std::istringstream& ss, Client& cl, std::string& reply, const CmdController& controller)
{
    std::string word;
    Args* arg = &controller.getParser().getArgument();
    switch (c)
    {
        case 'o': 
            if (ss>>word && word.size() > 0)
            {
                Channel *channel = &controller.getServer().getChannel(channel_);
                if (channel->isInChannel(word))
                {
                    channel->removeOperator(*controller.getServer().getClientByName(word));
                    if (arg->arg4 == "")
                        arg->arg4 = word;
                    else
                        arg->arg4 += " " + word;
                }
                else
                    erasechar(*arg, 'o');
            }
            else
                reply = std::string(ERR_HELPOPERATOR(cl.getNick(), channel_));
            break;
        case 'i': opper_ &= ~(1 << 1);
            break;
        case 't': opper_ &= ~(1 << 2);
            break;
        case 'n': opper_ &= ~(1 << 3);
            break;
        case 'l': opper_ &= ~(1 << 4);
            break;
        case 'b':
            if (ss>>word && word.size() > 0)
            {
                    offBan(word);
                if (arg->arg4 == "")
                    arg->arg4 = word;
                else
                    arg->arg4 += " " + word;
            }
            else
                erasechar(*arg, 'b');
            break;
        case 'k': opper_ &= ~(1 << 6);            
            break;
        default:
            erasechar(*arg, c);
            break;
    }
}

void    ChannelMode::setMode(const CmdController& controller, Client& cl, std::string& reply)
{
    std::string str = controller.getParser().getArgument().arg2;
    std::istringstream  ss(controller.getParser().getArgument().arg3);
    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] == '-')
        {
            i++;
            while(i < str.length() && str[i] != '+')
                setoff(str[i++], ss, cl, reply, controller);
        }
        if (i < str.length() && str[i] == '+')
        {
            i++;
            while(i < str.length() && str[i] != '-')
                seton(str[i++], ss, cl, reply, controller);
        }
    }
}

char    ChannelMode::getModus() const
{
    return opper_;
}

int     ChannelMode::getClientLimit() const
{
    return clientLimit_;
}

void    ChannelMode::setClientLimit(int limit)
{
    clientLimit_ = limit;
}

std::string     ChannelMode::getPassword() const
{
    return password_;
}

void    ChannelMode::setPassword(std::string str)
{
    password_ = str;
}

void    ChannelMode::setBan(std::string name, std::string nick)
{
    name += "!*@*";
    for (ban_type::iterator it = banList_.begin(); it != banList_.end(); it++)
    {
        if (it->first == name)
            return;
    }
    banList_.push_back(std::make_pair(name, std::make_pair((unsigned)time(NULL), nick)));
}

void    ChannelMode::offBan(std::string name)
{
    for (ban_type::iterator it = banList_.begin(); it != banList_.end(); it++)
    {
        if (it->first == name)
        {
            banList_.erase(it);
            return;
        }
    }
}

std::string ChannelMode::getBan(std::string nick, std::string& reply)
{
    reply = "";
    for (ban_type::iterator it = banList_.begin(); it != banList_.end(); it++)
        reply += RPL_BANLIST(nick, it->second.second, channel_, it->first, to_string(it->second.first));
    reply += RPL_ENDOFBANLIST(nick, channel_);
    return reply;
}

bool        ChannelMode::isBan(std::string name)
{
    name += "!*@*";
    for (ban_type::iterator it = banList_.begin(); it != banList_.end(); it++)
    {
        if (it->first == name)
            return true;
    }
    return false;
}

std::string     ChannelMode::isModeOn()
{
    std::string reply = "";

    if (opper_ & (1<<0))
        reply += "o";
    if (opper_ & (1<<1))
        reply += "i";
    if (opper_ & (1<<2))
        reply += "t";
    if (opper_ & (1<<3))
        reply += "n";
    if (opper_ & (1<<4))
        reply += "l";
    if (opper_ & (1<<5))
        reply += "b";
    if (opper_ & (1<<6))
        reply += "k";
    return reply;
}

void        ChannelMode::setTopic(std::string msg, std::string nick, std::string time, Client& writer)
{
    topic_->creationTimeTopic = time;
    topic_->topic = msg;
    topic_->topic_nick = nick;
    topic_->writer = &writer;
}

const Topic&      ChannelMode::getTopic() const
{
    return *topic_;
}
