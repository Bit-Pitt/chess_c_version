#ifndef SYNC_H
#define SYNC_H

#include <semaphore.h>
#include <queue>
#include <mutex>
#include <atomic>

#include "Types.h"
#include "chessboard/scacchiera.h"


struct ComandoMossa {
    Posizione src;
    Posizione dest;
};



struct EventoGUI {
    Scacchiera scacchiera;
    bool mossaValida;
    StatoPartita statoPartita;
    std::string vincitore;
};


struct SyncContext {

    std::queue<ComandoMossa> inputQueue;
    std::queue<EventoGUI> outputQueue;

    std::mutex inputMutex;
    std::mutex outputMutex;

    sem_t inputReady;       //almeno una mossa disponibile per Game
    sem_t moveProcessed;    // Game ha finito di elaborare la mossa

    std::atomic<bool> running{true};


    SyncContext() {
        sem_init(&inputReady, 0, 0);
        sem_init(&moveProcessed, 0, 0);
    }


    ~SyncContext() {
        sem_destroy(&inputReady);
        sem_destroy(&moveProcessed);
    }
};

#endif