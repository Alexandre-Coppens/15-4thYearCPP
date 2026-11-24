#ifndef COMPAT_HH
#define COMPAT_HH

#include <stdexcept>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#ifdef _MSC_VER
#include <BaseTsd.h>
using ssize_t = SSIZE_T;
#endif
namespace netsys
{
    using fd_t = SOCKET;
    inline void ensure_network()
    {
        static const int ready = [] {
            WSADATA data;
            const int rc = WSAStartup(MAKEWORD(2, 2), &data);
            if(rc != 0)
                throw std::runtime_error("WSAStartup failed");
            return rc;
        }();
        (void)ready;
    }
}
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
namespace netsys
{
    using fd_t = int;
    inline void ensure_network() {}
}
#endif

#endif /* COMPAT_HH */
