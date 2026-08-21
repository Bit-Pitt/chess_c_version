#include "Cavallo.h"
#include "../chessboard/scacchiera.h"
#include "../utils/logicUtils.h"
#include <stdexcept>

Cavallo::Cavallo(Colore colore) : Pezzo(colore) {}

std::vector<Posizione> Cavallo::destinations(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) {
    Pezzo* pezzo = scacchiera.getPezzo(csrc);

    if (pezzo == nullptr || pezzo->getTipo() != TipoPezzo::KNIGHT)
        throw std::runtime_error("[Internal error] Non è presente un cavallo nella casella!");

    auto dest = movimentoCavallo(scacchiera, csrc, giocatore, true);
    return filtroInchiodatura(scacchiera, csrc, dest);
}

std::vector<Posizione> Cavallo::caseControllate(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) {
    Pezzo* pezzo = scacchiera.getPezzo(csrc);

    if (pezzo == nullptr || pezzo->getTipo() != TipoPezzo::KNIGHT)
        throw std::runtime_error("[Internal error] Non è presente un cavallo nella casella!");

    return movimentoCavallo(scacchiera, csrc, giocatore, false);
}

TipoPezzo Cavallo::getTipo() const {
    return TipoPezzo::KNIGHT;
}

bool Cavallo::hasMaiMosso() const {
    return false;
}