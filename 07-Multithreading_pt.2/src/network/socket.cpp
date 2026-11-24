#ifndef _WIN32
#include <sys/socket.h>
#endif

#include "socket.h"
#include "util.h"
#include "timestamp.h"

using namespace std;

namespace netsys
{
    Socket::Socket(const int domain, const int type)
      : FileDescriptor((ensure_network(), SystemCall("socket", socket(domain, type, 0))))
    {
    }

    Socket::Socket(FileDescriptor &&fd, const int domain, const int type)
      : FileDescriptor(move(fd))
    {
        int actual_value;
        socklen_t len = sizeof(actual_value);
        
#ifndef _WIN32
        SystemCall(
            "getsockopt",
            getsockopt(fd_num(), SOL_SOCKET, SO_DOMAIN, &actual_value, &len)
        );
              
        if(len != sizeof(actual_value) or actual_value != domain)
            throw runtime_error("socket domain mismatch");

        len = sizeof(actual_value);
#else
        (void)domain;
#endif
        
        SystemCall(
            "getsockopt",
            getsockopt(fd_num(), SOL_SOCKET, SO_TYPE, reinterpret_cast<char *>(&actual_value), &len)
        );
              
        if(len != sizeof(actual_value) || actual_value != type)
            throw runtime_error("socket type mismatch");
    }

    Address Socket::get_address(const std::string &name_of_function, const std::function<int(fd_t, sockaddr *, socklen_t *)> &function) const
    {
        Address::raw address;
        socklen_t size = sizeof(address);

        SystemCall(
            name_of_function, function(
                fd_num(),
                &address.as_sockaddr,
                &size
            )
        );

        return Address(address, size);
    }

    Address Socket::local_address(void) const
    {
        return get_address("getsockname", getsockname);
    }

    Address Socket::peer_address(void) const
    {
        return get_address("getpeername", getpeername);
    }

    void Socket::bind(const Address & address)
    {
        SystemCall(
            "bind", ::bind(
                fd_num(),
                &address.to_sockaddr(),
                address.size()
            )
        );
    }

    void Socket::connect(const Address & address)
    {
        SystemCall(
            "connect", ::connect(
                fd_num(),
                &address.to_sockaddr(),
                address.size()
            )
        );
    }

#ifdef _WIN32
    UDPSocket::received_datagram UDPSocket::recv()
    {
        static const int RECEIVE_MTU = 65536;

        Address::raw datagram_source_address;
        char msg_payload[RECEIVE_MTU];
        int namelen = sizeof(datagram_source_address);

        const int recv_len = SystemCall(
            "recvfrom", ::recvfrom(
                fd_num(), msg_payload, RECEIVE_MTU, 0,
                &datagram_source_address.as_sockaddr, &namelen
            )
        );

        register_read();

        received_datagram ret = {
            Address(datagram_source_address, static_cast<size_t>(namelen)),
            timestamp_ms(),
            string(msg_payload, recv_len)
        };

        return ret;
    }
#else
    UDPSocket::received_datagram UDPSocket::recv()
    {
        static const ssize_t RECEIVE_MTU = 65536;

        Address::raw datagram_source_address;
        msghdr header;
        zero(header);
        iovec msg_iovec;
        zero(msg_iovec);

        char msg_payload[RECEIVE_MTU];
        char msg_control[RECEIVE_MTU];

        header.msg_name = &datagram_source_address;
        header.msg_namelen = sizeof(datagram_source_address);

        msg_iovec.iov_base = msg_payload;
        msg_iovec.iov_len = sizeof(msg_payload);
        header.msg_iov = &msg_iovec;
        header.msg_iovlen = 1;

        header.msg_control = msg_control;
        header.msg_controllen = sizeof(msg_control);

        ssize_t recv_len = SystemCall(
            "recvmsg", recvmsg(fd_num(), &header, 0)
        );

        register_read();

        if(header.msg_flags & MSG_TRUNC)
            throw runtime_error("recvfrom (oversized datagram)");
            
        if(header.msg_flags)
            throw runtime_error("recvfrom (unhandled flag)");

        uint64_t timestamp = -1;
        cmsghdr *ts_hdr = CMSG_FIRSTHDR(&header);
        
        while(ts_hdr)
        {
            if(ts_hdr->cmsg_level == SOL_SOCKET and ts_hdr->cmsg_type == SO_TIMESTAMPNS)
            {
                const timespec * const kernel_time = reinterpret_cast<timespec *>(CMSG_DATA(ts_hdr));
                timestamp = timestamp_ms(*kernel_time);
            }
            
            ts_hdr = CMSG_NXTHDR(&header, ts_hdr);
        }

        received_datagram ret = {
            Address(
                datagram_source_address,
                header.msg_namelen
            ),
            timestamp,
            string(msg_payload, recv_len)
        };

        return ret;
    }
#endif

    void UDPSocket::sendto(const Address & destination, const string & payload)
    {
        const ssize_t bytes_sent = SystemCall(
            "sendto", ::sendto(
                fd_num(), payload.data(),
#ifdef _WIN32
                static_cast<int>(payload.size()),
#else
                payload.size(),
#endif
                0,
                &destination.to_sockaddr(),
                destination.size()
            )
        );

        register_write();

        if(size_t(bytes_sent) != payload.size())
            throw runtime_error("datagram payload too big for sendto()");
    }

    void UDPSocket::send(const string & payload)
    {
        const ssize_t bytes_sent = SystemCall(
            "send", ::send(
                fd_num(), payload.data(),
#ifdef _WIN32
                static_cast<int>(payload.size()),
#else
                payload.size(),
#endif
                0
            )
        );

        register_write();

        if(size_t(bytes_sent) != payload.size())
            throw runtime_error("datagram payload too big for send()");
    }

    void TCPSocket::listen(const int backlog)
    {
        SystemCall("listen", ::listen(fd_num(), backlog));
    }

    TCPSocket TCPSocket::accept()
    {
        register_read();
        return TCPSocket(FileDescriptor(SystemCall("accept", ::accept(fd_num(), nullptr, nullptr))));
    }

    template <typename option_type>
    void Socket::setsockopt(const int level, const int option, const option_type & option_value)
    {
        SystemCall(
            "setsockopt", ::setsockopt(
                fd_num(), level, option,
                reinterpret_cast<const char *>(&option_value),
                static_cast<socklen_t>(sizeof(option_value))
            )
        );
    }

    void Socket::set_reuseaddr()
    {
        setsockopt(SOL_SOCKET, SO_REUSEADDR, int(true));
    }

    void UDPSocket::set_timestamps(void)
    {
#ifndef _WIN32
        setsockopt(SOL_SOCKET, SO_TIMESTAMPNS, int(true));
#else
        (void)this;
#endif
    }
}
