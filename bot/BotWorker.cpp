#include "BotWorker.h"

#include "../pieces/Pezzo.h"
#include "../utils/logicUtils.h"
#include "ChessBot.h"

#include <thread>
#include <iostream>

bool DEBUG = true;

void BotWorker::run(SyncContext& sync) {

    while (sync.running) {

        sem_wait(&sync.taskReady);

        if (!sync.running)
            break;

        MossaBotTask task;

        {
            std::lock_guard<std::mutex> lock(sync.taskMutex);

            if (sync.taskQueue.empty())
                continue;

            task = std::move(sync.taskQueue.front());
            sync.taskQueue.pop();
        }

        if (DEBUG){
            std::cout << "[WORKER " << std::this_thread::get_id() << "] "
            << "inizio task: ("
            << task.src.riga << ", " << task.src.colonna
            << ") -> ("
            << task.dest.riga << ", " << task.dest.colonna
            << ")\n";
        }

        ChessBot bot(task.scacchiera, 0.0);

        double valore = bot.valutaMossaTask(task.src, task.dest, task.giocatore);

        RisultatoBotTask risultato{
            task.src,
            task.dest,
            valore
        };

        {
            std::lock_guard<std::mutex> lock(sync.resultMutex);
            sync.resultQueue.push(std::move(risultato));
        }

        if (DEBUG){
            std::cout << "[WORKER " << std::this_thread::get_id() << "] "
            << "fine task, score = "
            << valore
            << "\n";
        }

        sem_post(&sync.resultReady);
    }
}