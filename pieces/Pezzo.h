#pragma once
#include <vector>
#include "../Types.h"
#include "../utils/logicUtils.h"

class Scacchiera;

class Pezzo {
    protected:
        Colore colore;

    public:
        explicit Pezzo(Colore colore);
        virtual ~Pezzo() = default;

        Colore getColore() const;
        virtual TipoPezzo getTipo() const = 0;
        virtual std::vector<Posizione> destinations(Scacchiera& scacchiera, 
                                                    Posizione csrc, Colore giocatore) = 0;

        virtual std::vector<Posizione> caseControllate(Scacchiera& scacchiera, 
                                                    Posizione csrc, Colore giocatore) = 0;
        
        virtual bool hasMaiMosso() const;
        virtual RisultatoMossa sposta(Scacchiera& scacchiera, Posizione csrc, Posizione cdest);
        virtual void setMaiMosso(bool valore);

    protected:
        std::vector<Posizione> filtroInchiodatura(Scacchiera& scacchiera, Posizione csrc,
                                                 const std::vector<Posizione>& destinazioni);
};