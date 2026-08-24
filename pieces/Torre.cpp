#include "Torre.h"
#include "../chessboard/scacchiera.h"
#include "../utils/logicUtils.h"
#include <stdexcept>

Torre::Torre(Colore colore) : Pezzo(colore), maiMosso(true) {}


RisultatoMossa Torre::sposta(Scacchiera& scacchiera, Posizione csrc, Posizione cdest) {
    RisultatoMossa res = Pezzo::sposta(scacchiera,csrc,cdest);
    maiMosso = false;
    return res;
}

std::vector<Posizione> Torre::destinations(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) {
    Pezzo* pezzo = scacchiera.getPezzo(csrc);
    if (pezzo == nullptr || pezzo->getTipo() != TipoPezzo::ROOK) throw std::runtime_error("[Internal error] Non è presente una torre nella casella!");

    std::vector<Posizione> dest;
    auto aggiungi = [&](std::vector<Posizione> v) { dest.insert(dest.end(), v.begin(), v.end()); };

    aggiungi(movimentoVerticale(scacchiera, csrc, giocatore, true, true,false));
    aggiungi(movimentoVerticale(scacchiera, csrc, giocatore, false, true,false));
    aggiungi(movimentoOrizzontale(scacchiera, csrc, giocatore, true, true,false));
    aggiungi(movimentoOrizzontale(scacchiera, csrc, giocatore, false, true,false));

    return filtroInchiodatura(scacchiera, csrc, dest);
}

std::vector<Posizione> Torre::caseControllate(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) {
    Pezzo* pezzo = scacchiera.getPezzo(csrc);
    if (pezzo == nullptr || pezzo->getTipo() != TipoPezzo::ROOK) throw std::runtime_error("[Internal error] Non è presente una torre nella casella!");

    std::vector<Posizione> posControllate;
    auto aggiungi = [&](std::vector<Posizione> v) { posControllate.insert(posControllate.end(), v.begin(), v.end()); };

    aggiungi(movimentoVerticale(scacchiera, csrc, giocatore, true, false, true));
    aggiungi(movimentoVerticale(scacchiera, csrc, giocatore, false, false, true));
    aggiungi(movimentoOrizzontale(scacchiera, csrc, giocatore, true, false, true));
    aggiungi(movimentoOrizzontale(scacchiera, csrc, giocatore, false, false, true));

    return posControllate;
}

TipoPezzo Torre::getTipo() const {
    return TipoPezzo::ROOK;
}

bool Torre::hasMaiMosso() const {
    return maiMosso;
}


void Torre::setMaiMosso(bool valore) {
    maiMosso = valore;
}