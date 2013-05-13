#include "ConsoleInterrupt.h"
#include "ArgParser.h"
#include "Packet.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <memory>
#include "BigSink.h"
#include <boost/iostreams/stream.hpp>

struct Options
{
    std::string sFileName;
    std::string sFileExt;
    bool bDebug;
    bool bMmap;
    size_t nCount;

    Options(int argc, char* argv[])
    {
        sFileExt = "txt";
        nCount = 10;
        ArgParser args("mc-recv-pkt");
        args.add("--output-file", sFileName, "output file name");
        args.add("--output-ext", sFileExt, "output file extension");
        args.add("--debug", bDebug, "debug packet details");
        args.add("--mmap", bMmap, "use memory-mapped file");
        args.add("--count", nCount, "iterations");
        args.parse(argc, argv);
    }
};

ConsoleInterrupt g_interrupt;

class Counter
{
public:
    typedef std::function<bool(const void* pData, size_t nBytes)> SinkCallback;
    typedef unsigned long long Num;

    Counter(Num nCount) 
    :   m_num(nCount)
    {
    }

    bool operator()(SinkCallback sinkCallback)
    {
        for (Num num = 0; num < m_num; ++num)
        {
            sinkCallback(static_cast<const void*>(&num), sizeof(Num));
        }
        return true;
    }

private:
    Num m_num;
};
        
size_t recordPackets(Counter& source, std::ostream& strm, bool bZeroTime = false)
{
    size_t nBytesTotal = 0;
    source([&](const void* pData, size_t nBytes) -> bool
    {
        // check if we were interrupted
        g_interrupt.triggerThrow();
        nBytesTotal += Packet::writeRawBinary(strm, pData, nBytes, bZeroTime);
        return true;
    });
    return nBytesTotal;
}

size_t debugPackets(Counter& source, std::ostream& strm, bool bZeroTime = false)
{
    size_t nBytesTotal = 0;
    source([&](const void* pData, size_t nBytes) -> bool
    {
        // check if we were interrupted
        g_interrupt.triggerThrow();
        nBytesTotal += Packet::writeDebugText(strm, pData, nBytes, bZeroTime);
        return true;
    });
    return nBytesTotal;
}

int main(int argc, char* argv[])
{
    try
    {
        // get command line / config file options
        Options options(argc, argv);
        Counter source(options.nCount);
        
        if (options.sFileName.size())
        {
            std::stringstream strm;
            strm << options.sFileName << "." << options.sFileExt;
            std::string sFileFull = strm.str();
            std::cout << "Writing to " << sFileFull << std::endl;
            if (options.bMmap)
            {
                std::cout << "Using MMAP" << std::endl;
                BigSink sink(options.sFileName, options.sFileExt, 1024);
                boost::iostreams::stream<BigSink> out(sink);
                recordPackets(source, out, true);                
            }
            else
            {
                std::ofstream stream(sFileFull, std::ofstream::binary);
                recordPackets(source, stream, true);
            }
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
