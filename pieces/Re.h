#pragma once

#include "Pezzo.h"

class Re : public Pezzo {
private:
    bool maiMosso;

public:
    explicit Re(Colore colore);

    RisultatoMossa sposta(Scacchiera& scacchiera, Posizione csrc, Posizione cdest) override;
    std::vector<Posizione> destinations(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) override;
    std::vector<Posizione> caseControllate(Scacchiera& scacchiera, Posizione csrc, Colore giocatore) override;

    TipoPezzo getTipo() const override;
    bool hasMaiMosso() const override;

    void setMaiMosso(bool valore);

private:
    std::vector<Posizione> arrocco(Scacchiera& scacchiera, Posizione csrc, Colore giocatore, const std::vector<Posizione>& controllateAvversario);
};