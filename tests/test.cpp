#include <catch2/catch_all.hpp>

#include <iostream>

#include "tcp_stream.hpp"
#include "tcp_connector.hpp"
#include "tcp_acceptor.hpp"

TEST_CASE("Class instantiates")
{
    TcpStream       stream;
    // TcpConnector    client;
    TcpAcceptor     server(8080, "127.0.0.1");
}