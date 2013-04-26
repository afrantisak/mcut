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
    bool bOutFmtAscii;
    bool bOutFmtHex;
    FileName sFileName;

    Options(int argc, char* argv[])
    {
        ArgParser args("mc-echo");
        args.add("local-ip", sLocalIp, "local ip address");
        args.add("remote-ip", sRemoteIp, "remote ip address");
        args.add("remote-port", nPort, "remote port");
        args.add("--outfmt-ascii", bOutFmtAscii, "output character data");
        args.add("--outfmt-hex", bOutFmtHex, "output hex codes");
        args.add("--output-file", sFileName, "output to file");
        
        // TODO: load from a config file first then override options from the command line
        args.parse(argc, argv);
    }
};

using namespace mcut;

bool recordRaw(Source& source, std::ostream& strm)
{
    source([&](const void* pData, size_t nBytes) -> bool
    {
        strm.write(static_cast<const char*>(pData), nBytes);
        strm.flush();
        return true;
    });
}

bool recordText(Source& source, std::ostream& strm, bool bAscii, bool bHex)
{
    source([&](const void* pData, size_t nBytes) -> bool
    {
        if (bAscii && bHex)
        {
            // TODO do something cool here
            strm << "<NOT IMPLEMENTED>";
        }
        else if (bAscii)
        {
            const char* pChar = static_cast<const char*>(pData);
            size_t nBytesLeft = nBytes;
            while (nBytesLeft--)
            {
                strm << *(pChar++) << " ";
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
        Source source(options.sLocalIp, channel);
        
        if (options.sFileName.size())
        {
            std::cout << "Writing to " << options.sFileName << std::endl;
            std::ofstream stream(options.sFileName, std::ofstream::binary);
            recordRaw(source, stream);
        }
        else if (options.bOutFmtAscii || options.bOutFmtHex)
        {
            recordText(source, std::cout, options.bOutFmtAscii, options.bOutFmtHex);
        }
        else
        {
            recordRaw(source, std::cout);
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
