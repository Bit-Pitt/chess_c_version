#ifndef BOT_COORDINATOR_H
#define BOT_COORDINATOR_H

#include "../Sync.h"

class BotCoordinator {

private:
    double epsilon;

public:
    BotCoordinator(double epsilon);

    void run(SyncContext& sync);
};

#endif
