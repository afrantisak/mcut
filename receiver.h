#pragma once
#include <vector>
#include <string>
#include <functional>
#include <boost/asio.hpp>

struct Channel
{
    typedef std::string Name;
    typedef std::string Ip;
    typedef unsigned short Port;
    
    Name name;
    Ip ip;
    Port port;
};

class Receiver
{
public:
    typedef boost::asio::io_service Service;
    typedef boost::asio::ip::address Address;
    typedef unsigned short Port;
    typedef std::function<bool(const void*, size_t)> Callback;
    
    Receiver(Service& io_service, const Address& local_address, 
             const Address& remote_address, const Port& remote_port, Callback callback);

protected:
    void async_receive_next();

private:
    typedef boost::asio::ip::udp::socket Socket;
    typedef boost::asio::ip::udp::endpoint Endpoint;
    typedef boost::system::error_code ErrorCode;
    typedef std::vector<char> Buffer;

    Socket m_socket;
    Endpoint m_remote_endpoint;
    Callback m_callback;
    Buffer m_buffer;
};

