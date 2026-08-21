#include "scacchiera.h"
#include "Regina.h"
#include <iostream>
#include <stdexcept>
#include <iomanip>

Scacchiera::Scacchiera() {
    for (auto& riga : scacchiera)
        riga.fill(nullptr);
}

bool Scacchiera::casellaValida(Posizione pos) const {
    return pos.riga >= 0 && pos.riga < 8 && pos.colonna >= 0 && pos.colonna < 8;
}

void Scacchiera::aggiungiPezzo(Pezzo* pezzo, Posizione pos) {
    if (!casellaValida(pos))
        throw std::out_of_range("Posizione non valida");
    scacchiera[pos.riga][pos.colonna] = pezzo;
}

Pezzo* Scacchiera::getPezzo(Posizione pos) const {
    if (!casellaValida(pos))
        throw std::out_of_range("Posizione non valida");
    return scacchiera[pos.riga][pos.colonna];
}

bool Scacchiera::casellaVuota(Posizione pos) const {
    return getPezzo(pos) == nullptr;
}

bool Scacchiera::caselleVuote(const std::vector<Posizione>& caselle) const {
    for (Posizione pos : caselle)
        if (!casellaVuota(pos)) return false;
    return true;
}

bool Scacchiera::caselleValide(const std::vector<Posizione>& caselle) const {
    for (Posizione pos : caselle)
        if (!casellaValida(pos)) return false;
    return true;
}

Posizione Scacchiera::getPosRe(Colore giocatore) const {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            Pezzo* pezzo = scacchiera[i][j];
            if (pezzo != nullptr && pezzo->getTipo() == TipoPezzo::KING && pezzo->getColore() == giocatore)
                return {i, j};
        }

    throw std::runtime_error("Re non trovato!");
}

bool Scacchiera::controllaCasaPromo(Posizione pos) const {
    Pezzo* pezzo = getPezzo(pos);

    if (pezzo == nullptr || pezzo->getTipo() != TipoPezzo::PAWN)
        return false;

    if (pezzo->getColore() == Colore::WHITE && pos.riga == 7)
        return true;

    if (pezzo->getColore() == Colore::BLACK && pos.riga == 0)
        return true;

    return false;
}

void Scacchiera::promuovi() {
    Posizione casaPromo{-1, -1};

    for (int i = 0; i < 8; i++) {
        if (controllaCasaPromo({0, i})) {
            casaPromo = {0, i};
            break;
        }

        if (controllaCasaPromo({7, i})) {
            casaPromo = {7, i};
            break;
        }
    }

    if (!casellaValida(casaPromo))
        throw std::runtime_error("Non è stata trovata casa di promozione!");

    Pezzo* pedone = getPezzo(casaPromo);
    aggiungiPezzo(new Regina(pedone->getColore()), casaPromo);
}