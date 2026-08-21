#pragma once

#include "Pezzo.h"

class Alfiere : public Pezzo {
public:
    explicit Alfiere(Colore colore);

    std::vector<Posizione> destinations(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) override;
    std::vector<Posizione> caseControllate(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) override;

    TipoPezzo getTipo() const override;
    bool hasMaiMosso() const override;
};