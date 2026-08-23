
L’eventuale esempio di codice:
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_TASK 64

// ── Definizione Task ──────────────────────────────────────────
typedef struct {
    void (*funzione)(void* arg);   // puntatore a funzione
    void* arg;                     // argomento generico
} Task;

// ── Thread Pool ───────────────────────────────────────────────
typedef struct {
    pthread_t*      workers;
    Task            coda[MAX_TASK];
    int             testa, coda_idx, count;

    pthread_mutex_t mtx_coda;       // protegge la coda
    sem_t           sem_task;       // conta task disponibili
    sem_t           sem_completati; // conta task finiti (barrier)

    int             n_threads;
    int             stop;
} ThreadPool;

// ── Funzione eseguita da ogni worker ──────────────────────────
void* worker_loop(void* arg) {
    ThreadPool* pool = (ThreadPool*)arg;

    while (1) {
        // Dorme finché non c'è un task disponibile
        sem_wait(&pool->sem_task);

        // Controlla se deve fermarsi
        if (pool->stop) break;

        // Preleva task dalla coda (sezione critica)
        pthread_mutex_lock(&pool->mtx_coda);
            Task t = pool->coda[pool->testa];
            pool->testa = (pool->testa + 1) % MAX_TASK;
            pool->count--;
        pthread_mutex_unlock(&pool->mtx_coda);

        // Esegue il task (fuori dalla sezione critica!)
        t.funzione(t.arg);

        // Segnala al main che ha finito
        sem_post(&pool->sem_completati);
    }
    return NULL;
}

// ── Inizializzazione ──────────────────────────────────────────
void pool_init(ThreadPool* pool, int n_threads) {
    pool->n_threads = n_threads;
    pool->testa = pool->coda_idx = pool->count = 0;
    pool->stop  = 0;

    pthread_mutex_init(&pool->mtx_coda, NULL);
    sem_init(&pool->sem_task,       0, 0);
    sem_init(&pool->sem_completati, 0, 0);

    pool->workers = malloc(n_threads * sizeof(pthread_t));
    for (int i = 0; i < n_threads; i++)
        pthread_create(&pool->workers[i], NULL, worker_loop, pool);
}

// ── Aggiunta task ─────────────────────────────────────────────
void pool_submit(ThreadPool* pool, void (*fn)(void*), void* arg) {
    pthread_mutex_lock(&pool->mtx_coda);
        pool->coda[pool->coda_idx].funzione = fn;
        pool->coda[pool->coda_idx].arg      = arg;
        pool->coda_idx = (pool->coda_idx + 1) % MAX_TASK;
        pool->count++;
    pthread_mutex_unlock(&pool->mtx_coda);

    sem_post(&pool->sem_task);   // sveglia un worker
}

// ── Aspetta N task completati (barrier) ───────────────────────
void pool_wait(ThreadPool* pool, int n_task) {
    for (int i = 0; i < n_task; i++)
        sem_wait(&pool->sem_completati);
}

// ── Distruzione ───────────────────────────────────────────────
void pool_destroy(ThreadPool* pool) {
    pool->stop = 1;
    // Sblocca tutti i worker dormienti
    for (int i = 0; i < pool->n_threads; i++)
        sem_post(&pool->sem_task);
    for (int i = 0; i < pool->n_threads; i++)
        pthread_join(pool->workers[i], NULL);

    pthread_mutex_destroy(&pool->mtx_coda);
    sem_destroy(&pool->sem_task);
    sem_destroy(&pool->sem_completati);
    free(pool->workers);
}


USO:
typedef struct {
    Scacchiera* scacchiera;
    int         pos_riga;
    int         pos_col;
    Mossa*      risultato;    // output: array mosse calcolate
    int*        n_mosse;      // output: quante mosse
} ArgCalcoloMosse;

void calcola_mosse_task(void* arg) {
    ArgCalcoloMosse* a = (ArgCalcoloMosse*)arg;
    Pezzo* p = get_pezzo(a->scacchiera, a->pos_riga, a->pos_col);

    // solo letture sulla scacchiera → nessuna race condition
    *a->n_mosse = p->case_controllate(
        a->scacchiera,
        a->pos_riga, a->pos_col,
        a->risultato
    );
}

void turno_bianco(Game* game, ThreadPool* pool) {
    // 1. Raccogli posizioni pezzi bianchi
    Posizione pezzi_bianchi[16];
    int n = get_pezzi_bianchi(game->scacchiera, pezzi_bianchi);

    // 2. Prepara argomenti e sottometti task
    ArgCalcoloMosse args[16];
    for (int i = 0; i < n; i++) {
        args[i].scacchiera = game->scacchiera;
        args[i].pos_riga   = pezzi_bianchi[i].riga;
        args[i].pos_col    = pezzi_bianchi[i].col;
        args[i].risultato  = game->mosse_buffer[i];
        args[i].n_mosse    = &game->n_mosse[i];
        pool_submit(pool, calcola_mosse_task, &args[i]);
    }

    // 3. Main si blocca qui finché tutti gli N task non finiscono
    pool_wait(pool, n);

    // 4. Ora game->mosse_buffer è completo, gestisci il turno
    aggrega_e_applica_mossa(game);
}
