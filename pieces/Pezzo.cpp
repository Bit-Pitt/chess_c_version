#include "Pezzo.h"
#include "../chessboard/scacchiera.h"
#include <iostream>

Pezzo::Pezzo(Colore colore) : colore(colore) {}

Colore Pezzo::getColore() const { 
    return colore; 
}

bool Pezzo::hasMaiMosso() const { 
    return false; 
}

RisultatoMossa Pezzo::sposta(Scacchiera& scacchiera, Posizione csrc, Posizione cdest) {
    scacchiera.aggiungiPezzo(nullptr,csrc);
    if (!scacchiera.casellaVuota(cdest)) std::cout << "Catturato un pezzo!\n";
    scacchiera.aggiungiPezzo(this,cdest);
    return RisultatoMossa::NonPromozione;
}