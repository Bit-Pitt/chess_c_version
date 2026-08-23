
#pragma once

enum class Colore {
    WHITE,
    BLACK
};

enum class TipoPezzo {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

struct Posizione {
    int riga;
    int colonna;

    bool operator==(const Posizione& altra) const {
        return riga == altra.riga && colonna == altra.colonna;
    }

    bool operator<(const Posizione& other) const {
    return riga != other.riga ? riga < other.riga : colonna < other.colonna;
    }
    
    bool operator!=(const Posizione& altra) const {
        return !(*this == altra);
    }
};

enum class TipoMossa {
    NORMALE,
    ARROCCO
};

struct Mossa {
    Posizione posizione;
    TipoMossa tipo;
};




enum class Direzione { AVANTI_SX, AVANTI_DX, INDIETRO_SX, INDIETRO_DX };

enum class RisultatoMossa {
    NonPromozione,
    Promozione
};


enum class TipoGiocatore {
    UMANO,
    BOT
};


struct ConfigurazioneGiocatori {
    TipoGiocatore bianco;
    TipoGiocatore nero;
};


enum class StatoPartita {
    IN_CORSO,
    PATTA,
    VINCE_BIANCO,
    VINCE_NERO
};