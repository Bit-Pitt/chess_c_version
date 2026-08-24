#include "GameClass.h"
#include "../utils/graphicUtils.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>




Game::Game(Scacchiera& scacchiera, ConfigurazioneGiocatori configurazione) : scacchiera(scacchiera), tipoBianco(configurazione.bianco), tipoNero(configurazione.nero) {
    if (tipoBianco == TipoGiocatore::BOT)
        botBianco = std::make_unique<ChessBot>(this->scacchiera, 0.05);

    if (tipoNero == TipoGiocatore::BOT) {
        double epsilon = (tipoBianco == TipoGiocatore::BOT) ? 1.0 : 0.05;
        botNero = std::make_unique<ChessBot>(this->scacchiera, epsilon);
    }

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




std::vector<std::string> Game::muovi( Scacchiera& scacchiera,TipoPezzo nome,Posizione csrc,Posizione cdest,Colore giocatore,std::vector<Pezzo*>& pezziPersi) {

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

Colore Game::getTurno(){
    return turno;
}

MossaBot Game::ottieniMossa(Colore giocatore) {
    if (!isBot(giocatore))
        throw std::runtime_error("Il giocatore non è un bot");

    if (giocatore == Colore::WHITE)
        return botBianco->scegliMossa(giocatore);

    return botNero->scegliMossa(giocatore);
}



void Game::eseguiMossaBot(const MossaBot& mossa) {
    std::vector<Posizione> movimento = {mossa.src, mossa.dest};
    std::string stringaMossa = creaStringa(movimento, scacchiera);

    std::cout << "[BOT] Mossa scelta: " << stringaMossa << "\n";

    std::vector<std::string> risultato = eseguiMossa(stringaMossa);

    if (risultato.empty() || risultato[0] != "true"){
        throw std::runtime_error("Il bot ha generato una mossa non valida");
    }
}

bool Game::isBot(Colore giocatore) const {
    if (giocatore == Colore::WHITE)
        return tipoBianco == TipoGiocatore::BOT;

    return tipoNero == TipoGiocatore::BOT;
}


std::vector<std::string> Game::checkFinePartita() {
    int res = partitaFinita(scacchiera, turno);

    if (res == 1) {
        statoPartita = StatoPartita::PATTA;
        return {"true", "PATTA"};
    }

    if (res == 2) {
        Colore vincitore = (turno == g1) ? g2 : g1;

        if (vincitore == Colore::WHITE)
            statoPartita = StatoPartita::VINCE_BIANCO;
        else
            statoPartita = StatoPartita::VINCE_NERO;

        return { "true", vincitore == Colore::WHITE ? "WHITE" : "BLACK" };
    }

    return {"false"};
}


std::vector<std::string> Game::eseguiMossa(const std::string& mossaStringa) {
    std::vector<std::string> risultato = processaMossa(mossaStringa);

    if (risultato.empty() || risultato[0] != "true")
        return risultato;

    if (risultato.size() > 1 && risultato[1] == "Promozione")
        promuovi();
    
    return risultato;
}

bool Game::isPartitaFinita() const {
    return statoPartita != StatoPartita::IN_CORSO;
}

std::string Game::getVincitore() const {
    if (statoPartita == StatoPartita::VINCE_BIANCO)
        return "WHITE";

    if (statoPartita == StatoPartita::VINCE_NERO)
        return "BLACK";

    if (statoPartita == StatoPartita::PATTA)
        return "PATTA";

    return "";
}



/**
 *  Comportamento diverso in base a giocatore UMANO o BOT
 */
void Game::run(SyncContext& sync) {
    while (sync.running) {

        if (isBot(turno)) {

            RichiestaBot richiesta{
                Scacchiera(scacchiera),
                turno,
                getEpsilonBot(turno)
            };

            {
                std::lock_guard<std::mutex> lock(sync.botInputMutex);
                sync.richiestaBot = std::move(richiesta);
            }

            sem_post(&sync.botInputReady);

            sem_wait(&sync.botMoveReady);

            if (!sync.running)
                break;

            MossaBot mossa;

            {
                std::lock_guard<std::mutex> lock(sync.botOutputMutex);
                mossa = sync.rispostaBot;
            }

            eseguiMossaBot(mossa);

        } else {

            sem_wait(&sync.inputReady);

            if (!sync.running)
                break;

            ComandoMossa comando;

            {
                std::lock_guard<std::mutex> lock(sync.inputMutex);
                comando = sync.inputQueue.front();
                sync.inputQueue.pop();
            }

            std::vector<Posizione> movimento = {comando.src, comando.dest};
            std::string stringaMossa = creaStringa(movimento, scacchiera);

            eseguiMossa(stringaMossa);
        }

        checkFinePartita();
        
        //Aggiorni scacchiera+stato partita e notifich tramite "guiUpdateDisponibile" che la scacchiera è aggiornata
        EventoGUI evento{
            Scacchiera(scacchiera),
            true,
            statoPartita,
            getVincitore(),
            turno
        };
        

        {
            std::lock_guard<std::mutex> lock(sync.guiMutex);
            sync.ultimoEventoGUI = std::move(evento);
            sync.guiUpdateDisponibile = true;
        }
    }
}


double Game::getEpsilonBot(Colore giocatore) const {
    if (tipoBianco == TipoGiocatore::BOT &&
        tipoNero == TipoGiocatore::BOT) {

        if (giocatore == Colore::WHITE)
            return 0.1;

        return 1.0;
    }

    return 0.1;
}