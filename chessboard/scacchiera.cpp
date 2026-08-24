#include "scacchiera.h"
#include "../pieces/Regina.h"
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <string>
#include "../pieces/Pedone.h"
#include "../pieces/Torre.h"
#include "../pieces/Re.h"
#include "../pieces/Alfiere.h"
#include "../pieces/Cavallo.h"
#include "../pieces/Regina.h"

Scacchiera::Scacchiera() {
    for (auto& riga : scacchiera)
        riga.fill(nullptr);
}

Scacchiera::~Scacchiera() {
    for (auto& riga : scacchiera)
        for (Pezzo*& pezzo : riga) {
            delete pezzo;
            pezzo = nullptr;
        }
}

bool Scacchiera::casellaValida(Posizione pos) const {
    return pos.riga >= 0 && pos.riga < 8 && pos.colonna >= 0 && pos.colonna < 8;
}

void Scacchiera::aggiungiPezzo(Pezzo* pezzo, Posizione pos) {
    if (!casellaValida(pos))
        throw std::out_of_range("Posizione non valida");
    scacchiera[pos.riga][pos.colonna] = pezzo;
}

Pezzo* Scacchiera::getPezzo(Posizione pos) const {
    if (!casellaValida(pos))
        throw std::out_of_range("Posizione non valida");
    return scacchiera[pos.riga][pos.colonna];
}

bool Scacchiera::casellaVuota(Posizione pos) const {
    return getPezzo(pos) == nullptr;
}

bool Scacchiera::caselleVuote(const std::vector<Posizione>& caselle) const {
    for (Posizione pos : caselle)
        if (!casellaVuota(pos)) return false;
    return true;
}

bool Scacchiera::caselleValide(const std::vector<Posizione>& caselle) const {
    for (Posizione pos : caselle)
        if (!casellaValida(pos)) return false;
    return true;
}

Posizione Scacchiera::getPosRe(Colore giocatore) const {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            Pezzo* pezzo = scacchiera[i][j];
            if (pezzo != nullptr && pezzo->getTipo() == TipoPezzo::KING && pezzo->getColore() == giocatore)
                return {i, j};
        }

    throw std::runtime_error("Re non trovato!");
}

bool Scacchiera::controllaCasaPromo(Posizione pos) const {
    Pezzo* pezzo = getPezzo(pos);

    if (pezzo == nullptr || pezzo->getTipo() != TipoPezzo::PAWN)
        return false;

    if (pezzo->getColore() == Colore::WHITE && pos.riga == 7)
        return true;

    if (pezzo->getColore() == Colore::BLACK && pos.riga == 0)
        return true;

    return false;
}

void Scacchiera::promuovi() {
    Posizione casaPromo{-1, -1};

    for (int i = 0; i < 8; i++) {
        if (controllaCasaPromo({0, i})) {
            casaPromo = {0, i};
            break;
        }

        if (controllaCasaPromo({7, i})) {
            casaPromo = {7, i};
            break;
        }
    }

    if (!casellaValida(casaPromo))
        throw std::runtime_error("Non è stata trovata casa di promozione!");

    Pezzo* pedone = getPezzo(casaPromo);
    aggiungiPezzo(new Regina(pedone->getColore()), casaPromo);
}

void Scacchiera::print() const {
    std::cout << "\n\n";

    const int spaziPerCasella = 6;

    for (int i = 7; i >= 0; --i) {

        std::cout << "---------------------------------------------------------\n";
        std::cout << "|";

        for (int j = 0; j < 8; ++j) {

            Posizione pos{i, j};

            std::string nome = nomePezzoInCasella(getPezzo(pos));

            int spazi = spaziPerCasella - static_cast<int>(nome.length()) - 1;

            if (j != 7)
                std::cout << " " << nome << std::string(spazi, ' ') << "|";
            else
                std::cout << " " << nome << std::string(spazi, ' ') << "|\n";
        }
    }

    std::cout << "---------------------------------------------------------\n";
    std::cout << "\n\n";
}

std::string Scacchiera::nomePezzoInCasella(Pezzo* pezzo) const {
    if (pezzo == nullptr)
        return "";

    std::string nome;

    switch (pezzo->getTipo()) {
        case TipoPezzo::PAWN:
            nome = "P";
            break;

        case TipoPezzo::ROOK:
            nome = "R";
            break;

        case TipoPezzo::KNIGHT:
            nome = "C";
            break;

        case TipoPezzo::BISHOP:
            nome = "B";
            break;

        case TipoPezzo::QUEEN:
            nome = "Q";
            break;

        case TipoPezzo::KING:
            nome = "K";
            break;
    }

    if (pezzo->getColore() == Colore::WHITE)
        nome += "W";
    else
        nome += "B";

    return nome;
}

// Construttore deep copy
Scacchiera::Scacchiera(const Scacchiera& altra) {
    for (auto& riga : scacchiera)
        riga.fill(nullptr);

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {

            Pezzo* pezzo = altra.scacchiera[i][j];

            if (pezzo == nullptr)
                continue;

            Posizione pos{i, j};
            Pezzo* nuovoPezzo = nullptr;

            switch (pezzo->getTipo()) {
                case TipoPezzo::PAWN:
                    nuovoPezzo = new Pedone(pezzo->getColore());
                    break;

                case TipoPezzo::ROOK:
                    nuovoPezzo = new Torre(pezzo->getColore());
                    break;

                case TipoPezzo::KNIGHT:
                    nuovoPezzo = new Cavallo(pezzo->getColore());
                    break;

                case TipoPezzo::BISHOP:
                    nuovoPezzo = new Alfiere(pezzo->getColore());
                    break;

                case TipoPezzo::QUEEN:
                    nuovoPezzo = new Regina(pezzo->getColore());
                    break;

                case TipoPezzo::KING:
                    nuovoPezzo = new Re(pezzo->getColore());
                    break;
            }

            nuovoPezzo->setMaiMosso(pezzo->hasMaiMosso());
            aggiungiPezzo(nuovoPezzo, pos);
        }
    }
}
Scacchiera& Scacchiera::operator=(const Scacchiera& altra) {
    if (this == &altra)
        return *this;

    for (auto& riga : scacchiera) {
        for (Pezzo*& pezzo : riga) {
            delete pezzo;
            pezzo = nullptr;
        }
    }

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {

            Pezzo* pezzo = altra.scacchiera[i][j];

            if (pezzo == nullptr)
                continue;

            Posizione pos{i, j};
            Pezzo* nuovoPezzo = nullptr;

            switch (pezzo->getTipo()) {
                case TipoPezzo::PAWN:
                    nuovoPezzo = new Pedone(pezzo->getColore());
                    break;

                case TipoPezzo::ROOK:
                    nuovoPezzo = new Torre(pezzo->getColore());
                    break;

                case TipoPezzo::KNIGHT:
                    nuovoPezzo = new Cavallo(pezzo->getColore());
                    break;

                case TipoPezzo::BISHOP:
                    nuovoPezzo = new Alfiere(pezzo->getColore());
                    break;

                case TipoPezzo::QUEEN:
                    nuovoPezzo = new Regina(pezzo->getColore());
                    break;

                case TipoPezzo::KING:
                    nuovoPezzo = new Re(pezzo->getColore());
                    break;
            }

            nuovoPezzo->setMaiMosso(pezzo->hasMaiMosso());

            aggiungiPezzo(nuovoPezzo, pos);
        }
    }

    return *this;
}