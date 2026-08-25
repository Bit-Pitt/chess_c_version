#include "../utils/graphicUtils.h"
#include "chessGUI.h"
#include "../pieces/Pezzo.h"
#include "../chessboard/scacchiera.h"
#include "../gameLogic/GameClass.h"
#include "../utils/logicUtils.h"
#include "../Types.h"

#include <iostream>
#include <stdexcept>
#include <cctype>
#include <thread>
#include <chrono>

#include <iostream>
#include <stdexcept>

void ChessGUI::aggiornaGUI() {

    disegnaScacchiera();

    for (const Posizione& pos : mossa) {
        evidenziaCasella(pos);
    }

    disegnaPezzi();

    if (partitaFinita) {
        disegnaFinePartita();
    }
}


ChessGUI::ChessGUI(Game& partita, SyncContext& sync) : partita(partita), sync(sync), scacchieraVisualizzata(partita.getBoard()), partitaFinita(false), vincitore(""),
      finestra(sf::VideoMode(DIMENSIONE_SCACCHIERA, DIMENSIONE_SCACCHIERA), "Scacchi") {

    creaImmagini();
    aggiornaGUI();
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

    Scacchiera& scacchiera = scacchieraVisualizzata;

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

            sf::Vector2u dimensioni = it->second.getSize();

            sprite.setOrigin(
                dimensioni.x / 2.0f,
                dimensioni.y / 2.0f
            );

            int rigaGrafica = 7 - r;

            float x = c * DIMENSIONE_CASELLA +
                      DIMENSIONE_CASELLA / 2.0f;

            float y = rigaGrafica * DIMENSIONE_CASELLA +
                      DIMENSIONE_CASELLA / 2.0f;

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


void ChessGUI::run() {
    while (finestra.isOpen() && sync.running) {

        sf::Event evento;

        while (finestra.pollEvent(evento)) {

            if (evento.type == sf::Event::Closed) {
                sync.running = false;
                sem_post(&sync.inputReady);
                sem_post(&sync.botInputReady);
                sem_post(&sync.botMoveReady);
                finestra.close();
            }

            if (evento.type == sf::Event::MouseButtonPressed &&
                evento.mouseButton.button == sf::Mouse::Left &&
                !partitaFinita) {

                click(evento.mouseButton.x, evento.mouseButton.y);
            }
        }

        // Controllo se Game ha prodotto un nuovo stato
        {
            std::lock_guard<std::mutex> lock(sync.guiMutex);

            if (sync.guiUpdateDisponibile) {

                scacchieraVisualizzata = sync.ultimoEventoGUI.scacchiera;

                if (sync.ultimoEventoGUI.statoPartita != StatoPartita::IN_CORSO) {
                    partitaFinita = true;
                    vincitore = sync.ultimoEventoGUI.vincitore;
                }

                sync.guiUpdateDisponibile = false;
            }
        }

        finestra.clear();
        aggiornaGUI();
        finestra.display();
    }
}



/**
 * Quando è formula la mossa --> sveglio game thread
 */
void ChessGUI::click(int x, int y) {
    Posizione pos = convertiPixelPosizione(x, y);

    if (mossa.empty()) {

        Pezzo* pezzo = scacchieraVisualizzata.getPezzo(pos);

        if (pezzo == nullptr) {
            std::cout << "Casella vuota: seleziona un pezzo.\n";
            return;
        }

        mossa.push_back(pos);
        aggiornaGUI();
        return;
    }

    if (mossa[0] == pos) {
        mossa.clear();
        aggiornaGUI();
        return;
    }

    mossa.push_back(pos);

    ComandoMossa comando{mossa[0], mossa[1]};

    {
        std::lock_guard<std::mutex> lock(sync.inputMutex);
        sync.inputQueue.push(comando);
    }

    sem_post(&sync.inputReady);

    mossa.clear();
}




    

void ChessGUI::mostraFinePartita(const std::string& vincitore) {

    partitaFinita = true;
    this->vincitore = vincitore;
    std::cout << "Vincitore: " << vincitore << "\n";
}


void ChessGUI::disegnaFinePartita() {

    sf::RectangleShape overlay(
        sf::Vector2f(
            8 * DIMENSIONE_CASELLA,
            8 * DIMENSIONE_CASELLA
        )
    );

    overlay.setPosition(0, 0);
    overlay.setFillColor(sf::Color(0, 0, 0, 180));

    finestra.draw(overlay);

    std::string testo;

    if (vincitore == "WHITE") {
        testo = "Vincitore: Player White";
    }
    else if (vincitore == "BLACK") {
        testo = "Vincitore: Player Black";
    }
    else {
        testo = "Partita finita in patta";
    }

    sf::Font font;

    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Errore: font non trovato\n";
        return;
    }

    sf::Text testoGrafico;

    testoGrafico.setFont(font);
    testoGrafico.setString(testo);
    testoGrafico.setCharacterSize(40);
    testoGrafico.setFillColor(sf::Color::White);

    sf::FloatRect bounds = testoGrafico.getLocalBounds();

    testoGrafico.setOrigin(
        bounds.left + bounds.width / 2.0f,
        bounds.top + bounds.height / 2.0f
    );

    testoGrafico.setPosition(
        4 * DIMENSIONE_CASELLA,
        4 * DIMENSIONE_CASELLA
    );

    finestra.draw(testoGrafico);

}