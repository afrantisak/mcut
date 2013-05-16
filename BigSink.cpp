#include "BigSink.h"
#include "BigSinkImpl.h"
#include <sstream>

BigSink::BigSink(std::string sFilename, std::string sExtension, size_t nChunkSize)
:   m_pImpl(new Private::BigSinkImpl(sFilename, sExtension, nChunkSize))
{
}

BigSink::~BigSink()
{
}

std::streamsize BigSink::write(const char* s, std::streamsize n)
{
    return m_pImpl->write(s, n);
}

