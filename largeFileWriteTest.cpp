#include "ConsoleInterrupt.h"
#include "ArgParser.h"
#include "Packet.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

struct Options
{
    std::string sFileName;
    bool bDebug;

    Options(int argc, char* argv[])
    {
        ArgParser args("mc-recv-pkt");
        args.add("--output-file", sFileName, "output to file");
        args.add("--debug", bDebug, "debug packet details");
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
        
void recordPackets(Counter& source, std::ostream& strm)
{
    source([&](const void* pData, size_t nBytes) -> bool
    {
        // check if we were interrupted
        g_interrupt.triggerThrow();
        Packet::writeRawBinary(strm, pData, nBytes);
        return true;
    });
}

void debugPackets(Counter& source, std::ostream& strm)
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
        Counter source(65537);
        
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
