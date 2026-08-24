

#ifndef BOT_COORDINATOR_H
#define BOT_COORDINATOR_H

#include "../Sync.h"

class BotCoordinator {

private:

    int numeroWorker;

public:

    BotCoordinator(int numeroWorker);

    void run(SyncContext& sync);
};

#endif