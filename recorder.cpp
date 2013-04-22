#include "receiver.h"
#include <iostream>
#include <boost/asio.hpp>

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: recorder <listen_address> <multicast_address>\n";
            std::cerr << "  For IPv4, try:\n";
            std::cerr << "    receiver 0.0.0.0 239.255.0.1\n";
            std::cerr << "  For IPv6, try:\n";
            std::cerr << "    receiver 0::0 ff31::8000:1234\n";
            return 1;
        }
        
        Receiver::Address local_address = boost::asio::ip::address::from_string(argv[1]);
        Receiver::Address remote_address = boost::asio::ip::address::from_string(argv[2]);

        const short multicast_port = 5000;

        boost::asio::io_service service;
        Receiver receiver(service, local_address, remote_address, multicast_port, 
                   [](const void* pData, size_t nBytes) -> bool
        {
            std::cout << static_cast<const char*>(pData) << std::endl;
            return true;
        });
        service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
