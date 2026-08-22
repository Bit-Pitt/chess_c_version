#include "GameClass.h"
#include "../utils/graphicUtils.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>


Game::Game(Scacchiera& scacchiera) : scacchiera(scacchiera) {
    creaFilePartita();
}


void Game::creaFilePartita() {
    std::ofstream file("ultimo_game.txt");

    if (!file.is_open()) {
        throw std::runtime_error("Impossibile creare ultimo_game.txt");
    }

    file << "Ultima partita giocata.\n";
}


Scacchiera& Game::getBoard() {
    return scacchiera;
}


std::vector<std::string> Game::processaMossa(const std::string& mossaStringa) {
    std::stringstream ss(mossaStringa);

    std::string nomeStringa;
    std::string srcStringa;
    std::string destStringa;
    std::string extra;

    ss >> nomeStringa >> srcStringa >> destStringa;

    if (nomeStringa.empty() || srcStringa.empty() || destStringa.empty() || (ss >> extra)) {
        std::cout << "Input non valido\n";
        return {"false"};
    }

    TipoPezzo nome = traduciNome(nomeStringa);
    Posizione csrc = stringToPos(srcStringa);
    Posizione cdest = stringToPos(destStringa);

    std::vector<Pezzo*>& pezziPersi = (turno == g1) ? pezziPersiB : pezziPersiW;

    std::cout << "[DEBUG] parametri: "
              << nomeStringa << " "
              << srcStringa << " "
              << destStringa << " "
              << (turno == Colore::WHITE ? "WHITE" : "BLACK")
              << "\n";

    std::vector<std::string> res = muovi(scacchiera, nome, csrc, cdest, turno, pezziPersi);

    if (res.empty() || res[0] == "false") {
        std::cout << "Mossa non valida\n";
        return {"false"};
    }

    std::ofstream file("ultimo_game.txt", std::ios::app);

    if (file.is_open()) {
        file << "\"" << mossaStringa << "\",\n";
    }

    std::cout << "DEBUG: CAMBIO TURNO\n";

    turno = (turno == g1) ? g2 : g1;

    scacchiera.print();

    if (res.size() > 1) {
        return {"true", "Promozione"};
    }

    return {"true"};
}


std::vector<std::string> Game::checkFinePartita() {
    int res = partitaFinita(scacchiera, turno);

    if (res == 1) {
        std::cout << "Patta\n";
        return {"true", "PATTA"};
    }

    if (res == 2) {
        Colore vincitore = (turno == g1) ? g2 : g1;

        std::cout << "Vince: ";

        if (vincitore == Colore::WHITE) {
            std::cout << "WHITE\n";
            return {"true", "WHITE"};
        }
        else {
            std::cout << "BLACK\n";
            return {"true", "BLACK"};
        }
    }

    return {"false"};
}


std::vector<std::string> Game::muovi( Scacchiera& scacchiera,TipoPezzo nome,Posizione csrc,Posizione cdest,Colore giocatore,std::vector<Pezzo*>& pezziPersi) {
    std::cout<<"DEBUG: EFFETTUO MOSSA";

    Pezzo* piece = scacchiera.getPezzo(csrc);

    if (piece == nullptr ||
        !controllaNome(piece, nome) ||
        !controllaGiocatore(giocatore, piece)) {
        return {"false"};
    }

    std::vector<Posizione> possibiliDest = getPossibleDestination(scacchiera, piece, csrc, giocatore);

    // Controllo speciale: arrocco
    
    if (isArrocco(scacchiera, cdest, giocatore)) {
        if (std::find(possibiliDest.begin(), possibiliDest.end(), cdest) != possibiliDest.end()) {

            effettuaArrocco(scacchiera, cdest);
            return {"true"};
        }
    }

    // Mossa normale
    if (std::find(possibiliDest.begin(), possibiliDest.end(), cdest)
            != possibiliDest.end()) {

        RisultatoMossa risultato =
            piece->sposta(scacchiera, csrc, cdest);

        if (risultato == RisultatoMossa::Promozione)
            return {"true", "Promozione"};

        return {"true"};
    }

    return {"false"};
}


void Game::promuovi() {
    scacchiera.promuovi();
}