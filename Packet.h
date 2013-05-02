#include <time.h>
#include <iostream>

namespace Packet
{
    
    struct Header
    {
        typedef uint32_t Length;
        typedef uint8_t  Version; // 0
        typedef uint8_t  Pad1;
        typedef uint16_t Pad2;
        typedef uint64_t Timestamp;

        Header(Length nLength)
            :   m_nLength(nLength),
                m_nVersion(0),
                m_nPad1(0),
                m_nPad2(0),
                m_nTimestamp(0)
        {
            clock_gettime(CLOCK_MONOTONIC, reinterpret_cast<timespec*>(&m_nTimestamp));
        }
            
        Length m_nLength;
        Version m_nVersion;
        Pad1 m_nPad1;
        Pad2 m_nPad2;
        Timestamp m_nTimestamp;
    };

    size_t writeRawBinary(std::ostream& strm, const void* pData, size_t nBytes, bool bZeroTime = false);
    size_t writeDebugText(std::ostream& strm, const void* pData, size_t nBytes, bool bZeroTime = false);

};//namespace Packet