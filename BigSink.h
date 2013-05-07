#pragma once
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/concepts.hpp>
#include <string>
#include <thread>
#include <atomic>

class BigSink : public boost::iostreams::sink
{
public:
    BigSink(const std::string& sFileName, size_t nChunkSize);
    ~BigSink();

    std::streamsize write(const char_type* s, std::streamsize n);

private:
    void load();

    std::string m_sFileName;
    size_t m_nChunkSize;
    size_t m_nBytesTotal;
    size_t m_nBytesChunk;
    void* m_pWrite;
    void* m_pWriteNew;
    void* m_pWriteOld;

    typedef boost::iostreams::mapped_file_params SinkParams;
    typedef boost::iostreams::mapped_file_sink Sink;
    struct Shared
    {
        std::thread m_thread;
        std::atomic<bool> m_bStop;
        Sink m_sink;
    };
    std::shared_ptr<Shared> m_pShared;
    
};

