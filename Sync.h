#ifndef SYNC_H
#define SYNC_H

#include <semaphore.h>
#include <queue>
#include <mutex>
#include <atomic>
#include <string>

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

struct MossaBotTask {
    Scacchiera scacchiera;
    Posizione src;
    Posizione dest;
    Colore giocatore;
};


struct RisultatoBotTask {
    Posizione src;
    Posizione dest;
    double valore;
};


struct StatoMonitor {
    Colore turno;
    std::string ultimaMossa;
    StatoPartita statoPartita;
    std::string vincitore;
};


struct SyncContext {

    // GUI --> Game
    std::queue<ComandoMossa> inputQueue;
    std::mutex inputMutex;
    sem_t inputReady;

    // Game --> Bot coordinator
    RichiestaBot richiestaBot;
    std::mutex botInputMutex;
    sem_t botInputReady;

    //Bot coordinator --> GAME
    MossaBot rispostaBot;
    std::mutex botOutputMutex;
    sem_t botMoveReady;

    //Game-->GUI
    EventoGUI ultimoEventoGUI;
    std::mutex guiMutex;
    bool guiUpdateDisponibile = false;

    // BOT COORDINATOR -> WORKERS
    std::queue<MossaBotTask> taskQueue;
    std::mutex taskMutex;
    sem_t taskReady;


    // WORKERS -> BOT COORDINATOR
    std::queue<RisultatoBotTask> resultQueue;
    std::mutex resultMutex;
    sem_t resultReady;

    // Game --> monitor
    std::queue<StatoMonitor> monitorQueue;
    std::mutex monitorMutex;
    sem_t monitorReady;



    std::atomic<bool> running{true};

    SyncContext() {
        sem_init(&inputReady, 0, 0);
        sem_init(&botInputReady, 0, 0);
        sem_init(&botMoveReady, 0, 0);
        sem_init(&taskReady, 0, 0);
        sem_init(&resultReady, 0, 0);
        sem_init(&monitorReady, 0, 0);
    }

    ~SyncContext() {
        sem_destroy(&inputReady);
        sem_destroy(&botInputReady);
        sem_destroy(&botMoveReady);
        sem_destroy(&taskReady);
        sem_destroy(&resultReady);
        sem_destroy(&monitorReady);
    }
};

#endif