#include "BigSink.h"


BigSink::BigSink(const std::string& sFileName, size_t nChunkSize)
    :   m_sFileName(sFileName),
        m_nChunkSize(nChunkSize),
        m_pWrite(0),
        m_pWriteNew(0),
        m_pWriteOld(0),
        m_nBytesTotal(0),
        m_nBytesChunk(0),
        m_pShared(new Shared())
{
    m_pShared->m_bStop = false;
    m_pShared->m_thread = std::thread(&BigSink::load, this);

    SinkParams p(m_sFileName.c_str());
    p.new_file_size = m_nChunkSize;
    p.length = -1;
    m_pShared->m_sink = Sink(p);
    m_pWrite = m_pShared->m_sink.data();
}

BigSink::~BigSink()
{
    if (m_nBytesTotal)
    {
        m_pShared->m_bStop = true;
        m_pShared->m_thread.join();
        m_pShared.reset();
        truncate(m_sFileName.c_str(), m_nBytesChunk);
    }
}

std::streamsize BigSink::write(const char* s, std::streamsize n)
{
    // will we overflow?
    if (m_nBytesChunk + n > m_nChunkSize)
    {
        size_t nBytesFirst = m_nChunkSize - m_nBytesChunk;
        memcpy(m_pWrite, s, nBytesFirst);
        // TODO: wait for next chunk to be available
        memcpy(m_pWriteNew, s + nBytesFirst, n - nBytesFirst);
        m_nBytesChunk = n - nBytesFirst;
        m_pWriteOld = m_pWrite;
        m_pWrite = m_pWriteNew;
        // TODO queue up new chunk
        // TODO garbage collect old chunk
    }
    else
    {
        memcpy(m_pWrite, s, n);
        m_nBytesChunk += n;
    }
    m_nBytesTotal += n;
}

void BigSink::load()
{
    while (!m_pShared->m_bStop)
    {
        
    }
}
