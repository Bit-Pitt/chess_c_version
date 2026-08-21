#pragma once

#include <vector>
#include "../Types.h"

class Pezzo;
class Scacchiera;

int valorePezzo(const Pezzo* pezzo);
bool controllaGiocatore(Colore giocatore, const Pezzo* pezzo);
Colore nomeAvversario(Colore giocatore);

struct InfoScacchi {
    int numeroScacchi;
    std::vector<Posizione> posizioniScacco;
};

InfoScacchi infoScacchi(const Scacchiera& scacchiera, Colore giocatore);
std::vector<Posizione> caseInLinea(Posizione posRe, Posizione posScacco);


bool mattoDaSingoloScacco(const Scacchiera& scacchiera, Colore giocatore, const InfoScacchi& info);
int partitaFinita(const Scacchiera& scacchiera, Colore giocatore);
std::vector<Posizione> getPossibleDestination(const Scacchiera& scacchiera, Pezzo* pezzo, Posizione csrc, Colore giocatore);


bool pezzoNemico(Pezzo* pezzo, Colore giocatore);

std::vector<Posizione> movimentoVerticale(Scacchiera& scacchiera, Posizione csrc, Colore giocatore, bool avanti, bool destinazioni = false, bool caseControllate = false);
std::vector<Posizione> movimentoOrizzontale(Scacchiera& scacchiera, Posizione csrc, Colore giocatore, bool destra, bool destinazioni = false, bool caseControllate = false);
std::vector<Posizione> movimentoRe(Scacchiera& scacchiera, Posizione csrc, Colore giocatore, bool destinazioni = false, bool caseControllate = false);std::pair<int,int> calcolaStep(Direzione direzione);
std::vector<Posizione> movimentoDiagonale(Scacchiera& scacchiera, Posizione csrc, Colore giocatore, Direzione direzione, bool destinazioni = false, bool caseControllate = false);
std::vector<Posizione> movimentoCavallo(Scacchiera& scacchiera, Posizione csrc, Colore giocatore, bool destinazioni = false, bool caseControllate = false);
void debugPrintCaselle(const std::vector<Posizione>& positions, const std::string& testo = "");
std::vector<Posizione> caseControllateDaGiocatore(Scacchiera& scacchiera, Colore giocatore, bool togliRe = false, Colore coloreRe = Colore::WHITE, bool nonConsiderareRe = false);
std::vector<Posizione> caseRaggiungibiliDaGiocatore(Scacchiera& scacchiera, Colore giocatore, bool nonConsiderareRe = true);
void effettuaArrocco(Scacchiera& scacchiera, Posizione cdest);