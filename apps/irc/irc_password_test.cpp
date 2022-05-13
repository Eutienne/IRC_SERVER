
#include "irc_server.hpp"
#include "connectable_client.hpp"

#include <string>
#include <iostream>
#include <thread>

#include <assert.h>

IrcServer g_server(8080, "hello");

ConnectableClient g_client(8080, "127.0.0.1");
ConnectableClient g_client_2(8080, "127.0.0.1");


void serverJob()
{
    g_server.start();
}

void testPassword()
{
    g_client.connect("User1");

    g_client.send("PASS hello\nCAP LS\nNICK User1\nUSER usr usr usr :usr\n");
    sleep(1);
    /*
    std::string response = g_client.receive();
    assert(!response.empty());
    std::cout << response << "\n";
    */

    g_client_2.connect("User1");
    g_client_2.send("PASS wrongPassword\n");
    sleep(1);
    std::string response = g_client_2.receive();
    std::cout << "RES RECEIVED----------->|" << response << "|" << std::endl;
    assert(!response.empty() && response == ":Password incorrect\n");
    std::cout << response << "\n";
}

int main(void)
{
    std::thread server_thread(serverJob);
    sleep(3);

    assert(g_server.isRunning() == true);
    testPassword();

    g_server.stop();
    assert(g_server.isRunning() == false);
    server_thread.join();

    return (0);
}