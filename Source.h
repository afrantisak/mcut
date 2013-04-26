#pragma once
#include <string>
#include <functional>
#include "receiver.h"

namespace mcut
{
    
    class Source
    {
    public:
        typedef std::function<bool(const void* pData, size_t nBytes)> SinkCallback;
        typedef std::string Ip;
        
        Source(const Ip& sLocalIp, const Channel& channel);
        
        bool operator()(SinkCallback sinkCallback);
        
    private:
        Ip m_sLocalIp;
        Channel m_channel;
    };

};//namespace mcut
