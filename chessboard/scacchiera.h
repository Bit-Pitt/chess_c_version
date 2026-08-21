#pragma once

#include <array>
#include <vector>
#include "Pezzo.h"

class Scacchiera {
private:
    std::array<std::array<Pezzo*, 8>, 8> scacchiera;

public:
    Scacchiera();

    bool casellaValida(Posizione pos) const;
    void aggiungiPezzo(Pezzo* pezzo, Posizione pos);

    Pezzo* getPezzo(Posizione pos) const;

    bool casellaVuota(Posizione pos) const;
    bool caselleVuote(const std::vector<Posizione>& caselle) const;
    bool caselleValide(const std::vector<Posizione>& caselle) const;

    void print() const;

    Posizione getPosRe(Colore giocatore) const;

    bool controllaCasaPromo(Posizione pos) const;
    void promuovi();
};