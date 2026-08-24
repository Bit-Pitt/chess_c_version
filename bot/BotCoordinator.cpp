
#include "BotCoordinator.h"
#include "ChessBot.h"
#include <thread>
#include <chrono>

BotCoordinator::BotCoordinator(double epsilon) : epsilon(epsilon) {
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

        MossaBot mossa = bot.scegliMossa(richiesta.giocatore);

        {
            std::lock_guard<std::mutex> lock(sync.botOutputMutex);
            sync.rispostaBot = mossa;
        }

        sem_post(&sync.botMoveReady);
    }
}