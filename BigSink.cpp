#include "BigSink.h"

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

