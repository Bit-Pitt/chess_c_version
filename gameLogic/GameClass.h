#pragma once

#include "../chessboard/scacchiera.h"
#include "Types.h"
#include "../utils/logicUtils.h"

#include <vector>
#include <string>

class Game {
private:
    Scacchiera& scacchiera;

    Colore g1 = Colore::WHITE;
    Colore g2 = Colore::BLACK;
    Colore turno = Colore::WHITE;

    std::vector<Pezzo*> pezziPersiW;
    std::vector<Pezzo*> pezziPersiB;

    void creaFilePartita();

public:
    Game(Scacchiera& scacchiera);

    Scacchiera& getBoard();

    // Restituisce:
    // {false}
    // {true}
    // {true, "Promozione"}
    std::vector<std::string> processaMossa(const std::string& mossaStringa);

    // Restituisce:
    // {false}
    // {true, "PATTA"}
    // {true, "WHITE"/"BLACK"}
    std::vector<std::string> checkFinePartita();

    // Effettua la mossa se valida
    std::vector<std::string> muovi(
        Scacchiera& scacchiera,
        TipoPezzo nome,
        Posizione csrc,
        Posizione cdest,
        Colore giocatore,
        std::vector<Pezzo*>& pezziPersi
    );

    void promuovi();
};