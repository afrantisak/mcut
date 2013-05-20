#include "BigSinkImpl.h"
#include <sstream>

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
        m_bRequestFetch(false),
        m_fileCur(),
        m_fileNew(),
        m_fileOld()
{
    m_bStop = false;
    m_threadFetch = std::thread(&BigSinkImpl::fetcherThread, this);

    // request the first chunk
    m_bRequestFetch = true;

    // wait for it
    while (m_bRequestFetch);
    
    // pointer bookkeeping
    swapChunks();

    // request the next chunk now
    m_bRequestFetch = true;
}

Private::BigSinkImpl::~BigSinkImpl()
{
    if (m_nBytesTotal)
    {
        m_bStop = true;
        m_threadFetch.join();
        
        m_fileOld.close();
        m_fileCur.close();
        m_fileNew.close();

        // shrink the current file down 
        truncate(getFilename(m_nChunks - 1).c_str(), m_nBytesChunk);
        
        // delete the new file we had queued up
        unlink(getFilename(m_nChunks).c_str());
    }
}

std::streamsize Private::BigSinkImpl::write(const char* s, std::streamsize n)
{
    // will we overflow?
    if (m_nBytesChunk + n > m_nChunkSize)
    {
        size_t nBytesFirst = m_nChunkSize - m_nBytesChunk;
        
        // copy as much as we can into the old buffer
        memcpy(m_pWriteCur + m_nBytesChunk, s, nBytesFirst);
        
        // Wait for next chunk to be available; hopefully won't wait long if at all.
        // If it turns out that we spend time in this spinlock, 
        // then we need to tune parameters, like chunk size
        while (m_bRequestFetch);
        
        // copy the rest into the new buffer
        memcpy(m_pWriteNew, s + nBytesFirst, n - nBytesFirst);
        m_nBytesChunk = n - nBytesFirst;
        
        swapChunks();
        
        // Request new chunk so it is hopefully ready by the time we finish this one
        m_bRequestFetch = true;
    }
    else
    {
        memcpy(m_pWriteCur + m_nBytesChunk, s, n);
        m_nBytesChunk += n;
    }
    m_nBytesTotal += n;
}

void Private::BigSinkImpl::swapChunks()
{
    std::swap(m_pWriteOld, m_pWriteCur);
    std::swap(m_pWriteCur, m_pWriteNew);
    std::swap(m_fileOld, m_fileCur);
    std::swap(m_fileCur, m_fileNew);
    m_nChunks++;
}

std::string Private::BigSinkImpl::getFilename(size_t nChunk)
{
    std::stringstream strm;
    strm << m_sFilename << "." << nChunk << "." << m_sExtension;
    return strm.str();
}

void Private::BigSinkImpl::fetcherThread()
{
    while (!m_bStop)
    {
        if (!m_bRequestFetch)
        {
            // spin while waiting for main thread to request a new buffer
            continue;
        }    
        
        // a new buffer was requested; fetch it
        FileParams p(getFilename(m_nChunks));
        p.new_file_size = m_nChunkSize;
        p.length = -1;
        m_fileNew = File(p);
        m_pWriteNew = m_fileNew.data();

        // free the old buffer
        m_fileOld.close();

        // reset the request flag to indicate it is ready
        m_bRequestFetch = false;
    }
}

