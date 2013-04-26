#include "recorder.h"
#include <boost/asio.hpp>

Recorder::Recorder(const Ip& sLocalIp, const Channel& channel)
:   m_sLocalIp(sLocalIp),
    m_channel(channel)
{
}

bool Recorder::operator()(SinkCallback sinkCallback)
{
    Receiver::Address local_address = boost::asio::ip::address::from_string(m_sLocalIp);
    Receiver::Address remote_address = boost::asio::ip::address::from_string(m_channel.ip);
    short remote_port = m_channel.port;
    
    boost::asio::io_service service;
    Receiver receiver(service, local_address, remote_address, remote_port, sinkCallback);
    service.run();
    
    return true;
}
    
