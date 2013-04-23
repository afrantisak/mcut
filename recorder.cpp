#include "receiver.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include "ArgParser.h"

struct Options
{
    typedef std::string Ip;
    typedef short Port;
    
    Options(int argc, char* argv[])
    {
        ArgParser args("recorder");
        args.add_option("local_ip", "local ip address");
        args.add_option("remote_ip", "remote ip address");
        args.add_option("remote_port", "remote port");
        
        args.parse_args(argc, argv);

        sLocalIp = args.value("local_ip").as<Ip>();
        sRemoteIp = args.value("remote_ip").as<Ip>();
        nPort = boost::lexical_cast<Port>(args.value("remote_port").as<std::string>());
    }
    
    Ip sLocalIp;
    Ip sRemoteIp;
    Port nPort;
};

int main(int argc, char* argv[])
{
    try
    {
        Options options(argc, argv);
    
        Receiver::Address local_address = boost::asio::ip::address::from_string(options.sLocalIp);
        Receiver::Address remote_address = boost::asio::ip::address::from_string(options.sRemoteIp);
        short remote_port = options.nPort;
        
        std::cout << "Listening on " << local_address << " " << remote_address << ":" << remote_port << std::endl;

        boost::asio::io_service service;
        Receiver receiver(service, local_address, remote_address, remote_port, 
                   [](const void* pData, size_t nBytes) -> bool
        {
            std::cout << static_cast<const char*>(pData);
            return true;
        });
        service.run();
    }
    catch (int n)
    {
        return n;
    }
    catch (std::exception& e)
   {
        if (e.what())
            std::cerr << "exception: " << e.what() << "\n";
    }

    return 0;
}
