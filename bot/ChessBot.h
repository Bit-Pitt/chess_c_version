#ifndef CHESS_BOT_H
#define CHESS_BOT_H

#include "../chessboard/scacchiera.h"
#include "../Types.h"

#include <vector>


struct MossaBot {
    Posizione src;
    Posizione dest;
    double valore;
};

class ChessBot {

private:
    Scacchiera& scacchiera;
    double epsilon;

    double valutaMossa(Posizione src, Posizione dest, Colore giocatore);
    bool vaAvanti(Posizione src, Posizione dest, Colore giocatore);
    bool isAttaccato(Scacchiera& scacchiera, Posizione pos, Colore avversario);

public:
    ChessBot(Scacchiera& scacchiera, double epsilon = 0.10);

    std::vector<MossaBot> getTopMoves(Colore giocatore);
    //MossaBot scegliMossa(Colore giocatore);
    double valutaMossaTask(Posizione src, Posizione dest, Colore giocatore);

    std::vector<MossaBot> getMosseLegali(Colore giocatore);
    MossaBot scegliMossa(const std::vector<MossaBot>& mosse);

};

#endif