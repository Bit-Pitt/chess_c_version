#ifndef GRAPHIC_UTILS_H
#define GRAPHIC_UTILS_H

#include <string>
#include <vector>

#include "../Types.h"

class Pezzo;
class Scacchiera;

char getIniziale(Pezzo* pezzo);

std::string creaStringa(const std::vector<Posizione>& mossa, Scacchiera& scacchiera);

std::string vantaggio(Scacchiera& scacchiera);

Posizione stringToPos(const std::string& string);

std::string posToString(const Posizione& pos);

TipoPezzo traduciNome(const std::string& iniziale);

void printPezziPersi(const std::vector<Pezzo*>& pezziPersi, Colore colore);

bool controllaNome(Pezzo* pezzo, TipoPezzo tipo);

bool controllaInput(const std::vector<std::string>& mossa);

#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>


class ChessGUI {
private:

    static constexpr int DIMENSIONE_CASELLA = 100;
    static constexpr int DIMENSIONE_SCACCHIERA = 800;

    Game& partita;

    sf::RenderWindow finestra;

    std::vector<Posizione> mossa;

    std::map<std::string, sf::Texture> immagini;

public:

    ChessGUI(Game& partita);

    void run();

private:

    void aggiornaGUI();

    void disegnaScacchiera();

    void disegnaPezzi();

    void evidenziaCasella(Posizione pos);

    Posizione convertiPixelPosizione(int x, int y);

    void click(int x, int y);

    void mostraFinePartita(const std::string& vincitore);

    void creaImmagini();
};


#endif
