#include "ArgParser.h"
#include "receiver.h"
#include "Source.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <boost/asio.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

struct Options
{
    typedef std::string Ip;
    typedef short Port;
    typedef std::string FileName;
    
    Ip sLocalIp;
    Ip sRemoteIp;
    Port nPort;
    FileName sFileName;

    Options(int argc, char* argv[])
    {
        ArgParser args("mc-wrap");
        args.add("local-ip", sLocalIp, "local ip address");
        args.add("remote-ip", sRemoteIp, "remote ip address");
        args.add("remote-port", nPort, "remote port");
        args.add("--output-file", sFileName, "output to file");
        
        args.parse(argc, argv);
    }
};

struct PacketHeader
{
    typedef uint32_t Length;
    typedef uint32_t Pad;
    typedef uint64_t Timestamp;

    PacketHeader(Length nLength)
        :   m_nLength(nLength),
            m_nTimestamp(0)
    {
        clock_gettime(CLOCK_MONOTONIC, reinterpret_cast<timespec*>(&m_nTimestamp));
    }
        
    Length m_nLength;
    Timestamp m_nTimestamp;
};

using namespace mcut;

void recordPacket(Source& source, std::ostream& strm)
{
    source([&](const void* pData, size_t nBytes) -> bool
    {
        PacketHeader hdr(nBytes);
        strm.write(reinterpret_cast<const char*>(&hdr), sizeof(hdr));
        strm.write(static_cast<const char*>(pData), nBytes);
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
        Source source(options.sLocalIp, channel);
        
        if (options.sFileName.size())
        {
            std::cout << "Writing to " << options.sFileName << std::endl;
            std::ofstream stream(options.sFileName, std::ofstream::binary);
            recordPacket(source, stream);
        }
        else
        {
            recordPacket(source, std::cout);
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
