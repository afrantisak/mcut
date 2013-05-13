#pragma once
#include <boost/iostreams/concepts.hpp>
#include <string>
#include <memory>

namespace Private
{
    class BigSinkImpl;
};

class BigSink : public boost::iostreams::sink
{
public:
    BigSink(std::string sFilename, std::string sExtension, size_t nChunkSize);
    ~BigSink();

    std::streamsize write(const char_type* s, std::streamsize n);

private:
    // boost::iostreams makes copies of this sink
    // and BigSinkImpl uses stuff that cannot be copied 
    // so we must pImpl it with a shared_ptr so only one will exist
    std::shared_ptr<Private::BigSinkImpl> m_pImpl;
};

