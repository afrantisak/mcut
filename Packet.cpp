#include "Packet.h"
#include <iomanip>
#include <sstream>

size_t Packet::writeRawBinary(std::ostream& strm, const void* pData, size_t nBytes, bool bZeroTime)
{
    Packet::Header hdr(nBytes);
    if (bZeroTime)
    {
        hdr.m_nTimestamp = 0;
    }
        
    strm.write(reinterpret_cast<const char*>(&hdr), sizeof(hdr));
    strm.write(static_cast<const char*>(pData), nBytes);
    return sizeof(hdr) + nBytes;
}

size_t Packet::writeDebugText(std::ostream& strmOrig, const void* pData, size_t nBytes, bool bZeroTime)
{
    Packet::Header hdr(nBytes);
    if (bZeroTime)
    {
        hdr.m_nTimestamp = 0;
        std::cout << "ZERO";
    }

    std::stringstream strm;
    strm << "Pkt(" << nBytes << "): ";

    // TODO: dump header, too
    const char* pChar = static_cast<const char*>(pData);
    size_t nBytesLeft = nBytes;
    while (nBytesLeft--)
    {
        strm << std::setw(2) << std::setfill('0') << std::hex << (int)static_cast<unsigned char>(*pChar++) << " " << std::dec;
    }
    strm << std::endl;
    
    strmOrig << strm;
    
    return strm.str().size();
}
