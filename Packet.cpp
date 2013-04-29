#include "Packet.h"
#include <iomanip>

void Packet::writeRawBinary(std::ostream& strm, const void* pData, size_t nBytes)
{
    Packet::Header hdr(nBytes);
    strm.write(reinterpret_cast<const char*>(&hdr), sizeof(hdr));
    strm.write(static_cast<const char*>(pData), nBytes);
}

void Packet::writeDebugText(std::ostream& strm, const void* pData, size_t nBytes)
{
    Packet::Header hdr(nBytes);
    strm << "Pkt(" << nBytes << "): ";

    // TODO: dump header, too
    const char* pChar = static_cast<const char*>(pData);
    size_t nBytesLeft = nBytes;
    while (nBytesLeft--)
    {
        strm << std::setw(2) << std::setfill('0') << std::hex << (int)static_cast<unsigned char>(*pChar++) << " " << std::dec;
    }
    strm << std::endl;
}
