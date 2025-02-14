#ifndef CHANNEL_MODE_HPP
# define CHANNEL_MODE_HPP

# include "client.hpp"
# include "replies.hpp"
# include "channel_topic.hpp"
# include "cmd_controller.hpp"

# include <list>

/*
    opper(ator)_
    0 = o   4 = l
    1 = i   5 = b
    2 = t   6 = k
    3 = n   
*/

class Client;
class Topic;
class CmdController;

class ChannelMode
{
    typedef std::list<std::pair<std::string, std::pair<unsigned int, std::string> > > ban_type;
    public:
        ChannelMode();
        ~ChannelMode();

        void        setChan(std::string name);

        bool        isInvite(const Client& cl);
        void        removeInvite(const Client& cl);
        bool        Invite() const;
        void        addInvite(Client& cl);
        void        setMode(const CmdController& controller, Client& cl, std::string& reply);
        void        seton(char c, std::istringstream& ss, Client& cl, std::string& reply, const CmdController& controller);
        void        setoff(char c, std::istringstream& ss, Client& cl, std::string& reply, const CmdController& controller);
        char        getModus() const;
        std::string isModeOn();


        int         getClientLimit() const;
        void        setClientLimit(int limit);
        
        void        setBan(std::string name, std::string nick);
        void        offBan(std::string name);
        bool        isBan(std::string name);
        std::string getBan(std::string nick, std::string& reply);
        

        std::string getPassword() const;
        void        setPassword(std::string str);

        void            setTopic(std::string msg, std::string nick, std::string time, Client& writer);
        const Topic&    getTopic() const;

    private:
        std::set<Client *>  inventation_;
        char                opper_;
        int                 clientLimit_;
        std::string         password_;
        std::string         channel_;
        ban_type            banList_;
        Topic*              topic_;
};

#endif