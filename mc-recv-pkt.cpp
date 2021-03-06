#include "ConsoleInterrupt.h"
#include "ArgParser.h"
#include "Packet.h"
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

ConsoleInterrupt g_interrupt;
        
void recordPackets(mcut::Source& source, std::ostream& strm)
{
    source([&](const void* pData, size_t nBytes) -> bool
    {
        // check if we were interrupted
        g_interrupt.triggerThrow();

        Packet::writeRawBinary(strm, pData, nBytes);
        return true;
    });
}

void debugPackets(mcut::Source& source, std::ostream& strm)
{
    source([&](const void* pData, size_t nBytes) -> bool
    {
        // check if we were interrupted
        g_interrupt.triggerThrow();

        Packet::writeDebugText(strm, pData, nBytes);
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
        
        if (options.sFileName.size())
        {
            std::cout << "Writing to " << options.sFileName << std::endl;
            std::ofstream stream(options.sFileName, std::ofstream::binary);
            recordPackets(source, stream);
        }
        else
        {
            if (options.bDebug)
                debugPackets(source, std::cout);
            else
                recordPackets(source, std::cout);
        }
    }
    catch (int n)
    {
        return n;
    }
    catch (ConsoleInterrupt::Interrupted e)
    {
        std::cout << "Interrupted!" << std::endl;
    }
    catch (std::exception& e)
    {
        if (e.what())
            std::cerr << "exception: " << e.what() << "\n";
        return 127;
    }

    return 0;
}
