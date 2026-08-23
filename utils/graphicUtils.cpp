#include "graphicUtils.h"

#include "../pieces/Pezzo.h"
#include "../chessboard/scacchiera.h"
#include "../gameLogic/GameClass.h"
#include "logicUtils.h"
#include "../Types.h"

#include <iostream>
#include <stdexcept>
#include <cctype>
#include <thread>
#include <chrono>

#include <iostream>
#include <stdexcept>

char getIniziale(Pezzo* pezzo) {
    if (pezzo == nullptr)
        return 'e';

    switch (pezzo->getTipo()) {
        case TipoPezzo::PAWN:
            return 'P';

        case TipoPezzo::ROOK:
            return 'R';

        case TipoPezzo::KNIGHT:
            return 'C';

        case TipoPezzo::BISHOP:
            return 'B';

        case TipoPezzo::QUEEN:
            return 'Q';

        case TipoPezzo::KING:
            return 'K';

        default:
            return 'e';
    }
}

std::string getNomeImmagine(Pezzo* pezzo) {
    if (pezzo == nullptr)
        return "";

    char iniziale;

    switch (pezzo->getTipo()) {
        case TipoPezzo::PAWN:
            iniziale = 'P';
            break;

        case TipoPezzo::ROOK:
            iniziale = 'R';
            break;

        case TipoPezzo::KNIGHT:
            iniziale = 'C';
            break;

        case TipoPezzo::BISHOP:
            iniziale = 'B';
            break;

        case TipoPezzo::QUEEN:
            iniziale = 'Q';
            break;

        case TipoPezzo::KING:
            iniziale = 'K';
            break;

        default:
            return "";
    }

    char colore = (pezzo->getColore() == Colore::WHITE) ? 'W' : 'B';

    std::string nome;
    nome += iniziale;
    nome += colore;

    return nome;
}

std::string creaStringa(const std::vector<Posizione>& mossa, Scacchiera& scacchiera) {
    if (mossa.size() < 2)
        throw std::invalid_argument("Mossa non valida");

    Posizione csrc = mossa[0];
    Posizione cdest = mossa[1];

    std::string stringCsrc = posToString(csrc);
    std::string stringDest = posToString(cdest);

    Pezzo* pezzo = scacchiera.getPezzo(csrc);
    char iniziale = getIniziale(pezzo);

    std::string stringa;
    stringa += iniziale;
    stringa += " ";
    stringa += stringCsrc;
    stringa += " ";
    stringa += stringDest;

    std::cout << "[DEBUG] " << stringa << std::endl;

    return stringa;
}

std::string vantaggio(Scacchiera& scacchiera) {
    int somma = 0;

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {

            Posizione pos{i, j};
            Pezzo* pezzo = scacchiera.getPezzo(pos);

            if (pezzo == nullptr)
                continue;

            int valore = valorePezzo(pezzo);

            if (pezzo->getColore() == Colore::WHITE)
                somma += valore;
            else
                somma -= valore;
        }
    }

    if (somma == 0)
        return "Giocatori in parità";

    if (somma > 0)
        return "White  +" + std::to_string(somma);

    return "Black  +" + std::to_string(-somma);
}


Posizione stringToPos(const std::string& string) {
    if (string.length() != 2)
        throw std::invalid_argument("Stringa da convertire non valida");

    char colonna = std::toupper(string[0]);

    if (colonna < 'A' || colonna > 'H')
        throw std::invalid_argument("Colonna non valida");

    if (!std::isdigit(string[1]))
        throw std::invalid_argument("Riga non valida");

    int col = colonna - 'A';
    int row = string[1] - '1';

    if (row < 0 || row > 7)
        throw std::invalid_argument("Riga non valida");

    return Posizione{row, col};
}


std::string posToString(const Posizione& pos) {
    if (pos.riga < 0 || pos.riga > 7 ||
        pos.colonna < 0 || pos.colonna > 7) {

        throw std::invalid_argument("Indici posizione non validi");
    }

    char lettera = 'A' + pos.colonna;
    char numero = '1' + pos.riga;

    std::string risultato;
    risultato += lettera;
    risultato += numero;

    return risultato;
}

TipoPezzo traduciNome(const std::string& iniziale) {
    if (iniziale.length() != 1) {
        throw std::invalid_argument(
            "Iniziale del pezzo non valida"
        );
    }

    char c = std::toupper(iniziale[0]);

    switch (c) {

        case 'P':
            return TipoPezzo::PAWN;

        case 'R':
            return TipoPezzo::ROOK;

        case 'K':
            return TipoPezzo::KING;

        case 'Q':
            return TipoPezzo::QUEEN;

        case 'B':
            return TipoPezzo::BISHOP;

        case 'C':
            return TipoPezzo::KNIGHT;

        default:
            throw std::invalid_argument(
                "Iniziale del pezzo non riconosciuta"
            );
    }
}

void printPezziPersi(const std::vector<Pezzo*>& pezziPersi, Colore colore) {
    // TODO
    // Esempio:
    // 3 Pawn, 1 Queen

    std::cout << "Pezzi persi: ";

    int pedoni = 0;
    int torri = 0;
    int cavalli = 0;
    int alfieri = 0;
    int regine = 0;
    int re = 0;

    for (Pezzo* pezzo : pezziPersi) {

        if (pezzo == nullptr)
            continue;

        switch (pezzo->getTipo()) {

            case TipoPezzo::PAWN:
                pedoni++;
                break;

            case TipoPezzo::ROOK:
                torri++;
                break;

            case TipoPezzo::KNIGHT:
                cavalli++;
                break;

            case TipoPezzo::BISHOP:
                alfieri++;
                break;

            case TipoPezzo::QUEEN:
                regine++;
                break;

            case TipoPezzo::KING:
                re++;
                break;
        }
    }

    bool primo = true;

    if (pedoni > 0) {
        std::cout << pedoni << " Pawn";
        primo = false;
    }

    if (torri > 0) {
        if (!primo) std::cout << ", ";
        std::cout << torri << " Rook";
        primo = false;
    }

    if (cavalli > 0) {
        if (!primo) std::cout << ", ";
        std::cout << cavalli << " Knight";
        primo = false;
    }

    if (alfieri > 0) {
        if (!primo) std::cout << ", ";
        std::cout << alfieri << " Bishop";
        primo = false;
    }

    if (regine > 0) {
        if (!primo) std::cout << ", ";
        std::cout << regine << " Queen";
        primo = false;
    }

    if (re > 0) {
        if (!primo) std::cout << ", ";
        std::cout << re << " King";
    }

    if (primo)
        std::cout << "nessuno";

    std::cout << std::endl;
}


bool controllaNome(Pezzo* pezzo, TipoPezzo tipo) {
    if (pezzo == nullptr)
        return false;

    return pezzo->getTipo() == tipo;
}


bool controllaInput(const std::vector<std::string>& mossa) {
    if (mossa.size() != 3)
        return false;

    const std::string& csrc = mossa[1];
    const std::string& cdest = mossa[2];

    if (csrc.length() != 2 || cdest.length() != 2)
        return false;

    char colSrc = std::tolower(csrc[0]);
    char colDest = std::tolower(cdest[0]);

    if (colSrc < 'a' || colSrc > 'h')
        return false;

    if (colDest < 'a' || colDest > 'h')
        return false;

    if (!std::isdigit(csrc[1]) || !std::isdigit(cdest[1]))
        return false;

    int rigaSrc = csrc[1] - '0';
    int rigaDest = cdest[1] - '0';

    if (rigaSrc < 1 || rigaSrc > 8)
        return false;

    if (rigaDest < 1 || rigaDest > 8)
        return false;

    const std::string inizialiPezzi = "PKCQBR";

    char iniziale = std::toupper(mossa[0][0]);

    if (inizialiPezzi.find(iniziale) == std::string::npos)
        return false;

    return true;
}


