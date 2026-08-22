#include "graphicUtils.h"

#include "../pieces/Pezzo.h"
#include "../chessboard/scacchiera.h"
#include "../gameLogic/GameClass.h"
#include "logicUtils.h"
#include "../Types.h"

#include <iostream>
#include <stdexcept>
#include <cctype>


#include <iostream>
#include <stdexcept>

ChessGUI::ChessGUI(Game& partita)
    : partita(partita),
      finestra(
          sf::VideoMode(DIMENSIONE_SCACCHIERA, DIMENSIONE_SCACCHIERA),
          "Scacchi"
      ) {

    creaImmagini();

    aggiornaGUI();
}


void ChessGUI::run() {

    while (finestra.isOpen()) {

        sf::Event evento;

        while (finestra.pollEvent(evento)) {

            if (evento.type == sf::Event::Closed) {
                finestra.close();
            }

            if (evento.type == sf::Event::MouseButtonPressed) {

                if (evento.mouseButton.button == sf::Mouse::Left) {

                    click(
                        evento.mouseButton.x,
                        evento.mouseButton.y
                    );
                }
            }
        }

        finestra.clear();

        aggiornaGUI();

        finestra.display();
    }
}


void ChessGUI::aggiornaGUI() {

    disegnaScacchiera();

    for (const Posizione& pos : mossa) {
        evidenziaCasella(pos);
    }

    disegnaPezzi();
}


void ChessGUI::disegnaScacchiera() {

    for (int r = 0; r < 8; ++r) {

        for (int c = 0; c < 8; ++c) {

            sf::RectangleShape casella(
                sf::Vector2f(
                    DIMENSIONE_CASELLA,
                    DIMENSIONE_CASELLA
                )
            );

            casella.setPosition(
                c * DIMENSIONE_CASELLA,
                r * DIMENSIONE_CASELLA
            );

            if ((r + c) % 2 == 0)
                casella.setFillColor(sf::Color(180, 180, 180));
            else
                casella.setFillColor(sf::Color::White);

            finestra.draw(casella);
        }
    }
}

void ChessGUI::evidenziaCasella(Posizione pos) {

    int rigaGrafica = 7 - pos.riga;

    sf::RectangleShape evidenziata(
        sf::Vector2f(
            DIMENSIONE_CASELLA,
            DIMENSIONE_CASELLA
        )
    );

    evidenziata.setPosition(
        pos.colonna * DIMENSIONE_CASELLA,
        rigaGrafica * DIMENSIONE_CASELLA
    );

    evidenziata.setFillColor(sf::Color::Transparent);

    evidenziata.setOutlineColor(sf::Color::Red);

    evidenziata.setOutlineThickness(4);

    finestra.draw(evidenziata);
}

Posizione ChessGUI::convertiPixelPosizione(int x, int y) {

    int colonna = x / DIMENSIONE_CASELLA;

    int rigaGrafica = y / DIMENSIONE_CASELLA;

    int riga = 7 - rigaGrafica;

    return Posizione{riga, colonna};
}


void ChessGUI::disegnaPezzi() {

    Scacchiera& scacchiera = partita.getBoard();

    for (int r = 0; r < 8; ++r) {

        for (int c = 0; c < 8; ++c) {

            Posizione pos{r, c};

            Pezzo* pezzo = scacchiera.getPezzo(pos);

            if (pezzo == nullptr)
                continue;

            std::string nome = getNomeImmagine(pezzo);

            auto it = immagini.find(nome);

            if (it == immagini.end())
                continue;

            sf::Sprite sprite;

            sprite.setTexture(it->second);

            // Dimensioni effettive dell'immagine
            sf::Vector2u dimensioni = it->second.getSize();

            // Centro dello sprite
            sprite.setOrigin(
                dimensioni.x / 2.0f,
                dimensioni.y / 2.0f
            );

            int rigaGrafica = 7 - r;

            float x = c * DIMENSIONE_CASELLA
                    + DIMENSIONE_CASELLA / 2.0f;

            float y = rigaGrafica * DIMENSIONE_CASELLA
                    + DIMENSIONE_CASELLA / 2.0f;

            sprite.setPosition(x, y);

            finestra.draw(sprite);
        }
 
    }
}

void ChessGUI::creaImmagini() {

    if (!immagini["PB"].loadFromFile("immagini_pezzi/pawn_black.png"))
        throw std::runtime_error("Impossibile caricare pawn_black.png");

    if (!immagini["PW"].loadFromFile("immagini_pezzi/pawn_white.png"))
        throw std::runtime_error("Impossibile caricare pawn_white.png");

    if (!immagini["RB"].loadFromFile("immagini_pezzi/rook_black.png"))
        throw std::runtime_error("Impossibile caricare rook_black.png");

    if (!immagini["RW"].loadFromFile("immagini_pezzi/rook_white.png"))
        throw std::runtime_error("Impossibile caricare rook_white.png");

    if (!immagini["KB"].loadFromFile("immagini_pezzi/king_black.png"))
        throw std::runtime_error("Impossibile caricare king_black.png");

    if (!immagini["KW"].loadFromFile("immagini_pezzi/king_white.png"))
        throw std::runtime_error("Impossibile caricare king_white.png");

    if (!immagini["BB"].loadFromFile("immagini_pezzi/bishop_black.png"))
        throw std::runtime_error("Impossibile caricare bishop_black.png");

    if (!immagini["BW"].loadFromFile("immagini_pezzi/bishop_white.png"))
        throw std::runtime_error("Impossibile caricare bishop_white.png");

    if (!immagini["CB"].loadFromFile("immagini_pezzi/knight_black.png"))
        throw std::runtime_error("Impossibile caricare knight_black.png");

    if (!immagini["CW"].loadFromFile("immagini_pezzi/knight_white.png"))
        throw std::runtime_error("Impossibile caricare knight_white.png");

    if (!immagini["QB"].loadFromFile("immagini_pezzi/queen_black.png"))
        throw std::runtime_error("Impossibile caricare queen_black.png");

    if (!immagini["QW"].loadFromFile("immagini_pezzi/queen_white.png"))
        throw std::runtime_error("Impossibile caricare queen_white.png");
}


void ChessGUI::click(int x, int y) {
    Posizione pos = convertiPixelPosizione(x, y);

    std::cout << "Hai cliccato: "
              << pos.riga << " "
              << pos.colonna
              << "\n";

    Scacchiera& scacchiera = partita.getBoard();

    // Primo click: selezione del pezzo
    if (mossa.empty()) {

        Pezzo* pezzo = scacchiera.getPezzo(pos);

        if (pezzo == nullptr) {
            std::cout << "Casella vuota: seleziona un pezzo.\n";
            return;
        }

        mossa.push_back(pos);
        return;
    }

    // Secondo click
    if (mossa.size() == 1) {

        // Se clicco di nuovo sulla stessa casella annullo la selezione
        if (mossa[0] == pos) {
            std::cout << "Selezione annullata.\n";
            mossa.clear();
            return;
        }

        mossa.push_back(pos);

        std::string stringaMossa =
            creaStringa(mossa, scacchiera);

        std::vector<std::string> risultato =
            partita.processaMossa(stringaMossa);

        if (!risultato.empty() &&
            risultato[0] == "true") {

            if (risultato.size() > 1 &&
                risultato[1] == "Promozione") {

                std::cout << "PROMOZIONE AUTOMATICA A REGINA\n";

                partita.promuovi();
            }
        }
        else {
            std::cout << "Mossa non valida.\n";
        }

        // In ogni caso la prossima mossa parte da zero
        mossa.clear();
    }
}




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


