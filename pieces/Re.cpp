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

    std::vector<Posizione> controllateAvversario =
        caseControllateDaGiocatore(scacchiera, avversario, true, giocatore);

    dest.erase(std::remove_if(dest.begin(), dest.end(), [&](const Posizione& pos) {
        return std::find(controllateAvversario.begin(), controllateAvversario.end(), pos)
               != controllateAvversario.end();
    }), dest.end());

    std::vector<Posizione> arroccoDest =
        arrocco(scacchiera, csrc, giocatore, controllateAvversario);

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


std::vector<Posizione> Re::arrocco(Scacchiera& scacchiera, Posizione csrc, Colore giocatore, const std::vector<Posizione>& controllateAvversario) {

    std::vector<Posizione> dests;

    if (!maiMosso)
        return dests;

    if (giocatore == Colore::WHITE) {

        Posizione posRe{0, 4};

        Pezzo* torreCorta = scacchiera.getPezzo(Posizione{0, 7});

        if (torreCorta != nullptr &&
            torreCorta->getTipo() == TipoPezzo::ROOK &&
            torreCorta->getColore() == giocatore &&
            torreCorta->hasMaiMosso()) {

            Posizione pos1{0, 5};
            Posizione pos2{0, 6};

            if (scacchiera.casellaVuota(pos1) &&
                scacchiera.casellaVuota(pos2) &&
                std::find(controllateAvversario.begin(), controllateAvversario.end(), posRe) == controllateAvversario.end() &&
                std::find(controllateAvversario.begin(), controllateAvversario.end(), pos1) == controllateAvversario.end() &&
                std::find(controllateAvversario.begin(), controllateAvversario.end(), pos2) == controllateAvversario.end()) {

                dests.push_back(pos2);
            }
        }

        Pezzo* torreLunga = scacchiera.getPezzo(Posizione{0, 0});

        if (torreLunga != nullptr &&
            torreLunga->getTipo() == TipoPezzo::ROOK &&
            torreLunga->getColore() == giocatore &&
            torreLunga->hasMaiMosso()) {

            Posizione pos1{0, 2};
            Posizione pos2{0, 3};
            Posizione posVuota{0, 1};

            if (scacchiera.casellaVuota(posVuota) &&
                scacchiera.casellaVuota(pos1) &&
                scacchiera.casellaVuota(pos2) &&
                std::find(controllateAvversario.begin(), controllateAvversario.end(), posRe) == controllateAvversario.end() &&
                std::find(controllateAvversario.begin(), controllateAvversario.end(), pos1) == controllateAvversario.end() &&
                std::find(controllateAvversario.begin(), controllateAvversario.end(), pos2) == controllateAvversario.end()) {

                dests.push_back(pos1);
            }
        }

    } else {

        Posizione posRe{7, 4};

        Pezzo* torreCorta = scacchiera.getPezzo(Posizione{7, 7});

        if (torreCorta != nullptr &&
            torreCorta->getTipo() == TipoPezzo::ROOK &&
            torreCorta->getColore() == giocatore &&
            torreCorta->hasMaiMosso()) {

            Posizione pos1{7, 5};
            Posizione pos2{7, 6};

            if (scacchiera.casellaVuota(pos1) &&
                scacchiera.casellaVuota(pos2) &&
                std::find(controllateAvversario.begin(), controllateAvversario.end(), posRe) == controllateAvversario.end() &&
                std::find(controllateAvversario.begin(), controllateAvversario.end(), pos1) == controllateAvversario.end() &&
                std::find(controllateAvversario.begin(), controllateAvversario.end(), pos2) == controllateAvversario.end()) {

                dests.push_back(pos2);
            }
        }

        Pezzo* torreLunga = scacchiera.getPezzo(Posizione{7, 0});

        if (torreLunga != nullptr &&
            torreLunga->getTipo() == TipoPezzo::ROOK &&
            torreLunga->getColore() == giocatore &&
            torreLunga->hasMaiMosso()) {

            Posizione pos1{7, 2};
            Posizione pos2{7, 3};
            Posizione posVuota{7, 1};

            if (scacchiera.casellaVuota(posVuota) &&
                scacchiera.casellaVuota(pos1) &&
                scacchiera.casellaVuota(pos2) &&
                std::find(controllateAvversario.begin(), controllateAvversario.end(), posRe) == controllateAvversario.end() &&
                std::find(controllateAvversario.begin(), controllateAvversario.end(), pos1) == controllateAvversario.end() &&
                std::find(controllateAvversario.begin(), controllateAvversario.end(), pos2) == controllateAvversario.end()) {

                dests.push_back(pos1);
            }
        }
    }

    return dests;
}