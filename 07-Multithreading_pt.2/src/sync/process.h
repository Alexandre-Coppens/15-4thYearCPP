#pragma once
#include "stream.h"

#include "pthread.h"
#include "semaphore.h"

// Squelette volontairement sans thread.
// start() exécute run() dans le thread appelant pour garder un comportement
// séquentiel. Remplacer cet appel par un thread POSIX. Voir sujet.tex.

namespace syncsys
{
    void* process_run(void*);

    class process
    {
        public:
            process()
            {
                // DONE: sémaphore privé, initialisé à 0
                sem_init(&sem, 0, 0);
            }

            virtual ~process()
            {
                // DONE: détruire le sémaphore privé
                sem_destroy(&sem);
            }

            // Le squelette reste séquentiel : run() est appelé ici.
            // DONE: lancer run() dans un nouveau thread (process_run),
            //       et ne plus l'appeler dans le thread courant.
            void start() { pthread_create(&thread, NULL, &process_run, this); }

            void cancel()
            {
                // DONE: annuler le thread seulement s'il a été créé
                pthread_cancel(thread);
            }

            void join()
            {
                // DONE: attendre la fin du thread seulement s'il a été créé
                void* result;
                pthread_join(thread, &result);
            }

            void notify()
            {
                // DONE: réveiller le thread bloqué sur le sémaphore privé
                sem_post(&sem);
            }

            virtual void run() = 0;
        protected:
            // DONE: pthread_t thread;
            pthread_t thread;
            // DONE: sem_t sem;
            sem_t sem;
    };

    template<typename T>
    class producer : public process
    {
        public:
            producer(stream<T> *_s) : process(), s(_s) {}
            virtual void run() = 0;
        protected:
            stream<T> *s;
    };

    template<typename T>
    class consumer : public process
    {
        public:
            consumer(stream<T> *_s) : process(), s(_s) {}
            virtual void run() = 0;
        protected:
            stream<T> *s;
    };
}
