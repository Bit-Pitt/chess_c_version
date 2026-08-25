# Chess C++

Progetto di un software scacchi sviluppato in **C++**, con interfaccia grafica tramite **SFML** e diverse modalità di gioco.

## Funzionalità principali

* Implementazione completa delle regole degli scacchi:
* Interfaccia grafica interattiva con SFML.
* Modalità:

  * **2 giocatori**
  * **1 giocatore vs bot**
  * **bot vs bot**, con un bot più forte e uno che gioca in modocasuale (`epsilon = 1.0`)
* Bot basato su una euristica "semplice" per valutare la bontà di una mossa (scacchi, catture, avanzamenti e difesa dei pezzi...)
* L'architettura concorrente spiegata di seguito

## Programmazione concorrente

La concorrenza è organizzata su più livelli.

### Thread principali

* **GUI thread**: gestisce eventi e rendering della finestra.
* **Game thread**: mantiene lo stato della partita ed esegue le mosse.
* **Bot Coordinator**: coordina il calcolo della mossa del bot.
* **Monitor thread**: osserva gli aggiornamenti della partita.

### Thread pool del bot

Il Bot Coordinator genera un task per ogni possibile mossa e li inserisce in una **task queue** condivisa.

Un insieme di **worker thread** preleva i task e valuta le mosse in parallelo. I risultati vengono inseriti in una **result queue**, dalla quale il Bot Coordinator li raccoglie e seleziona la mossa finale secondo la propria euristica ed epsilon-greedy policy.

La sincronizzazione utilizza principalmente:

* `sem_t` per notificare la presenza di nuovi task o eventi;
* `std::mutex` per proteggere le code condivise;
* code `std::queue` secondo il paradigma **producer-consumer**.

La scacchiera reale rimane gestita dal **Game thread**; i worker operano su copie indipendenti della scacchiera, evitando accessi concorrenti allo stato principale della partita.

## Compilazione

Il progetto utilizza `g++`, `Makefile`, C++17 e SFML.

```bash
make
./scacchi
```
