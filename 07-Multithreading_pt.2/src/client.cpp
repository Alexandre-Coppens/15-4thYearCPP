#include <iostream>

#include "network/socket.h"
#include "network/util.h"
#include "network/poller.h"

using namespace std;
using namespace netsys::PollerShortNames;

int main(int argc, char *argv[])
{
    std::string host {"localhost"};
    std::string port {"4004"};
    std::string file {"toprint.png"};
    
    // if(argc < 1)
    //     abort();
    //
    // if(argc != 4)
    // {
    //     cerr << "Usage: " << argv[ 0 ] << " HOST PORT filepath" << endl;
    //     return EXIT_FAILURE;
    // }

    cerr << "Looking up " << host << ":" << port << endl;
    netsys::Address server( host, port );
    cerr << "Done. Found " << server.to_string() << endl;

    netsys::TCPSocket socket;
    cerr << "Connecting...";
    socket.connect(server);
    cerr << "done." << endl;

    netsys::Poller poller;

    poller.add_action(
        Action(
            socket, Direction::In, [&]() {
                //! print received file
                cout << socket.read();
                if(socket.eof())
                     return ResultType::Exit;
                else return ResultType::Continue;
            }
        )
    );
    
    //! send filename
    socket.write(file);

    cerr << "..................."<< endl;
    while(true)
    {
        const auto ret = poller.poll(-1);
        if(ret.result == PollResult::Exit)
        {
            cerr << "..................."<< endl;
            return ret.exit_status;
        }
    }
    
    cout << endl;
    return EXIT_SUCCESS;
}
