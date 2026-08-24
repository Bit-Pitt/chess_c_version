#pragma once

#include "Pezzo.h"

class Torre : public Pezzo {
public:
    explicit Torre(Colore colore);

    RisultatoMossa sposta(Scacchiera& scacchiera, Posizione csrc, Posizione cdest) override;
    std::vector<Posizione> destinations(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) override;
    std::vector<Posizione> caseControllate(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) override;

    TipoPezzo getTipo() const override;
    bool hasMaiMosso() const override;
    void setMaiMosso(bool valore) override;

private:
    bool maiMosso;
};

