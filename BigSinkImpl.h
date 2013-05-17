#pragma once
#include "BigSink.h"
#include <boost/iostreams/device/mapped_file.hpp>
#include <thread>
#include <atomic>

namespace Private
{
    class BigSinkImpl : public boost::iostreams::sink
    {
    public:
        BigSinkImpl(std::string sFilename, std::string sExtension, size_t nChunkSize);
        ~BigSinkImpl();

        std::streamsize write(const char_type* s, std::streamsize n);

    private:
        std::string getFilename(size_t nChunk);
        
        void threadFetch();
        
        void switchChunks();

        std::string m_sFilename;
        std::string m_sExtension;
        size_t m_nChunkSize;
        size_t m_nBytesTotal;
        size_t m_nBytesChunk;
        size_t m_nChunks;
        char* m_pWriteCur;
        char* m_pWriteNew;
        char* m_pWriteOld;

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

