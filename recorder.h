#include <string>
#include <functional>
#include "receiver.h"

class Recorder
{
public:
    typedef std::function<bool(const void* pData, size_t nBytes)> SinkCallback;
    typedef std::string Ip;
    
    Recorder(const Ip& sLocalIp, const Channel& channel);
    
    bool operator()(SinkCallback sinkCallback);
    
private:
    Ip m_sLocalIp;
    Channel m_channel;
};

