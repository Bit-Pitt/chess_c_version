#include "Pedone.h"
#include "../chessboard/scacchiera.h"
#include "../utils/logicUtils.h"
#include <stdexcept>

Pedone::Pedone(Colore colore) : Pezzo(colore), maiMosso(true) {}

RisultatoMossa Pedone::sposta(Scacchiera& scacchiera, Posizione csrc, Posizione cdest) {
    Pezzo::sposta(scacchiera,csrc,cdest);
    maiMosso = false;
    if (colore == Colore::WHITE && cdest.riga == 7) return RisultatoMossa::Promozione;
    if (colore == Colore::BLACK && cdest.riga == 0) return RisultatoMossa::Promozione;
    return RisultatoMossa::NonPromozione;
}

std::vector<Posizione> Pedone::destinations(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) {
    Pezzo* pezzo = scacchiera.getPezzo(csrc);

    if (pezzo == nullptr || pezzo->getTipo() != TipoPezzo::PAWN)
        throw std::runtime_error("[Internal error] Non è presente un pedone nella casella!");

    std::vector<Posizione> dest;

    int direzione = (giocatore == Colore::WHITE) ? 1 : -1;

    Posizione unoAvanti{csrc.riga + direzione, csrc.colonna};

    if (scacchiera.casellaValida(unoAvanti) && scacchiera.casellaVuota(unoAvanti))
        dest.push_back(unoAvanti);

    Posizione dueAvanti{csrc.riga + 2 * direzione, csrc.colonna};

    if (maiMosso && scacchiera.casellaValida(unoAvanti) && scacchiera.casellaValida(dueAvanti) &&
        scacchiera.casellaVuota(unoAvanti) && scacchiera.casellaVuota(dueAvanti))
        dest.push_back(dueAvanti);

    Posizione diagSx{csrc.riga + direzione, csrc.colonna - direzione};
    Posizione diagDx{csrc.riga + direzione, csrc.colonna + direzione};

    if (scacchiera.casellaValida(diagSx) && !scacchiera.casellaVuota(diagSx)) {
        Pezzo* p = scacchiera.getPezzo(diagSx);
        if (p->getColore() != giocatore)
            dest.push_back(diagSx);
    }

    if (scacchiera.casellaValida(diagDx) && !scacchiera.casellaVuota(diagDx)) {
        Pezzo* p = scacchiera.getPezzo(diagDx);
        if (p->getColore() != giocatore)
            dest.push_back(diagDx);
    }

    return filtroInchiodatura(scacchiera, csrc, dest);
}

std::vector<Posizione> Pedone::caseControllate(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) {
    Pezzo* pezzo = scacchiera.getPezzo(csrc);

    if (pezzo == nullptr || pezzo->getTipo() != TipoPezzo::PAWN)
        throw std::runtime_error("[Internal error] Non è presente un pedone nella casella!");

    std::vector<Posizione> posControllate;

    int direzione = (giocatore == Colore::WHITE) ? 1 : -1;

    Posizione diagSx{csrc.riga + direzione, csrc.colonna - direzione};
    Posizione diagDx{csrc.riga + direzione, csrc.colonna + direzione};

    if (scacchiera.casellaValida(diagSx))
        posControllate.push_back(diagSx);

    if (scacchiera.casellaValida(diagDx))
        posControllate.push_back(diagDx);

    return posControllate;
}

TipoPezzo Pedone::getTipo() const {
    return TipoPezzo::PAWN;
}

bool Pedone::hasMaiMosso() const {
    return maiMosso;
}

void Pedone::setMaiMosso(bool valore) {
    maiMosso = valore;
}