#include "Spooler.h"
#include "User.h"
#include "Document.h"

#include "../sync/process.h"
#include "../network/util.h"
#include <string>

namespace cups
{
    User::User(Spooler *spooler, netsys::TCPSocket &&tcp) noexcept
      : syncsys::producer<Job*>((syncsys::stream<Job*>*)spooler),
        client(std::move(tcp)), spooler(spooler), quota(0)
    {
        spooler->attach(this);
    }
    
    User::~User() noexcept
    {
        spooler->remove(this);
    }
    
    int User::getQuota() const
    {
        return quota;
    }

    void User::increaseQuota(int nbPages)
    {
        User::quota = quota + nbPages;
    }

    void User::decreaseQuota(int nbPages)
    {
        quota = quota - nbPages;
    }
    
    void User::run()
    {
        std::cerr << "New connection from "
                  << client.peer_address().to_string()
                  << std::endl;
             
        while(!client.eof())
        {
            // DONE: attendre d'être réveillé (sémaphore privé du process)
            sem_wait(&sem);

            std::string buff;
            const std::string chunk = client.read();
            Job *job = spooler->print(new Document(std::move(chunk)), this);

            // DONE: déposer job dans le stream (*s << job)
            *s << job;

            s->notifyProd();
        }
        
        std::cerr << client.peer_address().to_string()
                  << " closed the connection."
                  << std::endl;
            
        delete this;
    }
}
