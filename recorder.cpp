#include "receiver.h"
#include <iostream>
#include <boost/asio.hpp>
#include "ArgParser.h"

struct Options
{
    typedef std::string Ip;
    typedef short Port;
    
    Ip sLocalIp;
    Ip sRemoteIp;
    Port nPort;
    bool bOutputAscii;
    bool bOutputHex;

    Options(int argc, char* argv[])
    {
        ArgParser args("recorder");
        args.add_option("local-ip", "local ip address", sLocalIp);
        args.add_option("remote-ip", "remote ip address", sRemoteIp);
        args.add_option("remote-port", "remote port", nPort);
        args.add_option("--output-ascii", "output character data", bOutputAscii);
        args.add_option("--output-hex", "output hex codes", bOutputHex);
        
        // TODO: load from a config file first
        // override options from the command line
        args.parse_args(argc, argv);
    }
};

// TODO: consolidate Recorder and receiver so all the boost asio is hidden and in one place
class Recorder
{
public:
    typedef std::function<bool(const void* pData, size_t nBytes)> SinkCallback;
    typedef std::string Ip;
    
    Recorder(const Ip& sLocalIp, const Channel& channel)
    :   m_sLocalIp(sLocalIp),
        m_channel(channel)
    {
    }
    
    bool operator()(SinkCallback sinkCallback)
    {
        Receiver::Address local_address = boost::asio::ip::address::from_string(m_sLocalIp);
        Receiver::Address remote_address = boost::asio::ip::address::from_string(m_channel.ip);
        short remote_port = m_channel.port;
        
        std::cout << "Listening on " << local_address << " " << remote_address << ":" << remote_port << std::endl;

        boost::asio::io_service service;
        Receiver receiver(service, local_address, remote_address, remote_port, sinkCallback);
        service.run();
        
        return true;
    }
    
private:
    Ip m_sLocalIp;
    Channel m_channel;
};

int main(int argc, char* argv[])
{
    try
    {
        // get command line / config file options
        Options options(argc, argv);
        
        // create channel struct from options
        Channel channel = { "Default", options.sRemoteIp, options.nPort };
        
        std::cout << options.bOutputAscii << std::endl;
        std::cout << options.bOutputHex << std::endl;

        // set up source/sink
        Recorder(options.sLocalIp, channel)([](const void* pData, size_t nBytes) -> bool
        {
            std::cout << static_cast<const char*>(pData);
            return true;
        });
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
