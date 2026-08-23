

#include <string>
#include <vector>
#include <map>

#include <SFML/Graphics.hpp>

#include "../Types.h"
#include "../gameLogic/GameClass.h"
#include "../Sync.h"

class ChessGUI {

private:

    static constexpr int DIMENSIONE_CASELLA = 100;
    static constexpr int DIMENSIONE_SCACCHIERA = 800;

    Game& partita;
    SyncContext& sync;

    sf::RenderWindow finestra;

    std::vector<Posizione> mossa;
    Scacchiera scacchieraVisualizzata;      //La gui ha solo uno snapshot 

    std::map<std::string, sf::Texture> immagini;

    bool partitaFinita;
    std::string vincitore;

public:

    ChessGUI(Game& partita, SyncContext& sync);

    void run();

private:

    void aggiornaGUI();
    void disegnaScacchiera();
    void disegnaPezzi();
    void disegnaFinePartita();
    void evidenziaCasella(Posizione pos);
    Posizione convertiPixelPosizione(int x, int y);
    void click(int x, int y);
    void mostraFinePartita(const std::string& vincitore);
    void creaImmagini();
};
