#include "file_descriptor.h"
#include "util.h"

#include <algorithm>

#ifndef _WIN32
#include <unistd.h>
#endif

using namespace std;

namespace netsys
{
    const size_t FileDescriptor::BUFFER_SIZE;

    FileDescriptor::FileDescriptor(const fd_t fd)
      : fd_(fd), eof_(false), read_count_(0), write_count_(0)
    {
    }

    FileDescriptor::FileDescriptor(FileDescriptor &&other)
      : fd_(other.fd_), eof_(other.eof_),
        read_count_(other.read_count_),
        write_count_(other.write_count_)
    {
#ifdef _WIN32
        other.fd_ = INVALID_SOCKET;
#else
        other.fd_ = -1;
#endif
    }

    FileDescriptor::~FileDescriptor()
    {
#ifdef _WIN32
        if(fd_ == INVALID_SOCKET) return;
#else
        if(fd_ < 0) return;
#endif

        try
        {
#ifdef _WIN32
            SystemCall("closesocket", closesocket(fd_));
#else
            SystemCall( "close", close( fd_ ) );
#endif
        }
        catch(const exception &e)
        {
            print_exception(e);
        }
    }

    string::const_iterator FileDescriptor::write(const string::const_iterator &begin, const string::const_iterator &end)
    {
        if(begin >= end)
            throw runtime_error("nothing to write");

#ifdef _WIN32
        ssize_t bytes_written = SystemCall("send", ::send(fd_, &*begin, static_cast<int>(end - begin), 0));
#else
        ssize_t bytes_written = SystemCall("write", ::write(fd_, &*begin, end-begin));
#endif
        
        if(bytes_written == 0)
            throw runtime_error("write returned 0");
        register_write();

        return begin + bytes_written;
    }

    string FileDescriptor::read(const size_t limit)
    {
        char buffer[BUFFER_SIZE];

#ifdef _WIN32
        ssize_t bytes_read = SystemCall("recv", ::recv(fd_, buffer, static_cast<int>(min(BUFFER_SIZE, limit)), 0));
#else
        ssize_t bytes_read = SystemCall("read", ::read(fd_, buffer, min(BUFFER_SIZE, limit)));
#endif
        
        if(bytes_read == 0)
            set_eof();
        register_read();

        return string(buffer, bytes_read);
    }

    string::const_iterator FileDescriptor::write(const std::string &buffer, const bool write_all)
    {
        auto it = buffer.begin();
        
        do {
            it = write(it, buffer.end());
        }
        while(write_all && (it != buffer.end()));

        return it;
    }
}
