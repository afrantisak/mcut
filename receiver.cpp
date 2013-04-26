#include "receiver.h"

using namespace boost::asio;
using namespace mcut;

Receiver::Receiver(io_service& io_service, const Address& local_address, 
                   const Address& remote_address, const Port& remote_port, Callback callback)
    :   m_socket(io_service),
        m_remote_endpoint(),
        m_callback(callback),
        m_buffer(8192)
{
    // Create the socket so that multiple may be bound to the same address.
    Endpoint local_endpoint(local_address, remote_port);
    m_socket.open(local_endpoint.protocol());
    m_socket.set_option(ip::udp::socket::reuse_address(true));
    m_socket.bind(local_endpoint);

    // Join the multicast group.
    m_socket.set_option(ip::multicast::join_group(remote_address));

    // wait for the next packet
    async_receive_next();
}

void Receiver::async_receive_next()
{
    // wait for a packet
    m_socket.async_receive_from(buffer(m_buffer), m_remote_endpoint, 
                                [&](const ErrorCode& error, size_t bytes_recvd)
    {
        if (!error)
        {
            // pass the packet up to the caller
            if (m_callback(static_cast<const void*>(&m_buffer[0]), bytes_recvd))
            {
                // wait for another packet
                async_receive_next();
            }
        }
    }); 
}

