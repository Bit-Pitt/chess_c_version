#pragma once

#include "Pezzo.h"

class Regina : public Pezzo {
public:
    explicit Regina(Colore colore);

    std::vector<Posizione> destinations(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) override;
    std::vector<Posizione> caseControllate(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) override;

    TipoPezzo getTipo() const override;
    bool hasMaiMosso() const override;
};