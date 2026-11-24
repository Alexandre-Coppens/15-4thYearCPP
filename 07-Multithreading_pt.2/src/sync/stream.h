#pragma once

// Squelette volontairement non synchronisé.
// La synchronisation (mutex POSIX et sémaphores) est à écrire.
// Voir sujet.tex.

#include <vector>

namespace syncsys
{
    void* process_run(void*);

    template<typename T>
    class consumer;
    template<typename T>
    class producer;

    template<typename T>
    class stream
    {
        public:
            using type = T;
        public:
            explicit stream(int s) noexcept
                : ri(0), wi(0), pcurrent(0), ccurrent(0), used(0), size(s),
                  data(new T[s]())
            {
                // TODO: mutex des listes de producteurs et de consommateurs
                // TODO: sémaphores du tampon, des fins de segment et du répartiteur
            }

            ~stream() noexcept
            {
                for(auto &it : prod)
                    delete it;
                for(auto &it : cons)
                    delete it;
                delete[] data;
                // TODO: détruire les mutex et les sémaphores
            }

            void attach(producer<T> *p) noexcept
            {
                // TODO: exclusion sur la liste des producteurs,
                //       puis réveil de ce producteur pour qu'il puisse travailler
                prod.push_back(p);
            }

            void remove(producer<T> *p) noexcept
            {
                // TODO: exclusion sur la liste des producteurs
                for(auto it = prod.begin(); it != prod.end(); )
                {
                    if(p == *it)
                        it = prod.erase(it);
                    else
                        ++it;
                }
                if(prod.empty())
                    pcurrent = 0;
                else if(pcurrent >= static_cast<int>(prod.size()))
                    pcurrent = static_cast<int>(prod.size()) - 1;
            }

            void attach(consumer<T> *p) noexcept
            {
                // TODO: exclusion sur la liste des consommateurs,
                //       puis réveil de ce consommateur
                cons.push_back(p);
            }

            void remove(consumer<T> *p) noexcept
            {
                // TODO: exclusion sur la liste des consommateurs
                for(auto it = cons.begin(); it != cons.end(); )
                {
                    if(p == *it)
                        it = cons.erase(it);
                    else
                        ++it;
                }
                if(cons.empty())
                    ccurrent = 0;
                else if(ccurrent >= static_cast<int>(cons.size()))
                    ccurrent = static_cast<int>(cons.size()) - 1;
            }

            void notifyProd() noexcept
            {
                // TODO: signaler la fin de segment du producteur courant
                //       et réveiller le répartiteur
            }

            void notifyCons() noexcept
            {
                // TODO: signaler la fin de segment du consommateur courant
                //       et réveiller le répartiteur
            }

            void operator << (const T &d) noexcept
            {
                // TODO: accès exclusif au tampon
                wi = (wi + 1) % size;
                used++;
                data[wi] = d;
                // TODO: relâcher le tampon, puis signaler qu'un élément est lisible
            }

            void operator >> (T &i) noexcept
            {
                // TODO: attendre qu'un élément soit disponible, sans attente active
                // TODO: accès exclusif au tampon
                ri = (ri + 1) % size;
                i = data[ri];
                used--;
                // TODO: relâcher le tampon avant de changer de producteur
                nextProd();
            }

            void run() noexcept
            {
                // TODO: réveiller le premier producteur et le premier consommateur
                //       s'ils existent, puis boucle infinie :
                //       attendre un signal du répartiteur, puis nextCons()
            }
        protected:
            void nextProd() noexcept
            {
                // TODO: si le producteur courant a fini son segment
                //       et que le tampon est vide, passer au suivant et le réveiller.
                //       Ne pas bloquer ici.
            }

            void nextCons() noexcept
            {
                // TODO: si le consommateur courant a fini son segment
                //       et que le tampon est vide, passer au suivant et le réveiller.
                //       Ne pas bloquer ici.
            }
        protected:
            std::vector<consumer<T>*> cons;
            std::vector<producer<T>*> prod;
            int ri, wi, pcurrent, ccurrent, used, size;
            T *data;
    };
}
