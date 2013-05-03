#pragma once
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/concepts.hpp>
#include <string>

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

