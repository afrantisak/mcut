#pragma once
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/concepts.hpp>
#include <string>
#include <thread>
#include <atomic>

namespace Private
{
    class BigSinkImpl;
};

class BigSink : public boost::iostreams::sink
{
public:
    BigSink(const std::string& sFileName, size_t nChunkSize);
    ~BigSink();

    std::streamsize write(const char_type* s, std::streamsize n);

private:
    std::shared_ptr<Private::BigSinkImpl> m_pImpl;
    
};

