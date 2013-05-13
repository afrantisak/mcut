#include "BigSink.h"
#include <boost/iostreams/device/mapped_file.hpp>
#include <thread>
#include <atomic>
#include <sstream>

namespace Private
{
    class BigSinkImpl : public boost::iostreams::sink
    {
    public:
        BigSinkImpl(std::string sFilename, std::string sExtension, size_t nChunkSize);
        ~BigSinkImpl();

        std::streamsize write(const char_type* s, std::streamsize n);

    private:
        std::string getFilename();
        
        void threadFetch();

        std::string m_sFilename;
        std::string m_sExtension;
        size_t m_nChunkSize;
        size_t m_nBytesTotal;
        size_t m_nBytesChunk;
        size_t m_nChunks;
        void* m_pWriteCur;
        void* m_pWriteNew;
        void* m_pWriteOld;

        typedef boost::iostreams::mapped_file_params FileParams;
        typedef boost::iostreams::mapped_file_sink File;
        std::atomic<bool> m_bStop;
        
        // when set to true by the main thread, the fetch thread will prepare a new buffer.  
        // When the buffer is ready, the fetch thread sets the value to false again.
        std::thread m_threadFetch;
        std::atomic<bool> m_bRequestFetch;
        
        File m_fileCur;
        File m_fileNew;
        File m_fileOld;
    };
};// namespace Private

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

Private::BigSinkImpl::BigSinkImpl(std::string sFilename, std::string sExtension, size_t nChunkSize)
    :   m_sFilename(sFilename),
        m_sExtension(sExtension),
        m_nChunkSize(nChunkSize),
        m_pWriteCur(0),
        m_pWriteNew(0),
        m_pWriteOld(0),
        m_nBytesTotal(0),
        m_nBytesChunk(0),
        m_nChunks(0),
        m_fileCur(),
        m_fileNew(),
        m_fileOld()
{
    m_bStop = false;
    m_threadFetch = std::thread(&BigSinkImpl::threadFetch, this);

    FileParams p(getFilename());
    p.new_file_size = m_nChunkSize;
    p.length = -1;
    m_fileCur = File(p);
    m_pWriteCur = m_fileCur.data();
}

Private::BigSinkImpl::~BigSinkImpl()
{
    if (m_nBytesTotal)
    {
        m_bStop = true;
        m_threadFetch.join();
        truncate(getFilename().c_str(), m_nBytesChunk);
    }
}

std::streamsize Private::BigSinkImpl::write(const char* s, std::streamsize n)
{
    // will we overflow?
    if (m_nBytesChunk + n > m_nChunkSize)
    {
        size_t nBytesFirst = m_nChunkSize - m_nBytesChunk;
        
        // copy as much as we can in the old buffer
        memcpy(m_pWriteCur, s, nBytesFirst);
        
        // Wait for next chunk to be available; hopefully won't wait long if at all.
        // If it turns out that we spend time in this spinlock, 
        // then we need to tune parameters, like chunk size
        while (m_bRequestFetch);
        
        // copy the rest into the new buffer
        memcpy(m_pWriteNew, s + nBytesFirst, n - nBytesFirst);
        m_nBytesChunk = n - nBytesFirst;
        m_pWriteOld = m_pWriteCur;
        m_pWriteCur = m_pWriteNew;

        // Request new chunk so it is hopefully ready by the time we finish this one
        m_bRequestFetch = true;
    }
    else
    {
        memcpy(m_pWriteCur, s, n);
        m_nBytesChunk += n;
    }
    m_nBytesTotal += n;
}

std::string Private::BigSinkImpl::getFilename()
{
    std::stringstream strm;
    strm << m_sFilename << "." << m_nChunks << "." << m_sExtension;
    return strm.str();
}

void Private::BigSinkImpl::threadFetch()
{
    while (!m_bStop)
    {
        if (!m_bRequestFetch)
        {
            // spin while waiting for main thread to request a new buffer
            continue;
        }    

        // a new buffer was requested; fetch it
        FileParams p(getFilename());
        p.new_file_size = m_nChunkSize;
        p.length = -1;
        m_fileNew = File(p);
        m_nChunks++;

        // reset the request flag to indicate it is ready
        m_bRequestFetch = false;
        
        // free the old buffer
        m_fileOld.close();
    }
}
