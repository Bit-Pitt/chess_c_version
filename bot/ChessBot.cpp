#include "ChessBot.h"

#include "../utils/logicUtils.h"

#include <algorithm>
#include <random>
#include <iostream>
#include <cmath>

ChessBot::ChessBot(Scacchiera& scacchiera, double epsilon) : scacchiera(scacchiera), epsilon(epsilon) {}

/*
    1) Cicla su tutte le mosse del giocatore 
    2) valutaMossa() assegna un valore numerico
    3) ritorna le top5
*/
std::vector<MossaBot> ChessBot::getTopMoves(Colore giocatore) {
    std::vector<MossaBot> mosse;

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {

            Posizione src{i, j};
            Pezzo* pezzo = scacchiera.getPezzo(src);

            if (pezzo == nullptr || pezzo->getColore() != giocatore)
                continue;

            std::vector<Posizione> destinazioni = getPossibleDestination(scacchiera, pezzo, src, giocatore);

            for (const Posizione& dest : destinazioni) {

                //Rimuovo l'arrocco che pare dia problemi
                if (pezzo->getTipo() == TipoPezzo::KING && std::abs(dest.colonna - src.colonna) > 1)
                    continue;

                double valore = valutaMossa(src, dest, giocatore);
                mosse.push_back({src, dest, valore});
            }
        }
    }

    std::sort(mosse.begin(), mosse.end(), [](const MossaBot& a, const MossaBot& b) {
        return a.valore > b.valore;
    });

    if (mosse.size() > 5)
        mosse.resize(5);

    return mosse;
}


bool ChessBot::vaAvanti(Posizione src, Posizione dest, Colore giocatore) {
    if (giocatore == Colore::WHITE)
        return dest.riga > src.riga;

    return dest.riga < src.riga;
}

bool ChessBot::isAttaccato(Scacchiera& scacchiera, Posizione pos, Colore avversario) {
    std::vector<Posizione> controllate = caseControllateDaGiocatore(scacchiera, avversario, true);
    return std::find(controllate.begin(), controllate.end(), pos) != controllate.end();
}

/**
 *  Metodo "euristico"
 *  Valuta con score positivi 
 *  --> scacchi (in posizioni non controllate da avversario)
 *  --> catture di pezzi [se difesi valuta quale più forte dei due]
 *  --> avanzamento pedone / pezzo in un punto non controllato da avversario
 *  --> sposta pezzo attaccato in posizione sicura
 */
double ChessBot::valutaMossa(Posizione src, Posizione dest, Colore giocatore) {

    double score = 0.0;

    Colore avversario = nomeAvversario(giocatore);

    Pezzo* pezzo = scacchiera.getPezzo(src);
    Pezzo* catturato = scacchiera.getPezzo(dest);

    if (pezzo == nullptr)
        return -1000.0;


    // =========================================================
    // COPIA DELLA SCACCHIERA
    // =========================================================

    Scacchiera copia = scacchiera;

    Pezzo* pezzoCopia = copia.getPezzo(src);

    if (pezzoCopia == nullptr)
        throw std::runtime_error("[Errore nella copia: metodo{valutaMossa}]");


    pezzoCopia->sposta(copia, src, dest);


    // =========================================================
    // 1. SCACCO
    // =========================================================

    InfoScacchi infoAvversario = infoScacchi(copia, avversario);

    if (infoAvversario.numeroScacchi > 0) {

        std::vector<Posizione> controllateAvversario = caseControllateDaGiocatore(copia, avversario, true);

        bool destinazioneDifesa = std::find(controllateAvversario.begin(), controllateAvversario.end(), dest) != controllateAvversario.end();

        if (!destinazioneDifesa) {

            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dist(0, 3);

            score += dist(gen);
        }
    }


    // =========================================================
    // 2. CATTURA
    // =========================================================

    if (catturato != nullptr) {

        int valoreCatturato = valorePezzo(catturato);
        int valoreMio = valorePezzo(pezzo);

        std::vector<Posizione> controllateAvversario = caseControllateDaGiocatore(copia, avversario, true);

        bool difeso = std::find(controllateAvversario.begin(), controllateAvversario.end(), dest)!= controllateAvversario.end();

        if (!difeso) {

            score += valoreCatturato;

        } else if (valoreCatturato > valoreMio) {

            score += valoreCatturato - valoreMio;
        }
    }


    // =========================================================
    // 3. AVANZAMENTO PEDONE
    // =========================================================

    if (pezzo->getTipo() == TipoPezzo::PAWN && catturato == nullptr) {

        std::vector<Posizione> controllateAvversario = caseControllateDaGiocatore(copia, avversario, true);

        bool controllata = std::find(controllateAvversario.begin(), controllateAvversario.end(), dest) != controllateAvversario.end();

        if (!controllata)
            score += 0.7;
    }


    // =========================================================
    // 4. AVANZAMENTO DI UN ALTRO PEZZO
    // =========================================================

    if (pezzo->getTipo() != TipoPezzo::PAWN && pezzo->getTipo() != TipoPezzo::KING && catturato == nullptr) {

        std::vector<Posizione> controllateAvversario = caseControllateDaGiocatore(copia, avversario, true);

        bool controllata = std::find(controllateAvversario.begin(), controllateAvversario.end(), dest) != controllateAvversario.end();

        if (!controllata)
            score += 0.5;

        if (!controllata && vaAvanti(src, dest, giocatore)) {
            static std::random_device rd;
            static std::mt19937 gen(rd());

            std::uniform_real_distribution<double> probabilita(0.0, 1.0);

            static std::uniform_real_distribution<double> probabilitaAvanti(0.0, 1.0);

            if (probabilitaAvanti(gen) < 0.5)
                score += 0.2;
        }
    }

    // =========================================================
    // 5. Sposto pezzo attaccato 
    // =========================================================

    if (pezzo->getTipo() != TipoPezzo::PAWN && pezzo->getTipo() != TipoPezzo::KING) {

        bool attaccato = isAttaccato(scacchiera, src, avversario);
        bool destinazioneSicura = !isAttaccato(copia, dest, avversario);

        if (attaccato && destinazioneSicura)
            score += valorePezzo(pezzo)/3;
    }

    return score;
}



MossaBot ChessBot::scegliMossa(Colore giocatore) {
    std::vector<MossaBot> topMoves = getTopMoves(giocatore);

    if (topMoves.empty()){
        throw std::runtime_error("Il giocatore non ha mosse disponibili oppure la partita è terminata");
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_real_distribution<double> probabilita(0.0, 1.0);

    // 10%: mossa casuale tra tutte le mosse LEGALI
    if (probabilita(gen) < epsilon) {

        std::vector<MossaBot> tutteLeMosse;

        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {

                Posizione src{i, j};
                Pezzo* pezzo = scacchiera.getPezzo(src);

                if (pezzo == nullptr || pezzo->getColore() != giocatore)
                    continue;

                std::vector<Posizione> destinazioni = getPossibleDestination(scacchiera, pezzo, src, giocatore);

                for (const Posizione& dest : destinazioni){

                    if (pezzo->getTipo() == TipoPezzo::KING &&      //Elimino arrocco
                        std::abs(dest.colonna - src.colonna) > 1)
                        continue;

                    tutteLeMosse.push_back({src, dest, 0.0});
                }
            }
        }

        std::uniform_int_distribution<int> indice(0, static_cast<int>(tutteLeMosse.size()) - 1);

        return tutteLeMosse[indice(gen)];
    }

    double valoreMassimo = topMoves[0].valore;

    std::vector<MossaBot> migliori;

    for (const MossaBot& mossa : topMoves) {
        if (mossa.valore == valoreMassimo)
            migliori.push_back(mossa);
        else
            break;
    }

    std::uniform_int_distribution<int> indice(0, static_cast<int>(migliori.size()) - 1);

    return migliori[indice(gen)];
}