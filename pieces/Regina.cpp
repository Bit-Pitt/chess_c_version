#include "Regina.h"
#include "../chessboard/scacchiera.h"
#include "../utils/logicUtils.h"
#include <stdexcept>
#include <algorithm>

Regina::Regina(Colore colore) : Pezzo(colore) {}

std::vector<Posizione> Regina::destinations(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) {
    Pezzo* pezzo = scacchiera.getPezzo(csrc);

    if (pezzo == nullptr || pezzo->getTipo() != TipoPezzo::QUEEN)
        throw std::runtime_error("[Internal error] Non è presente una regina nella casella!");

    std::vector<Posizione> dest;

    auto aggiungi = [&](std::vector<Posizione> mosse) {
        dest.insert(dest.end(), mosse.begin(), mosse.end());
    };

    aggiungi(movimentoRe(scacchiera, csrc, giocatore, true,false));

    aggiungi(movimentoVerticale(scacchiera, csrc, giocatore,true, true,false));
    aggiungi(movimentoVerticale(scacchiera, csrc, giocatore, false, true,false));
    aggiungi(movimentoOrizzontale(scacchiera, csrc, giocatore,true, true,false));
    aggiungi(movimentoOrizzontale(scacchiera, csrc, giocatore, false, true,false));

    aggiungi(movimentoDiagonale(scacchiera, csrc, giocatore, Direzione::AVANTI_SX, true, false));
    aggiungi(movimentoDiagonale(scacchiera, csrc, giocatore, Direzione::AVANTI_DX, true, false));
    aggiungi(movimentoDiagonale(scacchiera, csrc, giocatore, Direzione::INDIETRO_SX, true, false));
    aggiungi(movimentoDiagonale(scacchiera, csrc, giocatore, Direzione::INDIETRO_DX, true, false));

    std::sort(dest.begin(), dest.end());
    dest.erase(std::unique(dest.begin(), dest.end()), dest.end());

    return filtroInchiodatura(scacchiera, csrc, dest);
}

std::vector<Posizione> Regina::caseControllate(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) {
    Pezzo* pezzo = scacchiera.getPezzo(csrc);

    if (pezzo == nullptr || pezzo->getTipo() != TipoPezzo::QUEEN)
        throw std::runtime_error("[Internal error] Non è presente una regina nella casella!");

    std::vector<Posizione> posControllate;

    auto aggiungi = [&](std::vector<Posizione> caselle) {
        posControllate.insert(posControllate.end(), caselle.begin(), caselle.end());
    };

    aggiungi(movimentoRe(scacchiera, csrc, giocatore, false, true));

    aggiungi(movimentoVerticale(scacchiera, csrc, giocatore, true,false, true));
    aggiungi(movimentoVerticale(scacchiera, csrc, giocatore, false,false, true));
    aggiungi(movimentoOrizzontale(scacchiera, csrc, giocatore, true,false,true));
    aggiungi(movimentoOrizzontale(scacchiera, csrc, giocatore, false,false, true));

    aggiungi(movimentoDiagonale(scacchiera, csrc, giocatore, Direzione::AVANTI_SX, false , true));
    aggiungi(movimentoDiagonale(scacchiera, csrc, giocatore, Direzione::AVANTI_DX, false, true));
    aggiungi(movimentoDiagonale(scacchiera, csrc, giocatore, Direzione::INDIETRO_SX, false,  true));
    aggiungi(movimentoDiagonale(scacchiera, csrc, giocatore, Direzione::INDIETRO_DX,false , true));

    std::sort(posControllate.begin(), posControllate.end());
    posControllate.erase(std::unique(posControllate.begin(), posControllate.end()), posControllate.end());

    return posControllate;
}

TipoPezzo Regina::getTipo() const {
    return TipoPezzo::QUEEN;
}

bool Regina::hasMaiMosso() const {
    return false;
}