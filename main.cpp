#include <iostream>
#include "GUI/chessGUI.h"
#include "chessboard/scacchiera.h"
#include "gameNoGui/game.h"
#include "pieces/Pedone.h"
#include "pieces/Torre.h"
#include "pieces/Re.h"
#include "pieces/Alfiere.h"
#include "pieces/Cavallo.h"
#include "pieces/Regina.h"
#include "bot/BotCoordinator.h"
#include "bot/BotWorker.h"
#include "GUI/monitor.h"

#include "gameLogic/GameClass.h"
#include "utils/graphicUtils.h"

#include <thread>
#include "Sync.h"


// Per avviare il game in modalità debug
const bool DEBUG = false;


int main() {

    // [TODO] --> FIX memory leak: delete pezzi creati con new
    std::cout << "Avvio partita\n";

    // Creazione scacchiera
    Scacchiera scacchiera;


    // =========================================================
    // PEDONI
    // =========================================================

    // Pedoni bianchi sulla seconda riga: (1, *)
    // Pedoni neri sulla settima riga: (6, *)
    for (int i = 0; i < 8; ++i) {

        Pedone* pw = new Pedone(Colore::WHITE);
        Pedone* pb = new Pedone(Colore::BLACK);

        scacchiera.aggiungiPezzo(pw, Posizione{1, i});
        scacchiera.aggiungiPezzo(pb, Posizione{6, i});
    }


    // =========================================================
    // TORRI
    // =========================================================

    scacchiera.aggiungiPezzo(new Torre(Colore::WHITE), Posizione{0, 0});
    scacchiera.aggiungiPezzo(new Torre(Colore::WHITE), Posizione{0, 7});
    scacchiera.aggiungiPezzo(new Torre(Colore::BLACK), Posizione{7, 0});
    scacchiera.aggiungiPezzo(new Torre(Colore::BLACK), Posizione{7, 7});


    // =========================================================
    // RE
    // =========================================================

    scacchiera.aggiungiPezzo(new Re(Colore::WHITE), Posizione{0, 4});
    scacchiera.aggiungiPezzo(new Re(Colore::BLACK), Posizione{7, 4});


    // =========================================================
    // ALFIERI
    // =========================================================

    scacchiera.aggiungiPezzo(new Alfiere(Colore::WHITE), Posizione{0, 2});
    scacchiera.aggiungiPezzo(new Alfiere(Colore::WHITE), Posizione{0, 5});
    scacchiera.aggiungiPezzo(new Alfiere(Colore::BLACK), Posizione{7, 2});
    scacchiera.aggiungiPezzo(new Alfiere(Colore::BLACK), Posizione{7, 5});


    // =========================================================
    // CAVALLI
    // =========================================================

    scacchiera.aggiungiPezzo(new Cavallo(Colore::WHITE), Posizione{0, 1});
    scacchiera.aggiungiPezzo(new Cavallo(Colore::WHITE), Posizione{0, 6});
    scacchiera.aggiungiPezzo(new Cavallo(Colore::BLACK), Posizione{7, 1});
    scacchiera.aggiungiPezzo(new Cavallo(Colore::BLACK), Posizione{7, 6});


    // =========================================================
    // REGINE
    // =========================================================

    scacchiera.aggiungiPezzo(new Regina(Colore::WHITE), Posizione{0, 3});
    scacchiera.aggiungiPezzo(new Regina(Colore::BLACK), Posizione{7, 3});


    // Stampa iniziale della scacchiera
    scacchiera.print();


    // =========================================================
    // AVVIO PARTITA
    // =========================================================

    if (DEBUG) {

        // Eventuale versione senza GUI
        partitaNoGui(scacchiera);

    } else {
        std::cout << "Scegli modalità di gioco:\n";
        std::cout << "1) Due giocatori\n";
        std::cout << "2) Un giocatore vs bot\n";
        std::cout << "3) Bot forte vs bot random\n";

        int scelta;
        std::cin >> scelta;


        ConfigurazioneGiocatori configurazione;
        if (scelta == 1) {
            configurazione = {TipoGiocatore::UMANO, TipoGiocatore::UMANO};
        }
        else if (scelta == 2) {  //il giocatore è il bianco
            configurazione = {TipoGiocatore::UMANO, TipoGiocatore::BOT};
        }
        else {
            configurazione = {TipoGiocatore::BOT, TipoGiocatore::BOT};
        }
        
        SyncContext sync;

        Game partita(scacchiera, configurazione);

        BotCoordinator botCoordinator(4);

        Monitor monitor;

        std::vector<std::thread> workerThreads;

        for (int i = 0; i < 4; ++i) {
            workerThreads.emplace_back([&]() {
                BotWorker worker;
                worker.run(sync);
            });
        }

        std::thread gameThread([&]() {
            partita.run(sync);
        });

        std::thread botThread([&]() {
            botCoordinator.run(sync);
        });

        std::thread monitorThread([&]() {
            monitor.run(sync);
        });

        std::thread guiThread([&]() {
            ChessGUI gui(partita, sync);
            gui.run();
        });

        guiThread.join();

        sync.running = false;
        //serve per far terminare gli altri thread (quando leggeranno running=False)
        sem_post(&sync.inputReady);
        sem_post(&sync.botInputReady);
        sem_post(&sync.botMoveReady);

        for (int i = 0; i < 4; ++i)
            sem_post(&sync.taskReady);

        gameThread.join();
        botThread.join();

        for (std::thread& worker : workerThreads)
            worker.join();

    }



    std::cout << "Grazie per aver giocato\n";

    return 0;
}



