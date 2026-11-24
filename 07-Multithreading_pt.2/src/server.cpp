/* simple TCP listener/server to demonstrate sourdough starter classes */
/* Keith Winstein <keithw@cs.stanford.edu>, January 2015 */

#include <iostream>
#include <fstream>
#include <thread>

#include "cups/Spooler.h"
#include "cups/User.h"
#include "cups/Printer.h"

#include "sync/process.h"
#include "network/util.h"

using namespace std;

namespace syncsys
{
    void* process_run(void *i)
    {
        auto *tmp = static_cast<syncsys::process*>(i);
        tmp->run();
        return 0;
    }
}

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
    
    cups::Spooler server(netsys::Address(host, port));
    server.join(new cups::Printer());

    // DONE: lancer syncsys::stream<cups::Job*>::run() dans un thread dédié.
    //       Spooler::run masque cette méthode : c'est la boucle d'acceptation,
    //       elle ne revient pas. Le répartiteur ne doit pas tourner dedans.
    
    server.run();

    return EXIT_SUCCESS;
}
