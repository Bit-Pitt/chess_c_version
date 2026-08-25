#include "monitor.h"

#include <iostream>

void Monitor::run(SyncContext& sync) {
    while (sync.running) {

        sem_wait(&sync.monitorReady);

        if (!sync.running)
            break;

        StatoMonitor stato;

        {
            std::lock_guard<std::mutex> lock(sync.monitorMutex);

            if (sync.monitorQueue.empty())
                continue;

            stato = std::move(sync.monitorQueue.front());
            sync.monitorQueue.pop();
        }

        std::cout << "[MONITOR] "
                  << "Turno: "
                  << (stato.turno == Colore::WHITE ? "WHITE" : "BLACK")
                  << " | Ultima mossa: " << stato.ultimaMossa;

        if (stato.statoPartita == StatoPartita::PATTA) {
            std::cout << " | PATTA";
        }
        else if (stato.statoPartita == StatoPartita::VINCE_BIANCO) {
            std::cout << " | VINCE WHITE";
        }
        else if (stato.statoPartita == StatoPartita::VINCE_NERO) {
            std::cout << " | VINCE BLACK";
        }

        std::cout << "\n";
    }
}