#include "receiver.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <boost/asio.hpp>
#include "ArgParser.h"

struct Options
{
    typedef std::string Ip;
    typedef short Port;
    typedef std::string FileName;
    
    Ip sLocalIp;
    Ip sRemoteIp;
    Port nPort;
    bool bOutputAscii;
    bool bOutputHex;
    FileName sFileName;

    Options(int argc, char* argv[])
    {
        ArgParser args("recorder");
        args.add_option("local-ip", sLocalIp, "local ip address");
        args.add_option("remote-ip", sRemoteIp, "remote ip address");
        args.add_option("remote-port", nPort, "remote port");
        args.add_option("--output-ascii", bOutputAscii, "output character data");
        args.add_option("--output-hex", bOutputHex, "output hex codes");
        args.add_option("--output-file", sFileName, "output to file");
        
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
        
        boost::asio::io_service service;
        Receiver receiver(service, local_address, remote_address, remote_port, sinkCallback);
        service.run();
        
        return true;
    }
    
private:
    Ip m_sLocalIp;
    Channel m_channel;
};

bool recordToStreamBinary(Recorder& record, std::ostream& strm)
{
    record([&](const void* pData, size_t nBytes) -> bool
    {
        strm.write(static_cast<const char*>(pData), nBytes);
        strm.flush();
        return true;
    });
}

bool recordToStreamText(Recorder& record, std::ostream& strm, bool bAscii, bool bHex)
{
    record([&](const void* pData, size_t nBytes) -> bool
    {
        if (bAscii)
        {
            const char* pChar = static_cast<const char*>(pData);
            size_t nBytesLeft = nBytes;
            while (nBytesLeft--)
            {
                strm << *(pChar++) << "  ";
            }
            strm << std::endl;
        }
        if (bHex)
        {
            const char* pChar = static_cast<const char*>(pData);
            size_t nBytesLeft = nBytes;
            while (nBytesLeft--)
            {
                strm << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(*pChar++) << " " << std::dec;
            }
            strm << std::endl;
        }
        return true;
    });
}

int main(int argc, char* argv[])
{
    try
    {
        // get command line / config file options
        Options options(argc, argv);
        std::cout << "Listening on " << options.sLocalIp << " " << options.sRemoteIp
                  << ":" << options.nPort << std::endl;
        Channel channel = { "Default", options.sRemoteIp, options.nPort };
        Recorder recorder(options.sLocalIp, channel);
        
        if (options.sFileName.size())
        {
            std::ofstream stream(options.sFileName, std::ofstream::binary);
            recordToStreamBinary(recorder, stream);
        }
        else if (options.bOutputAscii || options.bOutputHex)
        {
            recordToStreamText(recorder, std::cout, options.bOutputAscii, options.bOutputHex);
        }
        else
        {
            recordToStreamBinary(recorder, std::cout);
        }
    }
    catch (int n)
    {
        return n;
    }
    catch (std::exception& e)
    {
        if (e.what())
            std::cerr << "exception: " << e.what() << "\n";
        return 127;
    }

    return 0;
}
