/* simple TCP listener/server to demonstrate sourdough starter classes */
/* Keith Winstein <keithw@cs.stanford.edu>, January 2015 */

#include <iostream>
#include <fstream>

#include "network/socket.h"
#include "network/util.h"

using namespace std;

int main(int argc, char *argv[])
{
    std::string host {"localhost"};
    std::string port {"4004"};
    
    // if(argc < 1)
    //     abort();
    //
    // if(argc != 2)
    // {
    //     cerr << "Usage: " << argv[ 0 ] << " PORT" << endl;
    //     return EXIT_FAILURE;
    // }

    netsys::TCPSocket listening_socket;
    
    listening_socket.set_reuseaddr();
    listening_socket.bind(netsys::Address("::0", argv[1]));
    listening_socket.listen();
    
    cerr << "Listening on local address: "
         << listening_socket.local_address().to_string()
         << endl;

    while(true)
    {
        // TODO: traiter chaque client dans un thread détaché (std::thread).
        //       Ici la connexion est traitée dans le thread d'écoute :
        //       le accept suivant attend la fin de l'envoi.
        
        netsys::TCPSocket client = listening_socket.accept();

        cerr << "New connection from "
             << client.peer_address().to_string()
             << endl;

        std::string buff;
        const string chunk = client.read();
        ifstream stream(chunk, std::ios::binary);

        if(stream.is_open())
        {
            cerr << "sending " << chunk << endl;
            while(stream.good())
            {
                getline(stream,buff);
                buff += "\n";
                client.write(buff, true);
            }
            stream.close();
        }
        else
        {
            cerr << chunk + " file not found" << endl;
            client.write(chunk + " file not found");
        }

        cerr << client.peer_address().to_string()
             << " closed the connection."
             << endl;
    }

    return EXIT_SUCCESS;
}
