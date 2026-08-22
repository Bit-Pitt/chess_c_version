#include "game.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

#include "../utils/graphicUtils.h"
#include "../utils/logicUtils.h"


// ============================================================
// MUOVI
// ============================================================

bool muovi(Scacchiera& scacchiera, TipoPezzo nome, Posizione csrc, Posizione cdest, Colore giocatore) {

    Pezzo* pezzo = scacchiera.getPezzo(csrc);

    if (pezzo == nullptr)
        return false;

    if (pezzo->getTipo() != nome)
        return false;

    if (pezzo->getColore() != giocatore)
        return false;


    // Calcolo tutte le destinazioni valide del pezzo
    std::vector<Posizione> possibiliDest =
        getPossibleDestination(scacchiera, pezzo, csrc, giocatore);


    // ========================================================
    // ARROCCO
    // ========================================================

    if (nome == TipoPezzo::KING) {

        Posizione posizioneRe = scacchiera.getPosRe(giocatore);

        // Il re deve essere effettivamente nella sua posizione
        // di partenza.
        bool posizioneIniziale = (csrc == posizioneRe);

        // Le destinazioni 2 e 6 sono quelle dell'arrocco
        // rispetto alla riga del re.
        bool possibileArrocco =
            (cdest == Posizione{csrc.riga, 6} ||
             cdest == Posizione{csrc.riga, 2});

        if (posizioneIniziale && possibileArrocco) {

            if (std::find(possibiliDest.begin(), possibiliDest.end(), cdest) != possibiliDest.end()) {

                effettuaArrocco(scacchiera, cdest);

                return true;
            }
        }
    }


    // ========================================================
    // MOSSA NORMALE
    // ========================================================

    if (std::find(possibiliDest.begin(), possibiliDest.end(), cdest) != possibiliDest.end()) {

        RisultatoMossa risultato = pezzo->sposta(scacchiera, csrc, cdest);

        // Il risultato della sposta ci serve solamente per
        // sapere se il pedone deve essere promosso.
        //
        // Nel debug non ci interessa gestire i pezzi catturati.

        return true;
    }

    return false;
}


// ============================================================
// START GAME
// ============================================================

void startGame(Scacchiera& scacchiera) {

    // ========================================================
    // FILE PARTITA
    // ========================================================

    std::ofstream file("ultimo_game.txt");

    if (file.is_open()) {
        file << "Ultimo game effettuato.\n";
        file.close();
    }


    // ========================================================
    // GIOCATORI
    // ========================================================

    Colore g1 = Colore::WHITE;
    Colore g2 = Colore::BLACK;

    Colore giocatoreDiTurno = g1;


    // ========================================================
    // CICLO DI GIOCO
    // ========================================================

    while (true) {

        std::string nomeGiocatore =
            (giocatoreDiTurno == Colore::WHITE)
            ? "White"
            : "Black";

        Colore giocatoreNonDiTurno =
            (giocatoreDiTurno == g1)
            ? g2
            : g1;

        std::cout << "\n";
        std::cout << nomeGiocatore << " turn\n";


        // ====================================================
        // CONTROLLO FINE PARTITA
        // ====================================================

        int res = partitaFinita(scacchiera, giocatoreDiTurno);

        if (res == 1) {

            std::cout << "\n\n\n";
            std::cout << "Game ended for stalemate\n";
            std::cout << "\n\n\n";

            break;
        }

        if (res == 2) {

            std::string vincitore =
                (giocatoreNonDiTurno == Colore::WHITE)
                ? "White"
                : "Black";

            std::cout << "\n\n\n";
            std::cout << "Game ended, winner: " << vincitore << "\n";
            std::cout << "\n\n\n";

            break;
        }


        // ====================================================
        // INSERIMENTO MOSSA
        // ====================================================

        bool validMove = false;
        std::string mossaStringa;

        while (!validMove) {

            std::cout << "Immetti mossa: (es pedone da A2 a A3 --> P A2 A3): ";
            std::cout << "\n- per arrocco: K [src] [dest]\n";

            std::getline(std::cin, mossaStringa);


            // ------------------------------------------------
            // Parsing
            // ------------------------------------------------

            std::stringstream ss(mossaStringa);

            std::string nomeStringa;
            std::string srcStringa;
            std::string destStringa;
            std::string extra;

            ss >> nomeStringa >> srcStringa >> destStringa;


            // Controllo che ci siano esattamente 3 elementi
            if (nomeStringa.empty() ||
                srcStringa.empty() ||
                destStringa.empty() ||
                (ss >> extra)) {

                validMove = false;

            } else {

                std::vector<std::string> input = {
                    nomeStringa,
                    srcStringa,
                    destStringa
                };


                // ------------------------------------------------
                // Controllo formato input
                // ------------------------------------------------

                if (!controllaInput(input)) {

                    validMove = false;

                } else {

                    try {

                        TipoPezzo nome = traduciNome(nomeStringa);

                        Posizione csrc = stringToPos(srcStringa);

                        Posizione cdest = stringToPos(destStringa);

                        validMove = muovi(scacchiera, nome, csrc, cdest, giocatoreDiTurno);
                    }
                    catch (const std::exception& e) {

                        std::cout << "[DEBUG] Errore: " << e.what() << "\n";

                        validMove = false;
                    }
                }
            }


            // ------------------------------------------------
            // Mossa non valida
            // ------------------------------------------------

            if (!validMove) {

                std::cout << "Mossa non valida: "
                          << mossaStringa
                          << "\n";
            }
        }


        // ====================================================
        // SALVATAGGIO MOSSA
        // ====================================================

        std::ofstream file("ultimo_game.txt", std::ios::app);

        if (file.is_open()) {

            file << " \"" << mossaStringa << "\" ,\n";

            file.close();
        }


        // ====================================================
        // CAMBIO TURNO
        // ====================================================

        giocatoreDiTurno =
            (giocatoreDiTurno == g1)
            ? g2
            : g1;


        // ====================================================
        // DEBUG
        // ====================================================

        scacchiera.print();

        std::cout << vantaggio(scacchiera) << "\n";
    }
}


// ============================================================
// ENTRY POINT
// ============================================================

void partitaNoGui(Scacchiera& scacchiera) {
    startGame(scacchiera);
}