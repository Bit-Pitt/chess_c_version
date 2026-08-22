#include <iostream>

#include "chessboard/scacchiera.h"
#include "gameNoGui/game.h"
#include "pieces/Pedone.h"
#include "pieces/Torre.h"
#include "pieces/Re.h"
#include "pieces/Alfiere.h"
#include "pieces/Cavallo.h"
#include "pieces/Regina.h"

#include "gameLogic/GameClass.h"
#include "utils/graphicUtils.h"


// Per avviare il game in modalità debug
const bool DEBUG = true;


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

        Game partita(scacchiera);

        ChessGUI gui(partita);

        gui.run();
    }


    delete &scacchiera;

    std::cout << "Grazie per aver giocato\n";

    return 0;
}