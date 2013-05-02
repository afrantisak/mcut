#include "ConsoleInterrupt.h"
#include "ArgParser.h"
#include "Packet.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <memory>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/concepts.hpp>

struct Options
{
    std::string sFileName;
    bool bDebug;
    bool bMmap;

    Options(int argc, char* argv[])
    {
        ArgParser args("mc-recv-pkt");
        args.add("--output-file", sFileName, "output to file");
        args.add("--debug", bDebug, "debug packet details");
        args.add("--mmap", bMmap, "use memory-mapped file");
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

class BigSink : public boost::iostreams::sink
{
public:
    BigSink(const std::string& sFileName, size_t nChunkSize);
    ~BigSink();

    std::streamsize write(const char_type* s, std::streamsize n);

private:
    std::string m_sFileName;
    size_t m_nChunkSize;
    size_t m_nBytesTotal;
    void* m_pWrite;
    
    typedef boost::iostreams::mapped_file_params SinkParams;
    typedef boost::iostreams::mapped_file_sink Sink;
    std::shared_ptr<Sink> m_pSink;
};

BigSink::BigSink(const std::string& sFileName, size_t nChunkSize)
    :   m_sFileName(sFileName),
        m_nChunkSize(nChunkSize),
        m_pSink(),
        m_pWrite(0),
        m_nBytesTotal(0)
{
    SinkParams p(m_sFileName.c_str());
    p.new_file_size = m_nChunkSize;
    p.length = -1;
    m_pSink.reset(new Sink(p));
    m_pWrite = m_pSink->data();
}

BigSink::~BigSink()
{
    m_pSink.reset();
    if (m_nBytesTotal)
        truncate(m_sFileName.c_str(), m_nBytesTotal);
}

std::streamsize BigSink::write(const char* s, std::streamsize n)
{
    memcpy(m_pWrite, s, n);
    m_nBytesTotal += n;
}

int main(int argc, char* argv[])
{
    try
    {
        // get command line / config file options
        Options options(argc, argv);
        Counter source(10);
        
        if (options.sFileName.size())
        {
            std::cout << "Writing to " << options.sFileName << std::endl;
            if (options.bMmap)
            {
                std::cout << "Using MMAP" << std::endl;

                BigSink sink(options.sFileName, 1024);
                boost::iostreams::stream<BigSink> out(sink);
                size_t nBytesTotal = recordPackets(source, out, true);                
            }
            else
            {
                std::ofstream stream(options.sFileName, std::ofstream::binary);
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
//    catch (std::exception& e)
//    {
//        if (e.what())
//            std::cerr << "exception: " << e.what() << "\n";
//        return 127;
//    }

    return 0;
}
