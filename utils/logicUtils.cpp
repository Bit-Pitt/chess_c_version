#include "../chessboard/scacchiera.h"
#include "../utils/logicUtils.h"
#include "../pieces/Pezzo.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <set>

int valorePezzo(const Pezzo* pezzo) {
    if (pezzo == nullptr || pezzo->getTipo() == TipoPezzo::KING) return 0;
    if (pezzo->getTipo() == TipoPezzo::PAWN) return 1;
    if (pezzo->getTipo() == TipoPezzo::BISHOP || pezzo->getTipo() == TipoPezzo::KNIGHT) return 3;
    if (pezzo->getTipo() == TipoPezzo::ROOK) return 5;
    if (pezzo->getTipo() == TipoPezzo::QUEEN) return 9;
    throw std::runtime_error("Tipo di pezzo non riconosciuto [fun:valorePezzo]");
}

bool controllaGiocatore(Colore giocatore, const Pezzo* pezzo) {
    return pezzo != nullptr && giocatore == pezzo->getColore();
}

Colore nomeAvversario(Colore giocatore) {
    return giocatore == Colore::WHITE ? Colore::BLACK : Colore::WHITE;
}

InfoScacchi infoScacchi(Scacchiera& scacchiera, Colore giocatore) {
    InfoScacchi res{0, {}};
    Colore avversario = nomeAvversario(giocatore);
    Posizione posRe = scacchiera.getPosRe(giocatore);

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Posizione pos{i, j};
            Pezzo* pezzo = scacchiera.getPezzo(pos);

            if (pezzo != nullptr && pezzo->getColore() == avversario) {
                std::vector<Posizione> caselle = pezzo->caseControllate(scacchiera, pos, avversario);

                if (std::find(caselle.begin(), caselle.end(), posRe) != caselle.end()) {
                    res.posizioniScacco.push_back(pos);
                    res.numeroScacchi++;
                }
            }
        }
    }

    return res;
}

std::vector<Posizione> caseInLinea(Posizione posRe, Posizione posScacco) {
    std::vector<Posizione> pos;

    if (posRe.riga == posScacco.riga) {
        int inizio = std::min(posRe.colonna, posScacco.colonna);
        int fine = std::max(posRe.colonna, posScacco.colonna);

        for (int j = inizio + 1; j < fine; j++)
            pos.push_back({posRe.riga, j});

        return pos;
    }

    if (posRe.colonna == posScacco.colonna) {
        int inizio = std::min(posRe.riga, posScacco.riga);
        int fine = std::max(posRe.riga, posScacco.riga);

        for (int i = inizio + 1; i < fine; i++)
            pos.push_back({i, posRe.colonna});

        return pos;
    }

    int dx = posRe.riga < posScacco.riga ? 1 : -1;
    int dy = posRe.colonna < posScacco.colonna ? 1 : -1;

    int i = posRe.riga + dx;
    int j = posRe.colonna + dy;

    while (i != posScacco.riga) {
        pos.push_back({i, j});
        i += dx;
        j += dy;
    }

    return pos;
}


bool mattoDaSingoloScacco(Scacchiera& scacchiera, Colore giocatore, const InfoScacchi& info) {
    Posizione posRe = scacchiera.getPosRe(giocatore);
    Pezzo* re = scacchiera.getPezzo(posRe);

    if (!re->destinations(scacchiera, posRe, giocatore).empty())
        return false;

    Posizione posScacco = info.posizioniScacco[0];
    std::vector<Posizione> controllate = caseControllateDaGiocatore(scacchiera, giocatore, false, giocatore  , true );

    if (std::find(controllate.begin(), controllate.end(), posScacco) != controllate.end())
        return false;

    Pezzo* attaccante = scacchiera.getPezzo(posScacco);

    if (attaccante->getTipo() != TipoPezzo::KNIGHT) {
        std::vector<Posizione> raggiungibili = caseRaggiungibiliDaGiocatore(scacchiera, giocatore, true);
        std::vector<Posizione> inMezzo = caseInLinea(posRe, posScacco);

        for (const Posizione& pos : inMezzo)
            if (std::find(raggiungibili.begin(), raggiungibili.end(), pos) != raggiungibili.end())
                return false;
    }

    return true;
}



bool mattoDaSingoloScacco_debug(Scacchiera& scacchiera, Colore giocatore, const InfoScacchi& info) {

    const bool debug = true;

    Posizione posRe = scacchiera.getPosRe(giocatore);
    Pezzo* re = scacchiera.getPezzo(posRe);

    if (debug) {
        std::cout << "\n========== DEBUG mattoDaSingoloScacco ==========\n";
        std::cout << "Giocatore: " << (giocatore == Colore::WHITE ? "WHITE" : "BLACK") << "\n";
        std::cout << "Posizione re: (" << posRe.riga << ", " << posRe.colonna << ")\n";
        std::cout << "Posizione scacco: (" << info.posizioniScacco[0].riga << ", " << info.posizioniScacco[0].colonna << ")\n";
    }

    // =========================================================
    // 1. Il re deve essere impossibilitato a muoversi
    // =========================================================

    std::vector<Posizione> destinazioniRe = re->destinations(scacchiera, posRe, giocatore);

    if (debug) {
        std::cout << "[DEBUG] Destinazioni possibili del re: "
                  << destinazioniRe.size() << "\n";

        for (const Posizione& pos : destinazioniRe)
            std::cout << "        (" << pos.riga << ", " << pos.colonna << ")\n";
    }

    if (!destinazioniRe.empty()) {

        if (debug)
            std::cout << "[DEBUG] Il re può muoversi -> NON è matto\n";

        return false;
    }

    if (debug)
        std::cout << "[DEBUG] Il re non ha destinazioni -> continuo controllo matto\n";


    // =========================================================
    // 2. Controlliamo se è possibile catturare l'attaccante
    // =========================================================

    Posizione posScacco = info.posizioniScacco[0];

    std::vector<Posizione> controllate = caseControllateDaGiocatore(scacchiera, giocatore, false, giocatore  , true );

    if (debug) {
        std::cout << "[DEBUG] Case controllate dal giocatore:\n";

        for (const Posizione& pos : controllate)
            std::cout << "        (" << pos.riga << ", " << pos.colonna << ")\n";

        std::cout << "[DEBUG] Controllo se la posizione dello scacco è raggiungibile: ";

        if (std::find(controllate.begin(), controllate.end(), posScacco) != controllate.end())
            std::cout << "SI\n";
        else
            std::cout << "NO\n";
    }

    if (std::find(controllate.begin(), controllate.end(), posScacco) != controllate.end()) {

        if (debug)
            std::cout << "[DEBUG] L'attaccante può essere catturato -> NON è matto\n";

        return false;
    }


    // =========================================================
    // 3. Controlliamo l'attaccante
    // =========================================================

    Pezzo* attaccante = scacchiera.getPezzo(posScacco);

    if (attaccante == nullptr) {

        if (debug)
            std::cout << "[DEBUG] ERRORE: nessun pezzo nella posizione dello scacco\n";

        return false;
    }

    if (debug) {
        std::cout << "[DEBUG] Tipo attaccante: ";

        switch (attaccante->getTipo()) {
            case TipoPezzo::PAWN:
                std::cout << "PEDONE\n";
                break;
            case TipoPezzo::ROOK:
                std::cout << "TORRE\n";
                break;
            case TipoPezzo::KNIGHT:
                std::cout << "CAVALLO\n";
                break;
            case TipoPezzo::BISHOP:
                std::cout << "ALFIERE\n";
                break;
            case TipoPezzo::QUEEN:
                std::cout << "REGINA\n";
                break;
            case TipoPezzo::KING:
                std::cout << "RE\n";
                break;
        }
    }


    // =========================================================
    // 4. Se NON è un cavallo, controlliamo se possiamo
    //    interporre un pezzo tra attaccante e re
    // =========================================================

    if (attaccante->getTipo() != TipoPezzo::KNIGHT) {

        std::vector<Posizione> raggiungibili = caseRaggiungibiliDaGiocatore(scacchiera, giocatore, true);

        std::vector<Posizione> inMezzo = caseInLinea(posRe, posScacco);

        if (debug) {

            std::cout << "[DEBUG] Case tra re e attaccante:\n";

            for (const Posizione& pos : inMezzo)
                std::cout << "        (" << pos.riga << ", " << pos.colonna << ")\n";

            std::cout << "[DEBUG] Controllo se una di queste può essere raggiunta dal giocatore...\n";
        }

        for (const Posizione& pos : inMezzo) {

            bool raggiungibile =
                std::find(raggiungibili.begin(), raggiungibili.end(), pos) != raggiungibili.end();

            if (debug) {
                std::cout << "        (" << pos.riga << ", " << pos.colonna << ") -> "
                          << (raggiungibile ? "RAGGIUNGIBILE" : "non raggiungibile")
                          << "\n";
            }

            if (raggiungibile) {

                if (debug)
                    std::cout << "[DEBUG] È possibile interporre un pezzo -> NON è matto\n";

                return false;
            }
        }
    }
    else {

        if (debug)
            std::cout << "[DEBUG] Attaccante = CAVALLO, quindi non è possibile interporre pezzi\n";
    }


    // =========================================================
    // 5. Nessuna possibilità di salvarsi
    // =========================================================

    if (debug)
        std::cout << "[DEBUG] NESSUNA POSSIBILITÀ DI SALVEZZA -> MATTO\n";

    return true;
}


int partitaFinita(Scacchiera& scacchiera, Colore giocatore) {
    InfoScacchi info = infoScacchi(scacchiera, giocatore);
    int numScacchi = info.numeroScacchi;

    if (numScacchi == 0) {
        bool stallo = true;

        for (int i = 0; i < 8 && stallo; i++)
            for (int j = 0; j < 8 && stallo; j++) {
                Posizione pos{i, j};
                Pezzo* pezzo = scacchiera.getPezzo(pos);

                if (pezzo != nullptr && pezzo->getColore() == giocatore)
                    if (!pezzo->destinations(scacchiera, pos, giocatore).empty())
                        stallo = false;
            }

        if (stallo) return 1;
    }

    if (numScacchi >= 2) {
        Posizione posRe = scacchiera.getPosRe(giocatore);
        Pezzo* re = scacchiera.getPezzo(posRe);

        if (re->destinations(scacchiera, posRe, giocatore).empty())
            return 2;
    }

    if (numScacchi == 1 && mattoDaSingoloScacco(scacchiera, giocatore, info))
        return 2;

    return 0;
}


std::vector<Posizione> getPossibleDestination(Scacchiera& scacchiera, Pezzo* pezzo, Posizione csrc, Colore giocatore) {
    InfoScacchi info = infoScacchi(scacchiera, giocatore);
    int numScacchi = info.numeroScacchi;

    if (numScacchi >= 2) {
        Posizione posRe = scacchiera.getPosRe(giocatore);
        Pezzo* re = scacchiera.getPezzo(posRe);

        if (re->destinations(scacchiera, posRe, giocatore).empty())
            throw std::runtime_error("La partita dovrebbe essere terminata");

        if (pezzo->getTipo() == TipoPezzo::KING)
            return pezzo->destinations(scacchiera, csrc, giocatore);

        return {};
    }

    std::vector<Posizione> destinations = pezzo->destinations(scacchiera, csrc, giocatore);

    if (numScacchi == 1) {
        std::vector<Posizione> filtrate;
        Posizione posScacco = info.posizioniScacco[0];

        if (std::find(destinations.begin(), destinations.end(), posScacco) != destinations.end())
            filtrate.push_back(posScacco);

        Posizione posRe = scacchiera.getPosRe(giocatore);

        if (pezzo->getTipo() != TipoPezzo::KING) {
            for (const Posizione& pos : caseInLinea(posRe, posScacco))
                if (pos != csrc) filtrate.push_back(pos);
        } else {
            filtrate = pezzo->destinations(scacchiera, csrc, giocatore);
        }

        std::vector<Posizione> risultato;

        for (const Posizione& pos : destinations)
            if (std::find(filtrate.begin(), filtrate.end(), pos) != filtrate.end())
                risultato.push_back(pos);

        return risultato;
    }

    return destinations;
}




bool pezzoNemico(Pezzo* pezzo, Colore giocatore) {
    if (pezzo == nullptr) throw std::runtime_error("Dovrebbe essere passato un pezzo");
    return pezzo->getColore() != giocatore;
}

std::vector<Posizione> movimentoVerticale(Scacchiera& scacchiera, Posizione csrc, Colore giocatore, bool avanti, bool destinazioni, bool caseControllate) {
    if (destinazioni && caseControllate) throw std::runtime_error("API chiamata incorrettamente");
    std::vector<Posizione> dest;
    int passo = avanti ? 1 : -1;
    for (int i = csrc.riga + passo; i >= 0 && i < 8; i += passo) {
        Posizione pos{i, csrc.colonna};
        if (scacchiera.casellaVuota(pos)) dest.push_back(pos);
        else {
            Pezzo* pezzo = scacchiera.getPezzo(pos);
            if (caseControllate || (destinazioni && pezzoNemico(pezzo, giocatore))) dest.push_back(pos);
            break;
        }
    }
    return dest;
}

std::vector<Posizione> movimentoOrizzontale(Scacchiera& scacchiera, Posizione csrc, Colore giocatore, bool destra, bool destinazioni, bool caseControllate) {
    if (destinazioni && caseControllate) throw std::runtime_error("API chiamata incorrettamente");
    std::vector<Posizione> dest;
    int passo = destra ? 1 : -1;
    for (int j = csrc.colonna + passo; j >= 0 && j < 8; j += passo) {
        Posizione pos{csrc.riga, j};
        if (scacchiera.casellaVuota(pos)) dest.push_back(pos);
        else {
            Pezzo* pezzo = scacchiera.getPezzo(pos);
            if (caseControllate || (destinazioni && pezzoNemico(pezzo, giocatore))) dest.push_back(pos);
            break;
        }
    }
    return dest;
}

std::vector<Posizione> movimentoRe(Scacchiera& scacchiera, Posizione csrc, Colore giocatore, bool destinazioni, bool caseControllate) {
    if (destinazioni && caseControllate) throw std::runtime_error("API chiamata incorrettamente");
    std::vector<Posizione> dest;
    for (int di = -1; di <= 1; ++di)
        for (int dj = -1; dj <= 1; ++dj)
            if (di != 0 || dj != 0) {
                Posizione pos{csrc.riga + di, csrc.colonna + dj};
                if (!scacchiera.casellaValida(pos)) continue;
                if (caseControllate || !destinazioni || scacchiera.casellaVuota(pos) || pezzoNemico(scacchiera.getPezzo(pos), giocatore))
                    dest.push_back(pos);
            }
    return dest;
}

std::pair<int,int> calcolaStep(Direzione direzione) {
    switch (direzione) {
        case Direzione::AVANTI_SX: return {1, -1};
        case Direzione::AVANTI_DX: return {1, 1};
        case Direzione::INDIETRO_SX: return {-1, -1};
        case Direzione::INDIETRO_DX: return {-1, 1};
    }
    throw std::runtime_error("Direzione non esistente");
}

std::vector<Posizione> movimentoDiagonale(Scacchiera& scacchiera, Posizione csrc, Colore giocatore, Direzione direzione, bool destinazioni, bool caseControllate) {
    if (destinazioni && caseControllate) throw std::runtime_error("API chiamata incorrettamente");
    std::vector<Posizione> dest;
    auto [di, dj] = calcolaStep(direzione);
    Posizione pos{csrc.riga + di, csrc.colonna + dj};
    while (scacchiera.casellaValida(pos)) {
        if (scacchiera.casellaVuota(pos)) dest.push_back(pos);
        else {
            Pezzo* pezzo = scacchiera.getPezzo(pos);
            if (caseControllate || (destinazioni && pezzoNemico(pezzo, giocatore))) dest.push_back(pos);
            break;
        }
        pos = {pos.riga + di, pos.colonna + dj};
    }
    return dest;
}


std::vector<Posizione> movimentoCavallo(Scacchiera& scacchiera, Posizione csrc, Colore giocatore, bool destinazioni, bool caseControllate) {
    if (destinazioni && caseControllate) throw std::runtime_error("API chiamata incorrettamente");
    std::vector<Posizione> dest;
    const int mosse[8][2] = {{2,1},{2,-1},{-1,2},{1,2},{-2,1},{-2,-1},{-1,-2},{1,-2}};
    for (auto& m : mosse) {
        Posizione pos{csrc.riga + m[0], csrc.colonna + m[1]};
        if (!scacchiera.casellaValida(pos)) continue;
        if (caseControllate || !destinazioni || scacchiera.casellaVuota(pos) || pezzoNemico(scacchiera.getPezzo(pos), giocatore))
            dest.push_back(pos);
    }
    return dest;
}


void debugPrintCaselle(const std::vector<Posizione>& positions, const std::string& testo) {
    std::cout << "[DEBUG] caselle stampate, " << testo << "\n";
    if (positions.empty()) std::cout << "0";
    for (const Posizione& pos : positions) std::cout << "(" << pos.riga << "," << pos.colonna << ") ";
    std::cout << "\n";
}


std::vector<Posizione> caseControllateDaGiocatore(Scacchiera& scacchiera, Colore giocatore, bool togliRe, Colore coloreRe, bool nonConsiderareRe) {
    Pezzo* re = nullptr;
    Posizione posRe{-1, -1};
    if (togliRe) {
        posRe = scacchiera.getPosRe(coloreRe);
        re = scacchiera.getPezzo(posRe);
        scacchiera.aggiungiPezzo(nullptr, posRe);
    }

    std::set<Posizione> caseControllate;
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j) {
            Posizione pos{i, j};
            Pezzo* pezzo = scacchiera.getPezzo(pos);
            if (pezzo != nullptr && pezzo->getColore() == giocatore) {
                if (nonConsiderareRe && pezzo->getTipo() == TipoPezzo::KING) continue;
                for (const Posizione& p : pezzo->caseControllate(scacchiera, pos, giocatore)) caseControllate.insert(p);
            }
        }

    if (togliRe) scacchiera.aggiungiPezzo(re, posRe);
    return std::vector<Posizione>(caseControllate.begin(), caseControllate.end());
}


std::vector<Posizione> caseRaggiungibiliDaGiocatore(Scacchiera& scacchiera, Colore giocatore, bool nonConsiderareRe) {
    std::set<Posizione> caseRaggiungibili;

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Posizione pos{i, j};
            Pezzo* pezzo = scacchiera.getPezzo(pos);

            if (pezzo != nullptr && pezzo->getColore() == giocatore) {
                if (nonConsiderareRe && pezzo->getTipo() == TipoPezzo::KING)
                    continue;

                for (const Posizione& p : pezzo->destinations(scacchiera, pos, giocatore))
                    caseRaggiungibili.insert(p);
            }
        }
    }

    return std::vector<Posizione>(caseRaggiungibili.begin(), caseRaggiungibili.end());
}




void effettuaArrocco(Scacchiera& scacchiera, Posizione cdest) {

    // Arrocco corto Bianco
    if (cdest == Posizione{7, 6}) {
        std::cout<<"EFFETTUATO ARROCCO CORTO";
        Pezzo* re = scacchiera.getPezzo({7, 4});
        Pezzo* torre = scacchiera.getPezzo({7, 7});

        re->sposta(scacchiera, {7, 4}, {7, 6});
        torre->sposta(scacchiera, {7, 7}, {7, 5});
    }

    // Arrocco lungo Bianco
    else if (cdest == Posizione{7, 2}) {
        Pezzo* re = scacchiera.getPezzo({7, 4});
        Pezzo* torre = scacchiera.getPezzo({7, 0});

        re->sposta(scacchiera, {7, 4}, {7, 2});
        torre->sposta(scacchiera, {7, 0}, {7, 3});
    }

    // Arrocco corto Nero
    else if (cdest == Posizione{0, 6}) {
        Pezzo* re = scacchiera.getPezzo({0, 4});
        Pezzo* torre = scacchiera.getPezzo({0, 7});

        re->sposta(scacchiera, {0, 4}, {0, 6});
        torre->sposta(scacchiera, {0, 7}, {0, 5});
    }

    // Arrocco lungo Nero
    else if (cdest == Posizione{0, 2}) {
        Pezzo* re = scacchiera.getPezzo({0, 4});
        Pezzo* torre = scacchiera.getPezzo({0, 0});

        re->sposta(scacchiera, {0, 4}, {0, 2});
        torre->sposta(scacchiera, {0, 0}, {0, 3});
    }

    else {
        throw std::runtime_error(
            "Chiamato arrocco con destinazione non valida"
        );
    }
}


bool isArrocco(Scacchiera& scacchiera, Posizione cdest, Colore giocatore) {
    Posizione posRe = scacchiera.getPosRe(giocatore);

    if (giocatore == Colore::BLACK) {
        return posRe == Posizione{7, 4} &&
               (cdest == Posizione{7, 6} || cdest == Posizione{7, 2});
    }

    return posRe == Posizione{0, 4} &&
           (cdest == Posizione{0, 6} || cdest == Posizione{0, 2});
}