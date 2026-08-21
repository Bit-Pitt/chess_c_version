#include "Re.h"
#include "../chessboard/scacchiera.h"
#include "../utils/logicUtils.h"
#include <stdexcept>
#include <algorithm>

Re::Re(Colore colore) : Pezzo(colore), maiMosso(true) {}

RisultatoMossa Re::sposta(Scacchiera& scacchiera, Posizione csrc, Posizione cdest) {
    RisultatoMossa res = Pezzo::sposta(scacchiera,csrc,cdest);
    maiMosso = false;
    return res;
}

std::vector<Posizione> Re::destinations(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) {
    Pezzo* pezzo = scacchiera.getPezzo(csrc);

    if (pezzo == nullptr || pezzo->getTipo() != TipoPezzo::KING)
        throw std::runtime_error("[Internal error] Non è presente il re nella casella!");

    std::vector<Posizione> dest = movimentoRe(scacchiera, csrc, giocatore, true, false);

    Colore avversario = (giocatore == Colore::WHITE) ? Colore::BLACK : Colore::WHITE;

    std::vector<Posizione> controllateAvversario = caseControllateDaGiocatore(scacchiera, avversario, true, giocatore);

    dest.erase(std::remove_if(dest.begin(), dest.end(), [&](const Posizione& pos) {
        return std::find(controllateAvversario.begin(), controllateAvversario.end(), pos) != controllateAvversario.end();
    }), dest.end());

    std::vector<Posizione> arroccoDest = arrocco(scacchiera, csrc, giocatore, controllateAvversario);
    dest.insert(dest.end(), arroccoDest.begin(), arroccoDest.end());

    return dest;
}

std::vector<Posizione> Re::caseControllate(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) {
    Pezzo* pezzo = scacchiera.getPezzo(csrc);

    if (pezzo == nullptr || pezzo->getTipo() != TipoPezzo::KING)
        throw std::runtime_error("[Internal error] Non è presente il re nella casella!");

    return movimentoRe(scacchiera, csrc, giocatore, false, true);
}

TipoPezzo Re::getTipo() const {
    return TipoPezzo::KING;
}

bool Re::hasMaiMosso() const {
    return maiMosso;
}

void Re::setMaiMosso(bool valore) {
    maiMosso = valore;
}