#pragma once

#include "../chessboard/scacchiera.h"
#include "../Types.h"
#include "../utils/logicUtils.h"
#include "../bot/ChessBot.h"
#include <vector>
#include <string>
#include <memory>
#include "../Sync.h"


class Game {
private:
    Scacchiera& scacchiera;

    Colore g1 = Colore::WHITE;
    Colore g2 = Colore::BLACK;
    Colore turno = Colore::WHITE;
    StatoPartita statoPartita = StatoPartita::IN_CORSO;

    std::vector<Pezzo*> pezziPersiW;
    std::vector<Pezzo*> pezziPersiB;

    TipoGiocatore tipoBianco;
    TipoGiocatore tipoNero;

    std::unique_ptr<ChessBot> botBianco;
    std::unique_ptr<ChessBot> botNero;

    void creaFilePartita();

public:
    Game(Scacchiera& scacchiera, ConfigurazioneGiocatori configurazione);

    Colore getTurno();

    Scacchiera& getBoard();

    // Restituisce:
    // {false}
    // {true}
    // {true, "Promozione"}
    std::vector<std::string> processaMossa(const std::string& mossaStringa);

    // Restituisce:
    // {false}
    // {true, "PATTA"}
    // {true, "WHITE"/"BLACK"}
    std::vector<std::string> checkFinePartita();

    // Effettua la mossa se valida
    std::vector<std::string> muovi(
        Scacchiera& scacchiera,
        TipoPezzo nome,
        Posizione csrc,
        Posizione cdest,
        Colore giocatore,
        std::vector<Pezzo*>& pezziPersi
    );

    bool isBot(Colore giocatore) const;
    void eseguiMossaBot(const MossaBot& mossa);

    std::vector<std::string> eseguiMossa(const std::string& mossaStringa);
    bool isPartitaFinita() const;
    std::string getVincitore() const;
 
    void run(SyncContext& sync);

    double getEpsilonBot(Colore giocatore) const;
    
    void promuovi();
};