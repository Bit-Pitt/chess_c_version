#include "Alfiere.h"
#include "../chessboard/scacchiera.h"
#include "../utils/logicUtils.h"
#include <stdexcept>

Alfiere::Alfiere(Colore colore) : Pezzo(colore) {}

std::vector<Posizione> Alfiere::destinations(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) {
    Pezzo* pezzo = scacchiera.getPezzo(csrc);

    if (pezzo == nullptr || pezzo->getTipo() != TipoPezzo::BISHOP)
        throw std::runtime_error("[Internal error] Non è presente un alfiere nella casella!");

    std::vector<Posizione> dest;

    auto temp = movimentoDiagonale(scacchiera, csrc, giocatore, Direzione::AVANTI_SX, true, false);
    dest.insert(dest.end(), temp.begin(), temp.end());

    temp = movimentoDiagonale(scacchiera, csrc, giocatore, Direzione::AVANTI_DX, true, false);
    dest.insert(dest.end(), temp.begin(), temp.end());

    temp = movimentoDiagonale(scacchiera, csrc, giocatore, Direzione::INDIETRO_SX, true, false);
    dest.insert(dest.end(), temp.begin(), temp.end());

    temp = movimentoDiagonale(scacchiera, csrc, giocatore, Direzione::INDIETRO_DX, true, false);
    dest.insert(dest.end(), temp.begin(), temp.end());

    return filtroInchiodatura(scacchiera, csrc, dest);
}

std::vector<Posizione> Alfiere::caseControllate(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) {
    Pezzo* pezzo = scacchiera.getPezzo(csrc);

    if (pezzo == nullptr || pezzo->getTipo() != TipoPezzo::BISHOP)
        throw std::runtime_error("[Internal error] Non è presente un alfiere nella casella!");

    std::vector<Posizione> posControllate;

    auto temp = movimentoDiagonale(scacchiera, csrc, giocatore, Direzione::AVANTI_SX, false, true);
    posControllate.insert(posControllate.end(), temp.begin(), temp.end());

    temp = movimentoDiagonale(scacchiera, csrc, giocatore, Direzione::AVANTI_DX, false, true);
    posControllate.insert(posControllate.end(), temp.begin(), temp.end());

    temp = movimentoDiagonale(scacchiera, csrc, giocatore, Direzione::INDIETRO_SX, false, true);
    posControllate.insert(posControllate.end(), temp.begin(), temp.end());

    temp = movimentoDiagonale(scacchiera, csrc, giocatore, Direzione::INDIETRO_DX, false, true);
    posControllate.insert(posControllate.end(), temp.begin(), temp.end());

    return posControllate;
}

TipoPezzo Alfiere::getTipo() const {
    return TipoPezzo::BISHOP;
}

bool Alfiere::hasMaiMosso() const {
    return false;
}