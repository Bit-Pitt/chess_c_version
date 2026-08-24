#include "BotCoordinator.h"

#include "ChessBot.h"

#include <algorithm>
#include <chrono>
#include <thread>
#include <random>
#include <stdexcept>

BotCoordinator::BotCoordinator(int numeroWorker) : numeroWorker(numeroWorker) {
}

void BotCoordinator::run(SyncContext& sync) {

    while (sync.running) {

        sem_wait(&sync.botInputReady);

        if (!sync.running)
            break;

        RichiestaBot richiesta;

        {
            std::lock_guard<std::mutex> lock(sync.botInputMutex);
            richiesta = sync.richiestaBot;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));

        ChessBot bot(richiesta.scacchiera, richiesta.epsilon);

        std::vector<MossaBot> mosseLegali = bot.getMosseLegali(richiesta.giocatore);

        if (mosseLegali.empty())
            throw std::runtime_error("BotCoordinator: nessuna mossa disponibile o partita finita");


        // =====================================================
        // CREAZIONE TASK
        // =====================================================

        for (const MossaBot& mossa : mosseLegali) {

            MossaBotTask task{
                richiesta.scacchiera,
                mossa.src,
                mossa.dest,
                richiesta.giocatore
            };

            {
                std::lock_guard<std::mutex> lock(sync.taskMutex);
                sync.taskQueue.push(std::move(task));
            }

            sem_post(&sync.taskReady);      //una post per ogni task
        }


        // =====================================================
        // RACCOLTA RISULTATI
        // =====================================================

        std::vector<MossaBot> risultati;

        for (size_t i = 0; i < mosseLegali.size(); ++i) {

            sem_wait(&sync.resultReady);        //una wait per ogni task

            RisultatoBotTask risultato;

            {
                std::lock_guard<std::mutex> lock(sync.resultMutex);
                risultato = sync.resultQueue.front();
                sync.resultQueue.pop();
            }

            risultati.push_back({
                risultato.src,
                risultato.dest,
                risultato.valore
            });
        }


        // =====================================================
        // SCELTA FINALE
        // =====================================================

        MossaBot mossaScelta = bot.scegliMossa(risultati);

        {
            std::lock_guard<std::mutex> lock(sync.botOutputMutex);
            sync.rispostaBot = mossaScelta;
        }

        sem_post(&sync.botMoveReady);
    }
}