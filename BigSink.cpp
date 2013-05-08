#include "BigSink.h"

namespace Private
{
    class BigSinkImpl : public boost::iostreams::sink
    {
    public:
        BigSinkImpl(const std::string& sFileName, size_t nChunkSize);
        ~BigSinkImpl();

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
        std::thread m_thread;
        std::atomic<bool> m_bStop;
        Sink m_sink;
    };
};// namespace Private

BigSink::BigSink(const std::string& sFileName, size_t nChunkSize)
:   m_pImpl(new Private::BigSinkImpl(sFileName, nChunkSize))
{
}

BigSink::~BigSink()
{
}

std::streamsize BigSink::write(const char* s, std::streamsize n)
{
    return m_pImpl->write(s, n);
}

Private::BigSinkImpl::BigSinkImpl(const std::string& sFileName, size_t nChunkSize)
    :   m_sFileName(sFileName),
        m_nChunkSize(nChunkSize),
        m_pWrite(0),
        m_pWriteNew(0),
        m_pWriteOld(0),
        m_nBytesTotal(0),
        m_nBytesChunk(0)
{
    m_bStop = false;
    m_thread = std::thread(&BigSinkImpl::load, this);

    SinkParams p(m_sFileName.c_str());
    p.new_file_size = m_nChunkSize;
    p.length = -1;
    m_sink = Sink(p);
    m_pWrite = m_sink.data();
}

Private::BigSinkImpl::~BigSinkImpl()
{
    if (m_nBytesTotal)
    {
        m_bStop = true;
        m_thread.join();
        truncate(m_sFileName.c_str(), m_nBytesChunk);
    }
}

std::streamsize Private::BigSinkImpl::write(const char* s, std::streamsize n)
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

void Private::BigSinkImpl::load()
{
    while (!m_bStop)
    {
        
    }
}
