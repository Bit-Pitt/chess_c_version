#ifndef SYNC_H
#define SYNC_H

#include <semaphore.h>
#include <queue>
#include <mutex>
#include <atomic>

#include "Types.h"
#include "chessboard/scacchiera.h"
#include "bot/ChessBot.h"


struct ComandoMossa {
    Posizione src;
    Posizione dest;
};



struct EventoGUI {
    Scacchiera scacchiera;
    bool mossaValida;
    StatoPartita statoPartita;
    std::string vincitore;
    Colore turno;
};

struct RichiestaBot {
    Scacchiera scacchiera;
    Colore giocatore;
    double epsilon;
};

struct RispostaBot {
    MossaBot mossa;
};


struct SyncContext {

    std::queue<ComandoMossa> inputQueue;
    std::mutex inputMutex;
    sem_t inputReady;

    RichiestaBot richiestaBot;
    std::mutex botInputMutex;
    sem_t botInputReady;

    MossaBot rispostaBot;
    std::mutex botOutputMutex;
    sem_t botMoveReady;

    EventoGUI ultimoEventoGUI;
    std::mutex guiMutex;
    bool guiUpdateDisponibile = false;

    std::atomic<bool> running{true};

    SyncContext() {
        sem_init(&inputReady, 0, 0);
        sem_init(&botInputReady, 0, 0);
        sem_init(&botMoveReady, 0, 0);
    }

    ~SyncContext() {
        sem_destroy(&inputReady);
        sem_destroy(&botInputReady);
        sem_destroy(&botMoveReady);
    }
};

#endif