#include "receiver.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>

namespace po = boost::program_options;

struct Options
{
    std::string sLocalIp;
    std::string sRemoteIp;
    unsigned int nPort;
};

void help(std::ostream& out, const po::options_description& desc)
{
    out << "Usage: recorder [options] <local_ip> <remote_ip> <remote_port>" << std::endl;
    out << desc << std::endl;
}

Options getOptions(int argc, char* argv[])
{
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("local_ip", "local ip address")
        ("remote_ip", "remote ip address")
        ("remote_port", "remote port");
    
    po::positional_options_description p;
    p.add("local_ip", 1);
    p.add("remote_ip", 1);
    p.add("remote_port", 1);
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);    

    if (vm.count("help")) {
        help(std::cout, desc);
        throw 1;
    }
    
    if (vm.count("local_ip") == 0)
    {
        std::cout << "ERROR: local_ip is required" << std::endl;
        throw 1;
    }
    
    if (vm.count("remote_ip") == 0)
    {
        std::cout << "ERROR: remote_ip is required" << std::endl;
        throw 1;
    }
    
    if (vm.count("remote_port") == 0)
    {
        std::cout << "ERROR: remote_port is required" << std::endl;
        throw 1;
    }
    
    Options options;
    
    options.sLocalIp = vm["local_ip"].as<std::string>();
    options.sRemoteIp = vm["remote_ip"].as<std::string>();
    options.nPort = boost::lexical_cast<unsigned int>(vm["remote_port"].as<std::string>());
    
    return options;    
};

int main(int argc, char* argv[])
{
    try
    {
        Options options = getOptions(argc, argv);
    
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
