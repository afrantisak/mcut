#include "ArgParser.h"
#include "Source.h"
#include <iostream>
#include <fstream>
#include <iomanip>

struct Options
{
    typedef std::string Ip;
    typedef short Port;
    typedef std::string FileName;
    
    Ip sLocalIp;
    Ip sRemoteIp;
    Port nPort;
    FileName sFileName;
    bool bDebug;

    Options(int argc, char* argv[])
    {
        ArgParser args("mc-recv-pkt");
        args.add("local-ip", sLocalIp, "local ip address");
        args.add("remote-ip", sRemoteIp, "remote ip address");
        args.add("remote-port", nPort, "remote port");
        args.add("--output-file", sFileName, "output to file");
        args.add("--debug", bDebug, "debug packet details");
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

void recordPacket(mcut::Source& source, std::ostream& strm)
{
    source([&](const void* pData, size_t nBytes) -> bool
    {
        PacketHeader hdr(nBytes);
        strm.write(reinterpret_cast<const char*>(&hdr), sizeof(hdr));
        strm.write(static_cast<const char*>(pData), nBytes);
        return true;
    });
}

void debugPacket(mcut::Source& source, std::ostream& strm)
{
    source([&](const void* pData, size_t nBytes) -> bool
    {
        PacketHeader hdr(nBytes);
        strm << "Pkt(" << nBytes << "): ";

        // TODO: dump header, too
        const char* pChar = static_cast<const char*>(pData);
        size_t nBytesLeft = nBytes;
        while (nBytesLeft--)
        {
            strm << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(*pChar++) << " " << std::dec;
        }
        strm << std::endl;

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
        mcut::Channel channel = { "Default", options.sRemoteIp, options.nPort };
        mcut::Source source(options.sLocalIp, channel);
        
        // TODO: need clean ctrl-c handler so file doesn't get corrupted
        
        if (options.sFileName.size())
        {
            std::cout << "Writing to " << options.sFileName << std::endl;
            std::ofstream stream(options.sFileName, std::ofstream::binary);
            recordPacket(source, stream);
        }
        else
        {
            if (options.bDebug)
                debugPacket(source, std::cout);
            else
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
