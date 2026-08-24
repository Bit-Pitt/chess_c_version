#include "Pezzo.h"
#include "../chessboard/scacchiera.h"
#include <iostream>
#include <vector>
#include "../utils/logicUtils.h"
#include <algorithm>

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


std::vector<Posizione> Pezzo::filtroInchiodatura(Scacchiera& scacchiera, Posizione csrc, const std::vector<Posizione>& destinazioni) {

    std::vector<Posizione> caseMantengonoInchiodatura;

    // Controllo se il pezzo è inchiodato
    InfoScacchi info = infoScacchi(scacchiera, colore);

    // Rimuovo temporaneamente il pezzo
    scacchiera.aggiungiPezzo(nullptr, csrc);

    // Ricontrollo gli scacchi
    InfoScacchi infoNuove = infoScacchi(scacchiera, colore);

    // Riaggiungo il pezzo
    scacchiera.aggiungiPezzo(this, csrc);

    // Se togliendo il pezzo è comparso un nuovo scacco,
    // significa che il pezzo era inchiodato
    if (infoNuove.numeroScacchi > info.numeroScacchi) {

        Posizione posRe = scacchiera.getPosRe(colore);

        // Controllo gli scacchi comparsi dopo aver tolto il pezzo
        for (const Posizione& posScacco : infoNuove.posizioniScacco) {

            std::vector<Posizione> caseInMezzo = caseInLinea(posRe, posScacco);

            // Se il pezzo rimosso si trova sulla linea
            // tra re e pezzo che dà scacco, questa è la
            // linea dell'inchiodatura
            if (std::find(caseInMezzo.begin(), caseInMezzo.end(), csrc) != caseInMezzo.end()) {

                caseMantengonoInchiodatura = caseInMezzo;

                // È possibile anche catturare il pezzo che
                // sta effettuando l'inchiodatura
                caseMantengonoInchiodatura.push_back(posScacco);
            }
        }

        std::vector<Posizione> risultato;

        for (const Posizione& dest : destinazioni) {

            if (std::find(caseMantengonoInchiodatura.begin(), caseMantengonoInchiodatura.end(), dest) != caseMantengonoInchiodatura.end())
                risultato.push_back(dest);
        }

        return risultato;
    }

    return destinazioni;
}