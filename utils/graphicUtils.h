#ifndef GRAPHIC_UTILS_H
#define GRAPHIC_UTILS_H

#include <string>
#include <vector>
#include <map>

#include <SFML/Graphics.hpp>

#include "../Types.h"
#include "../gameLogic/GameClass.h"
#include "../Sync.h"


// =========================================================
// FUNZIONI DI UTILITÀ
// =========================================================

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

std::string getNomeImmagine(Pezzo* pezzo);


#endif